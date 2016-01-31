#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
#include "SLIPEncoding.h"

#define HOST "localhost"
#define PORT 12345

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

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
    
        ofFbo fbo;
    
        ofxOscSender sender;
        ofxOscSender PWMsender;
    
        ofxSyphonClient mClient;
        ofxSyphonClient tClient;
    
        bool		bSendSerialMessage;			// a flag for sending serial
        bool wrapInBundle = true; // TODO turn this into a parameter
    
        ofSerial	serial;
    
  private: 
    void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );
    //ofx::IO::PacketSerialDevice_<ofx::IO::SLIPEncoding, 0, 65536> device;

    
};
