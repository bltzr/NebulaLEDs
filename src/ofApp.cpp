#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowTitle("Interlude");

    receiver.setup(PORTIN);
    ofLog() << "Opened OSC Receiver";
  
    
    // display
    ofBackground(0,0,0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
  
    // Video player
    trame.setPixelFormat(OF_PIXELS_RGB);
    //trame.load("bright.mov");
    trame.load("/data/Interlude.mov");
    ofLog() << "Loaded Mov";
    trame.setLoopState(OF_LOOP_NONE);
  
    // Orb
    pixOrb.allocate(20, 1, 3);
    
    // Network input
    NetBuffer.allocate(width*height*3);
    for (int i=0;i<width*height*3;i++) NetBuffer.getData()[i] = 0;

  
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

    
    ledLine[0].dev = &device;
    ledLine[0].src = &pixels;
    ledLine[0].address = "/1";
    ledLine[0].nbPix = 264;
    ledLine[0].offset = 0;
    ledLine[0].size = 4;
    ledLine[0].Xsize = 66;
    
    ledLine[1].dev = &device3;
    ledLine[1].src = &pixels;
    ledLine[1].address = "/1";
    ledLine[1].nbPix = 264;
    ledLine[1].offset = 4;
    ledLine[1].size = 4;
    ledLine[1].Xsize = 66;

    ledLine[2].dev = &device2;
    ledLine[2].src = &pixels;
    ledLine[2].address = "/2";
    ledLine[2].nbPix = 264;
    ledLine[2].offset = 8;
    ledLine[2].size = 4;
    ledLine[2].Xsize = 66;

    ledLine[3].dev = &device;
    ledLine[3].src = &pixels;
    ledLine[3].address = "/2";
    ledLine[3].nbPix = 264;
    ledLine[3].offset = 12;
    ledLine[3].size = 4;
    ledLine[3].Xsize = 66;
    
    ledLine[4].dev = &device3;
    ledLine[4].src = &pixels;
    ledLine[4].address = "/2";
    ledLine[4].nbPix = 264;
    ledLine[4].offset = 16;
    ledLine[4].size = 4;
    ledLine[4].Xsize = 66;
    
    ledLine[5].dev = &device2;
    ledLine[5].src = &pixels;
    ledLine[5].address = "/1";
    ledLine[5].nbPix = 81;
    ledLine[5].offset = 20;
    ledLine[5].size = 2;
    ledLine[5].Xsize = 66;
    
    ledLine[6].dev = &device4;
    ledLine[6].src = &pixOrb;
    ledLine[6].address = "/1";
    ledLine[6].nbPix = 20;
    ledLine[6].offset = 0;
    ledLine[6].size = 1;
    ledLine[6].Xsize = 20;

}

//--------------------------------------------------------------
void ofApp::update(){


    // OSC STUFF:
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
      
          if(m.getAddress() == "/play"){
            ofLog() << "play" << m.getArgAsInt32(0);
            if(m.getArgAsBool(0)){trame.play(); playing = 1;}
            else if(!m.getArgAsBool(0)){trame.stop(); playing = 0; for (int i=0;i<width*height*3;i++) NetBuffer.getData()[i] = 0;}
          }
      
          else if(m.getAddress() == "/pause"){
            ofLog() << "pause" << m.getArgAsInt32(0);
            if(m.getArgAsBool(0)){trame.setPaused(1);}
            else if(!m.getArgAsBool(0)){trame.setPaused(1);}
          }
        
          else if(m.getAddress() == "/position"){
            ofLog() << "position" << m.getArgAsFloat(0);
            trame.setPosition(m.getArgAsFloat(0));
          }
        
          else if(m.getAddress() == "/speed"){
            ofLog() << "speed" << m.getArgAsFloat(0);
            trame.setSpeed(m.getArgAsFloat(0));
          }
        
          else if(m.getAddress() == "/image"){
            if (!playing){
                //NetBuffer.clear();
                NetBuffer = m.getArgAsBlob(0);
                /*
                NET = (unsigned char *)NetBuffer.getData();
                for(int i = 0;i < 64; i++) std::cerr << (int)NET[i] << " ";
                std::cerr << std::endl;
                ofLog() << "image size: " << NetBuffer.size();
                */
              }
          }
        
          else if(m.getAddress() == "/bright"){
              for (int i=0; i<6; i++){
                  setDither(i, m.getArgAsInt32(0));
              }
              //ofLog() << "d" << m.getArgAsInt32(0);
          }
        
          else if(m.getAddress() == "/orb"){
              setDither(6, m.getArgAsInt32(0));
              //ofLog() << "d" << m.getArgAsInt32(0);
          }
          
        
    }
  
    trame.update();
    
    if (trame.getIsMovieDone()){
        
    }
      
  
    // get part of the image for the LEDs
      
    if(playing){
        pixels = trame.getPixels();
        }

    else{
        pixels.setFromExternalPixels((unsigned char*)NetBuffer.getData(), 66, 22, 3);
        }
    

    //  Fond brightness
    for (int i=0; i<6; i++){
      setBrightness(i, 31);
    }

    //  Orb brightness
    setBrightness(6, 31);

    
 
    if (send){
      
      imgAsBuffer.clear();
      imgAsBuffer.append((const char*)pixels.getData(),pixels.size());
      ofxOscMessage m;
      m.setAddress("/image");
      m.addBlobArg(imgAsBuffer);
      sender.sendMessage(m);
      
    }
   
    

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



//-------------------------------------------------------------
void ofApp::draw(){
  
#ifdef __APPLE__

    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    //trame.play();
    trame.draw(20, 20, 450, 450);

  
    for (int i=0; i<6; i++) {
        ofImage img;
        img.setFromPixels(ledLine[i].pixelCrop);
        img.draw(500, ledLine[i].offset*15, 450, ledLine[i].size*10);
    }
    /*
    ofImage img;
    img.setFromPixels(ledLine[6].pixelCrop);
    img.draw(500, 600, 450, ledLine[6].size*30);
  */

    img.setFromPixels(pixels);
    img.draw(20, 20, 450, 450);
  
  
#endif


}

//-------------------------------------------------------------
void ofApp::exit(){

  playing = 0;
  for (int i=0;i<width*height*3;i++) NetBuffer.getData()[i] = 0;
  
  pixels.setFromExternalPixels((unsigned char*)NetBuffer.getData(), 66, 22, 3);
  // TODO: fill pixels with the Network Data


  // Fond dither
  for (int i=0; i<6; i++){
    setDither(i, 0);
  }
  //ofLog() << "d" <<  (int)Brights[0];
  
  //  Fond brightness
  for (int i=0; i<6; i++){
    setBrightness(i, 0);
  }
  //ofLog() << "b" << (int)Brights[3]/8;
  
  // Orb dither
  setDither(6, 0);
  //ofLog() << "dt" <<  (int)Brights[6];
  
  // orb brightness
  setBrightness(6, 0);
  //ofLog() << "bt" << (int)Brights[9]/8;

  
  for (int i=0; i<7; i++) {
    sendLine(i);
  }
  
  
}

//-------------------------------------------------------------
//-------------------------------------------------------------


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


