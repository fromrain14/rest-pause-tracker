#include "Logger.h"

bool Logger::begin() {
    if (!LittleFS.begin(true)) {
        return false;
    }
    if (!preferences.begin("logger")) {
        return false;
    }
    workoutNumber = preferences.getUShort("num", 0);
    return true;
}
void Logger::startWorkout() {
    workoutNumber++;
    preferences.putUShort("num", workoutNumber);
    char name[32];
    snprintf(name, sizeof(name), "/workout_%04u.csv", workoutNumber);
    file = LittleFS.open(name, "w");
    if (file) {
        file.printf("t,hr,state,ready\n");
    }
    samplesSinceFlush = 0;
}
void Logger::logSample(uint32_t tS, uint16_t hr, State state, bool ready) {
    if (!file) {
        return;
    }
    file.printf("%lu,%u,%d,%d\n", tS, hr, static_cast<int>(state), ready ? 1 : 0);
    samplesSinceFlush++;

    if (samplesSinceFlush >= LOG_FLUSH_EVERY_SAMPLES) {
        file.flush();
        samplesSinceFlush = 0;
    }
}

void Logger::endWorkout() {
    if (file) {
        file.flush();
        file.close();
    }
}

void Logger::dumpLastWorkout() {
    char name[32];
    snprintf(name, sizeof(name), "/workout_%04u.csv", workoutNumber);
    File f = LittleFS.open(name, "r");
    if (!f) {
        Serial.println("Failed to open last workout file");
        return;
    }
    Serial.printf("Dumping last workout (%s):\n", name);
    while (f.available()) {
        Serial.write(f.read());
    }
    f.close();
}