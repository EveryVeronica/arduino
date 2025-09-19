#include "UIManager.h"
#include "LoRaGateway.h"

#define MAX_NODES 20
#define NODE_TIMEOUT 2000UL // รอ 5 วินาที

// ========== Global Variables ==========
unsigned long countdown = 0;       // ตัวนับถอยหลัง
static unsigned long lastDraw = 0; // ตัวจับเวลา UI
bool isRunning = false;            // สถานะการทำงาน
int currentIndex = 0;
unsigned long lastPingTime = 0;

// Network define
#define DEFAULT_NETWORK_ID 0x00
#define DEFAULT_SERVER_ID 0x0A
#define RELAY_PIN 7
LoRaGateway lora;
UIManager ui;


// ------------------- ฟังก์ชันจัดการ LED/Relay -------------------
void setOutputs(bool on)
{
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}


bool ping()
{
    unsigned long now = millis();
    if (now - lastPingTime > NODE_TIMEOUT)
    {
        lastPingTime = now;
        return true;
    }
    else
    {
        return false;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println("=== Server Example 02 ===");

    ui.begin();

    pinMode(RELAY_PIN, OUTPUT);
    setOutputs(false); // ปิด LED/Relay
    lora.setupLoRa();
    Pk.network_id = DEFAULT_NETWORK_ID;
    Pk.source_id = DEFAULT_SERVER_ID;
}

void loop()
{

    Packet data = lora.LoRa_rxMode(); // ตรวจสอบข้อความขาเข้า
    uint8_t nodeIDs[MAX_NODES] = {0x01, 0x02, 0x03, 0x04, 0x05,
                                  0x06, 0x07, 0x08, 0x09, 0x0A,
                                  0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                  0x10, 0x11, 0x12, 0x13, 0x14};

    if (!isRunning || ping()) // ส่ง ping ทีละตัว หรือ timeout
    {
        Pk.target_id = nodeIDs[currentIndex];
        Pk.payload = 2; // วินาที
        Pk.msgType = MSG_ACK;
        lora.LoRa_txMode(Pk);
        isRunning = true;
        currentIndex++;
        Serial.println("=== ส่ง ping ===");
        Serial.println(Pk.target_id);
        Serial.println("=== payload ===");
        Serial.println(Pk.payload);
    }
    else
    {
        if (data.target_id == DEFAULT_SERVER_ID)
        {

            if (data.msgType == MSG_STATUS)
            {
                isRunning = false;
                ui.nodeId = data.source_id;
                ui.updateNode(data);
                Serial.println("=== ได้รับข้อมูลจาก Node ===");
                Serial.print("target_id: ");
                Serial.println(data.target_id);
                Serial.print("Node ID: ");
                Serial.println(data.source_id);
                Serial.print("Type: ");
                Serial.println(data.msgType);
                Serial.print("Payload: ");
                Serial.println(data.payload);
            }
            else if (data.msgType == MSG_ALERT)
            {
                ui.nodeId = data.source_id;
                ui.payload = data.payload;

                switch (data.payload)
                {
                case 0:
                    setOutputs(true); // เปิด LED/Relay
                    countdown = 60UL * 1; // ตั้งเวลา 1 นาที     // มีเหตุการณ์
                    break;

                case 1:
                    countdown = 60UL * 1; // ตั้งเวลา 1 นาที     // กรณีปกติ
                    break;

                case 2:
                    countdown = 60UL * 1; // ตั้งเวลา 1 นาที     // เฝ้าระวัง
                    break;
                }
            }
            else if (data.msgType == MSG_ACK)
            {
                countdown = 0; // ยกเลิกนับถอยหลัง
            }
        }
    }

    if (currentIndex >= MAX_NODES)
    {

        currentIndex = 0; // วน Node ใหม่
    }








  // เปลี่ยนหน้าอัตโนมัติทุก 10 วินาที
static unsigned long lastPageChange = 0;
static unsigned long lastPing[21] = {0}; // lastPing[1] ถึง lastPing[20]

if (millis() - lastPageChange > 10000)
{
    lastPageChange = millis();

    uint8_t totalPages = (ui.nodeCount + ui.ROWS_PER_PAGE - 1) / ui.ROWS_PER_PAGE;
    if (totalPages == 0) totalPages = 1;

    ui.page++;
    if (ui.page >= totalPages) ui.page = 0;

}





















    if (millis() - lastDraw > 1000) // แสดงผล UI ทุก 1 วินาที
    {
        

        if(countdown > 0 ){
           countdown--;
        }
        lastDraw = millis();
        ui.showNodesTable();
        ui.alert(countdown);
    }
}
