#pragma once
#include <cstdint>
#include "config.h"
#include "StateMachine.h"
#include <FastLED.h>

//o nicem nerozhoduje, jen signalizuje stav, vola ji main.cpp, aby se rozsvitila LED podle stavu statemachine
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