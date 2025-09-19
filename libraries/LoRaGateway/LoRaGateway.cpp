#include "LoRaGateway.h"
#include <SPI.h>


// ================= Global Variables =================
// ตัวแปรจริง พร้อมค่า default
Packet Pk = {
    0xFF,      // network_id (unknown)
    0xFF,      // source_id
    0xFF,      // target_id
    MSG_NONE,  // msgType
    0x00,      // hopCount
    0x00,       // payload
    0x00       // rssi
};


// กำหนด instance จริงของ rf95 (ต้องประกาศใน .cpp)
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void LoRaGateway::setupHardware()
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
}

void LoRaGateway::setupLoRa()
{
    setupHardware();
    if (!rf95.init())
    {
        Serial.println("LoRa init failed!");
        while (1);
    }
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed!");
        while (1);
    }
    rf95.setTxPower(10, false);
    Serial.println("LoRa init OK.");
}

bool LoRaGateway::runEvery(unsigned long delayMs) {
    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate >= delayMs){
        lastUpdate = millis();
        Serial.println(lastUpdate);
        return true;
    }

    return false;
}



bool LoRaGateway::LoRa_txMode(Packet &data)
{
    // buffer ขนาดเท่ากับ struct
    uint8_t buf[sizeof(Packet)];
    memcpy(buf, &data, sizeof(Packet));

    bool ok = rf95.send(buf, sizeof(buf));
    if (ok)
        rf95.waitPacketSent(); // รอจนส่งเสร็จจริง

    return ok;
}

Packet LoRaGateway::LoRa_rxMode()
{
    // เริ่มด้วย default
    Packet data = {
        0xFF,      // network_id
        0xFF,      // source_id
        0xFF,      // target_id
        MSG_NONE,  // msgType
        0x00,      // hopCount
        0x00,       // payload
        0x00       // rssi
    };
    memset(&data, 0, sizeof(Packet)); // เคลียร์ struct ป้องกันค่าขยะ

    if (rf95.available())
    {
        uint8_t buf[sizeof(Packet)] = {0};
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {
            if (len == sizeof(Packet))
            {
                memcpy(&data, buf, sizeof(Packet));
                data.rssi = rf95.lastRssi(); // ใส่ค่า RSSI ลง Packet
            }
        }
    }

    return data; // คืนค่า struct ไม่ว่ามีหรือไม่มีข้อมูล
}
