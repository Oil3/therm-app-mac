import SwiftUI
import CoreVideo

class ThermAppManager: ObservableObject {
    @Published var currentPixelBuffer: CVPixelBuffer? = nil
    @Published var lastTemperature: Float = 0
    @Published var frameCount: UInt16 = 0

    private let width = 384
    private let height = 288

    public var thermAppDevice: RefactoredThermAppDevice?

    init() {
        openDevice()
    }

    private func openDevice() {
        do {
            let bus = FWUSBBus()
            let usbDevice = try bus.findDevice(idVendor: 0x1772, idProduct: 0x0002)

            guard let fwUSB = usbDevice as? FWUSBDevice else {
                print("Didn't get an FWUSBDevice")
                return
            }

            let device = RefactoredThermAppDevice(
                device: fwUSB.device,
                writeEndpoint: fwUSB.writeEndpoint,
                readEndpoint: fwUSB.readEndpoint
            )

            device.onFrameReceived = { [weak self] frame in
                self?.handleNewFrame(frame)
            }

            self.thermAppDevice = device
            device.startReading()
        } catch {
            print("Failed to open ThermApp device: \(error)")
        }
    }
  private func handleNewFrame(_ frame: ThermappFrame) {
      let rawTemp = Float(frame.header.temperature)
      self.lastTemperature = (rawTemp - 14336) * 0.00652
      self.frameCount = frame.header.frameCount

      print("Handling frame: FrameCount \(self.frameCount), Temp \(self.lastTemperature)")

      let pixelCount = width * height
      let minVal = frame.pixels.min() ?? 0
      let maxVal = frame.pixels.max() ?? 1
      let range = Float(maxVal > minVal ? maxVal - minVal : 1)

      var normalized = [UInt8](repeating: 0, count: pixelCount)
      for i in 0..<pixelCount {
          let pixel = frame.pixels[i]
          let scaled = Float(pixel - minVal) / range
          normalized[i] = UInt8(scaled * 255) 
      }

    if let pixelBuffer = createBGRAPixelBuffer(width: width, height: height, grayscale: normalized) {
          DispatchQueue.main.async {
              self.currentPixelBuffer = pixelBuffer
          }
      }
  }

//  private func handleNewFrame(_ frame: ThermappFrame) {
//      print("New frame received: FrameCount \(frame.header.frameCount), Temp \(frame.header.temperature)")
//
//      let pixelCount = width * height
//      let minVal = frame.pixels.min() ?? 0
//      let maxVal = frame.pixels.max() ?? 1
//      let range = Float(maxVal > minVal ? maxVal - minVal : 1)
//
//      // Normalize to 8-bit grayscale
//      var normalized = [UInt8](repeating: 0, count: pixelCount)
//      for i in 0..<pixelCount {
//          let pixel = frame.pixels[i]
//          let scaled = Float(pixel - minVal) / range
//          normalized[i] = UInt8(max(0, min(255, scaled * 255))) 
//      }
//
//      if let pixelBuffer = createBGRAPixelBuffer(width: width, height: height, grayscale: normalized) {
//          DispatchQueue.main.async {
//              self.currentPixelBuffer = pixelBuffer
//          }
//      } else {
//          print("Failed to create pixelbuffer")
//      }
//  }

  private func createBGRAPixelBuffer(width: Int, height: Int, grayscale: [UInt8]) -> CVPixelBuffer? {
      var pixelBuffer: CVPixelBuffer?
      let attrs: [CFString: Any] = [
          kCVPixelBufferIOSurfacePropertiesKey: [:],
          kCVPixelBufferCGImageCompatibilityKey: true,
          kCVPixelBufferCGBitmapContextCompatibilityKey: true
      ]

      let status = CVPixelBufferCreate(
          kCFAllocatorDefault,
          width,
          height,
          kCVPixelFormatType_32BGRA,
          attrs as CFDictionary,
          &pixelBuffer
      )

      guard status == kCVReturnSuccess, let pb = pixelBuffer else {
          print("Failed to create pixel buffer: \(status)")
          return nil
      }

      CVPixelBufferLockBaseAddress(pb, [])
      defer { CVPixelBufferUnlockBaseAddress(pb, []) }

      guard let baseAddress = CVPixelBufferGetBaseAddress(pb) else {
          print("Failed to get base address of pixel buffer")
          return nil
      }

      let bytesPerRow = CVPixelBufferGetBytesPerRow(pb)
      for row in 0..<height {
          let srcRow = grayscale[row * width ..< (row + 1) * width]
          let dstRow = baseAddress.advanced(by: row * bytesPerRow)

          for (col, value) in srcRow.enumerated() {
              let pixel = dstRow.advanced(by: col * 4)
              pixel.storeBytes(of: value, as: UInt8.self)         
              pixel.advanced(by: 1).storeBytes(of: value, as: UInt8.self) 
              pixel.advanced(by: 2).storeBytes(of: value, as: UInt8.self) 
              pixel.advanced(by: 3).storeBytes(of: 255, as: UInt8.self)   
          }
      }

      return pb
  }

    deinit {
        thermAppDevice?.stopReading()
    }
}
