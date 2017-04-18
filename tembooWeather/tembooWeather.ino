#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WidgetRTC.h>
#include <Temboo.h>
#include "TembooAccount.h"
WiFiClient client;

char auth[] = "a4d69c753a8e4b1fac95fee91e20aad2"; char ssid[] = "CS390IOT"; char pass[] = "12345678";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass); // WifiClient automatically uses this
}

void loop() {
  Blynk.run();
  scrapeWeather();
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
  int code = 0;
  while(GetWeatherByAddressChoreo.available()) {
    //get code variable by observing the output and taking code from it.
    char c = GetWeatherByAddressChoreo.read();    
    code = 0;
    Serial.print(c);
  }  
    Serial.print("code: "); Serial.println(code);
    if(code <= 12 || code == 17 || code == 18 || code == 35 || ( code >= 37 || code <= 40) || code == 45 || code == 47){
      //do something
    }

  GetWeatherByAddressChoreo.close();
  delay(10000);
}

