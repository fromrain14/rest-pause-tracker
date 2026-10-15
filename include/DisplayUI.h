#pragma once
#include <cstdint>
#include "config.h"
#include "StateMachine.h"
#include <Adafruit_SH110X.h>

class DisplayUI {
    public:
        DisplayUI();

        bool begin();

        void draw(const StateMachine& fsm, uint32_t now);

    private:
        Adafruit_SH1106G display;
        uint32_t lastDrawTime = 0;
};