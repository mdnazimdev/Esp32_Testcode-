#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <U8g2lib.h>

// === OLED Setup ===
// SDA = GPIO 21, SCL = GPIO 22
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// === RFID Setup ===
// SPI: SCK=18, MISO=19, MOSI=23
#define SS_PIN 4
#define RST_PIN 5
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, "Place RFID Card...");
  u8g2.sendBuffer();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Read UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }

  // Authenticate and read Block 4
  byte block = 4;
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }

  status = rfid.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Read failed");
    return;
  }

  // Convert buffer to string
  String name = "";
  for (int i = 0; i < 16; i++) {
    if (buffer[i] != 0 && buffer[i] != 255 && buffer[i] != 32) {
      name += (char)buffer[i];
    }
  }

  // Serial Output
  Serial.println("====== CARD SCANNED ======");
  Serial.print("UID: "); Serial.println(uid);
  Serial.print("Name: "); Serial.println(name);

  // OLED Output
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Card Detected:");
  u8g2.drawStr(0, 35, ("UID: " + uid).c_str());
  u8g2.drawStr(0, 55, ("Name: " + name).c_str());
  u8g2.sendBuffer();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(3000);
}
