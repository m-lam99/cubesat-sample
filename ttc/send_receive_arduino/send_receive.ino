
//char buff[3] = {0x41,0x54,0x3F};

//char buff[3] = {0x41,0x54,0x56};

//char buff[4] = {0x41,0x54,0x4D,0x01};

//char buff[5] = {0x41,0x54,0x52,0x00,0x13};

char buff2[20] = {0x41,0x54,0x53,0x00,0x15,0x47,0x65,0x6E,0x65,0x72,0x61,0x6C,0x20,0x4B,0x65,0x6E,0x6F,0x62,0x69,0x21};

//char buff [7] = {0x41,0x54,0x54,0x23,0x00,0x00,0x00};

//char buff [11] = {0x41,0x54,0x52,0x23,0x00,0x00,0x00,0x23,0x00,0x01,0x00};

void setup() {
  Serial.begin(9600);
  Serial1.begin(19200);
 
}

void loop() {
//  Serial1.write(buff,5);
//  delay(500);
//  for(int i=0;i<12;i++){
//    char output = Serial1.read();
//    Serial.print(output);
//    delay(50);
//  }
//  delay(500);
  
  Serial1.write(buff2,20);
  delay(500);
//  for(int i=0;i<15;i++){
//    char output = Serial1.read();
//    Serial.print(output);
//    delay(50);
//  }
//  delay(500);

}