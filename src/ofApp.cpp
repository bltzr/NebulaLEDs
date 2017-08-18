#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowTitle("NebulaLEDs");

    receiver.setup(PORT);
    ofLog() << "Opened OSC Receiver";
  
    
    // display
    ofBackground(0,0,0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
  
    trame.setPixelFormat(OF_PIXELS_RGB);
  
    //trame.load("bright.mov");
    trame.load("/data/Nebula.mov");
    ofLog() << "Loaded Mov";
    trame.setLoopState(OF_LOOP_NORMAL);
  
    if (send){
      // open an outgoing connection to HOST:PORT
      sender.setup(HOST, PORT);
      ofLog() << "Opened OSC Sender";
    }
  
  
  
    if(playing){
      trame.play();
      }
  
    
    // setup Serial
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    
    //ofLogNotice("ofApp::setup") << "Connected Devices: ";
    
    device.name = "/dev/cu.usbmodem1369841";
    device2.name = "/dev/cu.usbmodem1455771";
    device3.name = "/dev/cu.usbmodem1383111";
    //device4.name = "/dev/cu.usbmodem1366241";
    device4.name = "/dev/cu.usbmodem1365391";
  
    
    
    device.setup();
    device2.setup();
    device3.setup();
    device4.setup();
    
    //ligne 1 : Fond - T1 - /l1 - nPixels = 565 - x = 0 - h = 13
    //ligne 2 : Fond - T1 - /l2 - nPixels = 495 - x = 13 - h = 11
    //ligne 3 : Fond - T2 - /l1 - nPixels = 565 - x = 24 - h = 13
    //ligne 3 : Tour - T2 - /l2 - nPixels = 459 - x = 0 - h = Tour.width
    
    ledLine[0].dev = &device;
    ledLine[0].src = &pixels;
    ledLine[0].address = "/1";
    ledLine[0].nbPix = 315;
    ledLine[0].offset = 0;
    ledLine[0].size = 7;
    ledLine[0].Xsize = 45;
    
    ledLine[1].dev = &device;
    ledLine[1].src = &pixels;
    ledLine[1].address = "/2";
    ledLine[1].nbPix = 250;
    ledLine[1].offset = 7;
    ledLine[1].size = 6;
    ledLine[1].Xsize = 45;

    ledLine[2].dev = &device2;
    ledLine[2].src = &pixels;
    ledLine[2].address = "/1";
    ledLine[2].nbPix = 270;
    ledLine[2].offset = 13;
    ledLine[2].size = 6;
    ledLine[2].Xsize = 45;

    ledLine[3].dev = &device2;
    ledLine[3].src = &pixels;
    ledLine[3].address = "/2";
    ledLine[3].nbPix = 225;
    ledLine[3].offset = 19;
    ledLine[3].size = 5;
    ledLine[3].Xsize = 45;
    
    ledLine[4].dev = &device3;
    ledLine[4].src = &pixels;
    ledLine[4].address = "/1";
    ledLine[4].nbPix = 250;
    ledLine[4].offset = 24;
    ledLine[4].size = 6;
    ledLine[4].Xsize = 45;
    
    ledLine[5].dev = &device3;
    ledLine[5].src = &pixels;
    ledLine[5].address = "/2";
    ledLine[5].nbPix = 315;
    ledLine[5].offset = 30;
    ledLine[5].size = 7;
    ledLine[5].Xsize = 45;
    
    ledLine[6].dev = &device4;
    ledLine[6].src = &pixels;
    ledLine[6].address = "/1";
    ledLine[6].nbPix = 191;
    ledLine[6].offset = 37;
    ledLine[6].size = 5;
    ledLine[6].Xsize = 45;

}

