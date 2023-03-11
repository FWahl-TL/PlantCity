#include <ESP8266WiFi.h>        // Include the Wi-Fi library

const char* ssid     = "OnePlus";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "wasdwasd";     // The password of the Wi-Fi network

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  delay(10);
  Serial.println('\n');
  
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() { }