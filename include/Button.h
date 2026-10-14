#pragma once
#include <cstdint>
#include "config.h"
#include <Arduino.h>

enum class ButtonEvent {NONE, SHORT_PRESS, LONG_PRESS};
class Button {
    private:
        uint8_t pin;
        uint8_t mode;

        bool wasLongPress = false;
        bool rawButtonState = HIGH;
        bool stableButtonState = HIGH;
        uint32_t lastButtonStateTime = 0;
        uint32_t buttonPressTimeStart = 0;

    public:
        Button(uint8_t pin, uint8_t mode);

        void begin();
        
        ButtonEvent update();
};