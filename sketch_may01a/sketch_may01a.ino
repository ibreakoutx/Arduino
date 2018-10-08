/* Include the crypto library into your project */
#include <Crypto.h>
#include <Crypto_Utils.h>

/* The length of the key we will use for this HMAC */
/* The key can be of any length, 16 and 32 are common */
#define KEY_LENGTH 16

/* Define our */
byte key[KEY_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
byte iv[KEY_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Create the HMAC instance with our key */
SHA256HMAC hmac(key, KEY_LENGTH);

AES aes256(key, iv, AES::AES_MODE_256, AES::CIPHER_ENCRYPT);

void setup() {

  Serial.begin(115200);

  /* Update the HMAC with just a plain string (null terminated) */
  hmac.doUpdate("Hello World");

  /* And or with a string and length */
  const char *goodbye = "GoodBye World";
  hmac.doUpdate(goodbye, strlen(goodbye));

  /* And or with a binary message */
  byte message[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  hmac.doUpdate(message, sizeof(message));

  /* Finish the HMAC calculation and return the authentication code */
  byte authCode[SHA256HMAC_SIZE];
  hmac.doFinal(authCode);

  /* authCode now contains our 32 byte authentication code */
  for (byte i; i < SHA256HMAC_SIZE; i++)
  {
    Serial.print(authCode[i], HEX);
  }
}

void loop() {
}

