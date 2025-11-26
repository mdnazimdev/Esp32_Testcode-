#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

const char* ssid = "BASE1_2G";
const char* password = "LB@AH@base1PS!";

WebServer server(80);
const char* otaUser = "admin";
const char* otaPass = "1234";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, []() {
    if (!server.authenticate(otaUser, otaPass)) return server.requestAuthentication();
    server.send(200, "text/html",
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='firmware'>"
      "<input type='submit' value='Update'>"
      "</form>"
    );
  });

  server.on("/update", HTTP_POST, []() { server.send(200); }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.println("Update Start: " + upload.filename);
      Update.begin(UPDATE_SIZE_UNKNOWN);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Update.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      Update.end(true);
      Serial.println("Update Success! Rebooting...");
      ESP.restart();
    }
  });

  server.begin();
  Serial.println("OTA ready at IP: " + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
}
