#pragma once
#include <cstdint>
#include <cstddef>
#include "config.h"
#include <Arduino.h>
#include <NimBLEDevice.h>

class BleHeartRate : public NimBLEScanCallbacks, public NimBLEClientCallbacks {
    private:
        uint16_t lastHeartRate = 0;
        bool connected = false;
        uint32_t lastHeartRateUpdateTime = 0;
        NimBLEAddress foundAdress;
        bool doConnect = false;
        NimBLEClient* client = nullptr;
        bool connectToStrap();


    public:
        void update();

        void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
        void onDisconnect(NimBLEClient* pClient, int reason) override;
        
        bool isValid(const uint8_t* data, size_t length, uint16_t& hrValue);
        bool begin();
        bool getHeartRate(uint16_t& hr) const;
        bool isConnected() const;
};
    


