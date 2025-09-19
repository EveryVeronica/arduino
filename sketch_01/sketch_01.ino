
#include "LoRaGateway.h"
// Network define
#define DEFAULT_NETWORK_ID 0x00
#define DEFAULT_NODE_ID 0x01

// IO Pin
#define ALERT_PIN D3
#define BUILTIN_LED 2
#define RELAY_PIN D2

LoRaGateway lora;

#define STATUS_INTERVAL 5000        // 5 วิ
#define ALERT_REPEAT_INTERVAL 10000 // 10 วิ
#define ALERT_REPEAT_MAX 10
#define SHORT_PRESS_THRESHOLD 10000 // 10 วิ

unsigned long lastStatus = 0;
unsigned long lastAlert = 0;
bool isRelease = false;
bool longAlertActive = false;
int longAlertCount = 0;

unsigned long releaseTime = 0;
bool alertActive = false;
bool isRunning = false;
unsigned long countdown = 0; // ตัวนับถอยหลัง

// ------------------- ฟังก์ชันจัดการ LED/Relay -------------------
void setOutputs(bool on)
{
    digitalWrite(BUILTIN_LED, on ? LOW : HIGH);
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println("=== Node Example 02 ===");
    pinMode(ALERT_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    setOutputs(false); // ปิด LED/Relay
    lora.setupLoRa();

    Pk.network_id = DEFAULT_NETWORK_ID;
    Pk.source_id = DEFAULT_NODE_ID;
    Pk.target_id = 0x0A; // Server ID
}

// ------------------- loop -------------------
void loop()
{
    bool currentState = digitalRead(ALERT_PIN); // LOW = กด, HIGH = ปล่อย
    unsigned long currentTime = millis();

    // ตรวจสอบข้อความขาเข้า
    Packet data = lora.LoRa_rxMode();

    if (data.source_id == 0x0A && data.msgType == MSG_ACK && data.target_id == DEFAULT_NODE_ID)
    {
        countdown = data.payload;
        Serial.println("=== ได้รับคำสั่งจาก Server ===");
    }

    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate >= 1000)
    {
        lastUpdate = millis();

        if (countdown > 0)
        {
            countdown--;
            isRunning = false;
            Serial.println("=== นับถอยหลัง: " + String(countdown) + " วินาที ===");
        }
        else
        {

            if (!isRunning && countdown == 0)
            {

                if (currentState == LOW)
                {
                    Pk.msgType = MSG_ACK ; // ask for status
                    lora.LoRa_txMode(Pk);
                    

                    Pk.payload = 1; //ปกติ

                }
                else
                {
                    Pk.payload = 0; // 0 ผิดปกติ
                }

                Pk.msgType = MSG_STATUS; // ask for status
                lora.LoRa_txMode(Pk);
                isRunning = true;
                Serial.println("=== ส่ง Status ===");
            }
        }
    }

    if (currentState == LOW)
    {
        isRelease = false;
        alertActive = false;
        Pk.payload = 1; // 1 ปกติ
        setOutputs(false); // เปิด LED/Relay
    }

    if (currentState == HIGH)
    {
        isRelease = true;
        Pk.payload = 0; // 0 ผิดปกติ
        if (!alertActive)
        {
            alertActive = true;
            releaseTime = millis() + 5000UL; // อีก 5 วิ
        }
    }

    if (isRelease && millis() < releaseTime)
    {
        // ปล่อยไม่เกิน 5 วิ

        // ส่งทุก 1 วิ
        if (currentTime - lastAlert >= 1000)
        {
            lastAlert = currentTime;
            longAlertCount = 0;
            Pk.payload = 2; // 2 ผิดปกติ
            Pk.hopCount = longAlertCount;
            Pk.msgType = MSG_ALERT;
            lora.LoRa_txMode(Pk);
            Serial.println("=== ส่ง Alert ===");

            for (size_t i = 0; i < 5; i++)
            {
                setOutputs(true); // เปิด LED/Relay
                delay(500);
                setOutputs(false); // ปิด LED/Relay
                delay(500);
                
            }
            
        }
    }
    else
    {
        // ปล่อยค้างเกิน 5 วิ

        // ส่งทุก 10 วิ
        if (isRelease && currentTime - lastAlert >= ALERT_REPEAT_INTERVAL)
        {

            longAlertCount++;
            lastAlert = currentTime;
            Pk.hopCount = longAlertCount;
            Pk.msgType = MSG_ALERT; // ask for status
            lora.LoRa_txMode(Pk);
            Serial.println("=== ส่ง Alert ===");
            setOutputs(true); // เปิด LED/Relay
        }
    }
}
