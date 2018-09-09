#pragma once

#include "utils.hpp"
//#include "ofxSyphon.h"
#define PORTIN 98765
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp {

public:
    // public of methods:
    void setup();
    void update();
    void draw();
    void exit();

    // parameters:
    float fps {30.};
    float orbInc {0.01}, wallInc {0.01};
    float orbMin {0.2}, orbMax {1.};
    float orbDir {1.};
    
    ofColor orbColor {255, 255, 255, 255};

    // Serial events:
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);

private:
    
    // Private methods:
    void sendLine(int i);
    void setBrightness(int line, int brightness);
    void setDither(int line, int dither);
    void makeOrb();
    
    void setWallLum();
    void setOrbLum();
    
    void testSensor();
    void orbBreathe();
    
    // Components:
    ofPixels pixels, pixOrb;
    
    ofVideoPlayer trame;
    ofxOscReceiver receiver;
    
    ofBuffer imgAsBuffer, NetBuffer;
    
    ofImage img, PWMimg, Brightimg;
    unsigned char * Brights, * NET;
    
    ofx::IO::BufferedSerialDevice sensor;
    
    Teensy device, device2, device3, device4;
    LedLine ledLine[7];
    
    // Internal variables:
    bool playing {true};
    bool waiting {true};
    
    int width {66}, height {22};
    int OrbSize {20};
    
    int sensorValue {0};
    
    float orbLum {0.}, wallLum {0.};
    
    // Serial stuff:
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    std::string portName(int SN);
    
    std::vector<SerialMessage> serialMessages;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );
    


};
