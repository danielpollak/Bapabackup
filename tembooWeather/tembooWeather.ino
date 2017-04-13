#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h"
#include <Adafruit_NeoPixel.h>
#include <WidgetRTC.h>
WiFiClient client;

#define PIN_BUTTON   0
#define PIN_LED     15
#define PIN_BUZZER  13
#define SWITCH_PIN  A0

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, PIN_LED, NEO_GRB + NEO_KHZ800);
char auth[] = "a4d69c753a8e4b1fac95fee91e20aad2"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
const byte interruptPin = PIN_BUTTON; // the reset button
volatile byte ONOFF = HIGH;
WidgetLCD lcd(V0);
WidgetRTC rtc;
int v1 = 0, v2 = 0, v3 = 0;
String doorOpen, door_closed, RESET, alarm_on, alarm_off, system_on_off, police_called, disarm_button;

String timestamp(){  return String(hour()) + ":" + minute() + ":" + second() + "" + day() + "," + month() + "," + year();  }

void update_lcd(bool b){
  if(b)
    lcd.print(0, 0, "Open");
  else
    lcd.print(0, 0, "Closed");
}

void opened(){ alarmon();  update_lcd(true); }

void alarmon() {
  analogWrite(PIN_BUZZER, 100 * ONOFF);
  analogWriteFreq(100 * ONOFF);
  pixel.setPixelColor(0, pixel.Color(255/2 * ONOFF, 0, 0)); // 50%duty cycle
  Blynk.notify("Open");
  Blynk.virtualWrite(V1, HIGH * ONOFF);
  Blynk.setProperty(V1, "offLabel", "Calling");
  alarm_on = timestamp();
}
void alarmoff(){
  analogWrite(PIN_BUZZER, 0);
  analogWriteFreq(0);
  pixel.setPixelColor(0, pixel.Color(0, 0, 0));
  pixel.show();
  Blynk.virtualWrite(V1, LOW);
  Blynk.setProperty(V1, "offLabel", "OFF");
  alarm_off = timestamp();
}

void closed(){
  alarmoff(); update_lcd(false);  
}

void resetalarmoff(){
  alarmoff(); RESET = timestamp();
}

BLYNK_WRITE(V1){
  v1 = param.asInt(); police_called = timestamp();
}

BLYNK_WRITE(V2){
  v2 = param.asInt();
  alarmoff();  
  disarm_button = timestamp();
}
BLYNK_WRITE(V3){
  if(param.asInt())
    ONOFF = HIGH;
  else
    ONOFF = LOW;
  alarmoff(); 
  system_on_off = String(ONOFF);
}

bool door_open(){return analogRead(A0) > 500;} //bool door_open(){ if(analogRead(A0) > 500) return true; else return false; //check to see if door is open}

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP); //reverse logic: if LOW, it's closed.
  pinMode(interruptPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), resetalarmoff, RISING);
  
  Serial.begin(9600);
  pixel.begin(); pixel.show();

  Blynk.begin(auth, ssid, pass);
  rtc.begin();
  delay(2000);
  Serial.println(timestamp());
}

void loop() {
  Blynk.run();

  doorOpen = door_closed = RESET = alarm_on = alarm_off = system_on_off = police_called = disarm_button = "";
  
  if(door_open()){
    opened();
    doorOpen = timestamp();
  }
  while(door_open()){ // while door is open, wait for door to close.
      analogWrite(PIN_BUZZER, 100 * ONOFF);
      analogWriteFreq(100 * ONOFF);
      pixel.setPixelColor(0, pixel.Color(255/2 * ONOFF, 0, 0)); // 50%duty cycle
      pixel.show(); delay(500);
      pixel.setPixelColor(0, pixel.Color(0, 0, 0));
      pixel.show(); delay(500);
  }
  closed();
  door_closed = timestamp();
  /*--------------------TEMBOO--------------------*/
  //Invoke the client
  TembooChoreo AppendValuesChoreo(client);

  AppendValuesChoreo.begin();
  // Set Temboo account credentials
  AppendValuesChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendValuesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendValuesChoreo.setAppKey(TEMBOO_APP_KEY);
  // Set Choreo inputs
  AppendValuesChoreo.addInput("RefreshToken", "1/v01F5VjgcGad00K3iaDVIL7ZRgE9FyBRYxdxKp3l82A");
  AppendValuesChoreo.addInput("ClientSecret", "qPZ7Sq7JLWz9QqFfbZFqzHGW");
  
  String values = "[\n  [\n    \"";
  values += doorOpen;values += "\",\n    \"";
  values += door_closed;  values += "\",\n    \"";
  values += RESET;  values += "\",\n    \"";
  values += alarm_on;  values += "\",\n    \"";
  values += alarm_off;  values += "\",\n    \"";
  values += system_on_off;  values += "\",\n    \"";
  values += police_called;  values += "\",\n    \"";
  values += disarm_button;  values == "\"\n  ]\n]";
  Serial.println(values);
  //values = "[\n  [\n    \"timestamp\",\n    \"timestamp\",\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n,\n    \"timestamp\"\n  ]\n]";
  AppendValuesChoreo.addInput("Values", values);
  AppendValuesChoreo.addInput("ClientID", "422310769533-ks1l8016agn5jbe20l0qg362euq403fg.apps.googleusercontent.com");
  AppendValuesChoreo.addInput("SpreadsheetID", "1mKfLQbm9979IbW1J6j6reSl-xIpZOrBYQW40tA6QZ8Y");

  // Identify the Choreo to run
  AppendValuesChoreo.setChoreo("/Library/Google/Sheets/AppendValues");
    
  // Run the Choreo; when results are available, print them to serial
  int success = AppendValuesChoreo.run();
   // Whoof! let's let it pause and collect its thoughts here. Om.
  AppendValuesChoreo.close();
  delay(50);
  /*--------------------TEMBOO--------------------*/
  while(!door_open()){ delay(20); } // while door is closed, wait.
}


