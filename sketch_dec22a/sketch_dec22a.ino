#define WAKE_DEEP_SLEEP_PIN      13


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(WAKE_DEEP_SLEEP_PIN, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.printf("PIN : %d\n", digitalRead(WAKE_DEEP_SLEEP_PIN));
  

}
