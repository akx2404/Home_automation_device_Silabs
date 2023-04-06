#include <TimeLib.h>
#include <Arduino.h>
//tmElements_t tmSet;

#include <SoftwareSerial.h>
SoftwareSerial QuartzBT(10, 11); // RX | TX
#include "virtuabotixRTC.h"
virtuabotixRTC myRTC(6, 7, 8);

#include <TM1637Display.h>
#define CLK 3
#define DIO 4
TM1637Display display = TM1637Display(CLK, DIO);

void setup() {
  Serial.begin(115200); 
  QuartzBT.begin(9600);
  myRTC.setDS1302Time(00, 25, 16, 4, 06, 04, 2023); // seconds, minutes, hours, day of the week, day of the month, month, year
  display.setBrightness(5);
  display.clear();
}

void loop() {
   myRTC.updateTime();
   //myRTC.dayofmonth, myRTC.month), myRTC.year, myRTC.hours, myRTC.minutes, myRTC.seconds
   int tm = myRTC.hours*100 + myRTC.minutes;
   
   float n = Serial.parseFloat();
   Serial.println(n);
   QuartzBT.println(n);

   time_t myTimeStamp = tmConvert_t(myRTC.year, myRTC.month,myRTC.dayofmonth, myRTC.hours, myRTC.minutes, myRTC.seconds);
   Serial.println(myTimeStamp);
   QuartzBT.println(myTimeStamp);

   display.showNumberDecEx(tm, 0b11100000, true);

   QuartzBT.println('\n');
   delay(1000);
    
}

time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet); 
}
