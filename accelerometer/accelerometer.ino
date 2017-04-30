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
float interval = .5; // TOO SMALL OF A VALUE GIVES YOU TOO SMALL NUMBERS
void accelerometerInit(){
  secondTick.attach(interval, displace);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT); //buzzer pin
}
/* -- accelerometer --  */

void setup() {
  accelerometerInit();
  Serial.begin(9600); 
}

void displace() {
  // TODO: CONVERT DATA
  // 0b01 
  digitalWrite(S0, LOW); digitalWrite(S1, LOW);
  accelf[0] = analogRead(A0);
  float dx = 0.5 * (accelf[0] - acceli[0]) * pow(interval, 2);
  displacement[0] = displacement[0] + dx;
 
  // 0b10
  digitalWrite(S0, HIGH); digitalWrite(S1, LOW);
  accelf[1] = analogRead(A0);
  float dy = 0.5 * (accelf[1] - acceli[1]) * pow(interval, 2);
  displacement[1] = displacement[1] + dy;
  
  // 0b11
  digitalWrite(S0, LOW); digitalWrite(S1, HIGH);
  accelf[2] = analogRead(A0);
  float dz = 0.5 * (accelf[2] - acceli[2]) * pow(interval, 2);
  displacement[2] = displacement[2] + dz;

  //find accelf values
  acceli[0] = accelf[0];acceli[1] = accelf[1];acceli[2] = accelf[2];
  
  displacementval = sqrt(pow(displacement[0], 2) + pow(displacement[1], 2) + pow(displacement[2], 2));
  Serial.println(displacementval);
}

void loop() {
  delay(1000);
}
