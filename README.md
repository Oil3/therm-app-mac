## therm-app-mac 

Mac OS app for Opgal's "Therm-app" thermal camera. work-in-progress, that works.

`Last time I had used my thermapp 25hz was right before the pandemic, clients weren't plentiful.  I took the restaurant's tablet and went and asked the next door nail salon's manager if she wanted me to _tomarle tu temperatura_.  Good laugh, 36celcius, and almost no battery already. I also learnt you could actually turn the lense to focus; a whole new world.`


Very happy as I trust there is finally a use for this relatively pricey tool. 


## dec2024 : successful communication and raw frames are received, however still a mess and **needs much more work** for the Swift app.
### For the Swift app, the issue now is figuring out where in the packet chunks does the pixel data (the image we want) starts. I.e. at what byte offset do we parse the 384x288 following bytes;  Same for temperature.
No driver and no kernel needed.  
The command line tools use [libusb](https://libusb.info/) (so does the official Android apk), and are based on the [public debug code](https://github.com/Pidbip/ThermAppCam). 

The Swift app is 100% native and is inspired from [Didaktek's Simple-USB](https://github.com/didactek/deft-simple-usb) and [ftdi-synchronous-serial](https://github.com/didactek/ftdi-synchronous-serial).  

Everything is notarized and hardened.
### Download from [Releases](https://github.com/Oil3/therm-app-mac/releases) or the Zip files directly, or compile yourself:
### Compiling the CLI tools in terminal
`clang thermapp.c thermapp_main.c -I/opt/homebrew/include \
-L/opt/homebrew/lib -lusb-1.0 -lpthread \
-o thermapp`
or `thermapp_ffmpeg.c` instead of `thermapp_main.c`.  
I installed `libusb` with `brew install libusb` . Besides trying to update Python (insane thing), Homebrew has never made me lose time.
### Compiling the  CLI tools in Xcode
Command-line-tool as target, C as language, add info.plist in build settings, set a bundle name, set search paths for the libusb, setup hardened runtime in capability and authorize the library, automatic signing management with a Developper ID Application certificate, ,,, , compile and archive, zip the executable and finally upload for notarization via 'notarytool'.

How:
both command line tools expect libusb at `/opt/homebrew/lib/libusb-1.0.dylib` 
notarized/gatekeeper approved but might need to be launched from terminal.
One can check notarization of command-line tools with `spctl -assess --verbose --type install filepath` (or any filetype that can't have an embedded staple).

The swift app technically needs nothing, but until the why is figured out, it only recognizes the device after one of the command line tools has connected and exited at least once. Doing so actually creates a new row `"Current Required"`, inside macOS' System Information:
![image](https://github.com/user-attachments/assets/35390954-fd6f-4a34-9524-690b52cae8a0)



CLI 
`mac-thermapp-libusb` fetches and saves 50 raw frames (frame_%04d.raw).
`thermapp_ffmpeg` uses ffmpeg to fetch 1000 frames and saves a .mp4 (out.mp4) video.



The first succesful connection, albeit slightly off-the-charts![image](https://github.com/user-attachments/assets/14af3cde-cebc-459e-8cac-d0cc6eced568)
Getting better, it's actually 35celcius, but outside, and still a lot of flickering and still nowhere near the CLI tool - which itself is still very bad![image](https://github.com/user-attachments/assets/ba353980-bb10-40a0-8fa9-98fec1f0cf00)


here I'm passing a lighter in front; using the ffmpeg command line tool - that one I need to notarize still but you can compile yourself as well

https://github.com/user-attachments/assets/ca8ab25b-df41-431f-99d1-fcb387015aa8

