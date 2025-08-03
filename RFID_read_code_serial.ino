#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 5
#define SS_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your card to read block 4...");

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return;

  byte block = 4;
  byte buffer[18];
  byte size = sizeof(buffer);

  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)
  );
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.print("Block 4 data: ");
  for (int i = 0; i < 16; i++) {
    Serial.write(buffer[i]);
  }
  Serial.println();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(3000);
}
