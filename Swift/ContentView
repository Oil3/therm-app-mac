import SwiftUI
import AVKit

import CoreMedia

struct ContentView: View {
    @EnvironmentObject var manager: ThermAppManager
    var body: some View {
        VStack {
            if let pixelBuffer = manager.currentPixelBuffer {
              VStack {
                Text("therm-app")

                VideoPlayerView(pixelBuffer: pixelBuffer)
                    .frame(width: 640, height: 480)
              }
            } else {
                Text("No frame available")
            }

            Text(String(format: "Temp: %.2f °C", manager.lastTemperature))
            Text("FrameCount: \(manager.frameCount)")
        }
        .padding()
//    .onAppear {
//      manager.openDevice()
//    }
    }

}
struct VideoPlayerView: NSViewRepresentable {
let pixelBuffer: CVPixelBuffer

// Time tracker,a bit dirty
static var currentPresentationTime: CMTime = .zero

func makeNSView(context: Context) -> NSView {
    let videoView = NSView()
    let layer = AVSampleBufferDisplayLayer()
    layer.videoGravity = .resizeAspect
    videoView.layer = layer
    return videoView
}

func updateNSView(_ nsView: NSView, context: Context) {
    guard let layer = nsView.layer as? AVSampleBufferDisplayLayer else { return }

    var formatDescription: CMFormatDescription?
    let status = CMVideoFormatDescriptionCreateForImageBuffer(
        allocator: kCFAllocatorDefault,
        imageBuffer: pixelBuffer,
        formatDescriptionOut: &formatDescription
    )

    guard status == kCVReturnSuccess, let formatDesc = formatDescription else {
        print("Failed to create CMFormatDescription")
        return
    }

    // A bit dirty
    Self.currentPresentationTime = CMTimeAdd(Self.currentPresentationTime, CMTime(value: 1, timescale: 25))

    // Wrap the CVPixelBuffer in a CMSampleBuffer
    var timingInfo = CMSampleTimingInfo(
        duration: CMTime(value: 1, timescale: 25),
        presentationTimeStamp: Self.currentPresentationTime,
        decodeTimeStamp: .invalid
    )
    var sampleBuffer: CMSampleBuffer?
    let sampleBufferStatus = CMSampleBufferCreateForImageBuffer(
        allocator: kCFAllocatorDefault,
        imageBuffer: pixelBuffer,
        dataReady: true,
        makeDataReadyCallback: nil,
        refcon: nil,
        formatDescription: formatDesc,
        sampleTiming: &timingInfo,
        sampleBufferOut: &sampleBuffer
    )

    guard sampleBufferStatus == kCVReturnSuccess, let buffer = sampleBuffer else {
        print("Failed to create CMSampleBuffer")
        return
    }

    layer.flushAndRemoveImage()
    layer.enqueue(buffer)
    layer.setNeedsDisplay()
}
}


//struct VideoPlayerView: NSViewRepresentable {
//    let pixelBuffer: CVPixelBuffer
//
//    func makeNSView(context: Context) -> NSView {
//        let videoView = NSView()
//        let layer = AVSampleBufferDisplayLayer()
//        videoView.layer = layer
//        return videoView
//    }
//  
//
//  func updateNSView(_ nsView: NSView, context: Context) {
//      guard let layer = nsView.layer as? AVSampleBufferDisplayLayer else { return }
//      
//      // Create a CMSampleBuffer from the CVPixelBuffer
//      var formatDescription: CMFormatDescription?
//      let status = CMVideoFormatDescriptionCreateForImageBuffer(
//          allocator: kCFAllocatorDefault,
//          imageBuffer: pixelBuffer,
//          formatDescriptionOut: &formatDescription
//      )
//      
//      guard status == kCMReturnSuccess, let formatDesc = formatDescription else {
//          print("Failed to create CMFormatDescription")
//          return
//      }
//      
//      // Wrap the CVPixelBuffer in a CMSampleBuffer
//      var timingInfo = CMSampleTimingInfo(duration: CMTime(value: 1, timescale: 30), presentationTimeStamp: .zero, decodeTimeStamp: .invalid)
//      var sampleBuffer: CMSampleBuffer?
//      let sampleBufferStatus = CMSampleBufferCreateForImageBuffer(
//          allocator: kCFAllocatorDefault,
//          imageBuffer: pixelBuffer,
//          dataReady: true,
//          makeDataReadyCallback: nil,
//          refcon: nil,
//          formatDescription: formatDesc,
//          sampleTiming: &timingInfo,
//          sampleBufferOut: &sampleBuffer
//      )
//      
//      guard sampleBufferStatus == kCMReturnSuccess, let buffer = sampleBuffer else {
//          print("Failed to create CMSampleBuffer")
//          return
//      }
//      
//      // Display the sample buffer in the layer
//      layer.enqueue(buffer)
//      layer.setNeedsDisplay()
//  }
//  }
