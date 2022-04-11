
//char buff[3] = {0x41,0x54,0x3F};

char buff[3] = {0x41,0x54,0x56};

//char buff[4] = {0x41,0x54,0x4D,0x01};


void setup() {
  Serial.begin(9600);
  Serial1.begin(19200);
 
}

void loop() {
  Serial.print("Start");
  Serial.print("Writing");
  Serial1.write(buff,3);
  delay(500);
  for(int i=0;i<6;i++){
    char output = Serial1.read();
    Serial.print(output);
    delay(50);
  }
  delay(5000);

}
