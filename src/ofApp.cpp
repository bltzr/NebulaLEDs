#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowTitle("Interlude");

    receiver.setup(PORTIN);
    ofLog() << "Opened OSC Receiver";
  
    // display
    ofBackground(0,0,0);
    ofSetVerticalSync(true);
    ofSetFrameRate(30);

    dir.open("/data");
 
 	dir.allowExt("mov");
 	dir.listDir();
 	ofLog() << "dir size" << dir.size() ;

//go through and print out all the paths
for(int i = 0; i < dir.size(); i++){
    ofLogNotice(dir.getPath(i));
}

    // Video player
    trame.setPixelFormat(OF_PIXELS_RGB);
    //trame.load("bright.mov");
    trame.load("/data/190211_XP-White-01-Pi.mov");//"/data/Interlude.mov");//(
    ofLog() << "Loaded Mov";
    trame.setLoopState(OF_LOOP_NONE);
    trame.setVolume(1.);
    pixels.allocate(width, height, 3);
    if (playing) {trame.play();}
  
    // Orb
    pixOrb.allocate(OrbSize, 1, 3);
    makeOrb();
    
    // Network input
    NetBuffer.allocate(width*height*3);
    for (int i=0;i<width*height*3;i++) NetBuffer.getData()[i] = 0;
  
    
    // setup Serial
    
    ofLogNotice("ofApp::setup") << "Connected Devices: ";
    for (auto d : devicesInfo) ofLogNotice("ofApp::setup") << d;
    
    device.name = portName(1365390);//3972520); // "/dev/cu.usbmodem1369841";
    device2.name = portName(1369840);//3972260); //"/dev/cu.usbmodem1455771";
    device3.name = portName(1455770);//3902210); //"/dev/cu.usbmodem1383111";
    device4.name = portName(1383110);//3972320); //"/dev/cu.usbmodem1365391";
  

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
    ledLine[0].Xsize = width;
    
    ledLine[1].dev = &device3;
    ledLine[1].src = &pixels;
    ledLine[1].address = "/1";
    ledLine[1].nbPix = 264;
    ledLine[1].offset = 4;
    ledLine[1].size = 4;
    ledLine[1].Xsize = width;

    ledLine[2].dev = &device2;
    ledLine[2].src = &pixels;
    ledLine[2].address = "/2";
    ledLine[2].nbPix = 264;
    ledLine[2].offset = 8;
    ledLine[2].size = 4;
    ledLine[2].Xsize = width;

    ledLine[3].dev = &device;
    ledLine[3].src = &pixels;
    ledLine[3].address = "/2";
    ledLine[3].nbPix = 264;
    ledLine[3].offset = 12;
    ledLine[3].size = 4;
    ledLine[3].Xsize = width;
    
    ledLine[4].dev = &device3;
    ledLine[4].src = &pixels;
    ledLine[4].address = "/2";
    ledLine[4].nbPix = 264;
    ledLine[4].offset = 16;
    ledLine[4].size = 4;
    ledLine[4].Xsize = width;
    
    ledLine[5].dev = &device2;
    ledLine[5].src = &pixels;
    ledLine[5].address = "/1";
    ledLine[5].nbPix = 81;
    ledLine[5].offset = 20;
    ledLine[5].size = 2;
    ledLine[5].Xsize = width;
    
    ledLine[6].dev = &device4;
    ledLine[6].src = &pixOrb;
    ledLine[6].address = "/1";
    ledLine[6].nbPix = OrbSize;
    ledLine[6].offset = 0;
    ledLine[6].size = 1;
    ledLine[6].Xsize = OrbSize;
    
    for (int i=0; i<7; i++) {
        ledLine[i].setup();
    }

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
          if(m.getArgAsBool(0)){ 
          	trame.load(dir.getPath(ofRandom(dir.size())));
          	trame.setLoopState(OF_LOOP_NONE);
          	playing = 1; 
          	trame.play();
          }
          else if(!m.getArgAsBool(0)){
              playing = 0;
              trame.stop();
              for (int i=0;i<width*height*3;i++) NetBuffer.getData()[i] = 0;
          }
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
                NetBuffer = m.getArgAsBlob(0);
            }
        }
        
        else if(m.getAddress() == "/wallLum"){
            wallLum = m.getArgAsFloat(0);
            setWallLum();
        }
        
        
        else if(m.getAddress() == "/orbPeriod"){
            orbInc = fps / (m.getArgAsFloat(0) * 500.) ;
        }
        
        else if(m.getAddress() == "/orbColor"){
            orbColor.r = m.getArgAsInt32(0);
            orbColor.g = m.getArgAsInt32(1);
            orbColor.b = m.getArgAsInt32(2);
            makeOrb();
        }
        
        else if(m.getAddress() == "/sensor"){
            sensorValue = m.getArgAsInt32(0);
        }
        
    }
  
    //testSensor();
    
    if(playing){
        if (waiting){
            orbBreathe();
            sendLine(6);
        } else {
            trame.update();
            pixels = trame.getPixels();
            for (int i=0; i<6; i++) {
                sendLine(i);
            }
            
        }
    } else {
        pixels.setFromExternalPixels((unsigned char*)NetBuffer.getData(), 66, 22, 3);
        for (int i=0; i<6; i++) {
            sendLine(i);
        }
    }
}

