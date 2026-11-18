#include "StateMachine.h"

void StateMachine::update(ButtonEvent a, ButtonEvent b, bool hrReady, uint32_t now) {
    switch (state) {
        case State::SETUP:
            // A short -> READY (start workout)
            // B short -> +15 s minimum rest
            if (a == ButtonEvent::SHORT_PRESS) {
                state = State::READY;
                workoutStartMs = now;
                setCount = 0;
                restSumMs = 0;
                restCount = 0;
            }
            else if (b == ButtonEvent::SHORT_PRESS) {
                minPauseMs += 15000; // 15 s
                if (minPauseMs > MAX_REST_TIME_MS) {
                    minPauseMs = MIN_REST_TIME_MS;
                }
            }
            break;

        case State::READY:
            if (a == ButtonEvent::SHORT_PRESS) {
                if (wasAPress) {
                    restCount++;
                    restSumMs += now - lastAPressMs;
                }
                else {
                    lastAPressMs = now;
                    wasAPress = true;
                }
                lastAPressMs = now;
                setCount++;
                state = State::REST_RED;
            }
            else if (b == ButtonEvent::LONG_PRESS) {
                state = State::SUMMARY;
                workoutDurationMs = now - workoutStartMs; 
            }
            
            break;

        case State::REST_RED:
            if (a == ButtonEvent::LONG_PRESS) {
                state = State::READY;
            }
            else if (b == ButtonEvent::LONG_PRESS) {
                state = State::SUMMARY;
                workoutDurationMs = now - workoutStartMs; 
            }
            else if (hrReady && (now - lastAPressMs) >= minPauseMs) {
                state = State::READY;
            }
            else if ((hrReady && !((now - lastAPressMs) >= minPauseMs)) || (!hrReady && (now - lastAPressMs) >= minPauseMs)) {
                state = State::REST_ORANGE;
            }
            break;

        case State::REST_ORANGE:
            // A long -> READY (skip rest)
            // B long -> SUMMARY
            // HR ready and min. rest elapsed -> READY
            if (a == ButtonEvent::LONG_PRESS) {
                state = State::READY;
            }
            else if (b == ButtonEvent::LONG_PRESS) {
                state = State::SUMMARY;
                workoutDurationMs = now - workoutStartMs; 
            }
            else if (hrReady && (now - lastAPressMs) >= minPauseMs) {
                state = State::READY;
            }
            break;

        case State::SUMMARY:
            // any button -> SETUP
            if (a != ButtonEvent::NONE || b != ButtonEvent::NONE) {
                state = State::SETUP;
                wasAPress = false;
            }
            break;
    }
}
const char* StateMachine::printState(State s) const {
    switch (s) {
        case State::SETUP:
            return "SETUP";
        case State::READY:
            return "READY";
        case State::REST_RED:
            return "REST_RED";
        case State::REST_ORANGE:
            return "REST_ORANGE";
        case State::SUMMARY:
            return "SUMMARY";
        default:
            return "UNKNOWN";
    }
}
State StateMachine::getState() const {
    return state;
}

uint32_t StateMachine::getMinPauseMs() const {
    return minPauseMs;
}

uint8_t StateMachine::getSetCount() const {
    return setCount;
}

uint32_t StateMachine::getWorkoutDurationMs() const {
    return workoutDurationMs;
}

uint32_t StateMachine::getRestElapsedMs(uint32_t now) const {
    return now - lastAPressMs;
}

uint32_t StateMachine::getAvgRestMs() const {
    if (restCount == 0) {
        return 0;
    }
    return restSumMs / restCount;
}