/*--Blynk--*/
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WidgetRTC.h>
WidgetRTC rtc;
WiFiClient client;
char auth[] = "9b1b5f1de728419487c4dca8ef396462"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
volatile byte ONOFF = HIGH;
int row_idx = 0, v0 = 0, v2 = 0;
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
}
/*--Temboo--*/

/*--RFID--*/
//https://learn.sparkfun.com/tutorials/sparkfun-rfid-starter-kit-hookup-guide
/* -- RFID init -- */
#include <SoftwareSerial.h>
#define RFIDINTERRUPTPIN 4
SoftwareSerial rSerial(5, 7); // RX, TX
const int tagLen = 16;
const int idLen = 13;
char newTag[idLen]; // Empty array to hold a freshly scanned tag
const int kTags = 5;

// Put your known tags here!
char knownTags[kTags][idLen] = {"7F001AFE31AA", "7F001B09066B", "7F001B63E0E7", "7F001B4E577D", "7F001B4C0C24"};
void initRFID(){
   rSerial.begin(9600);
   pinMode(RFIDINTERRUPTPIN, INPUT);
   attachInterrupt(digitalPinToInterrupt(RFIDINTERRUPTPIN), rfidRead, RISING);
}
/*--RFID--*/

void setup() {
  Serial.begin(9600);
  blynkInit();
  tembooInit(); // depends on Blynk
  initRFID();
}

void loop() {
  Blynk.run();
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
    //do something
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
  // Set Choreo inputs
  SearchEventsChoreo.addInput("ResponseFormat", "xml");
  SearchEventsChoreo.addInput("RefreshToken", "1/1Q8i7PsjI3bWvJT35bfnaa8ZE6oeISthyGlf1rbNQoU3RlbBvBZqRSjLMTBEQUAr");
  SearchEventsChoreo.addInput("ClientSecret", "qPZ7Sq7JLWz9QqFfbZFqzHGW");
  SearchEventsChoreo.addInput("CalendarID", "o7j1lqsqfnbbl39grvld25hjas@group.calendar.google.com");
//  SearchEventsChoreo.addInput("SingleEvent", "1");
  SearchEventsChoreo.addInput("ClientID", "422310769533-ks1l8016agn5jbe20l0qg362euq403fg.apps.googleusercontent.com");
//  SearchEventsChoreo.addInput("MaxTime", "2017-04-20T23:59:59-04:00");
//  SearchEventsChoreo.addInput("MinTime", "2017-04-20T01:00:00-04:00");
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
  String stuff[5];
  String total = "";
  int pos = 0;
  while(SearchEventsChoreo.available()) {
    String summary = SearchEventsChoreo.readStringUntil('\x1F'); summary.trim(); // use “trim” to get rid of newlines
    String description = SearchEventsChoreo.readStringUntil('\x1E');
    int tmp = 0;
    String sub = "";
    if(summary == "description"){
//      total += description; //add this to "stuff" later
      while(description.indexOf('\n') > 0){
        bool here = false;
        tmp = description.indexOf('\n');
        sub = description.substring(0,tmp);
        for(int i = 0; i<5; i++){
          if(stuff[i] == sub){
            here = true;
          }
        }
        if(!here){
          stuff[pos] = sub;
          pos = pos + 1;
          here = false;
        }
        description = description.substring(tmp+1);
      }
    }
//    Serial.println("Summary: " + summary);
//    Serial.println("Description: " + description);
  }
//  Serial.println(total);
  Serial.println("Printing out stuff...");
  for(int x = 0; x<5; x++){
    Serial.println(stuff[x]);
  }
  delay(100);
  SearchEventsChoreo.close();
}

void getDate(String data[]){
//  while(year()!=1970){Serial.print(".");delay(1000);} // wait while rtc gets accessed, danger for watchdog
  String d = String(day());
  if(d.length()==1){
    d = "0" + d;
  }
  String m = String(month());
  if(m.length()==1){
    m = "0" + m;
  }
  data[0] = String(year()) + "-" + m + "-" + d + "T01:00:00-04:00";
  data[1] = String(year()) + "-" + m + "-" + d + "T23:59:59-04:00";
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
      if (readByte != 2 && readByte!= 13 && readByte != 10 && readByte != 3) {
        newTag[i] = readByte; 
        i++;
      }
      if (readByte == 3) {  tag = false; }      // If we see ASCII 3, ETX, the tag is over
    }
  
  // don't do anything if the newTag array is full of zeroes
  if (strlen(newTag)== 0) {return;}
  else //check against known tags, increase total if in known tags
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

int checkTag(char nTag[], char oTag[]) {
    for (int i = 0; i < idLen; i++) {
      if (nTag[i] != oTag[i]) {return 0;}
    }
  if(ONOFF){
      Blynk.virtualWrite(V1, "add", row_idx, nTag, "Here");
      row_idx = row_idx + 1;
  }
  return 1;
}
