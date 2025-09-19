#ifndef LORA_GATEWAY_H
#define LORA_GATEWAY_H
#include <RH_RF95.h>

// เลือกขาตามบอร์ดอัตโนมัติ
#if defined(ESP8266)
#define RFM95_CS D8
#define RFM95_RST D0
#define RFM95_INT D1

#elif defined(ESP32)
#define RFM95_CS 5
#define RFM95_RST 9
#define RFM95_INT 2

#elif defined(__SAM3X8E__) // Arduino Due
#define RFM95_CS 5
#define RFM95_RST 9
#define RFM95_INT 2

#else
#error "Please define pin mapping for your board!"
#endif

#define RF95_FREQ 433.0

// ================= Message Types =================
enum MsgType : uint8_t {
  MSG_STATUS  = 0x01,
  MSG_ALERT   = 0x02,
  MSG_FORWARD = 0x03,
  MSG_ACK     = 0x04,
  MSG_NONE    = 0xFF  // ไม่มีข้อความ
};

// ================= Packet Struct =================
struct Packet {
  uint8_t network_id;
  uint8_t source_id;
  uint8_t target_id;
  uint8_t msgType;
  uint8_t hopCount;
  int payload;  // 4 bytes บน ESP32 / ESP8266
  int8_t rssi; // เพิ่ม RSSI
} __attribute__((packed));
// ประกาศ extern
extern Packet Pk;
extern RH_RF95 rf95;

class LoRaGateway
{
public:
  void setupHardware(); 
  void setupLoRa();     
  bool runEvery(unsigned long delayMs);
  Packet LoRa_rxMode();             
  bool LoRa_txMode(Packet &packet); 
};

#endif // LORA_GATEWAY_H
