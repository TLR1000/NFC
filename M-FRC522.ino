# Setup that works with Wemos D1 D2 & mini
# connections:
# kleur   RFID-RC511  Wemos mini
# groen   SDA         D8
# oranje  SCK         D5
# blauw   MOSI        D7
# paars   MISO        D6
# zwart   GND         G
# geel    RST         D0
# rood    3.3V        3V3

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 16 // RST-PIN for RC522 - RFID - SPI - Modul GPIO16 (D0)
#define SS_PIN 15  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO15 (D8)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  Serial.begin(115200);    // Initialize serial communications
  while (!Serial) {
    // Wait for serial port to connect. Needed for native USB port only
  }
  SPI.begin();             // Init SPI bus
  mfrc522.PCD_Init();      // Init MFRC522
  delay(50);
  Serial.println("Looking for RFID/NFC tags...");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(1000); // Add delay to avoid flooding the serial monitor
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Error reading card serial.");
    delay(1000); // Add delay to avoid flooding the serial monitor
    return;
  }

  // Show the UID of the card
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Show additional information if available
  showCardDetails();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}

void showCardDetails() {
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("PICC type: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Try to read NDEF message
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status = mfrc522.MIFARE_Read(4, buffer, &size);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("NDEF data read successfully:");
    for (byte i = 0; i < size; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
  } else {
    Serial.print("Error reading NDEF data: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
}

