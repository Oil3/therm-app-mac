//           clang thermapp.c thermapp_main.c -I/opt/homebrew/include \
//   -L/opt/homebrew/lib -lusb-1.0 -lpthread \
// -o thermapp
#include <stdio.h>
#include <unistd.h>   // for sleep, usleep
#include <stdlib.h>   // for exit
#include <stdint.h>   // for fixed-width types like uint16_t
#include <string.h>   // for memset
#include "thermapp.h" //

#define CAPTURE_FRAME_COUNT  50  // how many frames to save?
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize ThermApp structure + libusb
    ThermApp *thermapp = thermapp_initUSB();
    if (!thermapp) {
        fprintf(stderr, "Failed to init ThermApp\n");
        return 1;
    }

    //GetTherm-App device by VID/PID
    int rc = thermapp_USB_checkForDevice(thermapp, VENDOR, PRODUCT);
    if (rc < 1) {
        fprintf(stderr, "No device found or open/claim error\n");
        thermapp_Close(thermapp);
        return 1;
    }

    // Threads to read frames
    rc = thermapp_FrameRequest_thread(thermapp);
    if (rc < 0) {
        fprintf(stderr, "Failed to create read threads\n");
        thermapp_Close(thermapp);
        return 1;
    }

    printf("Starting streaming from Therm-App...\n");

    // Capture n frames, saving each to disk
    // Storing frames in a short[] array (16-bit)
    short pixelBuffer[PIXELS_DATA_SIZE];
    memset(pixelBuffer, 0, sizeof(pixelBuffer));

    for (int frameIndex = 0; frameIndex < CAPTURE_FRAME_COUNT; frameIndex++) {
        // Loop until a new frame is available
        while (!thermapp_GetImage(thermapp, pixelBuffer)) {
            // No new frame yet; wait a bit
            usleep(150 * 1000); // 150 ms
        }

        // We have a new frame in pixelBuffer
        // Save to a raw 16-bit file: "frameXXXX.raw"
        char filename[256];
        snprintf(filename, sizeof(filename), "frame_%04d.raw", frameIndex);

        FILE *f = fopen(filename, "wb");
        if (!f) {
            fprintf(stderr, "Failed to open %s for writing\n", filename);
            continue;
        }
        // Write all the 384*288 = 110592 pixels, each 2 bytes (short)
        fwrite(pixelBuffer, sizeof(short), PIXELS_DATA_SIZE, f);
        fclose(f);

        // Print temperature and framecount
        float tempC = thermapp_getTemperature(thermapp);
        int   idVal = thermapp_getId(thermapp);
        unsigned short frameCount = thermapp_getFrameCount(thermapp);

        printf("Saved frame %d as %s (ID=%d, temperature=%.2f, frameCount=%u)\n",
               frameIndex, filename, idVal, tempC, frameCount);
    }

    // Close device and exit
    printf("Captured %d frames. Now shutting down ThermApp.\n", CAPTURE_FRAME_COUNT);
    thermapp_Close(thermapp);

    return 0;
}
      
// https://github.com/Oil3/therm-app-mac
// Quet Almahdi Morris - 2024 github.com/oil3
