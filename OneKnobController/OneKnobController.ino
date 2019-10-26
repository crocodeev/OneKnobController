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

long previousMillis1 = 0;

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
  //delay for wait input
  delay(1000);
  //setup serial for MIDI
  Serial.begin(31250);
  //Serial.begin(19200);
  //set pin mode 
  pinMode(BTN,      INPUT);
  pinMode(SW_INPUT, INPUT);
  pinMode(SW_MODE, INPUT);
  pinMode(POT,      INPUT);
  pinMode(SENS,     INPUT);

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
    
  byte noteNumber = chooseNote(input, octava);

  //show note

  displayNote(noteNumber);

  //midi work
   
   sendToMIDI(noteNumber, btnMode); 
  
    }  

//functions

byte chooseNote(byte pin, byte octava){
  int  val = analogRead(pin);
  byte number = map(val,0,1023,0,11) + octava;
  return number;
  }


void sendToMIDI(byte noteNumber, bool btnMode){

if(btnMode){//btn mode on
 
    bool btnState = digitalRead(BTN);
    //check: is current button state equal last?
    //not equal  means that user push on or push off btn
    if(digitalRead(BTN) != lastBtnState){

    //check: is pressed  
      if(digitalRead(BTN)){// btn state eqaul push on
    //check: is there new data at input
    if (noteNumber != previousNoteNumber){
    MIDI.sendNoteOff(previousNoteNumber,0,1);
    //Serial.println("Send note off");
    //Serial.println(previousNoteNumber);
    MIDI.sendNoteOn(noteNumber, 127, 1); 
    //Serial.println("Send note on");
    //Serial.println(noteNumber);
    }else{
    MIDI.sendNoteOn(noteNumber, 127, 1); 
    //Serial.println("Send note on");
    //Serial.println(noteNumber); 
    }     
        }else{
    MIDI.sendNoteOff(previousNoteNumber,0,1);
    //Serial.println("Send note off");
    //Serial.println(previousNoteNumber);
        }
    previousNoteNumber =  noteNumber;    
    lastBtnState = btnState;    
  }
}else{//btn mode off
   
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

//-------DISPLAY-NOTE-----------  

void displayNote(byte noteNumber)
{
  byte pattern;
  
  // get the digit pattern to be updated
  pattern = notes_patterns[noteNumber];

  // turn off the output of 74HC595
  digitalWrite(DIGIT_CLOCK_PIN, LOW);
  
  // update data pattern to be outputed from 74HC595
  // because it's a common anode LED, the pattern needs to be inverted
  shiftOut(DATA_PIN, BIT_CLOCK_PIN, MSBFIRST, ~pattern);
  
  // turn on the output of 74HC595
  digitalWrite(DIGIT_CLOCK_PIN, HIGH);
}    
