#include <Wire.h>

#define I2C_ADDR    0x50  // Base address (A0=GND, A1=GND, A2=GND)
#define EEPROM_SIZE 256   // 2 Kbit = 256 Bytes

void printHexByte(uint8_t b) {
  if (b < 0x10) Serial.print("0");
  Serial.print(b, HEX);
}

void dumpAT24C02() {
  Serial.println("\n--- AT24C02 EEPROM Dump (256 Bytes) ---");
  Serial.println("Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  |ASCII|");
  Serial.println("-------------------------------------------------------------------");

  for (uint16_t addr = 0; addr < EEPROM_SIZE; addr += 16) {
    // 1. Send single-byte memory address pointer
    Wire.beginTransmission(I2C_ADDR);
    Wire.write((uint8_t)addr); // Only 8-bit address byte required
    if (Wire.endTransmission() != 0) {
      Serial.println("\n[ERROR] Bus read failure during transmission!");
      return;
    }

    // 2. Read 16-byte chunk
    uint8_t buffer[16];
    uint8_t bytesRead = Wire.requestFrom((uint8_t)I2C_ADDR, (uint8_t)16);
    for (uint8_t i = 0; i < bytesRead; i++) {
      buffer[i] = Wire.read();
    }

    // 3. Print 4-digit Offset
    if (addr < 0x0100) Serial.print("0");
    if (addr < 0x0010) Serial.print("0");
    Serial.print(addr, HEX);
    Serial.print(": ");

    // 4. Output Hex Bytes
    for (uint8_t i = 0; i < 16; i++) {
      if (i < bytesRead) {
        printHexByte(buffer[i]);
        Serial.print(" ");
      } else {
        Serial.print("   ");
      }
    }

    Serial.print(" |");

    // 5. Output Printable ASCII
    for (uint8_t i = 0; i < bytesRead; i++) {
      char c = buffer[i];
      if (c >= 32 && c <= 126) {
        Serial.print(c);
      } else {
        Serial.print('.');
      }
    }
    Serial.println("|");
  }

  Serial.println("-------------------------------------------------------------------");
  Serial.println("Dump complete.");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(35, 34); // SDA = GPIO 35, SCL = GPIO 34

  Serial.print("Searching for AT24C02 at 0x");
  Serial.print(I2C_ADDR, HEX);
  Serial.println("...");

  // Poll until detected
  while (true) {
    Wire.beginTransmission(I2C_ADDR);
    if (Wire.endTransmission() == 0) {
      Serial.println("AT24C02 detected!");
      break;
    }
    Serial.print(".");
    delay(500);
  }

  dumpAT24C02();
}

void loop() {
  // Idle after execution
  delay(1000);
}