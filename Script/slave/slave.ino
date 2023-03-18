#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "ESP8266HTTPClient.h"
#include <Wire.h>

//Wlan
const char* ssid = "OnePlus 7T Pro"; // Enter your WiFi SSID //WLAN-697280 //JoshaPixel //OnePlus 7T Pro
const char* password = "1234567890"; // Enter Password   //rsps2725 //1234567890 //1234567890 

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

//I2C Connection
const uint8_t SLAVE_ADDRESS = 0x08; // I2C address of this device
const uint8_t BUFFER_SIZE = 64; // maximum buffer size for incoming messages
char buffer[BUFFER_SIZE]; // buffer to store incoming message
uint8_t buffer_index = 0; // current index in the buffer

void setup() {
Serial.begin(9600);

//I2C Connection
Wire.begin(SLAVE_ADDRESS); // initialize I2C communication with the given address
Wire.onReceive(receiveEvent); // register the receive event
Wire.onRequest(requestEvent); // register the request event

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
}

void sendData(){
  if (WiFi.status() == WL_CONNECTED) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["measured_humidity"] = measured_humidity;
    jsonDoc["measured_tank"] = measured_tank;
    jsonDoc["measured_light"] = measured_light;
    jsonDoc["current_temp"] = current_temp;
    jsonDoc["current_rain"] = current_rain;
    HTTPClient http;
    WiFiClient wifi;
    http.begin(wifi, "http://api.florianwahl.digital/api/new");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(jsonDoc.as<String>());
    if (httpCode = 200) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
     String payload = http.getString();
     Serial.println(payload);
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else
  {
    Serial.println("Waiting for Wifi...");    
  }
}

void getSensorValues(String message){
  String* values = handleRecieved(message);

  if(sizeof(values) == 3){
    measured_humidity = tryParseDouble(values[0]);
    measured_tank = tryParseDouble(values[1]);
    measured_light = tryParseDouble(values[2]);
  }
  else{
    Serial.println("Antwort vom Master ist nicht vollständig");
    measured_humidity = 0;
    measured_tank = 0;
    measured_light = 0;
  }
}

String* handleRecieved(String input) {
  String* tokens = new String[16];
  uint8_t token_index = 0;

  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);

    if (c != ';') {
      if (tokens[token_index].length() < 15) {
        tokens[token_index] += c;
      }

      if (i == input.length() - 1) {
        token_index++;
      }
    } else {
      token_index++;

      if (token_index >= 16) {
        break;
      }
    }
  }

  return tokens;
}

double tryParseDouble(String input) {
  char buffer[32]; // buffer to hold the string

  // Copy the input string to the buffer and add a null terminator
  input.toCharArray(buffer, sizeof(buffer));

  // Use atof() to convert the string to a double
  double result = atof(buffer);

  // If the result is 0.0, check if the input string is "0" or "-0"
  if (result == 0.0 && !(input.equals("0") || input.equals("-0"))) {
    // Parsing failed, return NaN
    result = NAN;
  }

  return result;
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

void sendCurrentWeatherData(){
  getCurrentWeatherData();
  String message = BuildMessage();
  Wire.write(message.c_str(), message.length());
}

String BuildMessage(){
  return String(current_temp) + ";" + String(current_rain);
}

void receiveEvent(int num_bytes) {
  while (Wire.available()) {
    char c = Wire.read();

    if (buffer_index < BUFFER_SIZE - 1) {
      buffer[buffer_index++] = c;
    }
  }
}

void requestEvent() {
  uint8_t message_length = buffer[0]; // read the message length from the buffer

  if (message_length > 0 && buffer_index == message_length) { // check if the entire message has been received
    buffer[buffer_index] = '\0'; // terminate the string
    
    String message(buffer + 1); // create a String from the buffer, starting at index 1 (after the message length byte)
    
    if(message_length == 0){
      sendCurrentWeatherData();
    }
    else if(message_length > 0){
      getSensorValues(message);
      sendData();
    }

    buffer_index = 0; // reset the buffer index
  }
}
