//Library for Bluetooth
#include <SoftwareSerial.h>

#include "IRLibAll.h"

//Library for Neopixel
#include <Adafruit_NeoPixel.h>

//Vulnerable Mode and Invincible Mode Pin
#define PSEUDO_5V 3
#define NPX_PIN  15
//Neopixel
#define NUMPIXELS 10

//TIME INTERVAL
#define STACK_CHECKUP_INTERVAL 1000

const byte address[6] = "00001";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NPX_PIN, NEO_GRB + NEO_KHZ800);

int slowStack = 0;
int iteration_bluetooth;
unsigned long lastSignal, lastStackCheck, lastIRReading, invincibleModeStart;
bool switch_, emitterFlag=0, invincibleMode=0;
char buffer_[32];
unsigned char text;
float averageLDRReading, iteration;
int ldrReading;

SoftwareSerial mySerial(10, 11);

//Setup IR
IRrecvPCI myReceiver(2);
IRdecode myDecoder;

//Setup Motor
int EN1 = 6;
int EN2 = 5;  //Roboduino Motor shield uses Pin 9
int IN1 = 7;
int IN2 = 4; //Latest version use pin 4 instead of pin 8


void Motor1(int pwm, boolean reverse) {
  analogWrite(EN1, pwm); //set pwm control, 0 for stop, and 255 for maximum speed
  if (reverse)  {digitalWrite(IN1, HIGH);}
  else  {digitalWrite(IN1, LOW);}
}

void Motor2(int pwm, boolean reverse) {
  analogWrite(EN2, pwm);
  if (reverse)  {digitalWrite(IN2, HIGH);}
  else  {digitalWrite(IN2, LOW);}
}

void setup() {
  //Serial begin
  Serial.begin(9600);
  //Setup RF
  mySerial.begin(57600);
  //Setup neopixel
  pixels.begin();

  for (int i = 4; i <= 7; i++) pinMode(i, OUTPUT);//pin for motor

  //Setup Pseudo 5v for IR receiver
  pinMode(PSEUDO_5V, OUTPUT);
  digitalWrite(PSEUDO_5V, HIGH);
  //
  //  //Start Test Code
  //  Motor1(255,0);
  //  Motor2(0, 0);
  //  delay(500);
  //  Motor1(0,1);
  //  Motor2(255, 0);
  //  delay(500);
  //  Motor1(0, 1);
  //  Motor2(255, 0);
  //  delay(500);
  //  Motor1(0,1);
  //  Motor2(255, 1);
  //  delay(500);
  //  Motor1(0, 0);
  //  Motor2(0, 0);

}

void testMotor(){
   Motor1(255,0);
   Motor2(0, 0);
   delay(500);
   Motor1(0,1);
   Motor2(255, 0);
   delay(500);
   Motor1(0, 1);
   Motor2(255, 0);
   delay(500);
   Motor1(0,1);
   Motor2(255, 1);
   delay(500);
   Motor1(0, 0);
   Motor2(0, 0);
}

void loop() {
  //Clearing buffer to read
  //  Serial.print("time: ");
  //  Serial.println(millis());

  //Listening RF
  if(mySerial.available()>0){
    delay(20);
    iteration_bluetooth = 0;
    while(mySerial.available()>0&&iteration_bluetooth<32){
      buffer_[iteration_bluetooth] = mySerial.read();
      iteration_bluetooth++;
    }
    Serial.println(buffer_);
  }
  //Check whether the first char is '@'
  if(buffer_[0] != '@') {
    strcpy(buffer_, "");
  }
  if(buffer_[0] == '@') {
    if(strlen(buffer_) >= 9){
      //A timeout of
      if(buffer_[8] == '!') {
        lastSignal = millis();
        //RIGHT SIDE
        if(buffer_[3]>30) {
          //motor1_value = (int)map(buffer_[3], 0, 127, 0, 255)
          Motor1(((int)map(buffer_[3], 0, 127, 0, 255)), false);
          Serial.println(map(buffer_[3], 0, 127, 0, 255));
        }
        else if(buffer_[3] < -30) {
          Motor1(((int)map(buffer_[3], 0, -127, 0, 255)), true);
          Serial.println(map(buffer_[3], 0, -127, 0, 255));
        }
        else{
          Motor1(0, false);
        }
        //LEFT SIDE
        if(buffer_[4]> 30) {
          Motor2(((int)map(buffer_[4], 0, 127, 0, 255)), false);
          Serial.println(map(buffer_[4], 0, 127, 0, 255));
        }
        else if(buffer_[4] < -30) {
          Motor2(((int)map(buffer_[4], 0, -127, 0, 255)), true);
          Serial.println(map(buffer_[4], 0, -127, 0, 255));
        }
        else{
          Motor2(0, false);
        }
        switch_ = 0;
      }
    }
  }
  //A timeout, if no correct signal is received for one second the motor will stop

  if(millis()>lastIRReading+100){
    myReceiver.enableIRIn();      //Restart receiver
    lastIRReading = millis();
    if (myReceiver.getResults()&&invincibleMode == 0) {
      Serial.println("pewpew");
      myDecoder.decode();           //Decode it
      myDecoder.dumpResults(false);  //Now print results. Use false for less detail
      myReceiver.enableIRIn();      //Restart receiver
      Motor1(0, false);
      Motor2(0, false);
      delay(1000);
      invincibleModeStart = millis();
      invincibleMode = 1;
    }
  }
  //if got hit with IR, will stop for 1 second, after that it will enter invincible mode for 2 seconds, which means cannot be shot by IR for that duration
  if(millis()>invincibleModeStart + 2000){invincibleMode = 0;}
  //
  if(millis () > (lastSignal + 1000)){
    Motor1(0, false);
    Motor2(0, false);
  }
}
