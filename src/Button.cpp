#include <cstdint>
#include "config.h"
#include "Button.h"
#include <Arduino.h>

Button::Button(uint8_t pin, uint8_t mode) : pin(pin), mode(mode) {};

void Button::begin() {
    pinMode(pin, mode);
};

ButtonEvent Button::update() {
    ButtonEvent event = ButtonEvent::NONE;
    uint32_t currentMillis = millis();
    bool buttonReading = digitalRead(pin);

    if (buttonReading != lastButtonState) {
        lastButtonState = buttonReading;
        lastButtonPressTime = currentMillis;
    }

    if (currentMillis - lastButtonPressTime > debounce_ms && lastButtonState != stableButtonState) {
        stableButtonState = lastButtonState;

        if (stableButtonState == LOW) {
            longButtonPressTime = currentMillis;         
        }
        else if (longPress == true) {
            longPress = false;
            buttonEvent = ButtonEvent::NONE;
        }
        else {
            buttonEvent = ButtonEvent::SHORT_PRESS;
        }
    }
    else {
        buttonEvent = ButtonEvent::NONE;
    }

    if (stableButtonState == LOW && currentMillis - longButtonPressTime > LONG_PRESS_MS && !longPress) {
        longPress = true;
        buttonEvent = ButtonEvent::LONG_PRESS;
    }
    return buttonEvent;
}