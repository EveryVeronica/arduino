// ===================== LoRa.h =====================
#ifndef LORA_H
#define LORA_H
#include <RH_RF95.h>

// Pin define (ปรับตามบอร์ดที่ใช้)
#define RFM95_CS D8
#define RFM95_RST D0
#define RFM95_INT D1
#define RF95_FREQ 433.0

// IO Pin
#define ALERT_PIN D3
#define BUILTIN_LED 2
#define RELAY_PIN D2

// Network define
#define NETWORK_ID 0x01
#define NODE_ID 0x01
#define TARGET_ID 0x01

// Message type
#define MSG_STATUS 0x01
#define MSG_ALERT 0x02
#define MSG_FORWARD 0x03
#define MSG_ACK 0x04


#define HOP_COUNT  0

// Struct packet
// Struct packet
struct LoRaPacket {
  uint8_t networkId;
  uint8_t sourceId;
  uint8_t targetId;
  uint8_t msgType;
  uint8_t hopCount;
  int payload;
} __attribute__((packed));


// ประกาศ extern เอาไว้ให้ไฟล์อื่นเรียกใช้
extern RH_RF95 rf95;

class LoRa {
public:
  void setupHardware();                   // ตั้งค่า pin, reset LoRa
  void setupLoRa();                       // init LoRa module
  bool sendLoRaAlert(LoRaPacket &packet);  // ส่งข้อความ ALERT
  void _get();                        // ตรวจสอบข้อความขาเข้า 
};

#endif // LORA_H
