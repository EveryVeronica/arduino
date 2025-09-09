#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#define ALERT_PIN D3
#define BUILTIN_LED 2
#define RELAY_PIN D2

LoRa lora;

#define STATUS_INTERVAL 5000        // 5 วิ
#define ALERT_REPEAT_INTERVAL 10000 // 10 วิ
#define ALERT_REPEAT_MAX 10
#define SHORT_PRESS_THRESHOLD 10000 // 10 วิ

unsigned long lastStatus = 0;
unsigned long lastAlert = 0;
bool isRelease = false;
bool longAlertActive = false;
int longAlertCount = 0;

unsigned long lastSendTime = 0;
unsigned long releaseTime = 0;
unsigned long startTime = 0; // เวลาเริ่มระบบ
bool alertActive = false;

LoRaPacket packet = {
    NETWORK_ID,
    NODE_ID,
    TARGET_ID,
    MSG_STATUS,
    HOP_COUNT,
    1 // payload = 1 (สถานะปกติ)
};

// ------------------- ฟังก์ชันจัดการ LED/Relay -------------------
void setOutputs(bool on)
{
    digitalWrite(BUILTIN_LED, on ? LOW : HIGH);
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}

void setup()
{
    Serial.begin(9600);
    delay(10);
    Serial.println("=== Node Example 02 === test ผ่าน");

    pinMode(ALERT_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    setOutputs(false); // ปิด LED/Relay

    lora.setupLoRa();
}

// ------------------- loop -------------------
void loop()
{

    bool currentState = digitalRead(ALERT_PIN); // LOW = กด, HIGH = ปล่อย
    unsigned long currentTime = millis();
    lora._get(); // ตรวจสอบข้อความขาเข้า

    if (currentState == LOW)
    {
        isRelease = false;
        alertActive = false;
    }

    if (currentState == HIGH)
    {
        isRelease = true;

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
            packet.msgType = MSG_STATUS;
            packet.payload = 0; // สถานะผิดปกติ
            packet.hopCount = longAlertCount;
            lora.sendLoRaAlert(packet);
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
            packet.msgType = MSG_ALERT;
            packet.payload = 1; // สถานะปกติ
            packet.hopCount = longAlertCount;
            lora.sendLoRaAlert(packet);
        }
    }
}
