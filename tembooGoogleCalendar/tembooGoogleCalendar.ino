#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
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
  TembooChoreo SearchEvents(client);
   // Invoke the Temboo client
  SearchEvents.begin();
   // Set Temboo account credentials
  SearchEvents.setAccountName(TEMBOO_ACCOUNT);
  SearchEvents.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SearchEvents.setAppKey(TEMBOO_APP_KEY);
  
  // Identify the Choreo to run
  SearchEvents.setChoreo("Library/Google/Calendar/SearchEvents");

  SearchEvents.close();
}

