int messwert = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  messwert = analogRead(A0);
  Serial.print("Wert: ");
  Serial.println(messwert);
  delay(500);
}
