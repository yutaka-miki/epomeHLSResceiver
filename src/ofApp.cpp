// Paste from Template (Top)-------------------------------------
#include "ofApp.h"

ofTrueTypeFont font;

int Width = 800;
int Height = 480;

int pgM21 = 0;
int  pReset = 0;

float deltaTime1 = 0.0;
float deltaTime2 = 0.0;

float pos1 = 0.0;
float pos2 = 0.0;
float posSpeed = 10.0;
// int PosTotal = 18;
int PosTotal = 16;
int mid = (PosTotal-2)/2;

int MorS;
int C_pos;

int Check_Angle = 0;
int CheckSpeed = 0;

float t = 0.0;
bool text = 0;
int StartT;
// Paste from Template (bottom) ----------------------------------

//i2c(Top)-----------------------------------------------------
/*
int i_i2c;
int handle;//master
int result;//master
bsc_xfer_t xfer; //slave
int status;//slave
char a ;
*/
//i2c(Bottom)--------------------------------------------------

// string videoPath;
ofxOMXPlayerSettings settings;  //essential
int W = 4;
int H = 4;
bool Check_Movie = 0;

//--------------------------------------------------------------
void ofApp::setup(){

    //大きすぎるデータの読み込みは動作に影響を与えるので注意！
    // Paste from Template (Top)-------------------------------------
    MyClass m;

    font.load("RictyDiminished-Regular.ttf",12);
    ofSetVerticalSync(false);
    // ofSetFrameRate(60);
    ofSetWindowShape(Width,Height);
    ofHideCursor();
    
    gpioInitialise();
    gpioSetMode(21,PI_INPUT);
    gpioSetMode(20,PI_OUTPUT);//master
    gpioSetMode(16,PI_INPUT);//slave

    MorS =  ofToInt(ofBufferFromFile("master_or_slave.txt").getText());
    if(MorS == 0){
        C_pos = 0;
    }
    if(MorS == 1){
        C_pos = PosTotal/2;
    }
    // Paste from Template (bottom) ----------------------------------

    //i2c(Top)-----------------------------------------------------
    /*
    handle = i2cOpen(1,0x04,0);//master
    if(handle<0) {std::cout << "can not open i2c" << endl; }
    xfer.control = (0x04<<16) | 0x305 ;
    */
    //i2c(Bottom)--------------------------------------------------

    //ofRPitouch(Top)----------------------------------------------
    touch.init("/dev/input/event0");    // change according to your setup (evdev)
    ofLog()<<touch.getName();
    //ofRPitouch(Bottom)-------------------------------------------

    // string v0= "https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8";
	// string v1 = "http://192.168.1.220/hlsStream/prog_index.m3u8";
    // string videoPath = v1;
    string videoPath = ofBufferFromFile("address.txt").getText();

    settings.videoPath = videoPath; //essential
	settings.useHDMIForAudio = false;	//default true
	settings.enableTexture = true;		//default true
	// settings.enableLooping = true;		//default true
	settings.enableAudio = false;		//default true, save resources by disabling
	// settings.doFlipTexture = true;		//default false

	std::cout << "setup" << endl; 
	omxPlayer.setup(settings);
    std::cout << "loadMovie" << endl; 
	omxPlayer.loadMovie(videoPath);

}

//--------------------------------------------------------------
void ofApp::update(){

     // Paste from Template (Top)-------------------------------------

    //ofRPitouch(Top)----------------------------------------------
    if(touch.getButton() == 1){ ofExit(); }
    //ofRPitouch(Bottom)-------------------------------------------

    //i2c(Top)-----------------------------------------------------
    /*
    i_i2c++;
    if(i_i2c >= 3){
        i_i2c = 0;
        //送信量を増やすと動かなくなることがあるので注意！
        
        if(MorS == 0){
            char buf[2] = {(char)CheckSpeed,(char)Check_Angle }; 
            result = i2cWriteI2CBlockData(handle,0x00,buf,2);
            if( result < 0 ) { std::cout << "can not send Data" << endl; }
        }
        if(MorS == 1){
            status = bscXfer(&xfer);
            if(xfer.rxCnt > 0){
                a = xfer.rxBuf[1];
                CheckSpeed = (unsigned char)a;
                a = xfer.rxBuf[2];
                Check_Angle = (unsigned char)a;
            }
        }
    }
    */
    //i2c(Bottom)--------------------------------------------------

    if(ofGetFrameRate() != 0){t += 1 / ofGetFrameRate(); }
    int StartT = 30;
    if(t >= StartT ){ t = 0; }

     if(gpioRead(16) == 1 ||  (int)t ==  0){
        if( pReset == 0 ){
            t = 0;
            gpioWrite(20,1);//master
        }
        pReset = 1; //slave
    }else{
        gpioWrite(20,0); //master
        pReset =  0;   
    }//if(gpioRead(16) == 1 ||  (int)t ==  0)

    if(ofGetFrameRate() != 0){ deltaTime1= deltaTime1+ 1/ofGetFrameRate(); }

    if(gpioRead(21) == 1 ){
        if(pgM21 == 0 ){
            pos1 = C_pos ;
            pos2 = pos1;

            if(deltaTime1 != 0){
              posSpeed  =  PosTotal / (2 * deltaTime1) ;
            //   posSpeed  = 1;
            }else{
               posSpeed = 1;
            }
            deltaTime1 = 0;
        }
        pgM21 = 1;
    }else{
        if(pgM21 == 1 ){

            pos1 = pos1 + PosTotal/2  ;
            if(pos1 <  0 || PosTotal <=  pos1){  pos1 = float(fmod(pos1,PosTotal)); }
            pos2 = pos1;

            if(deltaTime1 != 0){
                posSpeed  =  PosTotal / (2 * deltaTime1) ;
            }else{
               posSpeed = 1;
            }
            deltaTime1 = 0;

        }// if(pgM21 == 0 )
       pgM21 = 0;

    } //if(gpioRead(21) == 1 )

    if(ofGetFrameRate() != 0){ deltaTime2 = deltaTime2 + 1 / ofGetFrameRate(); }

    pos2 = pos2 +posSpeed *deltaTime2  ;
    if(pos2 <  0 || PosTotal <=  pos2){  pos2 = float(fmod(pos2,PosTotal)); }
    deltaTime2 = 0;

 // Paste from Template (bottom) ----------------------------------
}

