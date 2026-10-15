#include "LedController.h"

LedController::LedController(){}

void LedController::begin() {
    FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    firstRun = true;
}

void LedController::updateLEDs(const StateMachine& fsm) {
    if ((fsm.getState() == lastState) && !firstRun) {
        return;
    }
    lastState = fsm.getState();
    switch (fsm.getState()) {
        case State::SETUP:
        leds[0] = CRGB::Blue;
        break;
        case State::READY:
        leds[0] = CRGB(0, 255, 0);    // CRGB::Green má jen 128
        break;
        case State::REST_RED:
        leds[0] = CRGB::Red;
        break;
        case State::REST_ORANGE:
        leds[0] = CRGB(255, 60, 0);   // CRGB::Orange (255,165,0) svítí na LED žlutě
        break;
        case State::SUMMARY:
        leds[0] = CRGB::Purple;
        break;
    }
    firstRun = false;
    FastLED.show();
}