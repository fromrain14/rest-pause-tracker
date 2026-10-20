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
bool BleHeartRate::begin() {
    if (!NimBLEDevice::init("")) {
        return false;
    }
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(this);
    pScan->setActiveScan(true);
    if (! pScan->start(0)) {
        return false;
    }
    return true;
}
void BleHeartRate::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
     if (advertisedDevice->isAdvertisingService(NimBLEUUID((uint16_t)0x180D))) {
        foundAdress = advertisedDevice->getAddress();
        doConnect = true;
      //  Serial.println(advertisedDevice->getName().c_str());
      //  Serial.println(advertisedDevice->getAddress().toString().c_str());
        NimBLEDevice::getScan()->stop();
     }
}
void BleHeartRate::update(){
    if (doConnect) {
        doConnect = false;
        connectToStrap();
    }
}
bool BleHeartRate::connectToStrap() {
    if (client == nullptr) {
        client = NimBLEDevice::createClient();
        if (client != nullptr) {
            client->setClientCallbacks(this, false);
        }
    }
    if (client != nullptr) {
        bool connected = client->connect(foundAdress);
        if(!connected) {
            NimBLEDevice::getScan()->start(0);
            return false;
        }
        NimBLERemoteService* remoteService = client->getService(NimBLEUUID((uint16_t)0x180D));
        if (remoteService == nullptr) {
            client->disconnect();
            return false;
        }
        NimBLERemoteCharacteristic* remoteCharacteristic = remoteService->getCharacteristic(NimBLEUUID((uint16_t)0x2A37));
        if (remoteCharacteristic == nullptr) {
            client->disconnect();
            return false;
        }
        bool subscribed = remoteCharacteristic->subscribe(true,
            [this](NimBLERemoteCharacteristic* c, uint8_t* data, size_t length, bool isNotify) {
                uint16_t hr;
                if (isValid(data, length, hr)) {
                    lastHeartRate = hr;
                    lastHeartRateUpdateTime = millis();
                }
            });
        if (!subscribed) {
            client->disconnect();
            return false;
        }
    }
    else{
         return false;}
    BleHeartRate::connected=true;
    return true;
}
bool BleHeartRate::getHeartRate(uint16_t& hr) const {
    hr = lastHeartRate;
    if(!hr || (millis()-lastHeartRateUpdateTime)>HR_MAX_AGE_MS) {
        return false;
    }
    else
        return true;
}
bool BleHeartRate::isConnected() const {
    return connected;
}
void BleHeartRate::onDisconnect(NimBLEClient* pClient, int reaso){
    connected = false;
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(this);
    pScan->setActiveScan(true);
    pScan->start(0);

}