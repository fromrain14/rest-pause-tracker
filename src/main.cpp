#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FastLED.h>

// Rozlišení tvého 1.3" displeje z Laskakitu
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 1
#define NUM_LEDS 21
CRGB leds[NUM_LEDS];
// Vytvoření objektu pro čip SH1106 (-1 znamená bez Reset pinu)
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(115200);
    delay(2000); 
    Serial.println("--- Test 1.3 OLED (SH1106) ---");

    // Zapnutí I2C na pinech D4 (GPIO 6 =  SDA) a D5 (GPIO 7 = SCL)
    Wire.begin(6, 7);

    // 0x3C je standardní adresa tohoto Laskakit displeje
    if (!display.begin(0x3C, true)) {
        Serial.println("CHYBA: Displej neodpovida!");
        while (true);
    }   
    FastLED.addLeds<WS2812B, LED_PIN>(leds, NUM_LEDS);
    
    // display.clearDisplay();               // Vymaže to zrnění z paměti
    // display.setTextSize(2);               // Velikost písma 2
    // display.setTextColor(SH110X_WHITE);   // Zapne svícení pixelů
    // display.setCursor(10, 25);            // Vycentrování zhruba na střed
    
    // display.println("FUNGUJE!");          // Text do paměti
    // display.display();                    // Odeslání na obrazovku
}
uint32_t lastUpdate = millis();

void loop() {

  if(millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(10, 25);
    display.println(lastUpdate / 1000);
    display.display();
    if (lastUpdate / 1000 % 2 == 0) {
      uint8_t randomColor = random(0, 2);
      leds[0] = (randomColor == 0) ? CRGB::Red : CRGB::Blue;
    } else {
      leds[0] = CRGB::Green;
    }
    FastLED.show();
  }
}   