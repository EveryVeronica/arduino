#include "LoRaGateway.h"
// Network define
#define DEFAULT_NETWORK_ID 0x00
#define DEFAULT_NODE_ID 0x02

// IO Pin
#define ALERT_PIN D3
#define BUILTIN_LED 2
#define RELAY_PIN D2
#define LONG_ALERT_MAX 5  // ส่ง Long Alert สูงสุด 20 รอบ
LoRaGateway lora;

#define ALERT_REPEAT_INTERVAL 10000 // 10 วิ

unsigned long lastAlert = 0;
bool isRelease = false;
int longAlertCount = 0;

unsigned long releaseTime = 0;
bool alertActive = false;
bool isRunning = false;
unsigned long countdown = 0; // ตัวนับถอยหลัง

unsigned long lastCountdownUpdate = 0;
unsigned long lastShortAlertUpdate = 0;
unsigned long lastLongAlertUpdate = 0;

Packet data;

// ตัวแปรควบคุม LED กระพริบ
unsigned long lastBlink = 0;
bool blinkState = false;
int blinkCount = 0;

void handleShortAlert()
{
    if (blinkCount < 10) // กระพริบ 5 ครั้ง
    {
        if (millis() - lastBlink >= 500) // ทุก 500 ms
        {
            lastBlink = millis();
            blinkState = !blinkState;
            setOutputs(blinkState);
            if (!blinkState)
                blinkCount++; // นับเมื่อปิดไฟแล้ว
        }
    }
    else
    {
        setOutputs(false); // ปิด Relay หลังกระพริบครบ
    }
}

// ------------------- ฟังก์ชันจัดการ LED/Relay -------------------
void setOutputs(bool on)
{
    // digitalWrite(BUILTIN_LED, on ? LOW : HIGH);
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}

bool every(unsigned long interval, unsigned long &lastUpdate)
{
    if (millis() - lastUpdate >= interval)
    {
        lastUpdate = millis();
        return true;
    }
    return false;
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
    data = lora.LoRa_rxMode();

    if (data.source_id == 0x0A && data.msgType == MSG_ACK && data.target_id == DEFAULT_NODE_ID)
    {
        countdown = data.payload;
        Serial.println("=== ได้รับคำสั่งจาก Server ===");
    }

    // นับถอยหลัง
    if (countdown > 0 && every(1000, lastCountdownUpdate))
    {
        countdown--;
        isRunning = false;
        Serial.println("=== นับถอยหลัง: " + String(countdown) + " วินาที ===");
    }

    // 2) ถ้า countdown == 0 → ทำงานตอนหมดเวลา
    if (countdown == 0)
    {
        if (!isRunning)
        {
            isRunning = true;
            Pk.msgType = MSG_STATUS; // ask for status
            lora.LoRa_txMode(Pk);
            Serial.println("=== ส่ง Status ===");
        }

        if (currentState == LOW && alertActive)
        {
            Pk.msgType = MSG_ACK;
            lora.LoRa_txMode(Pk);
        }
    }

    // ตรวจสอบปุ่ม
    if (currentState == LOW)
    {
        isRelease = false;
        alertActive = false;
        Pk.payload = 1;    // 1 ปกติ
        setOutputs(false); // ปิด Relay
    }
    else
    {
        isRelease = true;
        Pk.payload = 0; // 0 ผิดปกติ
        if (!alertActive)
        {
            alertActive = true;
            releaseTime = millis() + 5000UL; // อีก 5 วิ
            blinkCount = 0;
            blinkState = false;
        }
    }

    // แจ้งเตือน
    if (millis() < releaseTime)
    {
        // ปล่อยไม่เกิน 5 วิ
        if (isRelease && every(1000, lastShortAlertUpdate))
        {
            longAlertCount = 0;
            Pk.payload = 2; // 2 ผิดปกติ
            Pk.hopCount = longAlertCount;
            Pk.msgType = MSG_ALERT;
            lora.LoRa_txMode(Pk);
            Serial.println("=== ส่ง Alert (สั้น) ===");

            // เรียกฟังก์ชันจัดการกระพริบ
            handleShortAlert();
        }
    }
    else
    {
        // ปล่อยค้างเกิน 5 วิ
        if (isRelease && longAlertCount < LONG_ALERT_MAX && every(10000, lastLongAlertUpdate)) //เมื่อส่งครบ 20 รอบ ให้หยุด
        {
            longAlertCount++;
            lastAlert = currentTime;
            Pk.hopCount = longAlertCount;
            Pk.msgType = MSG_ALERT;
            lora.LoRa_txMode(Pk);
            Serial.println("=== ส่ง Alert (ยาว): " + String(longAlertCount) + " รอบ ===");

            setOutputs(true); // เปิด Relay ค้าง
        }
    }
}
