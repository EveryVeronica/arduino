// ===================== UIManager.cpp =====================
#include "UIManager.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);



void UIManager::begin()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("SSD1306 failed");
        while (1)
            ;
    }

    display.clearDisplay();
    display.setCursor(10, 30);
    display.println(F("LoRa Receiver Ready"));
    display.display();
}

void UIManager::showNodesTable()
{
    display.clearDisplay();
    display.setTextSize(1);

    int top = 0;
    int rowHeight = 12;
    int colX[] = {0, 20, 40, 60, 107, 127};
    // หัวตาราง
    // Column header

    display.fillRect(0, top, 128, 16, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(5, top + 3);
    display.print(F("N-018"));

    display.fillRect(0, 13, 128, 15, SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE);
    // เส้นตาราง
    for (int i = 0; i <= ROWS_PER_PAGE + 1; i++)
    {
        if (i == 1)
        {
            top = 3; // เว้นบรรทัดหัวตาราง
        }
        display.drawLine(0, top + i * rowHeight, SCREEN_WIDTH, top + i * rowHeight, SSD1306_WHITE);
    }

    for (int i = 0; i < 6; i++)
    {
        if ((i >= 1 && i <= 4))
        {
            display.drawLine(colX[i], top + 12, colX[i], top + 12 + rowHeight * (ROWS_PER_PAGE + 1), SSD1306_WHITE);
        }
        else
        {
            display.drawLine(colX[i], 0, colX[i], 0 + rowHeight * (ROWS_PER_PAGE + 1 + 3), SSD1306_WHITE);
        }
    }

    // แสดง Node
    uint8_t totalPages = (nodeCount + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (totalPages == 0)
        totalPages = 1;

    uint8_t start = page * ROWS_PER_PAGE;
    for (uint8_t i = 0; i < ROWS_PER_PAGE; i++)
    {
        uint8_t idx = start + i;
        if (idx >= nodeCount)
            break;

        Packet pkt = nodeData[idx];
      


        unsigned long elapsed = millis() - lastUpdate[idx];
        bool online = elapsed < TIMEOUT_MS;
        int remain = online ? (TIMEOUT_MS - elapsed) / 1000 : 0;


        int y = top + 1 + rowHeight * (i + 1) + 2;
        display.setCursor(2, y);
        display.print(idx + 1);
        display.setCursor(22, y);
        display.print(pkt.source_id);
        display.setCursor(42, y);
        display.print(pkt.payload);
        display.setCursor(62, y);
        if (online)
            display.print(pkt.rssi); // แสดง RSSI
        else
            display.print(F("--")); 
        display.setCursor(112, y);
        display.print(online ? F("ON") : F("--"));
    }

    display.display();
}

void UIManager::alert(unsigned long countdown)
{

    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate >= 1000)
    {
        lastUpdate = millis();
        blinkState = !blinkState;

        if (countdown > 0)
        {
         

            display.fillRect(61, 1, 66, 11, SSD1306_BLACK);
            display.drawLine(107, 1, 107, 11, SSD1306_WHITE);

            display.setTextColor(SSD1306_WHITE);
            display.setCursor(107 + 8, 1 + 2);
            display.print(payload);


            if (blinkState)
            {
                // วาดแถบขาว
                display.fillRect(40, 1, 20, 11, SSD1306_WHITE);

                display.drawLine(40, 1, 40, 11, SSD1306_BLACK);
                display.drawLine(40, 1, 59, 1, SSD1306_BLACK);
                display.drawLine(59, 1, 59, 11, SSD1306_BLACK);
                display.drawLine(40, 11, 59, 11, SSD1306_BLACK);

                display.setTextColor(SSD1306_BLACK);
                display.setCursor(40 + 2, 1 + 2);
                display.print(nodeId);
            }
            else
            {
                // ลบด้วยดำ
                display.fillRect(40, 1, 20, 11, SSD1306_BLACK);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(40 + 2, 1 + 2);
                display.print(nodeId);
            }

            display.setTextColor(SSD1306_WHITE);
            display.setCursor(61 + 2, 1 + 2);
            String text;
            if (countdown >= 1000)
            {
                text = String(countdown / 1000) + "k S";
            }
            else
            {
                text = String(countdown) + " S";
            }

            display.print(text);
        }

        display.display();
    }
}

void UIManager::updateNode(Packet pkt)
{
    for (uint8_t i = 0; i < nodeCount; i++)
    {
        if (nodeData[i].source_id == pkt.source_id)
        {
            nodeData[i] = pkt;
            lastUpdate[i] = millis();
            return;
        }
    }
    if (nodeCount < MAX_NODES)
    {
        nodeData[nodeCount] = pkt;
        lastUpdate[nodeCount] = millis();
        nodeCount++;
    }
}