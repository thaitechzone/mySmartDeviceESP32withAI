#include <Arduino.h>
#include "LEDStatusIndicator.h"

// สร้าง instance ของ LED Status Indicator (GPIO2)
LEDStatusIndicator statusLED(2);

// ตัวแปรสำหรับเปลี่ยนสถานะทดสอบ
unsigned long lastStatusChangeTime = 0;
unsigned long statusChangeDuration = 5000;  // เปลี่ยนสถานะทุก 5 วินาที
LEDStatus statusSequence[] = {
    LED_OFF,
    LED_IDLE,
    LED_ACTIVE,
    LED_WARNING,
    LED_HEARTBEAT,
    LED_SOS,
    LED_ON,
    LED_OFF
};
uint8_t statusIndex = 0;
const uint8_t statusCount = sizeof(statusSequence) / sizeof(statusSequence[0]);

// ฟังก์ชันแสดงสถานะปัจจุบัน
void printCurrentStatus() {
    Serial.print("[");
    Serial.print(millis() / 1000);
    Serial.print("s] LED Status: ");
    
    switch (statusLED.getStatus()) {
        case LED_OFF:
            Serial.println("OFF (ปิด)");
            break;
        case LED_ON:
            Serial.println("ON (เปิด)");
            break;
        case LED_IDLE:
            Serial.println("IDLE (รอ - กระพริบช้า 1 วินาที)");
            break;
        case LED_ACTIVE:
            Serial.println("ACTIVE (ทำงาน - กระพริบเร็ว 0.4 วินาที)");
            break;
        case LED_WARNING:
            Serial.println("WARNING (เตือน - กระพริบสตกชัก 0.2 วินาที)");
            break;
        case LED_HEARTBEAT:
            Serial.println("HEARTBEAT (ออนไลน์ - สตรี่หัวใจ)");
            break;
        case LED_SOS:
            Serial.println("SOS (ข้อผิดพลาด - สัญญาณ SOS)");
            break;
        default:
            Serial.println("UNKNOWN");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // รอจนกว่า Serial พร้อม
    
    Serial.println("\n\n=== Smart Device ESP32 - LED Status Indicator ===");
    Serial.println("โปรแกรมควบคุม LED บ่งชี้สถานะ (GPIO2)");
    Serial.println("ใช้ Non-blocking Millis approach\n");
    
    // ตั้งค่า LED Status Indicator
    statusLED.begin();
    statusLED.setStatus(LED_IDLE);
    
    printCurrentStatus();
    lastStatusChangeTime = millis();
}

void loop() {
    // อัพเดท LED status (ต้องเรียกทุกรอบ loop)
    statusLED.update();
    
    // เปลี่ยนสถานะ LED ทุก 5 วินาที เพื่อทดสอบ
    unsigned long currentTime = millis();
    if (currentTime - lastStatusChangeTime >= statusChangeDuration) {
        lastStatusChangeTime = currentTime;
        
        statusIndex = (statusIndex + 1) % statusCount;
        statusLED.setStatus(statusSequence[statusIndex]);
        
        printCurrentStatus();
    }
    
    // ตัวอย่างการตอบสนองต่ออินพุตจากผู้ใช้ผ่าน Serial
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        Serial.print(">>> Command: ");
        Serial.println(command);
        
        if (command == "off") {
            statusLED.setStatus(LED_OFF);
            Serial.println("✓ LED OFF");
        } else if (command == "on") {
            statusLED.setStatus(LED_ON);
            Serial.println("✓ LED ON");
        } else if (command == "idle") {
            statusLED.setStatus(LED_IDLE);
            Serial.println("✓ LED IDLE (กระพริบช้า)");
        } else if (command == "active") {
            statusLED.setStatus(LED_ACTIVE);
            Serial.println("✓ LED ACTIVE (กระพริบเร็ว)");
        } else if (command == "warning") {
            statusLED.setStatus(LED_WARNING);
            Serial.println("✓ LED WARNING (กระพริบสตกชัก)");
        } else if (command == "heartbeat") {
            statusLED.setStatus(LED_HEARTBEAT);
            Serial.println("✓ LED HEARTBEAT (สตรี่หัวใจ)");
        } else if (command == "sos") {
            statusLED.setStatus(LED_SOS);
            Serial.println("✓ LED SOS (สัญญาณ SOS)");
        } else if (command == "status") {
            printCurrentStatus();
        } else if (command == "help") {
            Serial.println("\n=== Command Help ===");
            Serial.println("  off       - ปิด LED");
            Serial.println("  on        - เปิด LED");
            Serial.println("  idle      - LED กระพริบช้า (สถานะรอ)");
            Serial.println("  active    - LED กระพริบเร็ว (สถานะทำงาน)");
            Serial.println("  warning   - LED กระพริบสตกชัก (มีปัญหา)");
            Serial.println("  heartbeat - LED สตรี่หัวใจ (ออนไลน์)");
            Serial.println("  sos       - LED สัญญาณ SOS (ข้อผิดพลาดร้ายแรง)");
            Serial.println("  status    - แสดงสถานะปัจจุบัน");
            Serial.println("  help      - แสดงคำสั่งทั้งหมด\n");
        } else {
            Serial.println("✗ Unknown command. Type 'help' for commands.");
        }
    }
}

