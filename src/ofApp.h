#pragma once

#include "ofMain.h"
//#include "ofxSyphon.h"
#include "ofxSerial.h"
#include "ofxOsc.h"
#define PORTIN 98765
#define NUM_MSG_STRINGS 20


class SerialMessage
{
public:
    SerialMessage(): fade(0)
    {
    }
    
    SerialMessage(const std::string& _message,
                  const std::string& _exception,
                  int _fade):
    message(_message),
    exception(_exception),
    fade(_fade)
    {
    }
    
    std::string message;
    std::string exception;
    int fade;
};

class Teensy
{
public:
    ofx::IO::PacketSerialDevice_<ofx::IO::SLIPEncoding, ofx::IO::SLIPEncoding::END, 16384> dev;
    string name;
    
    void setup(){
        if (name == "") {
            ofLogError("Teensy") << "please set dev name before setup()";
            return;
        }
        if ( !dev.setup(name) )
            ofLogError("Teensy") << "Can't connect to " << name;
    }
};

class LedLine
{
public:
    ofPixels * src; // source
    string address; // address OSC
    int nbPix; // nombre de pixels
    int offset; // offset en nombre de lignes
    int size; // nombre de lignes ˆ prendre dans pix
    int Xsize; // longueur de la ligne en X
    Teensy * dev;
    ofPixels pixelCrop;
    uint8_t brightness;
    uint8_t dither;
    
};

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
void exit();

    ofPixels pixels, pixOrb;

    ofVideoPlayer trame;
    ofxOscReceiver receiver;

    ofBuffer imgAsBuffer, NetBuffer;

    ofImage img, PWMimg, Brightimg;
    unsigned char * Brights, * DMX, * NET;

    bool playing {true};
    bool waiting {true};
    
    float fps {30.};

    int width {66}, height {22};
    int OrbSize {20};
    
    int sensorValue {0};

    ofColor orbColor {255, 255, 255, 255};

    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    void sendLine(int i);
    void setBrightness(int line, int brightness);
    void setDither(int line, int dither);
    void makeOrb();
    
    void setWallLum();
    void setOrbLum();
    
    void testSensor();
    void orbBreathe();
    
    float orbLum {0.}, wallLum {0.};
    float orbInc {0.01}, wallInc {0.01};
    float orbMin {0.2}, orbMax {1.};
    float orbDir {1.};

    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    std::string portName(int SN);

    Teensy device, device2, device3, device4;
    
    ofx::IO::BufferedSerialDevice sensor;

    std::vector<SerialMessage> serialMessages;

    LedLine ledLine[7];

    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];


private:
    void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );



    
};
