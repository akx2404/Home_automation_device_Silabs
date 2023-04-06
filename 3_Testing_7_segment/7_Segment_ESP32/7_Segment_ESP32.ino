#include <TM1637Display.h>
 
const int CLK = 15;
const int DIO = 0;
 
int numCounter = 0;
 
TM1637Display display(CLK, DIO); 
 
void setup()
{
display.setBrightness(0x0a);
}
 
void loop()
{
for(numCounter = 0; numCounter < 1000; numCounter++) //Iterate numCounter
{
display.showNumberDec(numCounter);
delay(1000);
}
}
