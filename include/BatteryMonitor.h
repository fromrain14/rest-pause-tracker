#pragma once
#include <cstdint>
#include <cstddef>
#include "config.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Preferences.h>


class BatteryMonitor {
public:
    void begin();                    
    void update(uint32_t now);       
    uint16_t getMillivolts() const;  
    uint8_t getPercent() const;      
private:
    uint16_t measure() const;        
    uint16_t millivolts = 0;
    uint32_t lastMeasureMs = 0;
};
