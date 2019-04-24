#pragma once

#include "ofMain.h"
//#include "ofxSyphon.h"
#include "ofxSerial.h"
#include "ofxOsc.h"
#include "ofxLibwebsockets.h"
#define HOST "Nebula.local"
#define PORT 12345
#define PORTIN 98765
#define NUM_MSG_STRINGS 20
#define PLANETS_PORTIN 98765


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
    
        //ofFbo fbo, fboTour;
        ofPixels pixels, pixTour, PWMPix, BrightPix;
    
        //ofxSyphonClient mClient, tClient;
  
        ofVideoPlayer trame;
        ofxOscSender sender, eclipse, shadow;
        ofxOscReceiver receiver;
    
        ofxLibwebsockets::Server server;
  
        ofBuffer imgAsBuffer, NetBuffer, PWMBuffer, BrightBuffer;
  
        ofImage img, PWMimg, Brightimg;
        unsigned char * Brights, * DMX, * NET;
  
        int     playing = 1;
        float   position = 0.;
  
        //string host = "Nebula.local";
        int width = 45, height = 45, DMXchannels = 64;

        bool send = 0;
        bool bSetup;
        bool stop =0;
  
        void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
        void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
        void sendLine(int i);
        void sendDMX();
        void sendPosition();
        void setBrightness(int line, int brightness);
        void setDither(int line, int dither);
        void cleanAll();    
    
        // websocket methods
        void onMessage( ofxLibwebsockets::Event& args );
        void onConnect( ofxLibwebsockets::Event& args );
        void onOpen( ofxLibwebsockets::Event& args );
        void onClose( ofxLibwebsockets::Event& args );
        void onIdle( ofxLibwebsockets::Event& args );
  
        Teensy device, device2, device3, device4;
    
        std::vector<SerialMessage> serialMessages;
    
        LedLine ledLine[7];
    
        int current_msg_string;
        string msg_strings[NUM_MSG_STRINGS];
        float timers[NUM_MSG_STRINGS];
    
    
    private:
        void appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p );



    
};
