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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // --- OTA Page ---
  server.on("/", HTTP_GET, []() {
    if (!server.authenticate(otaUser, otaPass))
      return server.requestAuthentication();

    String html =
      "<html><body>"
      "<h2>ESP32 Web OTA Update</h2>"
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='firmware'>"
      "<input type='submit' value='Update Firmware'>"
      "</form>"
      "</body></html>";

    server.send(200, "text/html", html);
  });

  // --- OTA Update Handler ---
  server.on("/update", HTTP_POST,
    []() { 
      server.send(200, "text/plain", "Update complete. ESP restarting...");
    },
    []() {
      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update Start: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      }
      else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      }
      else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.println("Update finished. Rebooting...");
        } else {
          Update.printError(Serial);
        }
        ESP.restart();
      }
    }
  );

  server.begin();
  Serial.println("OTA server started.");
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}
