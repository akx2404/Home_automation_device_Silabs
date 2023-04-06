#include "DS1302.h"
#include <TM1637Display.h>

// Init the DS1302
DS1302 rtc(2, 4, 5);

//clock 5, DAT 4, rst 2

// Init a Time-data structure
Time t;


const int CLK = 15;
const int DIO = 0;
TM1637Display display(CLK, DIO); 


void setup()
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  Serial.begin(115200);

  // The following lines can be commented out to use the values already stored in the DS1302
   rtc.setDOW(WEDNESDAY);
   rtc.setTime(16, 19, 00);
   rtc.setDate(05, 04, 2023);

   display.setBrightness(0x0a);
}

void loop()
{
  t = rtc.getTime();

  // Send date over serial connection
  Serial.print("Today is the ");
  Serial.print(t.date, DEC);
  Serial.print(" Day,");
  Serial.print(rtc.getMonthStr());
  Serial.print(" Month,");
  Serial.print(t.year, DEC);
  Serial.println(" year");

  // Send Day-of-Week and time
  Serial.print(t.hour, DEC);
  Serial.print(" hour,");
  Serial.print(t.min, DEC);
  Serial.print(" minute,");
  Serial.print(t.sec, DEC);
  Serial.println(" second");

  // Send a divider for readability
  Serial.println(" - - - - - - - - - - - - - - - - - - - - -");

  int displaytime = (t.hour * 100) + t.min;
  display.showNumberDecEx(displaytime, 0b11100000, true);

  // Wait one second before repeating :)
  delay (1000);
  
}
