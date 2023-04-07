#define TEMP_PIN  19
#define RXp2 16
#define TXp2 17

float set_temp; //temperature that user wants to set

void OneWireReset(int Pin);
void OWwrite(int Pin, byte d);
byte OWread(int Pin);

const char* WIFI_SSID = "Airel_8308842346";
const char* WIFI_PASSWORD =  "air34689";

#include <WiFi.h>
#include <FirebaseESP32.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>


#define API_KEY "AIzaSyBrZHcv7wRpuarsakB6TgMFGlQvKHbIDmg"
#define DATABASE_URL "https://ds18b20-a074c-default-rtdb.firebaseio.com/"
#define USER_EMAIL "akx2404@gmail.com"
#define USER_PASSWORD ""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        14
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500


void setup() {
    digitalWrite(TEMP_PIN, LOW);
    pinMode(TEMP_PIN, INPUT);
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
    delay(100);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  
    /* Assign the api key (required) */
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Firebase.setDoubleDigits(5);

    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
    #endif

    pixels.begin();
}

void loop(){
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(250, 250, 0));
  pixels.show();

  
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
  float temp = Whole + Fract*0.01;


  if (SignBit) // If negative
  {
     //Serial.print("-");
     temp = -1*temp;
  }

  Serial.print(temp);
  Serial.print("\n");

  if (Firebase.ready()) 
  {
    
    //Firebase.setInt(fbdo, main, 5);
    Firebase.setFloat(fbdo, "/data/temp", temp);

  }

  if (Firebase.getString(fbdo, "data/set_temp")) {

      String val = fbdo.stringData();
      float set_temp = val.toFloat();
  }
  delay(1000);
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
