#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

#include "thermapp.h"

// For 384×288 ThermApp. If 640×480, change here + in FFmpeg command below.
#define THERMAPP_WIDTH  384
#define THERMAPP_HEIGHT 288

// Adjust how many frames to record, or -1 for infinite
#define FRAME_LIMIT 1000

static FILE* ffmpegPipe = NULL;

// Helper: Start an ffmpeg child process, writing raw 8-bit frames to its stdin
static FILE* start_ffmpeg(int width, int height, const char* outputFilename)
{
    // Example ffmpeg command line:
    // ffmpeg -y                 overwrite output
    //   -f rawvideo            input is raw frames
    //   -pix_fmt gray          8-bit grayscale
    //   -s 384x288             frame size
    //   -r 25                  frames per second (approx ThermApp rate)
    //   -i pipe:0              read from stdin
    //   -c:v libx264           encode H.264
    //   -pix_fmt yuv420p       ensure a standard yuv420p output
    //   out.mp4
    //
    // If you want to see it "live," you can pipe to an RTMP server or local window,
    // but let's just encode to a file for simplicity.

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -y -f rawvideo -pix_fmt gray -s %dx%d -r 25 -i pipe:0 "
        "-c:v libx264 -pix_fmt yuv420p \"%s\"",
        width, height, outputFilename);

    // popen opens a pipe to the command’s stdin
    FILE* pipe = popen(cmd, "w");
    if (!pipe) {
        fprintf(stderr, "Failed to start ffmpeg via popen\n");
        return NULL;
    }
    return pipe;
}

int main(void)
{
    // 1) Init + open ThermApp
    ThermApp* thermapp = thermapp_initUSB();
    if (!thermapp) {
        fprintf(stderr, "Error: thermapp_initUSB failed.\n");
        return 1;
    }

    if (thermapp_USB_checkForDevice(thermapp, VENDOR, PRODUCT) < 1) {
        fprintf(stderr, "Error: No ThermApp device found or claim error.\n");
        thermapp_Close(thermapp);
        return 1;
    }

    if (thermapp_FrameRequest_thread(thermapp) < 0) {
        fprintf(stderr, "Error: Could not start ThermApp read threads.\n");
        thermapp_Close(thermapp);
        return 1;
    }

    printf("ThermApp streaming started. Launching FFmpeg...\n");

    // 2) Start ffmpeg process, writing to out.mp4
    ffmpegPipe = start_ffmpeg(THERMAPP_WIDTH, THERMAPP_HEIGHT, "out.mp4");
    if (!ffmpegPipe) {
        thermapp_Close(thermapp);
        return 1;
    }

    // 3) We'll read frames in a loop, convert to 8-bit, then write them to ffmpeg
    short*  frame16 = (short*)malloc(sizeof(short) * THERMAPP_WIDTH * THERMAPP_HEIGHT);
    if (!frame16) {
        fprintf(stderr, "Out of memory (frame16)\n");
        pclose(ffmpegPipe);
        thermapp_Close(thermapp);
        return 1;
    }

    unsigned char* frame8 = (unsigned char*)malloc(THERMAPP_WIDTH * THERMAPP_HEIGHT);
    if (!frame8) {
        fprintf(stderr, "Out of memory (frame8)\n");
        free(frame16);
        pclose(ffmpegPipe);
        thermapp_Close(thermapp);
        return 1;
    }

    int framesCaptured = 0;
    while (1) {
        // Try to get a new frame
        if (!thermapp_GetImage(thermapp, frame16)) {
            // no new frame yet, wait a bit
            usleep(5000); // 5 ms
            continue;
        }

        // We have a new 16-bit frame in frame16 (size = THERMAPP_WIDTH*THERMAPP_HEIGHT)
        // Let's do a quick 16->8 mapping:
        // 1) find min & max in the current frame
        short minVal = frame16[0];
        short maxVal = frame16[0];
        for (int i = 1; i < THERMAPP_WIDTH * THERMAPP_HEIGHT; i++) {
            if (frame16[i] < minVal) minVal = frame16[i];
            if (frame16[i] > maxVal) maxVal = frame16[i];
        }
        float range = (float)(maxVal - minVal);
        if (range < 1.0f) range = 1.0f;

        // 2) scale each pixel to 0..255
        for (int i = 0; i < THERMAPP_WIDTH * THERMAPP_HEIGHT; i++) {
            float scaled = (frame16[i] - minVal) / range * 255.0f;
            if (scaled < 0)   scaled = 0;
            if (scaled > 255) scaled = 255;
            frame8[i] = (unsigned char)scaled;
        }

        // 3) Write that 8-bit data to ffmpeg’s stdin
        size_t written = fwrite(frame8, 1, THERMAPP_WIDTH * THERMAPP_HEIGHT, ffmpegPipe);
        if (written < (size_t)(THERMAPP_WIDTH * THERMAPP_HEIGHT)) {
            fprintf(stderr, "Error: wrote only %zu of %d bytes to ffmpeg.\n",
                    written, THERMAPP_WIDTH * THERMAPP_HEIGHT);
            break;
        }
        fflush(ffmpegPipe);

        framesCaptured++;
        if (FRAME_LIMIT > 0 && framesCaptured >= FRAME_LIMIT) {
            printf("Reached frame limit (%d). Stopping.\n", FRAME_LIMIT);
            break;
        }
    }

    // Cleanup
    printf("Closing ffmpeg...\n");
    pclose(ffmpegPipe);  // let ffmpeg finalize and close the output file

    printf("Closing ThermApp...\n");
    free(frame8);
    free(frame16);
    thermapp_Close(thermapp);

    printf("Done. Wrote out.mp4.\n");
    return 0;
}
