#include "ofApp.h"

const int width = 55;
const int height = 55;

const int LEDnumb = 565;
const int PWMnumb = 495;


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowTitle("NebulaLEDs");
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);
    
    mClient.setup();
    
    mClient.set("Fond","Max");

    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
    
}

//--------------------------------------------------------------
void ofApp::update(){


}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ofFbo fbo;
    fbo.allocate(45, 45, GL_RGB);
    fbo.begin();
    ofClear(0,0,0);
    fbo.end();
    ofPixels pixels;
    ofImage feedImg;
    
    fbo.begin();
    mClient.draw(0, 0);
    fbo.end();
    
    fbo.readToPixels(pixels);
    
    
    fbo.draw(0, 0);
    
    
    ofBuffer imgAsBuffer;
    imgAsBuffer.clear();
    imgAsBuffer.append((const char*)pixels.getData(),pixels.size()-64);
    
    ofxOscMessage m;
    m.setAddress("/image");
    m.addBlobArg(imgAsBuffer);
    sender.sendMessage(m);

    ofBuffer slipBuffer;
    ofx::IO::SLIPEncoding slip;
    slip.encode(imgAsBuffer, slipBuffer);
    
    ofxOscMessage s;
    s.setAddress("/SLIPimage");
    s.addBlobArg(slipBuffer);
    sender.sendMessage(s);
    
    
    // feedImg.setFromPixels(feedPxl);
    
    // mClient.bind();
    
    //ofTexture tex = mClient.getTexture();
    //ofPixels & pixels = tex.readToPixels();
    
    
    //ofPixels & pixels = mClient.getTextureReference().readToPixels(pixels);
    // ofTexture Nebula = mClient.getTexture()();

    

}

