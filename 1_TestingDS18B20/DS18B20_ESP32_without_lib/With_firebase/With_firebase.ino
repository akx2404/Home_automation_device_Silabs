#include "time.h"
#include<TimeLib.h>
#include<ESP32Time.h>

//json
#include "ArduinoJson.h"

#include "WiFi.h"
#include <FirebaseESP32.h>

const char* WIFI_SSID = "Airel_8308842346";
const char* WIFI_PASSWORD =  "air34689";

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyBrZHcv7wRpuarsakB6TgMFGlQvKHbIDmg"
#define DATABASE_URL "https://ds18b20-a074c-default-rtdb.firebaseio.com/"
#define USER_EMAIL "akx2404@gmail.com"
#define USER_PASSWORD ""

//Define Firebase Data object
FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

void streamCallback(StreamData data)
{
  Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data); //see addons/RTDBHelper.h
  Serial.println();
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
}

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
struct tm timeinfo;
String printLocalTime()
{
  char s[51];
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  strftime(s, 50, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  return String(s);
}

#define TEMP_PIN  4
void OneWireReset(int Pin);
void OWwrite(int Pin, byte d);
byte OWread(int Pin);

void setup() {
    digitalWrite(TEMP_PIN, LOW);
    pinMode(TEMP_PIN, INPUT);
    Serial.begin(115200);
    delay(100);
    //Serial.print("temperature measurement:\n");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  //Firebase.begin(DATABASE_URL, "12cOS7SDVROu16ekFqIzbc06k9En9xhfqgdkqiqj");
  
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, "/data"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
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
  float temp = Whole + Fract*0.01;

  if (SignBit) // If negative
  {
     //Serial.print("-");
     temp = -1*temp;
  }

  if (Firebase.ready())
    {
      FirebaseJson json;
      json.add("Temperature_C", temp);
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data", json) ? "ok" : fbdo.errorReason().c_str());
      json.add("Timestamp", printLocalTime());
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data", json) ? "ok" : fbdo.errorReason().c_str());
    }
  
  delay(5000);
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