//--------------------------------------------------------------
void ofApp::draw(){

    // if(0 <= pos && pos < PosTotal){
    if(pos2 < H){
            omxPlayer.draw(0, Height*(int(pos2)%H-H+1), Width*W , Height*H);
    }else if(H <= pos2 && pos2 < 2*H){
            omxPlayer.draw(-Width ,Height*(int(pos2)%H-H+1), Width*W, Height*H);
    }else if(2*H <= pos2 && pos2 < 3*H){
            omxPlayer.draw(-2*Width, Height*(int(pos2)%H-H+1), Width*W, Height*H);
    }else if(3*H <= pos2 && pos2 < 4*H){
            omxPlayer.draw(-3*Width, Height*(int(pos2)%H-H+1), Width*W, Height*H);
    }
    // }
    //std::cout << "pos2 : " <<  pos2 << endl; 

    if(Check_Movie == true){ omxPlayer.draw(0,0,Width,Height); }

    // omxPlayer.draw(0, 0, Width*W, Height*H);
    // ofDrawBitmapStringHighlight(omxPlayer.getInfo(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);

 // Paste from Template (Top)-------------------------------------
    // ofBackground(0);

    if(Check_Angle== 100){
        float x;
        float X = Width/(PosTotal-1) ;
        ofBackground(0);
        if(int(pos2) == 0){
            x = int(pos2)+ mid ;
            ofSetColor(255);
        }
        if(1 <= int(pos2) && int(pos2) <= mid){
            x = int(pos2)+ mid ;
            ofSetColor(0,0,255);
        }
        if(mid+1 <= int(pos2) && int(pos2) <= PosTotal-1){
            x = int(pos2)+mid-PosTotal;
            ofSetColor(255,0,0);
        }
        ofRect(X*x,0,X,Height);
        ofSetColor(255);
    }// if(Check_Angle== true)

    if(CheckSpeed== 100){
        ofBackground(0);
        ofSetColor(255);
        if(posSpeed > 65.0 ){ ofSetColor(255,0,0); }
        if(posSpeed < 55.0 ){ ofSetColor(0,0,255); }
        ofRect(Width-(200*posSpeed/PosTotal),0,Width,Height);
        ofSetColor(255);
    }

    if(text == 1){
        ofBackground(0);
        ofSetColor(255);
        font.drawString("Framerate: " + ofToString(ofGetFrameRate()) , 10, 20);
        font.drawString("t : " + ofToString(t) , 10, 40);
        font.drawString("pos2: " + ofToString(pos2) , 10, 80);
        font.drawString("C_pos: " + ofToString(C_pos) , 10, 100);
        // font.drawString(ofToString(touch.getButton()), 10, 280);
    }

 // Paste from Template (bottom) ----------------------------------

}

void ofApp::keyPressed(int key){

 // Paste from Template (Top)-------------------------------------
    if (key == 't'){ 
        if(text == 0){
            text = 1;
        }else{
            text = 0;
        }
    }

     if (key == 'c'){ 
        if(Check_Angle == 0){
            Check_Angle = 100;
        }else{
            Check_Angle = 0;
        }   
    }

     if (key == 'v'){ 
        if(CheckSpeed == 0){
            CheckSpeed = 100; 
        }else{
            CheckSpeed = 0; 
        }
    }

    if (key == 'b'){ 
        if(Check_Movie == 0){
            Check_Movie = 1;
        }else{
            Check_Movie= 0;
        }   
    }

    if (key == 'z'){ 
        if( C_pos == 0){
            C_pos = PosTotal/2; 
        }else{
            C_pos = 0; 
        }
    }

    if (key == 'r'){
        // omxPlayer.restartMovie();
        // omxPlayer.seekToFrame(0);
        // omxPlayer.stepFrameForward();
        // omxPlayer.togglePause();
    }
 // Paste from Template (bottom) ----------------------------------
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
