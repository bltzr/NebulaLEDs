#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxSerial.h"
#include "ofxOsc.h"
#define PORT 66666
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
    
};

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
    
        ofFbo fbo, fboTour;
        ofPixels pixels, pixTour;
    
        ofxSyphonClient mClient, tClient;
    
        ofxOscReceiver receiver;
    
        void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
        void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
        void sendLine(int i);
    void setBrightness(int line, int brightness);
    
        Teensy device, device2;
    
        std::vector<SerialMessage> serialMessages;
    
        LedLine ledLine[4];
    
        int current_msg_string;
        string msg_strings[NUM_MSG_STRINGS];
        float timers[NUM_MSG_STRINGS];
    
    
    private:
        void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );



    
};
