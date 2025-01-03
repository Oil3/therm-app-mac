## therm-app-mac 

Mac OS app for Opgal's "Therm-app" thermal camera. work-in-progress, that works.

No driver nor kernel nor 'sudo' needed.  


## jan2025 : now we can see something.
https://github.com/user-attachments/assets/d551f93d-7b25-4452-9184-69ab6e19cdb3

https://github.com/user-attachments/assets/d2dffa27-8ca5-4594-8966-9e2b99d725fc


###  I found the proper offsets

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


I use a micro-usb to usb-c Anker branded cable.    
Getting better, it's 25celcius, I don't know why image is flipped and rotated.

https://github.com/user-attachments/assets/d551f93d-7b25-4452-9184-69ab6e19cdb3
