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

  delay(40);
  scrapeCalendar();
  delay(1000);

}

void loop() {
  Blynk.run();
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
    SearchEventsChoreo.addInput("Query", "CS");
    SearchEventsChoreo.addInput("CalendarID", "o7j1lqsqfnbbl39grvld25hjas@group.calendar.google.com");
    SearchEventsChoreo.addInput("ClientSecret", "qPZ7Sq7JLWz9QqFfbZFqzHGW");
    SearchEventsChoreo.addInput("ClientID", "422310769533-ks1l8016agn5jbe20l0qg362euq403fg.apps.googleusercontent.com");
    SearchEventsChoreo.addInput("MinTime", "2017-04-20T01:00:00-04:00");
    SearchEventsChoreo.addInput("MaxTime", "2017-04-20T23:59:59-04:00");
    
    //before run, set output filters (DAN)
    String classSummaryPath = "//summary" //"/o/items/e/summary";
    String classDescriptionPath = "//description";
    SearchEventsChoreo.addOutputFilter("summary", classSummaryPath, "Response");
    SearchEventsChoreo.addOutputFilter("description", classDescriptionPath, "Response");
    
//    SearchEventsChoreo.addInput("MinTime", "2017-0" + String(day()) + "-20T01:00:00-04:00");
//    SearchEventsChoreo.addInput("MaxTime", "2017-0" + String(day()) + "-20T23:59:59-04:00");

  // Identify the Choreo to run
  SearchEventsChoreo.setChoreo("/Library/Google/Calendar/SearchEvents");
    
  // Run the Choreo; when results are available, print them to serial
  SearchEventsChoreo.run();
//  String c = "";
  while(SearchEventsChoreo.available()) {
//    String name = SearchEventsChoreo.readStringUntil('\x1F'); name.trim(); // use “trim” to get rid of newlines
//    String code = SearchEventsChoreo.readStringUntil('\x1E');
    char c = SearchEventsChoreo.read();
    
//    Serial.print(name); Serial.println(code);
  Serial.print(c);
  }
  //Serial.print(c);
  delay(1000);
  SearchEventsChoreo.close();
}

