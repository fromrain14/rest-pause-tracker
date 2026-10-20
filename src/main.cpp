#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FastLED.h>
#include "Button.h"
#include "StateMachine.h"
#include "DisplayUI.h"
#include "LedController.h"
#include "BleHeartRate.h"

Button btnA(BUTTON_PIN_A, INPUT_PULLUP);
Button btnB(BUTTON_PIN_B, INPUT_PULLUP);

StateMachine fsm;

DisplayUI displayUI;

LedController ledController;

BleHeartRate ble;

// Dočasná simulace tepu (než bude BLE): tep je "ready" po této době od začátku pauzy.
// < min. pauza → RED → ORANGE (tep) → READY, > min. pauza → RED → ORANGE (čas) → READY
constexpr uint32_t FAKE_HR_READY_MS = 30000;

void setup() {
    btnA.begin();
    btnB.begin();
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!displayUI.begin()) {
        Serial.println("Display initialization failed");
    }
    ledController.begin();
     if (!ble.begin()) {
        Serial.println("Ble initialization failed");
    }
}

void loop() {   
    uint32_t now = millis();
    State currentState = fsm.getState();
    ButtonEvent btnAEvent = btnA.update();
    ButtonEvent btnBEvent = btnB.update();

    if (btnAEvent != ButtonEvent::NONE) {
        Serial.print("Btn A Event: ");
        Serial.println((int)btnAEvent);
    }
        
    if (btnBEvent != ButtonEvent::NONE) {
        Serial.print("Btn B Event: ");
        Serial.println((int)btnBEvent);
    }

    
    bool hrReady = fsm.getRestElapsedMs(now) >= FAKE_HR_READY_MS;
    fsm.update(btnAEvent, btnBEvent, hrReady, now); 
    displayUI.draw(fsm, now);
    ledController.updateLEDs(fsm);
    if (currentState != fsm.getState()) {
        Serial.print("State changed to: ");
        Serial.println(fsm.printState(fsm.getState()));
        
    }

    ble.update();
    // Dočasný ladicí výpis tepu 1× za sekundu
    static uint32_t lastHrPrintMs = 0;
    if (now - lastHrPrintMs >= 1000) {
        lastHrPrintMs = now;
        uint16_t heartRate;
        if (ble.getHeartRate(heartRate)) {
            Serial.print("HR: ");
            Serial.println(heartRate);
        } else {
            Serial.println("HR: --");
        }
    }

}   