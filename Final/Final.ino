/* -- accelerometer --  */
#include <Ticker.h>
Ticker secondTick;
#define S0 12
#define S1 14
boolean SLUNG = false;
volatile int watchdogCount = 0;
float acceli[3] = {0, 0, 0};
float accelf[3] = {0, 0, 0};
float displacement[3] = {0, 0, 0};
float displacementval = 0;
float interval = .1; // TOO SMALL OF A VALUE GIVES YOU TOO SMALL NUMBERS
void accelerometerInit(){
  secondTick.attach(interval, displace);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT); //buzzer pin
}
void displace() { // TODO: CONVERT DATA
  /*NOTE: because consumer grade accelerometers cannot find this kind of data
  I am using it to determine whether the backpack has been "slung"*/
  // 0b01 
  digitalWrite(S0, LOW); digitalWrite(S1, LOW);
  accelf[0] = analogRead(A0);
  float dx = 0.5 * (accelf[0] - acceli[0]) * pow(interval, 2);
  displacement[0] = displacement[0] + dx;
  // 0b10
  digitalWrite(S0, HIGH); digitalWrite(S1, LOW);
  accelf[1] = analogRead(A0);
  float dy = 0.5 * (accelf[1] - acceli[1]) * pow(interval, 2);
  displacement[1] = displacement[1] + dy;
  // 0b11
  digitalWrite(S0, LOW); digitalWrite(S1, HIGH);
  accelf[2] = analogRead(A0);
  float dz = 0.5 * (accelf[2] - acceli[2]) * pow(interval, 2);
  displacement[2] = displacement[2] + dz;
  //find accelf values
  acceli[0] = accelf[0];acceli[1] = accelf[1];acceli[2] = accelf[2];
  displacementval = sqrt(pow(displacement[0], 2) + pow(displacement[1], 2) + pow(displacement[2], 2));
  Serial.println(displacementval);
  if(displacementval > 65){
    secondTick.detach();
    SLUNG = !SLUNG;
  }
}
/* -- accelerometer --  */

/*--Blynk--*/
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WidgetRTC.h>
WidgetRTC rtc;
WiFiClient client;
//char auth[] = "9b1b5f1de728419487c4dca8ef396462"; // Evan's
char auth[] = "a4d69c753a8e4b1fac95fee91e20aad2"; // Dan's
char ssid[] = "CS390IOT"; char pass[] = "12345678";
volatile byte ONOFF = HIGH;
int row_idx = 0, v0 = 0, v2 = 0;
bool reading = false;
WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V1);
void blynkInit(){
  Blynk.begin(auth, ssid, pass); // WifiClient automatically uses this
  rtc.begin();
  Blynk.run();Blynk.run();delay(1000);
  Blynk.virtualWrite(V0, HIGH);
  Blynk.virtualWrite(V1, "clr");
}
BLYNK_WRITE(V0){
  v0 = param.asInt(); ONOFF = v0;
  if(!ONOFF){
    Blynk.virtualWrite(V1, "clr");
    row_idx = 0;
  }
}
BLYNK_WRITE(V2){
  v2 = param.asInt();
  if(v2 && ONOFF){
    Blynk.virtualWrite(V1, "add", row_idx, "Name", "Value");
    row_idx = row_idx + 1;
  }
}
/*--Blynk--*/

/*--Temboo--*/
#include <Temboo.h>
#include "TembooAccount.h"
String date[2];
void tembooInit(){
  scrapeWeather();
  delay(3000);getDate(date);
  scrapeCalendar();
  delay(3000);
}
/*--Temboo--*/

/*--RFID--*/
//https://learn.sparkfun.com/tutorials/sparkfun-rfid-starter-kit-hookup-guide
#include <SoftwareSerial.h>
#define RFIDINTERRUPTPIN 4
SoftwareSerial rSerial(5, 7); // RX, TX
const int tagLen = 16;
const int idLen = 13;
char newTag[idLen]; // Empty array to hold a freshly scanned tag
char readTag[idLen] = "000000000000";
const int kTags = 5;

// Put your known tags here!
char knownTags[kTags][idLen] = {
  "7F001AFE31AA", // 0 CS >9000
  "7F001B09066B", // 1 umbrella
  "7F001B63E0E7", // 2 CS 390N
  "7F001B4E577D", // 3 CS 666.6
  "7F001B4C0C24"};// 4 soldering iron

boolean present [kTags] = {
  false, // 0 CS >9000
  false, // 1 umbrella
  false, // 2 CS 390N
  false, // 3 CS 666.6
  false};// 4 soldering iron
