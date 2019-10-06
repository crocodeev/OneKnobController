/*
  OneKnobController

*/

#include <MIDI.h>

#define BTN  2  //button
#define SW   3  //switch
#define POT  14 //analog potentiometr
#define SENS 15 //photoresistor sensor


bool swState;
bool lastBtnState = false;
bool btnMode = false;

long previousMillis1 = 0;
long modeActivatedTime = 3000;

byte octava = 36;
byte previousNoteNumber = octava;

//create MIDI object called midiOut
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  //delay for wait input
  delay(1000);
  //setup serial for MIDI
  Serial.begin(31250);
  //Serial.begin(19200);
  //set pin mode 
  pinMode(BTN,  INPUT);
  pinMode(SW,   INPUT);
  pinMode(POT,  INPUT);
  pinMode(SENS, INPUT);

 //change mode - switch "on" for button mode
 //for activate button mode 1. before begin set switch on 2. press btn for 3 seconds
  if(digitalRead(BTN)){
     delay(modeActivatedTime);
     if(millis() >= modeActivatedTime && digitalRead(BTN)){
        if (digitalRead(SW)){
            btnMode = true;
            Serial.println("btn mode set to true");
          }
      }
    } 
}

void loop() {
 
   //readind sensor or pot
   swState = digitalRead(SW);
   byte input;
   if(swState){
    input = POT;
    }else{
    input = SENS;  
    }
    
   byte noteNumber = chooseNote(input, octava);
   sendToMIDI(noteNumber, btnMode); 
  
    }  

//for quick test
   /* MIDI.sendNoteOn(42, 127, 1);
    delay(1000);                
    MIDI.sendNoteOff(42, 0, 1);*/

byte chooseNote( byte pin, byte octava){
  int  val = analogRead(pin);
  byte number = map(val,0,1023,0,11) + octava;
  return number;
  }

void sendToMIDI(byte noteNumber, bool btnMode){

if(btnMode){
 
    bool btnState = digitalRead(BTN);//нужна обработка предыдущего параметра ноты
if(digitalRead(BTN) != lastBtnState){
      if(digitalRead(BTN)){   
    MIDI.sendNoteOn(noteNumber, 127, 1); 
    //Serial.println("Send note on");
    //Serial.println(noteNumber);      
        }else{
    MIDI.sendNoteOff(previousNoteNumber,0,1);
    //Serial.println("Send note off");
    //Serial.println(previousNoteNumber);
        }
    previousNoteNumber =  noteNumber;    
    lastBtnState = btnState;    
  }
}else{
   
if (noteNumber != previousNoteNumber){
    MIDI.sendNoteOff(previousNoteNumber,0,1);
    //Serial.println("Send note off");
    //Serial.println(previousNoteNumber);
    MIDI.sendNoteOn(noteNumber, 127, 1); 
    //Serial.println("Send note on");
    //Serial.println(noteNumber);
    previousNoteNumber = noteNumber;
    }
}
  }  
