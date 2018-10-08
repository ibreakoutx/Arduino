void setup() {
  Serial.begin(9600);
}
 
void loop() {
  while(Serial.available()) {
   char incoming = Serial.read();
   Serial.print(incoming);
  }
}
