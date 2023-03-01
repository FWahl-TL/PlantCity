// I2C
#include <Wire.h>

const uint8_t SLAVE_ADDRESS = 0x08; // I2C address of this device
const uint8_t BUFFER_SIZE = 64; // maximum buffer size for incoming messages

char buffer[BUFFER_SIZE]; // buffer to store incoming message
uint8_t buffer_index = 0; // current index in the buffer


// Ampel Inputs:
int ampelGreen = 11;
int ampelYellow = 12;
int ampelRed = 13;

//Feuchtigkeitssensor
int sensorPin1 = 4;
int sensorPin2 = 5;

int sensor1Value = 0;
int sensor2Value = 0;

//Lichtsensor
int lichtPinAnalog = 3;
int lichtPinDigital = 10;

int lichtAnalogValue = 0;
int lichtDigitalValue = 0;

//API Data
double current_rain;
double current_temp;

void setup() 
{  
  //Serial Output for testing
  Serial.begin(9600);
  
  //Ampel
  pinMode(ampelGreen, OUTPUT);
  pinMode(ampelYellow, OUTPUT);
  pinMode(ampelRed, OUTPUT);

  //Licht
  pinMode(lichtPinDigital, INPUT);

  //Init I2C
  Wire.begin(SLAVE_ADDRESS); // initialize I2C communication with the given address
  Wire.onReceive(receiveEvent); // register the receive event
  Wire.onRequest(requestEvent); // register the request event
}

void loop() 
{
  delay(1000);

  //Get Sensor data  
  UpdateSensorValues();
  Serial.println(sensor1Value);
  Serial.println(sensor2Value);
  Serial.println(lichtAnalogValue);
  Serial.println(lichtDigitalValue);
  Serial.println("---");

  //Get API data

  //Do Gießen Logic

  //Send Back data

}

void setAmpelGreen()
{
   digitalWrite(ampelGreen, HIGH);
   digitalWrite(ampelYellow, LOW);
   digitalWrite(ampelRed, LOW);
}

void setAmpelYellow()
{
   digitalWrite(ampelGreen, LOW);
   digitalWrite(ampelYellow, HIGH);
   digitalWrite(ampelRed, LOW);
}

void setAmpelRed()
{
   digitalWrite(ampelGreen, LOW);
   digitalWrite(ampelYellow, LOW);
   digitalWrite(ampelRed, HIGH);
}

void UpdateSensorValues()
{
  sensor1Value = analogRead(sensorPin1);
  sensor2Value = analogRead(sensorPin2);

  lichtAnalogValue = analogRead(lichtPinAnalog);
  lichtDigitalValue = digitalRead(lichtPinDigital);
}

// I2C

void sendDataToSlave(int humidity, int tank, int light)
{
  String message = humitidy + ";" + tank + ";" + light; //build message string
  uint8_t message_length = message.length() + 1; // calculate the message length
  
  Wire.beginTransmission(SLAVE_ADDRESS); // start transmission to the slave device
  Wire.write(message_length); // send the message length
  Wire.write(message.c_str(), message_length); // send the message itself
  Wire.endTransmission(); // end transmission
}

void requestWeatherData()
{
  Wire.requestFrom(SLAVE_ADDRESS, 32); // request a maximum of 32 bytes from the slave device

  while (Wire.available()) {
    char c = Wire.read();
  
    if (c == '\0') { // end of string
      break;
    }

    String res = String(c);

    String* split = splitString(res, ';');

    string s_temp = split[0];
    string s_rain = split[1];

    current_temp = tryParseDouble(s_temp);
    current_rain = tryParseDouble(s_rain);    
    
    Serial.print("Received Weather Data: " + c); 
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
