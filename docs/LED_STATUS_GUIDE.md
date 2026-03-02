# LED Status Indicator Guide
## คู่มือควบคุม LED บ่งชี้สถานะ (Non-blocking)

---

## 📌 ภาพรวม

โปรแกรมนี้ใช้ **Millis()** เพื่อควบคุมการแสดงผล LED โดยไม่ใช้ `delay()` ซึ่งอนุญาตให้โปรแกรมอื่นทำงานได้พร้อมกัน

### GPIO Config
- **GPIO**: GPIO2
- **ลอจิก**: Active HIGH (HIGH = LED ON)

---

## 🎯 สถานะ LED

### 1. **LED_OFF** - ปิด
- LED ไม่สว่าง

### 2. **LED_ON** - เปิด
- LED สว่างตลอด

### 3. **LED_IDLE** - กระพริบช้า (500ms)
```
ON (500ms) → OFF (500ms) → ON (500ms) → ...
```
- **ใช้**: สถานะรอ, Idle, Connected

### 4. **LED_ACTIVE** - กระพริบเร็ว (200ms)
```
ON (200ms) → OFF (200ms) → ON (200ms) → ...
```
- **ใช้**: ทำงานอยู่, Processing, Active Connection

### 5. **LED_WARNING** - กระพริบสตกชัก (100ms)
```
ON (100ms) → OFF (100ms) → ON (100ms) → ...
```
- **ใช้**: เตือน, ลด bandwidth, มีปัญหา

### 6. **LED_HEARTBEAT** - สตรี่หัวใจ (Heartbeat Pattern)
```
ON (100ms) → OFF (50ms) → ON (100ms) → OFF (50ms) → OFF (600ms) → ...
```
- **ใช้**: ออนไลน์ สุขภาพดี, Normal Operation, Double Pulse Pattern

### 7. **LED_SOS** - สัญญาณ SOS (Morse Code)
```
···---··· (SOS in Morse)
Dot: 100ms ON
Dash: 300ms ON
Space: 300ms OFF
```
- **ใช้**: ข้อผิดพลาดร้ายแรง, Critical Error, Emergency Mode

---

## 💻 การใช้งาน

### การสร้าง Instance
```cpp
#include "LEDStatusIndicator.h"

// ใช้ GPIO2 (default)
LEDStatusIndicator statusLED;

// หรือใช้ GPIO อื่น
LEDStatusIndicator statusLED(5);  // GPIO5
```

### ใน setup()
```cpp
void setup() {
    statusLED.begin();              // ตั้งค่า GPIO OUTPUT
    statusLED.setStatus(LED_IDLE);  // ตั้งสถานะเริ่มต้น
}
```

### ใน loop()
```cpp
void loop() {
    statusLED.update();  // อัพเดท LED (เรียกทุกรอบ!)
    
    // ทำงานอื่นๆ ได้พร้อมกัน
    doOtherStuff();
}
```

### เปลี่ยนสถานะ
```cpp
statusLED.setStatus(LED_ACTIVE);    // เปลี่ยนเป็น Active
statusLED.setStatus(LED_WARNING);   // เปลี่ยนเป็น Warning
statusLED.setStatus(LED_OFF);       // ปิด LED
```

---

## 📊 ตัวอย่างการใช้งาน

### ตัวอย่างที่ 1: ตรวจสอบสถานะ WiFi
```cpp
#include "LEDStatusIndicator.h"

LEDStatusIndicator statusLED;

void setup() {
    statusLED.begin();
    statusLED.setStatus(LED_IDLE);  // ค้นหา WiFi
}

void loop() {
    statusLED.update();
    
    if (WiFi.status() == WL_CONNECTED) {
        statusLED.setStatus(LED_HEARTBEAT);  // ออนไลน์
    } else if (WiFi.status() == WL_CONNECT_FAILED) {
        statusLED.setStatus(LED_WARNING);    // ลม WiFi
    } else {
        statusLED.setStatus(LED_IDLE);       // ค้นหา...
    }
}
```

### ตัวอย่างที่ 2: Processing Data
```cpp
void loop() {
    statusLED.update();
    
    if (dataAvailable) {
        statusLED.setStatus(LED_ACTIVE);     // เริ่มโปรเซส
        processData();
        statusLED.setStatus(LED_HEARTBEAT);  // เสร็จ
    }
}
```

### ตัวอย่างที่ 3: Error Handling
```cpp
void loop() {
    statusLED.update();
    
    try {
        doSomethingRisky();
        statusLED.setStatus(LED_HEARTBEAT);
    } catch (...) {
        statusLED.setStatus(LED_SOS);  // Critical Error!
    }
}
```

### ตัวอย่างที่ 4: Relay Control Feedback
```cpp
#include "LEDStatusIndicator.h"

LEDStatusIndicator statusLED;
const int RELAY1 = 4;

void setup() {
    statusLED.begin();
    pinMode(RELAY1, OUTPUT);
}

void loop() {
    statusLED.update();
    
    // Relay ปิด → LED ปิด
    if (!digitalRead(RELAY1)) {
        statusLED.setStatus(LED_OFF);
    } 
    // Relay เปิด → LED เปิด
    else {
        statusLED.setStatus(LED_HEARTBEAT);
    }
}
```

---

## 🔧 API Reference

### Constructor
```cpp
LEDStatusIndicator(uint8_t pin = 2)
```
- **pin**: GPIO number (default: GPIO2)

### Methods

#### `begin()`
```cpp
void begin()
```
ตั้งค่า GPIO OUTPUT และปิด LED

#### `setStatus(LEDStatus status)`
```cpp
void setStatus(LEDStatus status)
```
ตั้งสถานะ LED ใหม่

