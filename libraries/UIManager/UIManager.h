// ===================== ui/UIManager.h =====================
#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include "LoRaGateway.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET_PIN -1

#define MAX_NODES 20
#define TIMEOUT_MS 60000 // 60 วิ





// ประกาศ extern เอาไว้ให้ไฟล์อื่นเรียกใช้
extern Adafruit_SSD1306 display;

class UIManager
{
public:
    void begin();
    void showNodesTable();
    void alert(unsigned long countdown);
    void updateNode(Packet pkt);
    bool blinkState = false;
  
    unsigned long lastBlink = 0;
    unsigned long lastUpdate[MAX_NODES];
    const uint8_t ROWS_PER_PAGE = 4;
    uint8_t nodeCount = 0;
    uint8_t page = 0;
    uint8_t nodeId;
    uint8_t payload;
// กำหนดตัวจริงให้ nodeData (ครั้งเดียวในทั้งโปรเจค)
Packet nodeData[MAX_NODES];


private:
};

#endif // UI_MANAGER_H
