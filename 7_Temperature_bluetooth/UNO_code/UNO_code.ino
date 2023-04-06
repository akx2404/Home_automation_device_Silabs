#include <SoftwareSerial.h>
SoftwareSerial QuartzBT(10, 11); // RX | TX

void setup() {
  Serial.begin(115200); 
  QuartzBT.begin(9600);
  Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
}

void loop() {
   float n = Serial.parseFloat();
   Serial.println(n);
   QuartzBT.println(n);
   delay(1000);
    
}
