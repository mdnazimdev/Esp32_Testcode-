#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Day Dreamers";
const char* password = "12121212";

// Time to wait between scan attempts (ms)
unsigned long scanInterval = 5000;
unsigned long lastScanTime = 0;

void connectToWiFi() {
  Serial.println("Scanning for Wi-Fi...");

  int n = WiFi.scanNetworks();
  bool found = false;

  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i) == ssid) {
      found = true;
      break;
    }
  }

  if (found) {
    Serial.print("Found target Wi-Fi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  } else {
    Serial.println("Target Wi-Fi not found.");
  }

  WiFi.scanDelete(); // Clear RAM used by scan
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Set ESP32 to Station mode
  connectToWiFi();     // Initial connection attempt
}

void loop() {
  // Check connection status
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long currentTime = millis();
    if (currentTime - lastScanTime > scanInterval) {
      Serial.println("Wi-Fi not connected. Rescanning...");
      connectToWiFi();
      lastScanTime = currentTime;
    }
  } else {
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
    delay(10000); // Connected - wait a bit before checking again
  }
}
