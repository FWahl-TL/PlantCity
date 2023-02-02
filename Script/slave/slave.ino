#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "ESP8266HTTPClient.h"

//Wlan
const char* ssid = "WLAN-697082"; // Enter your WiFi SSID
const char* password = "rsps2725"; // Enter Password

//Weather Api
const char* apiKey = "43b9340612ef48839e0a5d2c33643899";
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
// WiFi.begin(ssid, password, 0, NULL, true);
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
    delay(60000); //3 minutes
  }
  else
  {
    Serial.println("Waiting for Wifi...");    
  }

}

void sendData(StaticJsonDocument<200> jsonDoc){
  HTTPClient http;
  WiFiClient wifi;
  http.begin(wifi, "http://api.florianwahl.digital/new");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonDoc.as<String>());
  if (httpCode > 0) {
  Serial.printf("[HTTP] POST... code: %d\n", httpCode);
  String payload = http.getString();
  Serial.println(payload);
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

void getCurrentWeatherData(){
  String url = String("http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=" + String(units) + "&appid=" + String(apiKey));
  WiFiClient wifi;
  HTTPClient http;
  http.begin(wifi, url);
  int httpCode = http.GET();
  if (httpCode = 200) {
    String payload = http.getString();
    Serial.println(payload);
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);
    current_temp = doc["main"]["temp"];
    current_rain = doc["rain"]["1h"];
    Serial.println("request success Temp: " + String(current_temp) + " rain: " + String(current_rain));
  } else {
    Serial.println("Error on HTTP request: " + String(httpCode));
  }
  http.end();
}

