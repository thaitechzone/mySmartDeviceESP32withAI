# Smart Device ESP32 with AI - คู่มือการใช้งาน

## 📋 ภาพรวมโปรเจกต์

โปรเจกต์นี้เป็นอุปกรณ์อัจฉริยะที่ใช้บอร์ด **ESP32 DevKit V2** ประสาน AI เพื่อควบคุมอุปกรณ์ต่างๆ ผ่าน RS485 Interface พร้อมจอแสดงผล OLED

---

## 🔧 ข้อมูลบอร์ด ESP32 DevKit V2

### สเปค
| รายการ | ข้อมูล |
|---------|--------|
| Microcontroller | ESP32 (Dual-core Xtensa 32-bit) |
| RAM | 520 KB SRAM |
| Flash | 4 MB |
| Operating Voltage | 3.3V |

---

## 🔌 Pin Configuration

### LED สถานะ
| รายการ | GPIO | หน้าที่ |
|---------|------|--------|
| Status LED | GPIO2 | ตัวบ่งชี้สถานะบอร์ด |

### Relay Control (3 ตัว)
| Relay | GPIO | ลอจิก | ประเภท |
|-------|------|-------|--------|
| Relay 1 | GPIO4 | Active Low | Relay ON เมื่อ LOW |
| Relay 2 | GPIO16 | Active Low | Relay ON เมื่อ LOW |
| Relay 3 | GPIO17 | Active Low | Relay ON เมื่อ LOW |

### Switch Input (3 ตัว)
| Switch | GPIO | Pull-up | ลอจิก |
|--------|------|--------|-------|
| SW1 | GPIO34 | 10kΩ | Active Low (กด = LOW) |
| SW2 | GPIO35 | 10kΩ | Active Low (กด = LOW) |
| SW3 | GPIO32 | 10kΩ | Active Low (กด = LOW) |

### Isolated Input (2 อินพุต)
| Input | GPIO | Voltage Range | ลอจิก |
|-------|------|---------------|-------|
| ISO1 | GPIO33 | 10-24V TTL | Active Low |
| ISO2 | GPIO27 | 10-24V TTL | Active Low |

---

## 📱 OLED Display (I2C) - SSD1306

### การต่อสาย
| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| RST | GPIO5 (optional) |

### สเปค
- **ประเภท**: OLED 0.96" / 1.3"
- **Driver**: SSD1306
- **ความละเอียด**: 128x64 (หรือ 128x32)
- **I2C Address**: 0x3C (บางรุ่น 0x3D)
- **แรงดัน**: 3.3V (ห้ามเกิน 5V)

---

## 🔗 RS485 Communication (Serial 0)

### IC ใช้งาน
- **Transceiver**: MAX13487 (Auto-Direction)
- **Mode**: Half-duplex
- **บัส**: สวิตช์ SPDT เลือก RS232/RS485

### การต่อสาย
| ESP32 Pin | MAX13487 Pin | สัญญาณ |
|-----------|--------------|--------|
| TX0 (GPIO1) | DI | Data In |
| RX0 (GPIO3) | RO | Receive Out |
| Control GPIO / RTS | DE/RE | Direction Control |
| GND | GND | - |
| 3.3V | VCC | Power |

| MAX13487 | ต่อเครือข่าย |
|----------|-------------|
| A | RS485 Bus A |
| B | RS485 Bus B |

---

## 💡 การใช้งาน

### 1. Relay Control
```cpp
// ตั้งค่า Relay GPIO
const int RELAY1 = 4;
const int RELAY2 = 16;
const int RELAY3 = 17;

// ในส่วน setup()
pinMode(RELAY1, OUTPUT);
pinMode(RELAY2, OUTPUT);
pinMode(RELAY3, OUTPUT);

// เปิด Relay (Active Low - ต้องเป็น LOW)
digitalWrite(RELAY1, LOW);
delay(100);

// ปิด Relay (Active Low - ต้องเป็น HIGH)
digitalWrite(RELAY1, HIGH);
```

### 2. Switch Input
```cpp
// ตั้งค่า Switch GPIO
const int SW1 = 34;
const int SW2 = 35;
const int SW3 = 32;

// ในส่วน setup() - ไม่ต้อง pinMode เพราะเป็น Input ค่าเริ่มต้น

// อ่านค่า Switch
void readSwitches() {
    if (digitalRead(SW1) == LOW) {
        Serial.println("SW1 Pressed!");
    }
    if (digitalRead(SW2) == LOW) {
        Serial.println("SW2 Pressed!");
    }
    if (digitalRead(SW3) == LOW) {
        Serial.println("SW3 Pressed!");
    }
}
```

