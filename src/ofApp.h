#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
#include "ofxSerial.h"

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
        void exit();
    
        ofxOscSender sender;
        ofxOscSender PWMsender;
    
        ofxSyphonClient mClient;
    
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    ofx::IO::PacketSerialDevice device;
    std::vector<SerialMessage> serialMessages;
    
  private: 
    void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );
    //ofx::IO::PacketSerialDevice_<ofx::IO::SLIPEncoding, 0, 65536> device;

    
};
