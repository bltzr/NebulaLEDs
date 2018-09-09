//
//  utils.hpp
//  NebulaLEDs
//
//  Created by Pascal Baltazar on 09/09/2018.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxSerial.h"
#include "ofxOsc.h"

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
    int size; // nombre de lignes à prendre dans pix
    int Xsize; // longueur de la ligne en X
    Teensy * dev;
    ofPixels pixelCrop;
    uint8_t brightness;
    uint8_t dither;
    
    void setup(){
        pixelCrop.allocate(Xsize, size, 3);
        for (int i = 0; i<Xsize*size*3; ++i) pixelCrop[i]=0;
    }
    
};

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



#endif /* utils_hpp */
