#include "LoRa.h"
#include <SPI.h>

#define PING_TIMEOUT 2000 // ms 
// LoRa instance
RH_RF95 rf95(LORA_CS_PIN, LORA_IRQ_PIN);

void LoRa::setupHardware()
{
    pinMode(LORA_RST_PIN, OUTPUT);
    digitalWrite(LORA_RST_PIN, LOW);
    delay(10);
    digitalWrite(LORA_RST_PIN, HIGH);
    delay(10);
}

void LoRa::setupLoRa()
{
    setupHardware();
    if (!rf95.init())
    {
        Serial.println("LoRa init failed!");
        while (1)
            ;
    }
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed!");
        while (1)
            ;
    }
    rf95.setTxPower(10, false);
    Serial.println("LoRa init OK.");
}

bool LoRa::sendLoRaAlert(LoRaPacket &packet)
{ // ใช้ reference ลด copy
    // สร้าง buffer ขนาดเท่ากับ struct
    uint8_t buf[sizeof(LoRaPacket)];

    // คัดลอกข้อมูล struct ไปยัง buffer
    memcpy(buf, &packet, sizeof(LoRaPacket));

    // ส่ง packet
    bool ok = rf95.send(buf, sizeof(buf));
    if (ok)
        rf95.waitPacketSent(); // รอจนส่งเสร็จจริง
    return ok;                 // คืนค่า true/false
}





bool LoRa::pingNode(LoRaPacket &packet) {
    rf95.send((uint8_t*)&packet, sizeof(packet));
    rf95.waitPacketSent();
    return true; 
}



