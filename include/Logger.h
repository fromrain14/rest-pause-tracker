#pragma once
#include <LittleFS.h>
#include <Preferences.h>
#include "StateMachine.h"

class Logger {
public:
    bool begin();         // mounts LittleFS, opens NVS
    void startWorkout();  // increments workout number in NVS, opens a new file, writes the CSV header
    void logSample(uint32_t tS, uint16_t hr, State state, bool ready);  
    void endWorkout();    // flush + close
    void dumpLastWorkout(); // debug: prints the last workout to Serial


private:
    File file;
    Preferences preferences;
    uint16_t workoutNumber = 0;
    uint8_t samplesSinceFlush = 0;
};
