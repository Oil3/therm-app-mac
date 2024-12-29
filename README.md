## therm-app-mac
Mac OS app for Opgal's "Therm-app" thermal camera. work-in-progress, that works.

Last time I had used my thermapp 25hz was right before the pandemic, clients weren't plentiful.  I had the idea of using this tiny tablet to go ask the nail salon`s manager if she wanted me to _tomarle tu temperatura_.  Good laugh, 36celcius, and almost no battery already.

Since then, blablabla



# dec2024 : successful communication and raw frames are received, however still a mess and **needs much more work**
# I need to figure out calibration and/or proper offset values
No driver and no kernel needed.  
The command line tools use [libusb](https://libusb.info/) and are based on the [public debug code](https://github.com/Pidbip/ThermAppCam). Built in C.

The Swift app is 100% native and is inspired from [Didaktek's Simple-USB](https://github.com/didactek/deft-simple-usb) and [ftdi-synchronous-serial](https://github.com/didactek/ftdi-synchronous-serial).  

Everything is notarized and hardened.



How:
both command line tools expect libusb at `/opt/homebrew/lib/libusb-1.0.dylib` 
notarized/gatekeeper approved but needs to be ran inside terminal.  

The swift app technically needs nothing, but until the why is figured out, it only recognizes the device after one of the command line tools has connected and exited at least once.


CLI 
`mac-thermapp-libusb` fetches and saves 50 raw frames (frame_%04d.raw).
`thermapp_ffmpeg` uses ffmpeg to fetch 1000 frames and saves a .mp4 (out.mp4) video.



The first succesful connection, albeit slightly off-the-charts![image](https://github.com/user-attachments/assets/14af3cde-cebc-459e-8cac-d0cc6eced568)
Getting better, it's actually 35celcius, but outside, and still a lot of flickering and still nowhere near the CLI tool - which itself is still very bad![image](https://github.com/user-attachments/assets/ba353980-bb10-40a0-8fa9-98fec1f0cf00)


here I'm passing a lighter in front.

https://github.com/user-attachments/assets/ca8ab25b-df41-431f-99d1-fcb387015aa8

