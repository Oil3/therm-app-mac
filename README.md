## therm-app-mac
Mac OS app for Opgal's "Therm-app" thermal camera. work-in-progress, that works.

Last time I had used my thermapp 25hz was right before the pandemic, clients weren't plentiful.  I had the idea of using this tiny tablet to go ask the nail salon`s manager if she wanted me to _tomarle tu temperatura_.  Good laugh, 36celcius, and almost no battery already.

Since then, blablabla



# dec2024 : successful communication and raw frames are received

No driver and no kernel needed.  
The command line tools use [libusb](https://libusb.info/) and are based on the [public debug code](https://github.com/Pidbip/ThermAppCam),
The Swift app is 100% native and is inspired from [Didaktek's Simple-USB](https://github.com/didactek/deft-simple-usb) and [ftdi-synchronous-serial](https://github.com/didactek/ftdi-synchronous-serial)
Everything is notarized and hardened.


Usi
How:
both command line tools expect libusb at `/opt/homebrew/lib/libusb-1.0.dylib` 
notarized/gatekeeper approved but needs to be ran inside terminal
