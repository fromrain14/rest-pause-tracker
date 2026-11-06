#include "SerialSimulator.h"
#include <cstring>
#include <cstdlib>

void SerialSimulator::update() {
    simulatedBtnA = ButtonEvent::NONE;
    simulatedBtnB = ButtonEvent::NONE;
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') {
            buffer[numChars] = '\0';
            processLine(buffer);
            numChars = 0;
        } else if (c != '\r' && numChars < SIM_LINE_SIZE - 1) {
            buffer[numChars++] = c;
        }
    }
}
bool SerialSimulator::processLine(const char* line) {
    if (strncmp(line, "BTN,A", 11) == 0) {
        simulatedBtnA = ButtonEvent::SHORT_PRESS;
        return true;
    } else if (strncmp(line, "BTN,A_LONG", 10) == 0) {
        simulatedBtnA = ButtonEvent::LONG_PRESS;
        return true;
    } else if (strncmp(line, "BTN,B", 11) == 0) {
        simulatedBtnB = ButtonEvent::SHORT_PRESS;
        return true;
    } else if (strncmp(line, "BTN,B_LONG", 10) == 0) {
        simulatedBtnB = ButtonEvent::LONG_PRESS;
        return true;
    } else if (strncmp(line, "HR,", 3) == 0) {
        const char* hrStr = line + 3;
        char* endPtr;
        long hrValue = strtol(hrStr, &endPtr, 10);
        if (*endPtr == '\0' && hrValue >= 0 && hrValue <= UINT16_MAX) {
            simulatedHr = static_cast<uint16_t>(hrValue);
            return true;
        }
    }
    return false; // Neplatný příkaz
}
ButtonEvent SerialSimulator::getButtonA() const {
    return simulatedBtnA;
}
ButtonEvent SerialSimulator::getButtonB() const {
    return simulatedBtnB;
}
bool SerialSimulator::getHeartRate(uint16_t& hr) const {
    if (simulatedHr > 0) {
        hr = simulatedHr;
        return true;
    }
    return false;
}