boolean expected [kTags] = {
  false, // 0 CS >9000
  false, // 1 umbrella
  false, // 2 CS 390N
  false, // 3 CS 666.6
  false};// 4 soldering iron
  
void initRFID(){
   rSerial.begin(9600);
   pinMode(RFIDINTERRUPTPIN, INPUT);
   attachInterrupt(digitalPinToInterrupt(RFIDINTERRUPTPIN), rfidRead, RISING);
}
/*--RFID--*/
void setup() {
  Serial.begin(9600);
  Serial.println("**Blynk setup:**");
  blynkInit();
  Serial.println("**Temboo setup:**");
  tembooInit(); // depends on Blynk
  Serial.println("**RFID setup:**");
  initRFID();
  for(int i = 0; i < 5; i++){
    Serial.println(expected[i]); Serial.println(present[i]);
  }
  Serial.println("**Accelerometer:**");
  accelerometerInit(); // only initializes ticker here, shouldn't interfere with the temboo stuff
  //check errything is there 
}
void loop() {//
  Blynk.run();
  if(SLUNG){
    for(int i = 0; i < kTags; i++){
      Serial.println(String(present[i]) + "\t" + String(expected[i]));
      if(present[i] != expected[i]){
        // RIGHT HERE, MAKE SURE KNOWNTAGS GETS PRINTED GOOD
        Blynk.notify("Hey you're missing something"); // we explicitly wanted more than one notification.
        secondTick.attach(interval, displace);
        Serial.println("notify");
      }
    }
    SLUNG = false;
  }
}

void scrapeWeather() {
  TembooChoreo GetWeatherByAddressChoreo(client);
   // Invoke the Temboo client
  GetWeatherByAddressChoreo.begin();
   // Set Temboo account credentials
  GetWeatherByAddressChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetWeatherByAddressChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetWeatherByAddressChoreo.setAppKey(TEMBOO_APP_KEY);
  // Set Choreo inputs
  GetWeatherByAddressChoreo.addInput("Address", "Amherst, MA");
  GetWeatherByAddressChoreo.addInput("Day", "1");
  // Identify the Choreo to run
  GetWeatherByAddressChoreo.setChoreo("/Library/Yahoo/Weather/GetWeatherByAddress");
  //before run, set output filters (DAN)
  String weatherCodePath = "/rss/channel/item/yweather:forecast/@code";
  GetWeatherByAddressChoreo.addOutputFilter("weather", weatherCodePath, "Response");
  // Run the Choreo; when results are available, print them to serial
  GetWeatherByAddressChoreo.run();
  String code = "";
  while(GetWeatherByAddressChoreo.available()) { /*FROM HOW TO FILTER DATA*/
    String name = GetWeatherByAddressChoreo.readStringUntil('\x1F'); name.trim(); // use “trim” to get rid of newlines
    code = GetWeatherByAddressChoreo.readStringUntil('\x1E');
    if(name == "weather"){break;} // this leaves code being exactly what we need it to be.
  }
  code.trim(); int data = code.toInt();
  Serial.print("code:"); Serial.println(code);
  if(data <= 12 || data == 17 || data == 18 || data == 35 || ( data>= 37 || data<= 40) || data== 45 || data== 47){
    expected[1] = true;
  }

  GetWeatherByAddressChoreo.close();
  //delay(5000);
}