### 3. Isolated Input
```cpp
// ตั้งค่า Isolated Input GPIO
const int ISO1 = 33;
const int ISO2 = 27;

// อ่านค่า Isolated Input
void readIsolatedInputs() {
    bool iso1_state = digitalRead(ISO1); // false = LOW, true = HIGH
    bool iso2_state = digitalRead(ISO2);
    
    Serial.print("ISO1: ");
    Serial.println(iso1_state ? "HIGH" : "LOW");
}
```

### 4. OLED Display
```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 5
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Wire.begin(21, 22); // SDA, SCL
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (1);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Smart Device ESP32");
    display.display();
}

void updateDisplay(const char* text) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}
```

### 5. RS485 Communication
```cpp
void setupRS485() {
    Serial.begin(115200, SERIAL_8N1, 3, 1); // RX, TX
    // GPIO1 (TX), GPIO3 (RX) ถูกใช้สำหรับ UART0
}

void sendRS485Data(const char* data) {
    Serial.write(data);
    Serial.flush();
}

void receiveRS485Data() {
    if (Serial.available()) {
        String received = Serial.readStringUntil('\n');
        Serial.print("RS485 Received: ");
        Serial.println(received);
    }
}
```

---

## ⚡ Power Supply

### แรงดันต่างๆ ที่ต้องใช้
| ความต้องการ | แรงดัน | หมายเหตุ |
|-----------|--------|---------|
| ESP32 Core | 3.3V | Main MCU |
| OLED | 3.3V | I2C Display |
| MAX13487 | 3.3V | RS485 Transceiver |
| Relay | 5V-12V | ขึ้นอยู่กับข้อมูลจำเพาะของ Relay |
| Isolated Input | 10-24V | TTL Compatible |

---

## 📌 ลำดับการตั้งค่า Serial Monitor

```
9600 baud   - ทั่วไป
115200 baud - ความเร็วสูง
```

---

## ⚠️ ข้อควรระวัง

1. **OLED Voltage**: ห้ามจ่ายแรงดัน 5V เว้นแต่ OLED มีวงจรป้องกัน
2. **Isolated Input**: รองรับแรงดัน 10-24V TTL เท่านั้น - ห้ามเกินขึ้นไป
3. **Relay Click**: ตรวจสอบ Rating ของ Relay ตรงกับภาระที่ต่อ
4. **RS485 Termination**: หากใช้บัส RS485 ยาว ต้องเพิ่ม Termination Resistor (120Ω) ที่ปลายบัส
5. **Hot-Plug Protection**: ไม่ควรเสียบ/ถอด I2C หรือ RS485 ขณะเปิดเครื่อง

---

## 🔧 การ Debug

### LED Status
- ใช้ GPIO2 สำหรับตรวจสอบสถานะการทำงาน
- สามารถบลิงตามความต้อง

### Serial Monitor
```cpp
void setup() {
    Serial.begin(115200);
    while (!Serial); // รอ Serial พร้อม
    Serial.println("System Started!");
}
```

---

## 📁 โครงสร้าง Project

```
mySmartDeviceESP32withAI/
├── README.md                    (คู่มือนี้)
├── HardwareESP32Config.md       (รายละเอียดฮาร์ดแวร์)
├── platformio.ini               (ค่าตั้งค่า PlatformIO)
├── src/
│   └── main.cpp                 (โค้ดหลัก)
├── include/
│   └── README
├── lib/
│   └── README
└── test/
    └── README
```

---

## 📚 Libraries ที่ต้องใช้

```ini
lib_deps =
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.5
```

---

## 🚀 เริ่มต้น

1. **ติดตั้ง PlatformIO** ใน VS Code
2. **เปิด Project** นี้
3. **รันคำสั่ง**: `platformio run` หรือกด Build ใน PlatformIO
4. **อัปโหลด**: `platformio run --target upload`
5. **Model**: `platformio device monitor`

---

## 📞 Support

สำหรับปัญหาเพิ่มเติม ให้ตรวจสอบ:
- ตัวเชื่อมต่อ GPIO ให้ถูกต้อง
- แรงดันไฟฟ้า
- ลำดับ Baud Rate ของ Serial Port

---

**เวอร์ชัน**: 1.0  
**อัปเดตล่าสุด**: March 2026