| Status | ค่า |
|--------|-----|
| LED_OFF | ปิด |
| LED_ON | เปิด |
| LED_IDLE | กระพริบช้า 500ms |
| LED_ACTIVE | กระพริบเร็ว 200ms |
| LED_WARNING | กระพริบสตกชัก 100ms |
| LED_HEARTBEAT | สตรี่หัวใจ |
| LED_SOS | สัญญาณ SOS |

#### `update()`
```cpp
void update()
```
อัพเดทสถานะ LED ตามเวลา (ต้องเรียกทุกรอบ loop)

#### `turnOff()`
```cpp
void turnOff()
```
ปิด LED ทันที

#### `turnOn()`
```cpp
void turnOn()
```
เปิด LED ทันที

#### `getStatus()`
```cpp
LEDStatus getStatus() const
```
ได้สถานะปัจจุบันของ LED

---

## ⚙️ การทำงานภายใน

### Non-blocking Approach
```cpp
// ❌ ไม่ดี - บล็อก loop
void loop() {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
    // ระหว่าง delay โปรแกรมไม่ตอบสนอง
}

// ✓ ดี - ไม่บล็อก
statusLED.update();
// Loop ทำงานต่อเร็วๆ
```

### Time Tracking
```cpp
unsigned long lastToggleTime = 0;
unsigned long interval = 500;

if (millis() - lastToggleTime >= interval) {
    toggle();
    lastToggleTime = millis();
}
```

---

## 🧪 การทดสอบผ่าน Serial Monitor

รันโปรแกรมตัวอย่างใน `main.cpp` และสั่งการผ่าน Serial Monitor (115200 baud):

### Commands
```
off       - ปิด LED
on        - เปิด LED
idle      - กระพริบช้า
active    - กระพริบเร็ว
warning   - กระพริบสตกชัก
heartbeat - สตรี่หัวใจ
sos       - สัญญาณ SOS
status    - แสดงสถานะ
help      - ช่วยเหลือ
```

### ตัวอย่าง Output
```
=== Smart Device ESP32 - LED Status Indicator ===
โปรแกรมควบคุม LED บ่งชี้สถานะ (GPIO2)
ใช้ Non-blocking Millis approach

[0s] LED Status: IDLE (รอ - กระพริบช้า 1 วินาที)
[5s] LED Status: ACTIVE (ทำงาน - กระพริบเร็ว 0.4 วินาที)
[10s] LED Status: WARNING (เตือน - กระพริบสตกชัก 0.2 วินาที)

>>> Command: sos
✓ LED SOS (สัญญาณ SOS)
```

---

## 📈 Timing Reference

| Status | On Duration | Off Duration | Period | ใช้ |
|--------|-------------|--------------|--------|-----|
| OFF | 0ms | ∞ | N/A | Idle/Standby |
| ON | ∞ | 0ms | N/A | Active/Powered |
| IDLE | 500ms | 500ms | 1000ms | Waiting |
| ACTIVE | 200ms | 200ms | 400ms | Processing |
| WARNING | 100ms | 100ms | 200ms | Alert |
| HEARTBEAT | 2x(100+50)ms | 600ms | 1000ms | Online |
| SOS | Morse Code | - | ∞ | Emergency |

---

## 💡 Tips & Tricks

### 1. ใช้ SDA/SCL pins ของ GPIO ให้คุ้มค่า
```cpp
// ถ้า I2C ยังไม่ใช้
LEDStatusIndicator statusLED(22);  // GPIO22 (SCL)
```

### 2. Debug Mode
```cpp
// เพิ่มตัวอย่างเพื่อดูการจังหวะแต่ละสถานะ
for (int i = 0; i < 5000; i++) {
    statusLED.update();
    delay(1);  // 1ms loop
}
```

### 3. Multiple LEDs
```cpp
LEDStatusIndicator statusLED(2);     // Status
LEDStatusIndicator wifiLED(4);       // WiFi
LEDStatusIndicator errorLED(5);      // Error

void loop() {
    statusLED.update();
    wifiLED.update();
    errorLED.update();
}
```

### 4. Power Consumption
- LED 3mm typical: 20mA (3.3V)
- ใช้ resistor ขนาดเหมาะสม: R = (Vcc - Vled) / I = (3.3 - 2.0) / 0.02 = 65Ω (~100Ω safety)

---

## ⚠️ ข้อควรระวัง

1. **เรียก `update()` ทุกรอบ**: ถ้าลืม LED จะไม่กระพริบ
2. **ไม่ใช้ delay()**: โปรแกรมจะหยุด
3. **เช็ค GPIO ที่ว่าง**: GPIO2 อาจใช้ที่อื่นแล้ว
4. **Current limiting**: ใช้ resistor เพื่อป้องกัน GPIO

---

## 📂 File Structure
```
mySmartDeviceESP32withAI/
├── src/
│   └── main.cpp                    (ตัวอย่างการใช้งาน)
├── include/
│   └── LEDStatusIndicator.h        (Header file)
├── lib/
│   └── LEDStatusIndicator.cpp      (Implementation)
└── docs/
    └── LED_STATUS_GUIDE.md         (ไฟล์นี้)
```

---

## 🚀 Quick Start

```cpp
#include <Arduino.h>
#include "LEDStatusIndicator.h"

LEDStatusIndicator statusLED(2);

void setup() {
    statusLED.begin();
    statusLED.setStatus(LED_HEARTBEAT);
}

void loop() {
    statusLED.update();  // ⚠️ สำคัญที่สุด!
    
    // โค้ดอื่น...
}
```

---

**หมายเหตุ**: ดูตัวอย่างเพิ่มเติมใน `main.cpp`

