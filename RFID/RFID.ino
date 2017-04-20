int RFIDResetPin = 13;
//Register your RFID tags here
char tag1[13] = "1E009A4067A3";
char tag2[13] = "010230F28243";

void setup(){
  Serial.begin(9600);
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  //ONLY NEEDED IF CONTROLING THESE PINS - EG. LEDs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop(){
  char tagString[13];
  int index = 0;
  boolean reading = false;
  while(Serial.available()){
    int readByte = Serial.read(); //read next available byte
    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag
    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }
  checkTag(tagString); //Check if it is a match
  clearTag(tagString); //Clear the char of all value
  resetReader(); //eset the RFID reader
}

void checkTag(char tag[]){
  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, tag1)){ // if matched tag1, do this
    lightLED(2);
  }else{
    Serial.println(tag); //read out any unknown tag
  }

}

void resetReader(){
  digitalWrite(RFIDResetPin, LOW);
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
