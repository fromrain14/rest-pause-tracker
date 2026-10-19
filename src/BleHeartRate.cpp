#include "BleHeartRate.h"

bool BleHeartRate::isValid(const uint8_t* data, size_t length,  uint16_t& hrValue) {
    if (length < 1) {
        return false; 
    }
    uint8_t flags = data[0];
    if(flags & 0x01) {
        if (length < 3) {
            return false; 
        }
        else {
            hrValue = data[2] << 8 | data[1];
            return true;
        }
    } else {
        if (length < 2) {
            return false; 
        }
        else {
            hrValue = data[1];
            return true;
            
        }
    }

}