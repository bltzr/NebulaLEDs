This openFrameworks application allows to send images to APA102 LED strips through a bunch of teensy-based bridge applications (code included) via SLIP-OSC.

- master branch is for sending from a Max patcher (or other)
- player branch implements a standalone player, that parses the pixels of a (45*45) video to the LED strips, plus 64 DMX channels.

Please notice this has been developed for [a specific art project](http://baltazars.org/project/nebula), but hopefully bits and pieces could be useful for others…

Notably, in the Arduino sub-folder, there are applications for teensy to control APA102 LED strips (with global brightness and FastLED dithering support) and to output DMX on the UART port 1 (for teensy >= 3.x)

Some ofAddons are necessary to build this app:
- https://github.com/avilleret/ofxSerial.git - on the feature/SLIPSerialExample branch
- https://github.com/bakercp/ofxIO.git (on branch stable)


* WebSocket lib:


```
brew install openssl

cd /path/to/libwebsockets
mkdir build
cd build
cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64" ..
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib ..
make
cmake -DCMAKE_INSTALL_PREFIX:PATH=./install . && make install
```