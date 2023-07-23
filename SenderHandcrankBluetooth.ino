
//MP3 module library
#include <SoftwareSerial.h>
#include "RedMP3.h"
//Neopixel library
#include <Adafruit_NeoPixel.h>

#define RIGHT_SIDE_PIN A4
#define LEFT_SIDE_PIN A5

#define SENDING_INTERVAL 80 //in millisecond

#define SOUND_NPX_PIN 3
#define RIGHT_NPX_PIN 4
#define LEFT_NPX_PIN 5
#define NUMPIXELS 6

#define MP3_RX 7//RX of Serial MP3 module connect to D7 of Arduino
#define MP3_TX 8//TX to D8, note that D8 can not be used as RX on Mega2560, you should modify this if you donot use Arduino UNO
MP3 mp3(MP3_RX, MP3_TX);

#define DEBUG 0

double rightForwardPixels;
double leftForwardPixels;
SoftwareSerial mySerial(10, 11);
char text[32];
String command_;

long currentTime;
double iteration;

double averageReadingRight;
double averageReadingLeft;
double averageReadingGrabber;
double chargeRight;
double chargeLeft;

double chargeMeter;


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, SOUND_NPX_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightChargePixels = Adafruit_NeoPixel(NUMPIXELS, RIGHT_NPX_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftChargePixels = Adafruit_NeoPixel(NUMPIXELS, LEFT_NPX_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  delay(500);
  //Serial begin
  Serial.begin(9600);
  //RF Setup
  mySerial.begin(57600);

  command_="@A1XXXXX!";
  command_.toCharArray(text,32);
  //Polling setup
  currentTime=millis();
  iteration = 1;
  averageReadingRight=0;
  averageReadingLeft=0;
  averageReadingGrabber=0;
  //MP3 setup
  mp3.allCycle();
  delay(50);
//  mp3.nextSong();
//Setup CHARGE METER
  chargeLeft = 500;
  chargeRight = 500;
  chargeMeter = 500;
//Setup neopixel
  pixels.begin();
  rightChargePixels.begin();
  leftChargePixels.begin();
}

void loop() {
  if(millis()>currentTime+SENDING_INTERVAL){
    text[0]='@';
    text[1]='A';
    text[2]='1';
    text[6]='X';//Grabber open and close
    text[7]='X';//IR Emitter on/off
    text[8]='!';

// RIGHT WHEEL READING
//    averageReadingRight = 500;//Guard for if RIGHT WHEEL IS NOT ALR ATTACHED
    chargeRight = chargeRight + map((int)averageReadingRight, 0, 1023, -1200, 1200);
    if(chargeRight>550)
      chargeRight = chargeRight - map(chargeRight, 500, 1023, 80, 350);//So chargheRight tend to go to 500
    if(chargeRight<450)
      chargeRight = chargeRight + map(chargeRight, 500, 0, 80, 350);//So chargheRight tend to go to 500

    if(chargeRight>1023)
      chargeRight = 1023;//GUARD so it is bounded
    if(chargeRight<0)
      chargeRight = 0;

    text[3] = map((int)chargeRight, 0, 1023, -127, 127);//THE THING THAT IS SENT

    //GUARD SO THE VALUE DOESN'T COME AS WEIRD
    if(text[3]==0||text[3]==9||text[3]==10||text[3]==13||text[3]==32){
      text[3] = 1;
    }
    if(text[3]==127){
      text[3] = 126;
    }
//    Serial.println((int)text[3]);

    // Serial.print("Charge Right: ");
    // Serial.println(chargeRight);

    Serial.print("Average Reading Right: ");
    Serial.print(averageReadingRight);
    Serial.print(" ");

    //Right Charge Pixels Indicator
      //forward
    if(chargeRight>500){
      rightForwardPixels = (int)map(chargeRight, 500, 1023, 0, NUMPIXELS);
      for(int i=0;i<rightForwardPixels;i++){rightChargePixels.setPixelColor(i, rightChargePixels.Color(25,0,0));}//GREEN
      for(int i=rightForwardPixels;i<NUMPIXELS;i++){rightChargePixels.setPixelColor(i, rightChargePixels.Color(0, 0, 0)); }//BLACK
    }
      //backward
    if(chargeRight<500){
      rightForwardPixels = (int)map(chargeRight, 500, 0, 0, NUMPIXELS);
      for(int i=0;i<rightForwardPixels;i++){rightChargePixels.setPixelColor(i, rightChargePixels.Color(0,25,0));}//RED
      for(int i=rightForwardPixels;i<NUMPIXELS;i++){rightChargePixels.setPixelColor(i, rightChargePixels.Color(0, 0, 0)); }//BLACK
    }
    rightChargePixels.show();

// LEFT WHEEL READING
//    averageReadingLeft = 500; //Guard for if LEFT WHEEL IS NOT ALR ATTACHED
    chargeLeft = chargeLeft + map((int)averageReadingLeft, 0, 1023, -1200, 1200);
    if(chargeLeft>550){chargeLeft = chargeLeft - map(chargeLeft, 500, 1023, 80, 350);}
    if(chargeLeft<450){chargeLeft = chargeLeft + map(chargeLeft, 500, 0, 80, 350);}
    if(chargeLeft>1023){chargeLeft = 1023;}//GUARD
    if(chargeLeft<0){chargeLeft = 0;}

    text[4]= map((int)chargeLeft, 0, 1023, -127, 127);//THE THING THAT IS SENT
    //GUARD SO THE VALUE DOESN'T COME AS WEIRD
    if(text[4]==0||text[4]==9||text[4]==10||text[4]==13||text[4]==32){
      text[4] = 1;
    }
    if(text[4]==127){
      text[4] = 126;
    }
//    Serial.println((int)text[4]);
    //Left Charge Pixels Indicator
      //forward
    if(chargeLeft>500){
      leftForwardPixels = (int)map(chargeLeft, 500, 1023, 0, NUMPIXELS);
      for(int i=0;i<leftForwardPixels;i++){leftChargePixels.setPixelColor(i, leftChargePixels.Color(30,0,0));} //GREEN
      for(int i=leftForwardPixels;i<NUMPIXELS;i++){leftChargePixels.setPixelColor(i, leftChargePixels.Color(0, 0, 0));} //BLACK
    }
      //backward
    if(chargeLeft<500){
      leftForwardPixels = (int)map(chargeLeft, 500, 0, 0, NUMPIXELS);
      for(int i=0;i<leftForwardPixels;i++){leftChargePixels.setPixelColor(i, leftChargePixels.Color(0,25,0));}//RED
      for(int i=leftForwardPixels;i<NUMPIXELS;i++){leftChargePixels.setPixelColor(i, pixels.Color(0, 0, 0));}//BLACK
    }
    leftChargePixels.show();
    //
    // Serial.print("Charge Left: ");
    // Serial.println(chargeLeft);
    Serial.print("Average Reading Left: ");
    Serial.println(averageReadingLeft);
    // Serial.print("things sent: ");Serial.print(text);
    // Serial.print("text[3] in int: ");Serial.print((int)text[3]);
    // Serial.print("text[4] in int: ");Serial.println((int)text[4]);
    //Sending data
    for(int i=0; i<9; i++){
      mySerial.write(text[i]);
    }
//    Serial.println("Sending things");
    //Formula for MP3 Volume & Neopixel
    chargeMeter = chargeLeft + chargeRight;
    if(chargeMeter<0){chargeMeter = 0;}
    if(chargeMeter>2046){chargeMeter = 2046;}
    mp3.setVolume((int8_t)map((int)chargeMeter, 0, 2046, 0, 15));
//    Serial.print("averageReadingLeft: ");
//    Serial.println((int)averageReadingLeft);
    for(int i=0;i<(int)map(chargeMeter, 0, 1000, 0, NUMPIXELS);i++){
      pixels.setPixelColor(i, pixels.Color(0,5,0)); // GREEN
      pixels.show();
    }
    for(int i=(int)map(chargeMeter, 0, 1000, 0, NUMPIXELS);i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // BLACK
      pixels.show();
    }
//    Serial.println("chargeMeter: ");
//    Serial.println(chargeMeter);
    //Cleanup for the next averaging
    iteration = 1;
    currentTime=millis();
//    Serial.println(text);
    averageReadingRight=0;
    averageReadingLeft=0;
    averageReadingGrabber=0;
  }
  //Averaging things
  averageReadingRight = (averageReadingRight*(iteration-1)/iteration)+(double)((analogRead(RIGHT_SIDE_PIN))/iteration);
  averageReadingLeft = (averageReadingLeft*(iteration-1)/iteration)+(double)((analogRead(LEFT_SIDE_PIN))/iteration);
  iteration = iteration+1;
}
