int RFIDResetPin = 13;

void setup(){
  Serial.begin(9600);
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, LOW); delay(5); digitalWrite(RFIDResetPin, HIGH); delay(5);
  //ONLY NEEDED IF CONTROLING THESE PINS - EG. LEDs

}

void loop(){
  char tagString[13];
  int index = 0;
  boolean reading = false;
  while(Serial.available()){
    int readByte = Serial.read(); //read next available byte
    Serial.print(readByte);
    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag
    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }
  clearTag(tagString); //Clear the char of all value
  resetReader(); //eset the RFID reader
  Serial.println();
}

void resetReader(){
  digitalWrite(RFIDResetPin, LOW);delay(5);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]){
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}

boolean compareTag(char one[], char two[]){
  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}
