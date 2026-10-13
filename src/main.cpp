#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FastLED.h>

// Rozlišení 1.3" displeje z Laskakitu
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 1
#define NUM_LEDS 21
#define BUTTON_PIN 2
CRGB leds[NUM_LEDS];
// Vytvoření objektu pro čip SH1106
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


//State machine enum
enum class SystemState {
  SETUP,
  READY,
  REST_RED,
  REST_ORANGE,
  SUMMARY
};

//Global varialbe
SystemState currentState = SystemState::SETUP;
uint32_t minRestTimeMs = 120000; // 2 minutes
uint32_t restStartTime = 0;
uint8_t setCount = 0;
bool isHrRecovered = false;

//Debounce variables
uint32_t lastButtonPressTime = 0;
uint32_t debounceDelay = 50;
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
uint32_t summaryButtonTime = 3000;
bool sumaryButtonState = false;
uint32_t sumaryButtonPressTime = 0;
uint32_t doubleClickTime = 1000;
uint32_t doubleClickButtonPressTime = 5000;

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

    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  uint32_t currentMillis = millis();
  
  //debounce button reading
  bool buttonReading = digitalRead(BUTTON_PIN);
  if (buttonReading != lastButtonReading) {
    lastButtonPressTime = currentMillis;
    lastButtonReading = buttonReading;
  }

  if (currentMillis - lastButtonPressTime > debounceDelay) {
    if (buttonReading != stableButtonState) {
      stableButtonState = buttonReading;
      if (stableButtonState == LOW) {
        sumaryButtonPressTime = millis();
        }
      else if (currentMillis-sumaryButtonPressTime>summaryButtonTime) {
          sumaryButtonState = true;
          currentState = SystemState::SUMMARY;  
      }
      else {
        if (currentState == SystemState::SETUP) {
          currentState = SystemState::READY;
        }
        else if (currentState == SystemState::READY) {
          restStartTime = currentMillis;
          currentState = SystemState::REST_RED;
        } 
        else if (currentState == SystemState::SUMMARY) {
          restStartTime = currentMillis;
          currentState = SystemState::SETUP;
        }    
        else if (currentState == SystemState::REST_RED || currentState == SystemState::REST_ORANGE) {
          if(sumaryButtonPressTime-doubleClickButtonPressTime<doubleClickTime) {
          //double click pro skipnuti red a orange
        }
        doubleClickButtonPressTime=currentMillis;
        }
      }
    } 
  }
  switch(currentState) {

  }
        

  if(millis() - currentMillis >= 1000) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(10, 25);
    display.println(currentMillis / 1000);
    display.display();
    if (currentMillis / 1000 % 2 == 0) {
      uint8_t randomColor = random(0, 2);
      leds[0] = (randomColor == 0) ? CRGB::Red : CRGB::Blue;
    } else {
      leds[0] = CRGB::Green;
    }
    FastLED.show();
  }
}   