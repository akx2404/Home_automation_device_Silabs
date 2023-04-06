#include "BluetoothSerial.h"
String device_name = "ESP32-BT";

BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin(device_name); //Bluetooth device name
}

void loop() {
    SerialBT.println("hello hello");
    delay(1000);
}
