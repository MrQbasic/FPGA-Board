#include <Wire.h>

#define EEPROM_ADDR 0x50

void setup() {
  Serial.begin(115200);
  Wire.begin(35, 34); // SDA = GPIO 35, SCL = GPIO 34
  
  delay(1000);
  Serial.println("Writing 0xC0 to EEPROM byte 0x00...");

  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((uint8_t)0x00); // Target memory address
  Wire.write((uint8_t)0xC0); // Magic header byte for CY7C68013A
  
  uint8_t error = Wire.endTransmission();
  delay(10); // Wait for EEPROM write cycle to complete

  if (error == 0) {
    Serial.println("Success! Byte 0x00 written as 0xC0.");
    
    // Read back to verify
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)EEPROM_ADDR, (uint8_t)1);
    if (Wire.available()) {
      uint8_t val = Wire.read();
      Serial.print("Verified Byte 0x00 = 0x");
      if (val < 0x10) Serial.print("0");
      Serial.println(val, HEX);
    }
  } else {
    Serial.print("I2C Write Failed. Error code: ");
    Serial.println(error);
  }
}

void loop() {}