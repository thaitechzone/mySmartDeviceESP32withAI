#include "LEDStatusIndicator.h"

LEDStatusIndicator::LEDStatusIndicator(uint8_t pin)
    : ledPin(pin), currentStatus(LED_OFF), lastToggleTime(0), 
      ledState(false), patternIndex(0), patternTime(0) {
    // เตรียม SOS pattern: 3x dot(100ms) + 3x dash(300ms) + 3x dot(100ms)
    // Dot: 100ms on, 100ms off
    // Dash: 300ms on, 100ms off
    // Space between characters: 300ms
    pattern[0] = 100;   // dot on
    pattern[1] = 100;   // off
    pattern[2] = 100;   // dot on
    pattern[3] = 100;   // off
    pattern[4] = 100;   // dot on
    pattern[5] = 300;   // off (space)
    pattern[6] = 300;   // dash on
    pattern[7] = 100;   // off
    pattern[8] = 300;   // dash on
    pattern[9] = 100;   // off
    pattern[10] = 300;  // dash on
    pattern[11] = 300;  // off (space)
    pattern[12] = 100;  // dot on
    pattern[13] = 100;  // off
    pattern[14] = 100;  // dot on
    pattern[15] = 1000; // off (long pause before repeat)
}

void LEDStatusIndicator::begin() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);  // ปิด LED
}

void LEDStatusIndicator::setStatus(LEDStatus status) {
    if (currentStatus != status) {
        currentStatus = status;
        lastToggleTime = millis();
        patternIndex = 0;
        patternTime = 0;
        
        // ตั้งค่าเริ่มต้นตามสถานะ
        switch (currentStatus) {
            case LED_OFF:
                digitalWrite(ledPin, LOW);
                ledState = false;
                break;
            case LED_ON:
                digitalWrite(ledPin, HIGH);
                ledState = true;
                break;
            default:
                ledState = false;
                break;
        }
    }
}

void LEDStatusIndicator::update() {
    unsigned long currentTime = millis();
    
    if (currentStatus == LED_OFF || currentStatus == LED_ON) {
        return;  // ไม่ต้องทำอะไร
    }
    
    if (currentStatus == LED_SOS) {
        updateSOSPattern();
        return;
    }
    
    // สำหรับ LED_IDLE, LED_ACTIVE, LED_WARNING, LED_HEARTBEAT
    unsigned long interval = getBlinkInterval(currentStatus);
    
    if (currentStatus == LED_HEARTBEAT) {
        // สตรี่แบบหัวใจ: 2x กระพริบสั้น แล้วหยุด
        unsigned long beatCycle = 1000;  // รอบ 1 วินาที
        unsigned long timeInCycle = (currentTime - lastToggleTime) % beatCycle;
        
        if (timeInCycle < 100) {
            digitalWrite(ledPin, HIGH);
        } else if (timeInCycle < 150) {
            digitalWrite(ledPin, LOW);
        } else if (timeInCycle < 250) {
            digitalWrite(ledPin, HIGH);
        } else if (timeInCycle < 300) {
            digitalWrite(ledPin, LOW);
        } else {
            digitalWrite(ledPin, LOW);
        }
    } else {
        // กระพริบปกติ
        if (currentTime - lastToggleTime >= interval) {
            toggle();
            lastToggleTime = currentTime;
        }
    }
}

void LEDStatusIndicator::toggle() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
}

void LEDStatusIndicator::turnOff() {
    digitalWrite(ledPin, LOW);
    ledState = false;
    currentStatus = LED_OFF;
}

void LEDStatusIndicator::turnOn() {
    digitalWrite(ledPin, HIGH);
    ledState = true;
    currentStatus = LED_ON;
}

LEDStatus LEDStatusIndicator::getStatus() const {
    return currentStatus;
}

unsigned long LEDStatusIndicator::getBlinkInterval(LEDStatus status) {
    switch (status) {
        case LED_IDLE:
            return 500;  // กระพริบ 1 วินาที (0.5 on, 0.5 off)
        case LED_ACTIVE:
            return 200;  // กระพริบ 0.4 วินาที (0.2 on, 0.2 off)
        case LED_WARNING:
            return 100;  // กระพริบสตกชัก 0.2 วินาที (0.1 on, 0.1 off)
        case LED_HEARTBEAT:
            return 250;  // สำหรับการคำนวณใน update()
        case LED_SOS:
            return 0;    // ใช้พิเศษใน updateSOSPattern()
        default:
            return 500;
    }
}

void LEDStatusIndicator::updateSOSPattern() {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - patternTime;
    unsigned long patternDuration = pattern[patternIndex];
    
    if (elapsed >= patternDuration) {
        patternTime = currentTime;
        patternIndex = (patternIndex + 1) % 16;
        
        // เปิด LED สำหรับ index คู่, ปิด LED สำหรับ index คี่
        if (patternIndex % 2 == 0) {
            digitalWrite(ledPin, HIGH);  // ON
        } else {
            digitalWrite(ledPin, LOW);   // OFF
        }
    }
}
