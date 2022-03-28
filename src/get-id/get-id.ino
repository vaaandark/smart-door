#include <SPI.h>
#include <MFRC522.h>

#define CHIP_SELECTION_PIN 10
#define RESET_POWER_DOWN_PIN 9

MFRC522 rfid(CHIP_SELECTION_PIN, RESET_POWER_DOWN_PIN);

byte uid[4];

void setup() {
	Serial.begin(9600);
	SPI.begin();
	rfid.PCD_Init();
}

void printID(byte *uid_arr, byte size, int base)
{
	for (byte i = 0; i < size; ++i)
		Serial.print(uid_arr[i], base);
	Serial.println();
}

void loop() {
	if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
		for (byte i = 0; i < 4; ++i)
			uid[i] = rfid.uid.uidByte[i];

		Serial.print("UID (hex) :");
		printID(rfid.uid.uidByte, rfid.uid.size, HEX);

		Serial.print("UID (dec) : ");
		printID(rfid.uid.uidByte, rfid.uid.size, DEC);

		rfid.PICC_HaltA();
	}
}
