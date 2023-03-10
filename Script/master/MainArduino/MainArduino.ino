// I2C
#include <Wire.h>

#define SLAVE_ADDRESS 0x09 // I2C address of this device
const uint8_t BUFFER_SIZE = 64; // maximum buffer size for incoming messages

char buffer[BUFFER_SIZE]; // buffer to store incoming message
uint8_t buffer_index = 0; // current index in the buffer


//Feuchtigkeitssensor
int sensorPin1 = 0; //Erde
int sensorPin2 = 3; //Wassertank

//Lichtsensor
int lichtPinDigital = 4;

//LED
int ledPinDigital = 1;

//Pumpe
int pumpePinAnalog = 2;
double wateringLength = 0;

// Ampel Inputs:
int ampelGreen = 11;
int ampelYellow = 12;
int ampelRed = 13;
int ampelColor = 0;

double current_light = 0;
double current_humidity = 0;
double current_tank = 0;

//Grenzwerte
double dry = 100;
double wet = 500;

//API Data
double current_rain;
double current_temp;

//Settings:
double Feuchtigkeitsgrenze;
double Lichtgrenze;
double Pumpdauer;
String Lichtaktiv; //TRUE or FALSE

void setup() 
{  
  //Serial Output for testing
  Serial.begin(9600);
  
  //Ampel
  pinMode(ampelGreen, OUTPUT);
  pinMode(ampelYellow, OUTPUT);
  pinMode(ampelRed, OUTPUT);
  pinMode(pumpePinAnalog, OUTPUT);
  pinMode(ledPinDigital, OUTPUT)

  //Init I2C
  Wire.begin(SLAVE_ADDRESS); // initialize I2C communication with the given address
  // Wire.onReceive(receiveEvent); // register the receive event
  // Wire.onRequest(requestEvent); // register the request event

  //Settings preset:
  
  Feuchtigkeitsgrenze = 500;
  Lichtgrenze = 500;
  Pumpdauer = 500;
  Lichtaktiv = "TRUE"; //TRUE or FALSE
}

void loop() 
{
  delay(1000);

  //Get Sensor data  
  UpdateSensorValues();

  //Get API data
  requestWeatherData();

  //Do Gießen Logic
  watering();
  checkTank();
  checkLight();

  //Send Back data

  //Handle Settings received
   if (Serial.available()) {  // check if there are any bytes available on the serial port
    String input = Serial.readStringUntil('\n');  // read the input from the serial port until a newline character is received
    input.trim();  // remove any whitespace characters from the input
    
    // call the method with the input as argument
    if (input.length() > 0) {  // check if the input is not empty
      handleSettingsChange(input);
    }
  }
  Serial.println(current_humidity);
  Serial.println(current_tank);
  Serial.println(current_light);
  Serial.println("---");
}

void watering(){
  if(current_humidity <= dry){
    activatePump();
    if(ampelColor == 0){
      setAmpelYellow();
    }
  }
  else if(current_humidity <= wet){
    if(ampelColor == 1){
      setAmpelGreen();
    }
  }
}

void activatePump(){
  digitalWrite(pumpePinAnalog, HIGH);
  delay(wateringLength);
  digitalWrite(pumpePinAnalog, LOW);
}

void checkTank(){
  if(current_tank > 0){
    digitalWrite(ampelRed, LOW);
  }
  else{
    digitalWrite(ampelRed, HIGH);
  }
}

void checkLight(){
  if(current_light >= 700){
    digitalWrite(ledPinDigital, HIGH);
  }
  else {
    digitalWrite(ledPinDigital, LOW);
  }
}

void setAmpelGreen()
{
   digitalWrite(ampelGreen, HIGH);
   digitalWrite(ampelYellow, LOW);
   ampelColor = 0;
}

void setAmpelYellow()
{
   digitalWrite(ampelGreen, LOW);
   digitalWrite(ampelYellow, HIGH);
   ampelColor = 1;
}

void UpdateSensorValues()
{
  current_humidity = analogRead(sensorPin1);
  current_tank = analogRead(sensorPin2);

  current_light = analogRead(lichtPinAnalog);
}

// Handle Property Settings
void handleSettingsChange(String Input)
{
 Serial.println("New Settings requested: " + Input); 

  String* messageContent = splitString(Input,'=');
  if(sizeof(messageContent) != 2) 
  {
    Serial.println("Error Parsing Settings Command");
  }
  Serial.println("1: " + messageContent[0]);
  Serial.println("2 : " + messageContent[1]);
  
    if(messageContent[1] == NULL) return;
  if(messageContent[0] == "FG")
  {
    Feuchtigkeitsgrenze =  tryParseDouble(messageContent[1]);
  }
  if(messageContent[0] == "Lichtgrenze")
  {
    Lichtgrenze =  tryParseDouble(messageContent[1]);
  }
  if(messageContent[0] == "Pumpdauer")
  {
    Pumpdauer =  tryParseDouble(messageContent[1]);
  }
  if(messageContent[0] == "Lichtaktiv")
  {
    Lichtaktiv =  messageContent[1];
  }
  Serial.println("---");
  Serial.println("New Settings:");
  Serial.println(Feuchtigkeitsgrenze);
  Serial.println(Lichtgrenze);
  Serial.println(Pumpdauer);
  Serial.println(Lichtaktiv); 
  Serial.println("---");
}




// I2C

void sendDataToSlave()
{
  String message = String(current_humidity) + ";" + String(current_tank) + ";" + String(current_light); //build message string
  
  uint8_t message_length = message.length() + 1; // calculate the message length
  
  Wire.beginTransmission(SLAVE_ADDRESS); // start transmission to the slave device
  Wire.write(message_length); // send the message length
  Wire.write(message.c_str(), message_length); // send the message itself
  Wire.endTransmission(); // end transmission
}

void requestWeatherData()
{
  Wire.requestFrom(SLAVE_ADDRESS, 32); // request a maximum of 32 bytes from the slave device

  String res = "";

  while (Wire.available()) {
    char c = Wire.read();
  
    if (c == '\0') { // end of string
      break;
    }
    else{
      res += c;
    }
  }

  String* split = splitString(res, ';');

  String s_temp = split[0];
  String s_rain = split[1];

  current_temp = tryParseDouble(s_temp);
  current_rain = tryParseDouble(s_rain);    
  
  Serial.print("Received Weather Data: " + res);
}

//Helper Methods

String* splitString(String input, char delimiter) {
  String* tokens = new String[16];
  uint8_t token_index = 0;

  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);

    if (c != delimiter) {
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
