#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

// --- WiFi Credentials ---
const char* ssid = "Ahlam";
const char* password = "ps@1187!";

// --- RFID Pins ---
#define RST_PIN 27
#define SS_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- OLED Display (SH1106 128x64) ---
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// --- API Details ---
String apiKey = "afsgh_uasdas254885jncasdcbhdsc";
String serverUrl = "http://192.168.0.248/save_card_uid.php";

// --- Buzzer Pin ---
#define BUZZER_PIN 26

// --- WiFi Check Timer ---
unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 3000;

// --- Function Prototypes ---
void connectToWiFi();
void displayScanPrompt();
void sendGET(String uid);

void setup() {
  Serial.begin(115200);

  // OLED Init
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Connecting WiFi...");
  u8g2.sendBuffer();

  // Buzzer Pin Setup
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to WiFi
  connectToWiFi();

  // RFID Init
  SPI.begin();              
  mfrc522.PCD_Init();       
  delay(1000);

  // Show scan prompt
  displayScanPrompt();
}

void loop() {
  // Check WiFi regularly
  if (millis() - lastWiFiCheck > wifiCheckInterval) {
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi Lost. Reconnecting...");
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(10, 30, "WiFi Lost...");
      u8g2.drawStr(10, 50, "Reconnecting...");
      u8g2.sendBuffer();
      connectToWiFi();
    }
  }

  // RFID scan
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(200);
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

  // Activate buzzer
  tone(BUZZER_PIN, 1000, 500);  // 500 Hz for 1 sec

  // Show UID on display
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 20, "Card Detected!");
  u8g2.drawStr(10, 40, ("UID: " + uid).c_str());
  u8g2.sendBuffer();

  sendGET(uid);

  delay(1000);  // faster scan interval
  displayScanPrompt();
}

void sendGET(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String fullUrl = serverUrl + "?cardUid=" + uid + 
                     "&apiKey=" + apiKey + 
                     "&deviceHash=" + "ABCD" + 
                     "&businessId=" + "1";

    Serial.println("GET URL: " + fullUrl);
    http.begin(fullUrl);

    int httpCode = http.GET();
  //  Serial.println("Server error: " +http.errorToString(httpCode));
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpCode));
      Serial.println("Server response: " + response);
      
// Allocate a buffer and parse
DynamicJsonDocument doc(1024);
DeserializationError error = deserializeJson(doc, response);

if (error) {
  Serial.print("JSON parsing failed: ");
  Serial.println(error.c_str());
  return;
}

// Access values
String status = doc["status"];
String message = doc["message"];
String display_line_1 = doc["display_line_1"];
String display_line_2 = doc["display_line_2"];

Serial.println("Status: " + status);
Serial.println("Message: " + message);
Serial.println("Message: " + display_line_1);



      // Show server response
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
     // u8g2.drawStr(0, 20, "Server Response:");
     // u8g2.drawStr(0, 40, response.c_str());

     u8g2.drawStr(0, 20, message.c_str());
     u8g2.drawStr(0, 40, display_line_1.c_str());
      u8g2.sendBuffer();
    } else {
      Serial.println("HTTP  Error: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Connecting...");
  u8g2.sendBuffer();

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.println(WiFi.localIP());

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "WiFi Connected!");
    u8g2.sendBuffer();
  } else {
    Serial.println("\nFailed to connect.");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "WiFi Failed.");
    u8g2.sendBuffer();
  }
}

void displayScanPrompt() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Scan your card...");
  u8g2.sendBuffer();
}
