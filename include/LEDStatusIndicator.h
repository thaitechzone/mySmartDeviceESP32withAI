#ifndef LED_STATUS_INDICATOR_H
#define LED_STATUS_INDICATOR_H

#include <Arduino.h>

/**
 * LED Status Indicator Controller
 * ควบคุมการแสดงผลสถานะผ่าน LED โดยใช้ Millis (Non-blocking)
 * 
 * GPIO: GPIO2
 * Logic: Active HIGH (HIGH = LED ON)
 */

enum LEDStatus {
    LED_OFF,           // LED ปิด
    LED_ON,            // LED เปิดตลอด
    LED_IDLE,          // กระพริบช้า (0.5 วินาที) - สถานะรอ
    LED_ACTIVE,        // กระพริบเร็ว (0.2 วินาที) - ทำงานอยู่
    LED_WARNING,       // กระพริบสตกชัก (0.1 วินาที) - มีปัญหา
    LED_HEARTBEAT,     // สตรี่จังหวะหัวใจ (2x กระพริบสั้น แล้วหยุด) - ออนไลน์
    LED_SOS            // SOS (···---···) - ข้อผิดพลาดร้ายแรง
};

class LEDStatusIndicator {
private:
    uint8_t ledPin;
    LEDStatus currentStatus;
    unsigned long lastToggleTime;
    bool ledState;
    unsigned long pattern[16];  // สำหรับ SOS pattern
    uint8_t patternIndex;
    unsigned long patternTime;
    
public:
    /**
     * Constructor
     * @param pin GPIO pin number (default: 2)
     */
    LEDStatusIndicator(uint8_t pin = 2);
    
    /**
     * ตั้งค่า LED pin ให้เป็น OUTPUT
     */
    void begin();
    
    /**
     * ตั้งสถานะ LED
     * @param status สถานะที่ต้องการ
     */
    void setStatus(LEDStatus status);
    
    /**
     * อัพเดตสถานะ LED (เรียก loop() ทุกครั้ง)
     */
    void update();
    
    /**
     * ปิด LED ทันที
     */
    void turnOff();
    
    /**
     * เปิด LED ทันที
     */
    void turnOn();
    
    /**
     * ได้สถานะปัจจุบัน
     */
    LEDStatus getStatus() const;
    
private:
    /**
     * สลับสถานะ LED
     */
    void toggle();
    
    /**
     * ปรับเปลี่ยน interval ตามสถานะ
     */
    unsigned long getBlinkInterval(LEDStatus status);
    
    /**
     * ประมวลผล SOS pattern
     */
    void updateSOSPattern();
};

#endif // LED_STATUS_INDICATOR_H