//--------------------------------------------------------------
void ofApp::update(){


    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
      
      if(m.getAddress() == "/play"){
        //ofLog() << "b" << m.getArgAsInt32(0);
        if(m.getArgAsBool(0)){trame.play(); playing = 1;}
        else if(!m.getArgAsBool(0)){trame.stop(); playing = 0;}
      }
      
      /*
        else if(m.getAddress() == "/b"){
            //ofLog() << "b" << m.getArgAsInt32(0);
            for (int i=0; i<6; i++){
                setBrightness(i, m.getArgAsInt32(0));
            }
        }
      
        else if(m.getAddress() == "/t"){
            //ofLog() << "t" << m.getArgAsInt32(0);
            setBrightness(6, m.getArgAsInt32(0));
        }
      
        else if(m.getAddress() == "/d"){
            for (int i=0; i<6; i++){
                setDither(i, m.getArgAsInt32(0));
            }
            //ofLog() << "d" << m.getArgAsInt32(0);
        }
      
        else if(m.getAddress() == "/dt"){
            setDither(6, m.getArgAsInt32(0));
            //ofLog() << "d" << m.getArgAsInt32(0);
        }
     
        else if(m.getAddress() == "/image"){
          //    ofLog() << "nArgs" << m.getNumArgs();
          //NetBuffer.clear();
          NetBuffer = m.getArgAsBlob(0);
        }
     */
    }
  
      trame.update();
      
      // get part of the image for the LEDs
      
      if(playing){
        pixels = trame.getPixels();
        //ofLog() << "pixel format: " << pixels.getPixelFormat();
        //LEDs = pixels.getData();
        
        // get part of the image for the PWMs
        //ofPixels PWMPix;
        pixels.cropTo(PWMPix, 0, 42, 22, 1);
        DMX = PWMPix.getData();
        printf ("%s",DMX);
        //ofLog() << "DMX: " << DMX << "_";
        sendDMX();
        
        // get part of the image for the Brightnesses
        //ofPixels BrightPix;
        pixels.cropTo(BrightPix, 41, 42, 4, 1);
        Brights = BrightPix.getData();

 
        // /d - Fond dither
        for (int i=0; i<6; i++){
          setDither(i, (int)Brights[0]);
        }
        //ofLog() << "d" <<  (int)Brights[0];
        
        // /b - Fond brightness
        for (int i=0; i<6; i++){
          setBrightness(i, (int)Brights[3]/8);
        }
        //ofLog() << "b" << (int)Brights[3]/8;
        
        // /dt - Tour dither
        setDither(6, Brights[6]);
        //ofLog() << "dt" <<  (int)Brights[6];
        
        // /bt - Tour dither
        setBrightness(6, (int)Brights[9]/8);
        //ofLog() << "bt" << (int)Brights[9]/8;
   
        
     
        if (send){
          
          imgAsBuffer.clear();
          imgAsBuffer.append((const char*)pixels.getData(),pixels.size());

          ofxOscMessage m;
          m.setAddress("/image");
          m.addBlobArg(imgAsBuffer);
          sender.sendMessage(m);
          
          
          PWMBuffer.clear();
          PWMBuffer.append((const char*)PWMPix.getData(), PWMPix.size());

          ofxOscMessage n;
          n.setAddress("/PWMs");
          n.addBlobArg(PWMBuffer);
          sender.sendMessage(n);
          

          BrightBuffer.clear();
          BrightBuffer.append((const char*)BrightPix.getData(), BrightPix.size());
          
          ofxOscMessage o;
          o.setAddress("/brights");
          o.addBlobArg(PWMBuffer);
          sender.sendMessage(o);

        }

      }
      
      else{
        //LEDs = (unsigned char*) NetBuffer.getData();
        // TODO: fill pixels with the Network Data
      }
      //ofLog()<<brightness;
      

      
    //}

    for (int i=0; i<7; i++) {
    sendLine(i);
    }

}

