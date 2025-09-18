#include "LoRa.h"
#include <SPI.h>

// กำหนด instance จริงของ rf95
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void LoRa::setupHardware()
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
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

/* bool LoRa::sendLoRaAlert(LoRaPacket packet) {
    uint8_t message[] = {NETWORK_ID, NODE_ID, 0x02};
   return rf95.send(message, sizeof(message));

}
 */

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


bool LoRa::alert(uint8_t target_id){
    LoRaPacket data;
    data.smgType = 0x02;
    data.targetId = target_id;
    return sendData(data);
}


bool LoRa::sendData (LoRaPacket &data){
    // สร้าง buffer ขนาดเท่ากับ struct
    uint8_t buf[sizeof(LoRaPacket)];
    //คัดลอกข้อมูล struct ไปยัง buffer
    memcpy(buf,&data,sizeof(LoRaPacket));
    bool ok = rf95.send(buf, sizeof(buf));
    if (ok)
        rf95.waitPacketSent(); // รอจนส่งเสร็จจริง

    return ok; //คืนค่า true / false
}

LoRaPacket LoRa::receiveData(){
    LoRaPacket data;

    if (rf95.available())
    {   uint8_t buf[sizeof(LoRaPacket)] ={0}:
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {

            if (len == sizeof(LoRaPacket))
            {
                memcpy(&data,buf,sizeof(LoRaPacket));
                return data
            }
            
            /* code */
        }
        
        /* code */
    }
    
}

void LoRa::_get()
{
    if (rf95.available())
    {
        uint8_t buf[sizeof(LoRaPacket)] = {0};
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {
            if (len == sizeof(LoRaPacket))
            {
                LoRaPacket pkt;
                memcpy(&pkt, buf, sizeof(LoRaPacket));

                // ส่งตอบกลับทันที

                if (pkt.targetId)
                {

                pkt.msgType = 0x03;          // กำหนด msgType เป็น ACK ตัวอย่าง
                pkt.targetId = pkt.sourceId; // กำหนด targetId เป็น ID ของผู้ส่ง
                pkt.sourceId = NODE_ID;      // กำหนด sourceId เป็น ID ของเรา
                pkt.hopCount = HOP_COUNT;    // กำหนด hopCount เป็น

             // ส่ง ACK
                if (rf95.send((uint8_t*)&pkt, sizeof(pkt)))
                {
                    rf95.waitPacketSent();
                }

   
                }


            }
            else
            {
                Serial.println(F("Warning: Received packet size mismatch"));
            }
        }
    }
}
