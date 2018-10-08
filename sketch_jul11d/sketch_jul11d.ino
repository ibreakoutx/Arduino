#include <SoftwareSerial.h>

int i = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  i++;
  // put your main code here, to run repeatedly:
  if ( i < 1000) {
    delay(1000);
    Serial.println("AT");
  }

  if (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);
  }
}
