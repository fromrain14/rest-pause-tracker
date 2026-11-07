#pragma once
#include <LittleFS.h>
#include <Preferences.h>
#include "StateMachine.h"

class Logger {
public:
    bool begin();         // připojí LittleFS , otevře NVS
    void startWorkout();  // číslo tréninku +1 do NVS, otevře nový soubor, zapíše hlavičku
    void logSample(uint32_t tS, uint16_t hr, State state, bool ready);  
    void endWorkout();    // flush + close
    void dumpLastWorkout(); // pro debug: vypíše poslední trénink na Serial


private:
    File file;
    Preferences preferences;
    uint16_t workoutNumber = 0;
    uint8_t samplesSinceFlush = 0;
};
