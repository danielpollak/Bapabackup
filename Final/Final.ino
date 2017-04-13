#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient client;

char auth[] = "9b1b5f1de728419487c4dca8ef396462"; char ssid[] = "CS390IOT"; char pass[] = "12345678";
//volatile byte ONOFF = HIGH;
int row_idx = 0, v0 = 0;

void setup() {
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V1, "clr");
}

BLYNK_WRITE(V0){
  v0 = param.asInt();
  if(v1){
    Blynk.virtualWrite(V1, "add", row_idx, "Name", "Value");
    row_idx = row_idx + 1;
  }
}

void loop() {
  Blynk.run();
}
