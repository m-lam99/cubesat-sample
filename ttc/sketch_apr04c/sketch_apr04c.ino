
//char buff[3] = {0x41,0x54,0x3F};

char buff[4] = {0x41,0x54,0x4D,0x01};


void setup() {
  Serial.begin(9600);
  Serial1.begin(19200);
 
}

void loop() {
  Serial.print("Start");
  //if (Serial1.available()){
  Serial.print("Writing");
  Serial1.write(buff,3);
  for(int i=0;i<10;i++){
    char output = Serial1.read();
    Serial.print(output);
    delay(50);
    //}
    
  }
  delay(5000);

}
