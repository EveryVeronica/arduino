#ifndef LORA_H
#define LORA_H

#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

// ===== Global Config =====
#define LORA_CS_PIN 5
#define LORA_RST_PIN 9
#define LORA_IRQ_PIN 2
#define RF95_FREQ 433.0

// Packet struct
struct LoRaPacket
{
  uint8_t networkId;
  uint8_t sourceId;
  uint8_t targetId;
  uint8_t msgType;
  uint8_t hopCount;
  int payload;
} __attribute__((packed));

// ประกาศ extern เอาไว้ให้ไฟล์อื่นเรียกใช้
extern RH_RF95 rf95;

class LoRa
{
public:
  void setupHardware();                   // ตั้งค่า pin, reset LoRa
  void setupLoRa();                       // init LoRa module
  bool sendLoRaAlert(LoRaPacket &packet); // ส่งข้อความ ALERT
  // ส่ง packet และรอรับ response คืนค่า response ผ่าน reference
  bool sendAndReceiveLoRaAlert(LoRaPacket &packet, LoRaPacket &response, unsigned long timeout = 2000);
  bool pingNode(LoRaPacket &packet); // ส่ง ping ไปยัง nodeId

};

// ประกาศตัวแปร lora ให้ main.cpp ใช้งาน
extern LoRa lora;

#endif
