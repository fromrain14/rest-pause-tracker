#pragma once
#include <cstdint>
#include <cstddef>
#include "config.h"
#include <Arduino.h>

class BleHeartRate {
    private:
        public:

        bool isValid(const uint8_t* data, size_t length, uint16_t& hrValue);

};


