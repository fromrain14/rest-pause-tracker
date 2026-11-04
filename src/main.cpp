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
#include "HrBuffer.h"
#include "RecoveryAlgorithm.h"

Button btnA(BUTTON_PIN_A, INPUT_PULLUP);
Button btnB(BUTTON_PIN_B, INPUT_PULLUP);

StateMachine fsm;

DisplayUI displayUI;

LedController ledController;

BleHeartRate ble;

HrBuffer hrBuffer;

RecoveryAlgorithm recovery;

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

    
    // Výsledek algoritmu, počítá se 1× za sekundu po novém vzorku (viz níže)
    static bool hrReady = false;
    fsm.update(btnAEvent, btnBEvent, hrReady, now);
    displayUI.draw(fsm, now);
    ledController.updateLEDs(fsm);
    if (currentState != fsm.getState()) {
        Serial.print("State changed to: ");
        Serial.println(fsm.printState(fsm.getState()));
        // Vstup do pauzy (jen z READY po stisku A) → algoritmus počítá od nuly
        if (fsm.getState() == State::REST_RED) {
            recovery.startRest();
            hrReady = false;
        }
    }

    ble.update();
    // Vzorek tepu do bufferu 1× za sekundu (0 = bez dat, aby seděla časová osa) + ladicí výpis
    static uint32_t lastHrSampleMs = 0;
    if (now - lastHrSampleMs >= 1000) {
        lastHrSampleMs = now;
        uint16_t heartRate;
        if (ble.getHeartRate(heartRate)) {
            hrBuffer.push(heartRate);
            Serial.print("HR: ");
            Serial.println(heartRate);
        } else {
            hrBuffer.push(0);
            Serial.println("HR: --");
        }
        recovery.onSample();
        hrReady = recovery.isHrReady(hrBuffer);
    }

}   