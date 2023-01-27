#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "ESP8266HTTPClient.h"

//Wlan
const char* ssid = "OnePlus"; // Enter your WiFi SSID
const char* password = "1234asd1234"; // Enter Password

//Weather Api
const char* host = "api.openweathermap.org";
const char* apiKey = "your_API_KEY";
const char* city = "Wuppertal";
const char* units = "metric";
bool weatherRecieved = true;

double measured_humidity = 0;
double measured_tank = 0;
double measured_light = 0;
double current_temp = 0;
double current_rain = 0;

void setup() {
Serial.begin(9600);
// Connect to WiFi
WiFi.begin(ssid, password);
Serial.println("");
Serial.println("");

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print("*");
}
Serial.println("");
Serial.println("WiFi connection successful");
Serial.print("IP Address: ");
Serial.print(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    //Read the sensor values
    measured_humidity = readHumidity();
    measured_tank = readTank();
    measured_light = readLight();
    getCurrentWeatherData();
    //Create a JSON object
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["measured_humidity"] = measured_humidity;
    jsonDoc["measured_tank"] = measured_tank;
    jsonDoc["measured_light"] = measured_light;
    jsonDoc["current_temp"] = current_temp;
    jsonDoc["current_rain"] = current_rain;
    //Send the JSON object to the API
    sendData(jsonDoc);
    delay(180000); //3 minutes
  }
  else
  {
    Serial.println("Waiting for Wifi...", httpCode);    
  }

}

void sendData(StaticJsonDocument<200> jsonDoc){
  HTTPClient http;
  http.begin("api.florianwahl.digital");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonDoc, jsonDoc.measured_humidity);
  if (httpCode > 0) {
  Serial.printf("[HTTP] POST... code: %d\n", httpCode);
  if (httpCode == HTTP_CODE_OK) {
  String payload = http.getString();
  Serial.println(payload);
  }
  } else {
  Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

double readHumidity(){
//Replace with code to read humidity sensor
return 0;
}

double readTank(){
//Replace with code to read tank sensor
return 0;
}

double readLight(){
//Replace with code to read light sensor
return 0;
}

getCurrentWeatherData(){
  weatherRecieved = true;
  char* url = String("/data/2.5/weather?q=") + city + "&units=" + units + "&appid=" + apiKey;

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload);
    current_temp = doc["main"]["temp"];
    current_rain = doc["rain"]["h1"];
    Serial.println("request success Temp: " + current_temp + " rain: " + current_rain);
  } else {
    Serial.println("Error on HTTP request: " + String(httpCode));
  }
  http.end();
  delay(60000);
}

