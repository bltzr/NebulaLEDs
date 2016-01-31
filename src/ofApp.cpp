#include "ofApp.h"

const int width = 55;
const int height = 55;

const int LEDnumb = 565;
const int PWMnumb = 495;


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowTitle("NebulaLEDs");
    
    bSendSerialMessage = false;
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);
    
    mClient.setup();
    
    mClient.set("Fond","Max");
    tClient.set("Tour","Max");

    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
    
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
    // this should be set to whatever com port your serial device is connected to.
    // (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
    // arduino users check in arduino app....
    int baud = 115200;
    //serial.setup(0, baud); //open the first device
    serial.setup("/dev/tty.usbmodem1369841", baud); // mac osx example
    

}

//--------------------------------------------------------------
void ofApp::update(){

    //ofFbo fbo;
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
    

    
    ofBuffer imgAsBuffer;
    imgAsBuffer.clear();
    imgAsBuffer.append((const char*)pixels.getData(),pixels.size()-5109);
    
    ofxOscMessage m;
    m.setAddress("/led");
    m.addBlobArg(imgAsBuffer);
    
    // sender.sendMessage(m);
    // this code come from ofxOscSender::sendMessage in ofxOscSender.cpp
    static const int OUTPUT_BUFFER_SIZE = 327680;
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    // serialise the message

    if(wrapInBundle) p << osc::BeginBundleImmediate;
    appendMessage( m, p );
    if(wrapInBundle) p << osc::EndBundle;
    
    ofx::IO::SLIPEncoding slip;
    ofx::IO::ByteBuffer original(p.Data(),p.Size());
    
    ofx::IO::ByteBuffer encoded;
    slip.encode(original, encoded);
    
    ofLogNotice("slip2") << "original size : " << original.size();
    ofLogNotice("slip2") << "encoded size : " << encoded.size();
    
    serial.writeBytes(reinterpret_cast<unsigned char*>( encoded.getPtr()), encoded.size());
 
    
    
  /*
    
    ////////////////////////////
    // Deuxième ligne de LEDs //
    ////////////////////////////
    
    ofPixels pixels2;
    pixels.cropTo(pixels2, 25, 12, 24, 23);
    
    ofBuffer imgAsBuffer2;
    imgAsBuffer2.clear();
    imgAsBuffer2.append((const char*)pixels2.getData(),pixels2.size());
    
    ofLogNotice("pixels2") << "pixels2 number : " << pixels.size()-4380;
    
    ofxOscMessage m2;
    m2.setAddress("/led2");
    m2.addBlobArg(imgAsBuffer2);
    
    // this code come from ofxOscSender::sendMessage in ofxOscSender.cpp
    char buffer2[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p2( buffer2, OUTPUT_BUFFER_SIZE );
    
    // serialise the message
    if(wrapInBundle) p2 << osc::BeginBundleImmediate;
    appendMessage( m2, p2 );
    if(wrapInBundle) p2 << osc::EndBundle;
    
    ofx::IO::SLIPEncoding slip2;
    ofx::IO::ByteBuffer original2(p2.Data(),p2.Size());
    ofx::IO::ByteBuffer encoded2;
    slip2.encode(original2, encoded2);
    
    ofLogNotice("slip2") << "original size : " << original.size();
    ofLogNotice("slip2") << "encoded size : " << encoded.size();
    
    serial.writeBytes(reinterpret_cast<unsigned char*>( encoded2.getPtr()), encoded2.size());
    
    
    ////////////////////////////////////////////////
    // sending stuff via OSC for testing purposes //
    ////////////////////////////////////////////////
  
    ofxOscMessage s;

    ofBuffer slipBuffer;

    slipBuffer.append(reinterpret_cast<const char*>( encoded.getPtr()), encoded.size()); // getPtr() returns the const char* of the underlying buffer

    s.setAddress("/led2");
    s.addBlobArg(slipBuffer);
    sender.sendMessage(s);
    */
    

}



//--------------------------------------------------------------
void ofApp::draw(){
    
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    
    fbo.draw(0, 0);


    

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


