#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22  // Reset pin
#define SS_PIN  5   // SDA pin

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Sample UID to Name Map
struct User {
  byte uid[4];
  const char* name;
};

User users[] = {
  {{0xDE, 0xAD, 0xBE, 0xEF}, "Alice"},
  {{0x12, 0x34, 0x56, 0x78}, "Bob"},
};

void setup() {
  Serial.begin(115200);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  Serial.println("Place your RFID card...");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Check for known UID
  const char* name = "Unknown";
  for (int i = 0; i < sizeof(users)/sizeof(users[0]); i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (users[i].uid[j] != mfrc522.uid.uidByte[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      name = users[i].name;
      break;
    }
  }

  Serial.print("Welcome, ");
  Serial.println(name);
  delay(2000);

  mfrc522.PICC_HaltA();  // Stop reading
}
