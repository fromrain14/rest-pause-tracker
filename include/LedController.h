#pragma once
#include <cstdint>
#include "config.h"
#include "StateMachine.h"
#include <FastLED.h>

// Makes no decisions, only shows the current StateMachine state on the LED (called from main.cpp)
class LedController {
    private:
        CRGB leds[NUM_LEDS];
        State lastState = State::SETUP;
        bool firstRun = true;

    public:
        LedController();
        void updateLEDs(const StateMachine& fsm);
        void begin();
};