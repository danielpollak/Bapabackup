/* -- accelerometer --  */
#include <Ticker.h>
Ticker secondTick;
#define S0 12
#define S1 14
volatile int watchdogCount = 0;
float acceli[3] = {0, 0, 0};
float accelf[3] = {0, 0, 0};
float displacement[3] = {0, 0, 0};
float displacementval = 0;
/* -- accelerometer --  */

void setup() {
  ESP.wdtDisable();
  Serial.begin(9600);
  secondTick.attach(0.1, displace);
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);pinMode(A0, INPUT);
}

void displace() {
// TODO: CONVERT DATA
  // 0b01
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  accelf[0] = analogRead(A0);
  float dx = 0.5 * (accelf[0] - acceli[0]) * pow(0.1, 2);
  displacement[0] = displacement[0] + dx;
 
  // 0b10
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  accelf[1] = analogRead(A0);
  float dy = 0.5 * (accelf[1] - acceli[1]) * pow(0.1, 2);
  displacement[1] = displacement[1] + dy;
  
  // 0b11
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  accelf[2] = analogRead(A0);
  float dz = 0.5 * (accelf[2] - acceli[2]) * pow(0.1, 2);
  displacement[2] = displacement[2] + dz;
  
  //find accelf values
  acceli[0] = accelf[0];acceli[1] = accelf[1];acceli[2] = accelf[2];
  
  Serial.println(String(accelf[0]) + "\t" + String(accelf[1]) + "\t" + String(accelf[2]));
  Serial.println(String(displacement[0]) + "\t" + String(displacement[1]) + "\t" + String(displacement[2]));
  
//  Serial.println(analogRead(A0));
}

void loop() {
  delay(1000);
}
