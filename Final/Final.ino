#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h"
WiFiClient client;

char auth[] = "9b1b5f1de728419487c4dca8ef396462"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
volatile byte ONOFF = HIGH;
int row_idx = 0, v0 = 0, v2 = 0;

void setup() {
  Serial.begin(9600);
  
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, HIGH);
  Blynk.virtualWrite(V1, "clr");
  scrapeWeather();
}

BLYNK_WRITE(V0){
  v0 = param.asInt();
  ONOFF = v0;
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
