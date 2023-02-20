#include "OneWire.h"
#include "DallasTemperature.h"
#include "time.h"
#include<TimeLib.h>
#include<ESP32Time.h>

const int oneWireBus = 4;     

OneWire oneWire(oneWireBus);
DallasTemperature temp_sensor(&oneWire);

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
#define USER_PASSWORD "akxakx@giis"

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

void setup() {
  Serial.begin(115200);
  temp_sensor.begin();

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

void loop() {
  temp_sensor.requestTemperatures(); 
  float temperatureC = temp_sensor.getTempCByIndex(0);
  float temperatureF = temp_sensor.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");

  if (Firebase.ready())
    {
      FirebaseJson json;
      json.add("Temperature_C", temperatureC);
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data", json) ? "ok" : fbdo.errorReason().c_str());
      json.add("Temperature_F", temperatureF);
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data", json) ? "ok" : fbdo.errorReason().c_str());
      json.add("Timestamp", printLocalTime());
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data", json) ? "ok" : fbdo.errorReason().c_str());
      
      
    }
  
  delay(2000);
}
