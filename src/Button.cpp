#include "Button.h"

Button::Button(uint8_t pin, uint8_t mode) : pin(pin), mode(mode) {}

void Button::begin() {
    pinMode(pin, mode);
}

ButtonEvent Button::update() {
    ButtonEvent event = ButtonEvent::NONE;
    uint32_t currentMillis = millis();
    bool buttonReading = digitalRead(pin);

    if (buttonReading != rawButtonState) {
        rawButtonState = buttonReading;
        lastButtonStateTime = currentMillis;
    }

    if (currentMillis - lastButtonStateTime > DEBOUNCE_DELAY_MS && rawButtonState != stableButtonState) {
        stableButtonState = rawButtonState;

        if (stableButtonState == LOW) {
            buttonPressTimeStart = currentMillis;         
        }
        else if (wasLongPress == true) {
            wasLongPress = false;
        }
        else {
            event = ButtonEvent::SHORT_PRESS;
        }
    }

    if (stableButtonState == LOW && currentMillis - buttonPressTimeStart > LONG_PRESS_MS && !wasLongPress) {
        wasLongPress = true;
        event = ButtonEvent::LONG_PRESS;
    }
    return event;
}