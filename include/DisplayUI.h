#pragma once
#include <cstdint>
#include "config.h"
#include "StateMachine.h"
#include <Adafruit_SH110X.h>

class DisplayUI {
    public:
        DisplayUI();

        bool begin();

        void draw(const StateMachine& fsm, uint32_t now, uint8_t batteryPercent);

    private:
        Adafruit_SH1106G display;
        uint32_t lastDrawTime = 0;

        void drawSetup(const StateMachine& fsm, uint8_t batteryPercent);
        void drawRestRed(const StateMachine& fsm, uint32_t now);
        void drawRestOrange(const StateMachine& fsm, uint32_t now);
        void drawRestBody(const StateMachine& fsm, uint32_t now);
        void drawReady(const StateMachine& fsm);
        void drawSummary(const StateMachine& fsm);
        void printTime(uint32_t ms);
};