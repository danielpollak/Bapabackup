/*****************************
     RFID-powered lockbox

This sketch will move a servo when
a trusted tag is read with the 
ID-12/ID-20 RFID module

Pinout for SparkFun RFID USB Reader
Arduino ----- RFID module
5V            VCC
GND           GND
D2            TX

Pinout for SparkFun RFID Breakout Board
Arduino ----- RFID module
5V            VCC
GND           GND
D2            D0

Connect the servo's power, ground, and
signal pins to VCC, GND,
and Arduino D9

If using the breakout, you can also 
put an LED & 330 ohm resistor between 
the RFID module's READ pin and GND for 
a "card successfully read" indication

by acavis, 3/31/2015

Inspired by & partially adapted from
http://bildr.org/2011/02/rfid-arduino/

******************************/
/*
on rfid module, pins 1, 9, 11 go to GND, D2, and 5V, and pin 7 goes to pin 1 to set the format to ascii
*/
//https://learn.sparkfun.com/tutorials/sparkfun-rfid-starter-kit-hookup-guide

/* -- RFID init -- */
#include <SoftwareSerial.h>
#define RFIDINTERRUPTPIN 3
// Choose two pins for SoftwareSerial
SoftwareSerial rSerial(2, 7); // RX, TX
// For SparkFun's tags, we will receive 16 bytes on every tag read, but throw four away. The 13th space will always be 0, since proper strings in Arduino end with 0
// These constants hold the total tag length (tagLen) and the length of the part we want to keep (idLen),plus the total number of tags we want to check against (kTags)
const int tagLen = 16;
const int idLen = 13;
char newTag[idLen]; // Empty array to hold a freshly scanned tag
const int kTags = 4;

// Put your known tags here!
char knownTags[kTags][idLen] = {"7A005B0FF8D6"};
void initRFID(){
   Serial.begin(9600);
   rSerial.begin(9600);
   pinMode(RFIDINTERRUPTPIN, INPUT);
   attachInterrupt(digitalPinToInterrupt(RFIDINTERRUPTPIN), rfidRead, RISING);
}
/* -- RFID init -- */

void setup() {
  initRFID();
}

void rfidRead(){
  boolean tag = false;
  // This makes sure the whole tag is in the serial buffer before
  // reading, the Arduino can read faster than the ID module can deliver!
  //  if (rSerial.available() == tagLen) {tag = true;}
  if (rSerial.available() == tagLen) {
    tag = true;
  }
  if(tag) {
    Serial.println("H");
    int i = 0;// Counter for the newTag array
    int readByte;// Variable to hold each byte read from the serial buffer
  
    while (rSerial.available()) {
      // Take each byte out of the serial buffer, one at a time
      readByte = rSerial.read();
      Serial.print(char(readByte));

      /* This will skip the first byte (2, STX, start of text) and the last three, ASCII 13, CR/carriage return, ASCII 10, LF/linefeed, and ASCII 3, ETX/end of text, leaving only the unique part of the tag string. It puts the byte into
      the first space in the array, then steps ahead one spot */
      if (readByte != 2 && readByte!= 13 && readByte != 10 && readByte != 3) {newTag[i] = readByte; i++;}
      if (readByte == 3) {  tag = false; }      // If we see ASCII 3, ETX, the tag is over
    }
  
  // don't do anything if the newTag array is full of zeroes
  if (strlen(newTag)== 0) {return;}
  else
  {
    int total = 0;
    for (int ct=0; ct < kTags; ct++){
        total += checkTag(newTag, knownTags[ct]);
    }

  // Once newTag has been checked, fill it with zeroes to get ready for the next tag read
  for (int c=0; c < idLen; c++) {newTag[c] = 0;}
  }
  }
}
  

void loop() {
  delay(200);
  //Serial.println(digitalRead(RFIDINTERRUPTPIN));
}

// This function steps through both newTag and one of the known
// tags. If there is a mismatch anywhere in the tag, it will return 0,
// but if every character in the tag is the same, it returns 1
int checkTag(char nTag[], char oTag[]) {
    for (int i = 0; i < idLen; i++) {
      if (nTag[i] != oTag[i]) {return 0;}
    }
  return 1;
}

