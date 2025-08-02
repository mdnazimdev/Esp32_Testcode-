#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(19, 18); // SDA, SCL
  delay(1000);
  Serial.println("Scanning I2C...");

  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      nDevices++;
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found.");
  else
    Serial.println("Done.");
}

void loop() {}
