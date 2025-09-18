
#include "modules/LoRa.h"
#include "modules/UIManager.h"

// ========== Global Variables ==========
unsigned long countdown = 0; // ตัวนับถอยหลัง

LoRa lora;
UIManager ui;
LoRaPacket pktSend;

#define NETWORK_ID 0x01
#define NODE_ID 0x00
/* void simulateNodes()
{
  static unsigned long lastSim = 0;
  if (millis() - lastSim > 1000)
  { // ทุก 2 วินาที สร้าง packet ใหม่
    lastSim = millis();

    LoRaPacket pkt;
    pkt.networkId = 1;
    pkt.sourceId = random(0, 20); // Node ID 1-9
    pkt.targetId = 0;
    pkt.msgType = random(0, 2);
    pkt.hopCount = random(0, 5);
    pkt.payload = random(0, 2); // ค่าจำลอง

      
      
      nodeData เหมือนรับจาก LoRa จริง
  }
} */

void setup()
{
  Serial.begin(115200);

  lora.setupLoRa();
  ui.begin();
}

void loop()
{

  // รับ packet
  if (rf95.available())
  {
    LoRaPacket packet;
    uint8_t buf[sizeof(LoRaPacket)];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      if (len == sizeof(LoRaPacket))
      {
        memcpy(&packet, buf, sizeof(packet));

        if(packet.msgType == 0x03) {
           ui.updateNode(packet); // อัปเดตข้อมูล nodeData
        }else{
           ui.nodeId = packet.sourceId; // เก็บ Node ID ล่าสุดที่รับมา
           ui.payload = packet.payload; // เก็บ Payload ล่าสุดที่รับมา
           countdown += 60UL * 5;       // เพิ่มเวลา 5 นาที
        }
      }
    }
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

    pktSend.networkId = NETWORK_ID;
    pktSend.sourceId = 0x00; // Server ID
    pktSend.msgType = 0x03; // ask for status

    for (uint8_t i = 1; i <= 20; i++) { // nodeId 1 ถึง 20
        if (millis() - lastPing[i] > 1000) { // ping ทุก 1 วินาที
            lastPing[i] = millis();
            pktSend.targetId = i;   // ส่ง nodeId
            lora.pingNode(pktSend); // ส่ง ping
        }
    }
}

  // แสดงผลทุก 1 วินาที
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 1000)
  {
    lastDraw = millis();
    ui.showNodesTable();
    ui.alert(countdown);
  }
}
