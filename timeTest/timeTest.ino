#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WidgetRTC.h>
WiFiClient client;

char auth[] = "a4d69c753a8e4b1fac95fee91e20aad2"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
WidgetRTC rtc;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass); // WifiClient automatically uses this
  rtc.begin();
  Blynk.run();Blynk.run();delay(1000);
  Serial.println(String(year()) + "-" + month() + "-" + day() + "T01:00:00-04:00");
}

void loop() {
  Blynk.run();
  testGetTime();
  delay(5000);
}

void testGetTime(){
  if(year()!=1970){
    String d = String(day());
    if(d.length()==1){
      d = "0" + d;
    }
    String m = String(month());
    if(m.length()==1){
      m = "0" + m;
    }
    String currentDate = d + " " + m + " " + year();
    Serial.println(currentDate);
  
    Serial.println(String(year()) + "-" + m + "-" + d + "T01:00:00-04:00");
   }
   else{
    Serial.println("Year is 1970!");
   }
}

