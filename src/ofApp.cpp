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
    
    // sender.sendMessage(m);
    // this code come from ofxOscSender::sendMessage in ofxOscSender.cpp
    static const int OUTPUT_BUFFER_SIZE = 327680;
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    // serialise the message
    bool wrapInBundle = false; // TODO turn this into a parameter
    if(wrapInBundle) p << osc::BeginBundleImmediate;
    appendMessage( m, p );
    if(wrapInBundle) p << osc::EndBundle;

    ofx::IO::SLIPEncoding slip;
    ofx::IO::ByteBuffer original(p.Data(),p.Size());
    ofx::IO::ByteBuffer encoded;
    slip.encode(original, encoded);
    
    ofxOscMessage s;
    ofBuffer slipBuffer;
    ofLogNotice("slip") << "original size : " << original.size();
    ofLogNotice("slip") << "encoded size : " << encoded.size();
    slipBuffer.append(reinterpret_cast<const char*>( encoded.getPtr()), encoded.size()); // getPtr() returns the const char* of the underlying buffer
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

void ofApp::appendMessage( ofxOscMessage& message, osc::OutboundPacketStream& p )
{
    p << osc::BeginMessage( message.getAddress().c_str() );
	for ( int i=0; i< message.getNumArgs(); ++i )
	{
		if ( message.getArgType(i) == OFXOSC_TYPE_INT32 )
			p << message.getArgAsInt32( i );
		else if ( message.getArgType(i) == OFXOSC_TYPE_INT64 )
			p << (osc::int64)message.getArgAsInt64( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_FLOAT )
			p << message.getArgAsFloat( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_DOUBLE )
			p << message.getArgAsDouble( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_STRING || message.getArgType( i ) == OFXOSC_TYPE_SYMBOL)
			p << message.getArgAsString( i ).c_str();
		else if ( message.getArgType( i ) == OFXOSC_TYPE_CHAR )
			p << message.getArgAsChar( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_MIDI_MESSAGE )
			p << message.getArgAsMidiMessage( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_TRUE || message.getArgType( i ) == OFXOSC_TYPE_FALSE )
			p << message.getArgAsBool( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_TRIGGER )
			p << message.getArgAsTrigger( i );
		else if ( message.getArgType( i ) == OFXOSC_TYPE_TIMETAG )
			p << (osc::int64)message.getArgAsTimetag( i );
		//else if ( message.getArgType( i ) == OFXOSC_TYPE_RGBA_COLOR )
		//	p << message.getArgAsRgbaColor( i );
        else if ( message.getArgType( i ) == OFXOSC_TYPE_BLOB ){
            ofBuffer buff = message.getArgAsBlob(i);
            osc::Blob b(buff.getData(), (unsigned long)buff.size());
            p << b; 
		}else
		{
			ofLogError("ofxOscSender") << "appendMessage(): bad argument type " << message.getArgType( i );
			assert( false );
		}
	}
	p << osc::EndMessage;
}
