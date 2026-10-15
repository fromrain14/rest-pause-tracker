#pragma once
#include <cstdint>
#include "config.h"
#include "Button.h"

enum class State {SETUP, READY, REST_RED, REST_ORANGE, SUMMARY};

// Rozhoduje, v jaké fázi tréninku jsem, a vede si statistiku tréninku.
class StateMachine {
public:
    void update(ButtonEvent a, ButtonEvent b, bool hrReady, uint32_t now);


    State getState() const;


    uint32_t getMinPauseMs() const;

    // REST_RED / REST_ORANGE
    uint32_t getRestElapsedMs(uint32_t now) const;

    // SUMMARY
    uint8_t getSetCount() const;
    uint32_t getWorkoutDurationMs() const;
    uint32_t getAvgRestMs() const;
    const char* printState(State s) const;

private:
    State state = State::SETUP;
    uint32_t minPauseMs = MIN_REST_TIME_MS;

    uint32_t workoutStartMs = 0;
    uint32_t workoutDurationMs = 0;   // uloží se jednou při přechodu do SUMMARY

    uint32_t lastAPressMs = 0;        // konec poslední série = začátek aktuální pauzy
    bool wasAPress = false;           // platí lastAPressMs?

    uint8_t setCount = 0;
    uint32_t restSumMs = 0;           // součet dokončených pauz
    uint8_t restCount = 0;            // počet dokončených pauz
};