void ofApp::setBrightness(int i, int brightness) {

    // check for waiting messages
    
        LedLine line = ledLine[i];
        ofxOscMessage n;
        n.setAddress(line.address);
        n.addIntArg(brightness);
        
        // this code come from ofxOscSender::sendMessage in ofxOscSender.cpp
        static const int OUTPUT_BUFFER_SIZE = 16384;
        char buffer[OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
        
        // serialize the message
        
        p << osc::BeginBundleImmediate;
        appendMessage( n, p );
        p << osc::EndBundle;
        
        ofx::IO::ByteBuffer toEncode(p.Data(),p.Size());
        
        try {
            line.dev->dev.send(toEncode);
        } catch ( serial::SerialException e) {
            ofLogError("sendLine") << "failed to send data : " << e.what();
            line.dev->setup();
        }

}

void ofApp::setDither(int i, int dither) {
  
        // check for waiting messages
        
        LedLine line = ledLine[i];
        ofxOscMessage n;
        n.setAddress("/b");
        n.addIntArg(dither);
        
        // this code come from ofxOscSender::sendMessage in ofxOscSender.cpp
        static const int OUTPUT_BUFFER_SIZE = 16384;
        char buffer[OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
        
        // serialise the message
        
        p << osc::BeginBundleImmediate;
        appendMessage( n, p );
        p << osc::EndBundle;
        
        ofx::IO::ByteBuffer toEncode(p.Data(),p.Size());
        
        try {
          line.dev->dev.send(toEncode);
        } catch ( serial::SerialException e) {
          ofLogError("sendLine") << "failed to send data : " << e.what();
          line.dev->setup();
        }
  
}

//--------------------------------------------------------------
void ofApp::sendDMX() {
 
    ofBuffer imgAsBuffer;
    imgAsBuffer.clear();
    imgAsBuffer.append((const char*)PWMPix.getData(),64);
  
    //ofLog()<<(const char*)PWMPix.getData();
  
    ofxOscMessage m;
    m.setAddress("/DMX");
    m.addBlobArg(imgAsBuffer);
  
    // this code comes from ofxOscSender::sendMessage in ofxOscSender.cpp
    static const int OUTPUT_BUFFER_SIZE = 16384;
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
  
    // serialise the message
  
    p << osc::BeginBundleImmediate;
    appendMessage( m, p );
    p << osc::EndBundle;
  
    ofx::IO::ByteBuffer toEncode(p.Data(),p.Size());
  
    try {
      device4.dev.send(toEncode);
    } catch ( serial::SerialException e) {
      ofLogError("sendLine") << "failed to send data : " << e.what();
      device4.setup();
    }
  
  
}

//--------------------------------------------------------------
void ofApp::sendLine(int i) {

    LedLine &line = ledLine[i];
    //ofPixels pixelCrop;
    line.src->cropTo(line.pixelCrop, 0 , line.offset, line.Xsize, line.size);
    
    ofBuffer imgAsBuffer;
    imgAsBuffer.clear();
    imgAsBuffer.append((const char*)line.pixelCrop.getData(),line.nbPix*3);
    
    ofxOscMessage m;
    m.setAddress(line.address);
    m.addBlobArg(imgAsBuffer);
    
    // this code comes from ofxOscSender::sendMessage in ofxOscSender.cpp
    static const int OUTPUT_BUFFER_SIZE = 16384;
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    // serialise the message

    p << osc::BeginBundleImmediate;
    appendMessage( m, p );
    p << osc::EndBundle;
    
    ofx::IO::ByteBuffer toEncode(p.Data(),p.Size());
    
    try {
        line.dev->dev.send(toEncode);
    } catch ( serial::SerialException e) {
        ofLogError("sendLine") << "failed to send data : " << e.what();
        line.dev->setup();
    }
}



//--------------------------------------------------------------
void ofApp::draw(){
  

    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
  //trame.play();
    trame.draw(20, 20, 450, 450);

    /*
    for (int i=0; i<6; i++) {
        ofImage img;
        img.setFromPixels(ledLine[i].pixelCrop);
        img.draw(500, ledLine[i].offset*15, 450, ledLine[i].size*10);
    }
    
    ofImage img;
    img.setFromPixels(ledLine[6].pixelCrop);
    img.draw(500, 600, 450, ledLine[6].size*30);
    */
    ofImage img;
    img.setFromPixels(BrightPix);
    img.draw(500, 600, 450, 50);
  



}


void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Decoded serial packets will show up here.
    SerialMessage message(args.getBuffer().toString(), "", 255);
    serialMessages.push_back(message);
    
    // ofLogNotice("onSerialBuffer") << "got serial buffer : " << message.message;
}


void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);
    
    serialMessages.push_back(message);
    ofLogNotice("onSerialError") << "got serial error : " << message.exception;
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


