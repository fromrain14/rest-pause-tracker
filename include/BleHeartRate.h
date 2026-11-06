#pragma once
#include <cstdint>
#include <cstddef>
#include "config.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Preferences.h>

class BleHeartRate : public NimBLEScanCallbacks, public NimBLEClientCallbacks {
    private:
        uint16_t lastHeartRate = 0;
        bool connected = false;
        uint32_t lastHeartRateUpdateTime = 0;
        NimBLEAddress foundAdress;
        bool doConnect = false;
        NimBLEClient* client = nullptr;
        bool connectToStrap();
        Preferences preferences;
        NimBLEAddress savedAddress;
        // Zámek pro lastHeartRate + lastHeartRateUpdateTime (zapisuje NimBLE task, čte loop())
        mutable portMUX_TYPE hrMux = portMUX_INITIALIZER_UNLOCKED;

    public:
        void update();

        void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
        void onDisconnect(NimBLEClient* pClient, int reason) override;
        
        bool isValid(const uint8_t* data, size_t length, uint16_t& hrValue);
        bool begin();
        bool getHeartRate(uint16_t& hr) const;
        bool isConnected() const;

        void removeSavedAddress() {
            preferences.remove("address");
            preferences.remove("type");
            savedAddress = NimBLEAddress();
        }
};
    


