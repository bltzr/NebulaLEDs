#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
#include "SLIPEncoding.h"

#define HOST "localhost"
#define PORT 12345

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        ofxOscSender sender;
        ofxOscSender PWMsender;
    
        ofxSyphonClient mClient;
    
        ofx::IO::SLIPEncoding slip;
		
};
