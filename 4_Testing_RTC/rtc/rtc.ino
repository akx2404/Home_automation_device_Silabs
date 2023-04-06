#include "DS1302.h"

// Init the DS1302
DS1302 rtc(2, 4, 5);

// Init a Time-data structure
Time t;

void setup()
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  Serial.begin(115200);

  // The following lines can be commented out to use the values already stored in the DS1302
   rtc.setDOW(TUESDAY);
   rtc.setTime(19, 50, 00);
   rtc.setDate(04, 04, 2023);
}

void loop()
{
  // Get data from the DS1302
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

  long epc;
  

  // Wait one second before repeating :)
  delay (1000);
  
}
