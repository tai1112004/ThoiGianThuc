#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(3000);
  Serial.println("Quet I2C...");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Thay: 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Xong!");
}

void loop() {}