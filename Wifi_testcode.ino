#include <WiFi.h>
#include <PubSubClient.h>
#include <Update.h>

const char* ssid = "Day Dreamers";
const char* password = "12121212";
const char* mqtt_server = "192.168.0.228";  // Replace with your PC IP
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void performOTA(uint8_t* data, size_t len) {
  Serial.println("[OTA] Start updating");
  if (!Update.begin(len)) {
    Serial.println("[OTA] Update begin failed");
    return;
  }

  size_t written = Update.write(data, len);
  if (written != len) {
    Serial.printf("[OTA] Written %d/%d bytes\n", written, len);
    return;
  }

  if (!Update.end()) {
    Serial.printf("[OTA] Update end failed. Error: %s\n", Update.errorString());
    return;
  }

  if (Update.isFinished()) {
    Serial.println("[OTA] Update complete. Rebooting...");
    ESP.restart();
  } else {
    Serial.println("[OTA] Update not finished.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("[MQTT] Message on %s. Length: %d\n", topic, length);
  if (String(topic) == "/esp32/ota") {
    performOTA(payload, length);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting...");
    if (client.connect("ESP32_SLAVE")) {
      Serial.println("Connected");
      client.subscribe("/esp32/ota");
    } else {
      Serial.print("Failed. Retry in 2s. State=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
