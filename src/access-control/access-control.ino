#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "card-group.hpp"

#define CHIP_SELECTION_PIN 10
#define RESET_POWER_DOWN_PIN 9
#define SERVO_CONTROL_PIN 6

#define DEBUG 0

// 卡 UID 定义在文件 card-group.hpp 中
extern const byte card_group[CARD_NUM][UID_SIZE];

// 读卡和舵机
MFRC522 mfrc522(CHIP_SELECTION_PIN, RESET_POWER_DOWN_PIN);
Servo mg996r;

bool checkUID(byte *uid, byte *valid_uid, int size)
{
    for (int i = 0; i < size; ++i)
        if (uid[i] != valid_uid[i])
            return false;
    return true;
}

bool checkCard(byte *uid, int size, byte valid_card[][UID_SIZE], int num)
{
    for (int i = 0; i < num; ++i)
        if (checkUID(uid, valid_card[i], size))
            return true;
    return false;
}

#if DEBUG
void printID(byte *uid, byte size, int base)
{
    Serial.print("Card's UID: ");
	for (byte i = 0; i < size; ++i) {
		Serial.print(uid[i], base);
		Serial.print(' ');
	}
	Serial.println();
}
#endif

void setup()
{
    #if DEBUG
	Serial.begin(9600);
    #endif

    SPI.begin();
    mg996r.attach(SERVO_CONTROL_PIN);
    mg996r.write(0);
    pinMode(SERVO_CONTROL_PIN, OUTPUT);
    return;
}

void loop()
{
    // SPI 连接计数
    // 实现每隔一段时间重启 SPI 通信，避免因线被拔而出错
    static int SPI_counter;

    // 检测是否有卡
    if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

        // 打印调试信息
        #if DEBUG
        printID(mfrc522.uid.uidByte, mfrc522.uid.size, HEX);
        #endif

        // 检查卡是否有效
        if (checkCard(mfrc522.uid.uidByte, UID_SIZE, card_group, CARD_NUM)) {
            // 启动舵机开门
            mg996r.write(180);
            delay(3000);
            mg996r.write(0);
        } else {
            delay(3500);
        }
        SPI_counter += 3;

    } else {
        delay(100);
        SPI_counter++;
    }

	// 重启 SPI
    if(SPI_counter >= 20) {
        SPI_counter = 0;
        SPI.end();
        SPI.begin();
        mfrc522.PCD_Init();
    }

    return;
}
