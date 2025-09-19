#include "UIManager.h"
#include "LoRaGateway.h"

#define MAX_NODES 20
#define NODE_TIMEOUT 2000UL // รอ 5 วินาที

// ========== Global Variables ==========
unsigned long countdown = 0;             // ตัวนับถอยหลัง
static unsigned long lastDraw = 0;       // ตัวจับเวลา UI
bool isRunning = false;                  // สถานะการทำงาน
int currentIndex = 0;
unsigned long lastPingTime = 0;

// Network define
#define DEFAULT_NETWORK_ID 0x00
#define DEFAULT_SERVER_ID 0x0A

LoRaGateway lora;
UIManager ui;

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println("=== Server Example 02 ===");

    ui.begin();
    lora.setupLoRa();
}

void loop()
{
    unsigned long now = millis();

    uint8_t nodeIDs[MAX_NODES] = {0x01, 0x02, 0x03, 0x04, 0x05,
                                  0x06, 0x07, 0x08, 0x09, 0x0A,
                                  0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                  0x10, 0x11, 0x12, 0x13, 0x14};



    // ส่ง ping Node ทีละตัว หรือ timeout
    if (!isRunning || now - lastPingTime > NODE_TIMEOUT)
    {
        // ตรวจสอบ timeout
        if (isRunning && now - lastPingTime > NODE_TIMEOUT)
        {
            Serial.print("Node ");
            Serial.print(nodeIDs[currentIndex]);
            Serial.println(" timeout!");
            currentIndex++;
            if (currentIndex >= MAX_NODES) currentIndex = 0;
            isRunning = false; // reset เพื่อส่ง Node ถัดไป
        }

        lastPingTime = now;

        Pk.target_id = nodeIDs[currentIndex];
        Pk.payload = 2;//วินาที
        Pk.network_id = DEFAULT_NETWORK_ID;
        Pk.source_id = DEFAULT_SERVER_ID;
        Pk.msgType = MSG_ACK;

        lora.LoRa_txMode(Pk);

        Serial.print("=== ส่ง ping Node ");
        Serial.print(Pk.target_id);
        Serial.print(" payload ");
        Serial.println(Pk.payload);

        isRunning = true;
    }

    // ตรวจสอบข้อความขาเข้า
    Packet data = lora.LoRa_rxMode();

    if (data.target_id == DEFAULT_SERVER_ID)
    {
        // รับข้อความแล้ว
        isRunning = false;
        Serial.println("=== ได้รับข้อมูลจาก Node ===");
        Serial.print("target_id: "); Serial.println(data.target_id);
        Serial.print("Node ID: "); Serial.println(data.source_id);
        Serial.print("Type: "); Serial.println(data.msgType);
        Serial.print("Payload: "); Serial.println(data.payload);

        if (data.msgType == MSG_STATUS)
        {
            ui.nodeId = data.source_id;
            ui.updateNode(data);

            currentIndex++;
            if (currentIndex >= MAX_NODES) currentIndex = 0; // วน Node ใหม่
        }
        else if (data.msgType == MSG_ALERT)
        {
            ui.nodeId = data.source_id;
            ui.payload = data.payload;


            if(data.payload == 0) {
                // กรณีผิดปกติ
                countdown = 60UL * 5; // ตั้งเวลา 5 นาที
            } else {
                // กรณีปกติ
                countdown = 0; // ยกเลิกการนับถอยหลัง
            }   
         
        }
    }

    // แสดงผล UI ทุก 1 วินาที
    if (now - lastDraw > 1000)
    {
        lastDraw = now;
        ui.showNodesTable();
        ui.alert(countdown);
    }
}
