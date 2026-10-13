#pragma once
#include <cstdint>
#include "config.h"
#include <Arduino.h>

enum class ButtonEvent {NONE, SHORT_PRESS, LONG_PRESS};
class Button {
    private:
        uint32_t debounce_ms = DEBOUNCE_DELAY_MS;
        uint32_t longPressMs = LONG_PRESS_MS;
        uint8_t pin;
        uint8_t mode;
        ButtonEvent buttonEvent = ButtonEvent::NONE;

        bool longPress = false;
        bool lastButtonState = HIGH;
        bool stableButtonState = HIGH;
        uint32_t lastButtonPressTime = 0;
        uint32_t longButtonPressTime = 0;

    public:
        Button(uint8_t pin, uint8_t mode);

        void begin();
        
        ButtonEvent update();
};