void scrapeCalendar() {
  TembooChoreo SearchEventsChoreo(client);
  // Invoke the Temboo client
  SearchEventsChoreo.begin();
  // Set Temboo account credentials
  SearchEventsChoreo.setAccountName(TEMBOO_ACCOUNT);
  SearchEventsChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SearchEventsChoreo.setAppKey(TEMBOO_APP_KEY);
  // Set Choreo inputs
  SearchEventsChoreo.addInput("ResponseFormat", "xml");
  SearchEventsChoreo.addInput("RefreshToken", "1/1Q8i7PsjI3bWvJT35bfnaa8ZE6oeISthyGlf1rbNQoU3RlbBvBZqRSjLMTBEQUAr");
  SearchEventsChoreo.addInput("ClientSecret", "qPZ7Sq7JLWz9QqFfbZFqzHGW");
  SearchEventsChoreo.addInput("CalendarID", "o7j1lqsqfnbbl39grvld25hjas@group.calendar.google.com");
  //  SearchEventsChoreo.addInput("SingleEvent", "1");
  SearchEventsChoreo.addInput("ClientID", "422310769533-ks1l8016agn5jbe20l0qg362euq403fg.apps.googleusercontent.com");
  SearchEventsChoreo.addInput("MinTime", date[0]);
  SearchEventsChoreo.addInput("MaxTime", date[1]);
  //before run, set output filters (DAN)
  String classSummaryPath = "//e/summary";
  String classDescriptionPath = "//e/description";
  SearchEventsChoreo.addOutputFilter("summary", classSummaryPath, "Response");
  SearchEventsChoreo.addOutputFilter("description", classDescriptionPath, "Response");
  // Identify the Choreo to run
  SearchEventsChoreo.setChoreo("/Library/Google/Calendar/SearchEvents");
  // Run the Choreo; when results are available, print them to serial
  SearchEventsChoreo.run();
  while(SearchEventsChoreo.available()) {
    String summary = SearchEventsChoreo.readStringUntil('\x1F'); summary.trim(); // use “trim” to get rid of newlines
    Serial.println(summary);
    String description = SearchEventsChoreo.readStringUntil('\x1E');
    int pos = 0; // represents index of the tag of item just scanned in
    int tmp = 0;
    String sub = "";
    if(summary == "description"){
      while(description.indexOf('\n') > 0){
        bool here = false;
        tmp = description.indexOf('\n');
        sub = description.substring(0,tmp);
        for(int i = 0; i<5; i++){
          /*we just need a string for comparison*/
          String kTagString = "";
          for(int j = 0; j < idLen; j++){
            kTagString += (knownTags[i][j]);
          }
          Serial.println(kTagString);
          /*we just need a string for comparison*/
          if(kTagString == sub){ // which index of ktags is represented by this description?
            pos = i;break;
          }
        }
        if(!present[pos] && !expected[pos]){//  it is already in bag- redundancy checking.
          expected[pos] = true; // make expected at thing pos equal true
        }
        description = description.substring(tmp+1);
      }
    }
  }
  delay(100);SearchEventsChoreo.close();
}

void getDate(String data[]){
  String d = String(day());
  if(d.length()==1){d = "0" + d;}
  String m = String(month());
  if(m.length()==1){m = "0" + m;}
  data[0] = String(year()) + "-" + m + "-" + d + "T01:00:00-04:00";
  data[1] = String(year()) + "-" + m + "-" + d + "T23:59:59-04:00";
}

void rfidRead(){
  noInterrupts();
  boolean tag = false;
  // This makes sure the whole tag is in the serial buffer before reading, the Arduino can read faster than the ID module can deliver!
  if (rSerial.available() == tagLen) {tag = true;}
  if(tag) {
    int i = 0;// Counter for the newTag array
    int readByte;// Variable to hold each byte read from the serial buffer
    while (rSerial.available()) {
      // Take each byte out of the serial buffer, one at a time
      readByte = rSerial.read();
      Serial.print(char(readByte));
      /* This will skip the first byte (2, STX, start of text) and the last three, ASCII 13, CR/carriage return, ASCII 10, LF/linefeed, and ASCII 3, ETX/end of text, leaving only the unique part of the tag string. It puts the byte into
      the first space in the array, then steps ahead one spot */
      
      if (readByte != 2 && readByte!= 13 && readByte != 10 && readByte != 3) {
        newTag[i] = readByte; i++;
      } else if (readByte == 3) {  tag = false; }      // If we see ASCII 3, ETX, the tag is over
    }
    // don't do anything if the newTag array is full of zeroes
    if (strlen(newTag)== 0) {return;}
    else //check against known tags, increase total if in known tags
    {
      for(int i = 0; i<5; i++){
        if(checkTag(newTag, knownTags[i])){
          Serial.println("HI!");
          present[i] = !present[i]; break; // change the data in the known present tags
        }
      }
      for (int c=0; c < idLen; c++) {newTag[c] = 0;} // Once newTag has been checked, fill it with zeroes to get ready for the next tag read
    }
  }
  interrupts();
}
            //    newtag,       knowntags
boolean checkTag(char nTag[], char oTag[]) {
  bool same = true;
  for (int i = 0; i < idLen; i++) {
    if (nTag[i] != oTag[i]) {
      same = false;
    }
  }
  if(ONOFF && same){
    //Blynk.virtualWrite(V1, "add", row_idx, nTag, "Here");
    table.addRow(row_idx, nTag, "Here");
    row_idx = row_idx + 1;
  }
  return same;
}