void ofApp::testSensor(){
    if (sensorValue >= 8 && waiting) {
        trame.play();
        waiting=false;
        wallLum=1.;
        setWallLum();
    } else if ( sensorValue < 8 && !waiting ){
        trame.stop();
        waiting=true;
        wallLum=0.;
        setWallLum();
    }
}

void ofApp::orbBreathe(){
    orbLum+=orbDir*orbInc;
    if (orbLum<=orbMin) {orbLum=orbMin; orbDir=1.;}
    else if (orbLum>=orbMax) {orbLum=orbMax; orbDir=-1.;}
    size_t lum = size_t(orbLum * 1041.);
    setBrightness(6, bright[lum]);
    setDither(6, dither[lum]);
    //ofLogNotice("orb luminosity: ") << orbLum << " -> " << lum;
    //ofLogNotice("Orb - bright / dither: ") << int(bright[lum]) << " / " << int(dither[lum]) ;
}


void ofApp::makeOrb(){
    for (int i=0; i<OrbSize; ++i){
        pixOrb[i*3]  =orbColor.r;
        pixOrb[i*3+1]=orbColor.g;
        pixOrb[i*3+2]=orbColor.b;
    }
}

void ofApp::setWallLum(){
    //  Wall brightness
    int lum = int(wallLum * 1041.);
    uint8_t br = bright[lum];
    uint8_t di = dither[lum];
    //ofLogNotice("Wall luminosity: ") << wallLum << " -> " << lum;
    //ofLogNotice("Wall - bright / dither: ") << int(bright[lum]) << " / " << int(dither[lum]) ;
    for (int i=0; i<6; i++){
        setBrightness(i, br);
    }
    for (int i=0; i<6; i++){
        setDither(i, di);
    }
}

void ofApp::setBrightness(int i, uint8_t brightness) {

    // check for waiting messages
    
        LedLine line = ledLine[i];
        ofxOscMessage n;
        n.setAddress(line.address);
        n.addIntArg(int(brightness));
        
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

void ofApp::setDither(int i, uint8_t dither) {
  
        // check for waiting messages
        
        LedLine line = ledLine[i];
        ofxOscMessage n;
        n.setAddress("/b");
        n.addIntArg(int(dither));
        
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
    //pixels.draw(20, 20, 450, 450);

  
    for (int i=0; i<6; i++) {
        ofImage img;
        img.setFromPixels(ledLine[i].pixelCrop);
        img.draw(500, ledLine[i].offset*15, 450, ledLine[i].size*10);
    }
    
    ofImage imgOrb;
    imgOrb.setFromPixels(ledLine[6].pixelCrop);
    imgOrb.draw(500, 400, 450, ledLine[6].size*30);

    ofImage imgWall;
    imgWall.setFromPixels(pixels);
    imgWall.draw(20, 20, 450, 450);
  
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

std::string ofApp::portName(int SN)
{
    for (const auto& devInfo : devicesInfo){
        if ((devInfo.getHardwareId().find((std::to_string(SN))))<60){
            cout << "found: " << devInfo.getPort() << endl;
            return devInfo.getPort();
        }
    }
    return "";
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


