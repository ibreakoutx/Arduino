#include <ESP8266_AESLib.h>

AESLib aesLib;

String decMsg = "                                      " ;
void setup() {
    Serial.begin(115200);
    Serial.println("\nBooting...");  
  
  byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
    
  // put your setup code here, to run once:
  byte my_iv[N_BLOCK] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  aesLib.gen_iv(my_iv);

  Serial.println("AES Key: ");
  int i;
  for (i = 0; i < sizeof(my_iv); i++)
  {
     Serial.print(my_iv[i], DEC); 
     Serial.print(",");
  }

  Serial.println("");

  String msg = "i like dogs";
  
  unsigned long ms = micros ();
  String encMsg = aesLib.encrypt(msg, key, my_iv);
  Serial.print("Encryption took: ");
  Serial.println(micros() - ms);  
  Serial.println("encMsg: " + encMsg);  

  //decMsg = aesLib.decrypt(encMsg, key, my_iv);

  Serial.println("decMsg: " + decMsg);  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
