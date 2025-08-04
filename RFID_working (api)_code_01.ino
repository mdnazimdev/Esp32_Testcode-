#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <U8g2lib.h>

// WiFi credentials
const char* ssid = "Ahlam";
const char* password = "ps@1187!";

// RFID pins
#define RST_PIN 27
#define SS_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// OLED (SH1106 128x64)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// API details
String apiKey = "afsgh_uasdas254885jncasdcbhdsc";
String serverUrl = "http://192.168.0.248/save_card_uid.php";

void setup() {
  Serial.begin(115200);

  // Init OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Connecting WiFi...");
  u8g2.sendBuffer();

  // Connect WiFi
  connectToWiFi();

  // Init RFID
  SPI.begin();              
  mfrc522.PCD_Init();       
  delay(1000);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(500);
    return;
  }

  // Read UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.print("UID: ");
  Serial.println(uid);

  // Display UID
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 20, "Card Detected!");
  u8g2.drawStr(10, 40, ("UID: " + uid).c_str());
  u8g2.sendBuffer();

  // Send UID via GET
  sendGET(uid);

  delay(3000);
}

void sendGET(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String fullUrl = serverUrl + "?card_uid=" + uid + "&api_key=" + apiKey;

    Serial.println("GET URL: " + fullUrl);
    http.begin(fullUrl);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpCode));
      Serial.println("Server response: " + response);

      // Display server response
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 20, "Server Response:");
      u8g2.drawStr(0, 40, response.c_str());
      u8g2.sendBuffer();

    } else {
      Serial.println("HTTP GET Error: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "WiFi Connected!");
    u8g2.sendBuffer();
  } else {
    Serial.println("\nWiFi Failed");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "WiFi Failed!");
    u8g2.sendBuffer();
  }
}
