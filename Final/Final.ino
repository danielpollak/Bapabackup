#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h"
#include <WidgetRTC.h>
WidgetRTC rtc;
WiFiClient client;

char auth[] = "9b1b5f1de728419487c4dca8ef396462"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
String date[2];
volatile byte ONOFF = HIGH;
int row_idx = 0, v0 = 0, v2 = 0;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass); // WifiClient automatically uses this
  rtc.begin();
  Blynk.run();Blynk.run();delay(1000);
  scrapeWeather();
  delay(2000);
//  Serial.println("date: " + String(year()) + "-" + String(month()) + "-" + String(day()) + "T01:00:00-04:00");
  getDate(date);
//  Serial.println("date1: " + date[0]);
//  Serial.println("date2: " + date[1]);
  scrapeCalendar();
  Blynk.virtualWrite(V0, HIGH);
  Blynk.virtualWrite(V1, "clr");
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
  while(SearchEventsChoreo.available()) {
//    String summary = SearchEventsChoreo.readStringUntil('\x1F'); // use “trim” to get rid of newlines
    String description = SearchEventsChoreo.readStringUntil('\x1E');
    total += description;
  }
  Serial.println(total);
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
