#pragma once
#include <cstdint>
#include <cstddef>
#include "config.h"
#include <Arduino.h>
#include "Button.h"

class SerialSimulator {
    private:
        char buffer[SIM_LINE_SIZE];
        size_t numChars = 0;
        uint16_t simulatedHr = 0;
        ButtonEvent simulatedBtnA = ButtonEvent::NONE;
        ButtonEvent simulatedBtnB = ButtonEvent::NONE;
        bool dumpRequested = false; // příkaz DUMP (platí jen v průchodu, kdy přišel)
        //Soukromá metoda na zpracování hotového řádku.
        bool processLine(const char* line);

    public:
        void update();
        ButtonEvent getButtonA() const;
        ButtonEvent getButtonB() const;
        bool getHeartRate(uint16_t& hr) const;
        bool getDumpRequest() const;
};
