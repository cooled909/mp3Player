
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int buttonApin = 4;
int buttonBpin = 3;
int buttonCpin = 2;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));  
  }
  Serial.println(F("DFPlayer Mini online."));

  
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);
  pinMode(buttonCpin, INPUT_PULLUP);

  myDFPlayer.volume(10);
  myDFPlayer.play(1);
}

enum BT_States {BT_Start, BT_Wait, BT_Play, BT_Hold, BT_Pause, BT_BackSkip, BT_NextSkip} BT_State;

void TickFct_Button(){
  static bool pausePlay = true;
  switch(BT_State){ //transitions
    case BT_Start:
      BT_State = BT_Wait;
      break;
    case BT_Wait:
      if(pausePlay && digitalRead(buttonApin) == LOW){
        BT_State = BT_Pause;
      }
      else if(!pausePlay&& digitalRead(buttonApin) == LOW){
        BT_State = BT_Play;
      }
      else if (digitalRead(buttonCpin) == LOW){
        BT_State = BT_NextSkip;
      }
      else if (digitalRead(buttonBpin) == LOW){
        BT_State = BT_BackSkip;
      }
      else{
        BT_State = BT_Wait;
      }
      break;
    case BT_Play:
      BT_State = BT_Hold;
      break;
    case BT_Pause:
      BT_State = BT_Hold;
      break;
    case BT_Hold:
      if(digitalRead(buttonApin) == HIGH && digitalRead(buttonCpin) == HIGH && digitalRead(buttonBpin)){
        BT_State = BT_Wait;
      }
      else if(digitalRead(buttonApin) == LOW || digitalRead(buttonCpin) == LOW || digitalRead(buttonBpin)){
        BT_State = BT_Hold;
      }
      break;
    case BT_NextSkip:
      BT_State = BT_Hold;
      break;
    case BT_BackSkip:
      BT_State = BT_Hold;
      break;
    default:
      BT_State = BT_Start;
      break;
  }
  switch(BT_State){  //actions
    case BT_Wait:
      break;
    case BT_Play:
      pausePlay = 1;
      myDFPlayer.start();
      break;
    case BT_Pause:
      pausePlay = 0;
      myDFPlayer.pause();
      break;
    case BT_NextSkip:
      myDFPlayer.next();
      break;
    case BT_BackSkip:
      myDFPlayer.previous();
      break;
    case BT_Hold:
      break;
    default:
      break;
  }
}

void loop()
{
  int pausePlay = 1;
  TickFct_Button();
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
