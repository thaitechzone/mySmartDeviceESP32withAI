#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Clear the display
  display.clearDisplay();
  
  // Set text properties
  display.setTextSize(2);           // Set text size (2x normal)
  display.setTextColor(SSD1306_WHITE); // Set text color to white
  display.setCursor(0, 20);          // Set cursor position (x, y)
  
  // Display the text "ESP32 DEVKIT"
  display.println(F("ESP32"));
  display.println(F("DEVKIT"));
    // Draw a rectangle on the right side as a level indicator
    display.fillRect(SCREEN_WIDTH - 4, 0, 4, SCREEN_HEIGHT, SSD1306_WHITE);
  // Update the display with all of the above
  display.display();
  
  Serial.println("OLED Display Initialized");
}   

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello, World!");
  delay(1000);
}