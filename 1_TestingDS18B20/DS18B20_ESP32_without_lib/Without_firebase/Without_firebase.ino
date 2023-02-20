#define TEMP_PIN  4

void OneWireReset(int Pin);
void OWwrite(int Pin, byte d);
byte OWread(int Pin);

void setup() {
    digitalWrite(TEMP_PIN, LOW);
    pinMode(TEMP_PIN, INPUT);
    Serial.begin(115200);
    delay(100);
    Serial.print("temperature measurement:\n");
}

void loop(){
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  OneWireReset(TEMP_PIN);
  OWwrite(TEMP_PIN, 0xcc);
  OWwrite(TEMP_PIN, 0x44);

  OneWireReset(TEMP_PIN);
  OWwrite(TEMP_PIN, 0xcc);
  OWwrite(TEMP_PIN, 0xbe);

  LowByte = OWread(TEMP_PIN);
  HighByte = OWread(TEMP_PIN);
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;
  if (SignBit)
  {
    TReading = (TReading ^ 0xffff) + 1;
  }
  Tc_100 = (6 * TReading) + TReading / 4;

  Whole = Tc_100 / 100;
  Fract = Tc_100 % 100;


  if (SignBit) // If negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }

  Serial.print(Fract);

      Serial.print("\n");
  delay(2000);
}

void OneWireReset(int Pin)
{
     digitalWrite(Pin, LOW);
     pinMode(Pin, OUTPUT);
     delayMicroseconds(500);
     pinMode(Pin, INPUT);
     delayMicroseconds(500);
}

void OWwrite(int Pin, byte d)
{
   byte n;

   for(n=8; n!=0; n--)
   {
      if ((d & 0x01) == 1)
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(5);
         pinMode(Pin, INPUT);
         delayMicroseconds(60);
      }
      else
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(60);
         pinMode(Pin, INPUT);
      }

      d=d>>1;
   }

}

byte OWread(int Pin)
{
    byte d, n, b;

    d=0;

    for (n=0; n<8; n++)
    {
        digitalWrite(Pin, LOW);
        pinMode(Pin, OUTPUT);
        delayMicroseconds(5);
        pinMode(Pin, INPUT);
        delayMicroseconds(5);
        b = digitalRead(Pin);
        delayMicroseconds(50);
        d = (d >> 1) | (b<<7);
    }
    return(d);
}
