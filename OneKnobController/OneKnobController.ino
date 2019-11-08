/*
  OneKnobController

*/

#include <MIDI.h>

#define BTN  2  //button
#define SW_INPUT   3  //input switch
#define SW_MODE 4 // mode switch 
#define POT  14 //analog potentiometr
#define SENS 15 //photoresistor sensor
//7-segment below 
#define BIT_CLOCK_PIN 8
#define DIGIT_CLOCK_PIN 9
#define DATA_PIN 10


bool swState;
bool lastBtnState = false;

long previousMillis = 0;

byte octava = 36;
byte previousNoteNumber = octava;

//symbols array

const byte notes_patterns[12] = {

    B00111001,  // C
    B10111001,  // C#
    B01011110,  // d
    B11011110,  // d#
    B01111001,  // E
    B01110001,  // F
    B11110001,  // F#
    B01111101,  // G
    B11111101,  // G#
    B01110111,  // A
    B11110111,  // A#
    B01110110,  // H
  };

//create MIDI object called midiOut
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  //setup serial for MIDI
  Serial.begin(31250);
  //set pin mode 
  pinMode(BTN,      INPUT);
  pinMode(SW_INPUT, INPUT);
  pinMode(SW_MODE, INPUT);
  pinMode(POT,      INPUT);
  pinMode(SENS,     INPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(BIT_CLOCK_PIN, OUTPUT);
  pinMode(DIGIT_CLOCK_PIN, OUTPUT); 

 //change mode - switch "on" for button mode
 //for activate button mode 1. before begin set switch on 2. press btn for 3 seconds

}

void loop() {
 
  //reading sensor or pot
  
   swState = digitalRead(SW_INPUT);
   byte input;
   if(swState){
    input = POT;
    }else{
    input = SENS;  
    }

  //reading mode

  bool btnMode = digitalRead(SW_MODE); // HIGH button mode

  //taking note from input
    
  byte noteNumber = chooseNote(input);

  //show note

  displayNote(noteNumber);

  //midi work
  //поправка на октаву 
  sendToMIDI(noteNumber + octava, btnMode);
  //debounce
  delay(5);
    }  

//functions

byte chooseNote(byte pin){
  int  val = analogRead(pin);
  byte number = map(val,0,1024,0,12);
  return number;
  }


void sendToMIDI(byte noteNumber, bool btnMode){

//if button mode high, that mean button control is active
if(btnMode){

    //read current button state  
    bool btnState = digitalRead(BTN);
    
    //is current state not equal previous button state?
    if(btnState != lastBtnState){
      
    //is button pressed
      if(btnState){
          MIDI.sendNoteOn(noteNumber, 127, 1); 
        }else{
          MIDI.sendNoteOff(previousNoteNumber,0,1);
        }
      lastBtnState = btnState;
       //if current button state equal with previous    
  }else{
    if(btnState){
      //check is note change
      if(noteNumber != previousNoteNumber){
        MIDI.sendNoteOff(previousNoteNumber,0,1);
        MIDI.sendNoteOn(noteNumber, 127, 1);
        previousNoteNumber = noteNumber;
        }
    }
  }
  
//button mode is off  
}else{
   
if (noteNumber != previousNoteNumber){
    MIDI.sendNoteOff(previousNoteNumber,0,1);
    MIDI.sendNoteOn(noteNumber, 127, 1); 
    previousNoteNumber = noteNumber;
    }
}
  }

//-------DISPLAY-NOTE-----------  

void displayNote(byte noteNumber)
{
  byte pattern;
 
  pattern = notes_patterns[noteNumber];

  digitalWrite(DIGIT_CLOCK_PIN, LOW);
  
  shiftOut(DATA_PIN, BIT_CLOCK_PIN, MSBFIRST, ~pattern);
 
  digitalWrite(DIGIT_CLOCK_PIN, HIGH);
}    
