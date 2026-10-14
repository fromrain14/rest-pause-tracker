#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FastLED.h>
#include "Button.h"

Button btnA(BUTTON_PIN_A, INPUT_PULLUP);
Button btnB(BUTTON_PIN_B, INPUT_PULLUP);

void setup() {
    btnA.begin();
    btnB.begin();
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
}

void loop() {
    ButtonEvent btnAEvent = btnA.update();
    ButtonEvent btnBEvent = btnB.update();
    if (btnAEvent != ButtonEvent::NONE) { // nebo 0 / podle definice v Button.h
        Serial.print("Btn A Event: ");
        Serial.println((int)btnAEvent);
    }

    if (btnBEvent != ButtonEvent::NONE) {
        Serial.print("Btn B Event: ");
        Serial.println((int)btnBEvent);
    }
    
 
}   