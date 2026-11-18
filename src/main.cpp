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
#include "SerialSimulator.h"
#include "Logger.h"
#include "BatteryMonitor.h"

Button btnA(BUTTON_PIN_A, INPUT_PULLUP);
Button btnB(BUTTON_PIN_B, INPUT_PULLUP);

StateMachine fsm;

DisplayUI displayUI;

LedController ledController;

BleHeartRate ble;

HrBuffer hrBuffer;

RecoveryAlgorithm recovery;

SerialSimulator sim;

Logger logger;
uint32_t logStartMs = 0; // workout start time for the t column in the log

BatteryMonitor battery;

void setup() {
    btnA.begin();
    btnB.begin();
    Serial.begin(115200);
    // USB Serial on C3: the monitor takes a while to reconnect after reset -> wait up to 3 s
    while (!Serial && millis() < 3000) {
        delay(10);
    }
    Serial.println("Build: " __DATE__ " " __TIME__); // confirms the board runs the latest uploaded firmware
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!displayUI.begin()) {
        Serial.println("Display initialization failed");
    }
    ledController.begin();
     if (!ble.begin()) {
        Serial.println("Ble initialization failed");
    }
    if (!logger.begin()) {
        Serial.println("Logger initialization failed");
    }
    battery.begin();
}

void loop() {   
    uint32_t now = millis();
    State currentState = fsm.getState();
    ButtonEvent btnAEvent = btnA.update();
    ButtonEvent btnBEvent = btnB.update();
    // Serial commands are always read (DUMP works with the real strap too)
    sim.update();
    if (sim.getDumpRequest()) {
        logger.dumpLastWorkout();
    }
    // Simulation: a Serial command overrides the physical button (physical buttons still work)
    if (SIMULATION) {
        if (sim.getButtonA() != ButtonEvent::NONE) {
            btnAEvent = sim.getButtonA();
        }
        if (sim.getButtonB() != ButtonEvent::NONE) {
            btnBEvent = sim.getButtonB();
        }
    }

    if (btnAEvent != ButtonEvent::NONE) {
        Serial.print("Btn A Event: ");
        Serial.println((int)btnAEvent);
    }
        
    if (btnBEvent != ButtonEvent::NONE) {
        Serial.print("Btn B Event: ");
        Serial.println((int)btnBEvent);
    }

    
    // Algorithm result, recomputed once per second after a new sample (see below)
    static bool hrReady = false;
    fsm.update(btnAEvent, btnBEvent, hrReady, now);
    displayUI.draw(fsm, now, battery.getPercent());
    ledController.updateLEDs(fsm);
    if (currentState != fsm.getState()) {
        Serial.print("State changed to: ");
        Serial.println(fsm.printState(fsm.getState()));
        // Entering rest (only from READY after pressing A) -> algorithm starts from zero
        if (fsm.getState() == State::REST_RED) {
            recovery.startRest();
            hrReady = false;
        }
        // Workout start -> new log file; workout end -> close log
        if (currentState == State::SETUP && fsm.getState() == State::READY) {
            logger.startWorkout();
            logStartMs = now;
            Serial.println("Log started");
        }
        if (fsm.getState() == State::SUMMARY) {
            logger.endWorkout();
            Serial.println("Log closed");
        }
    }

    // Long B in SETUP -> forget the saved strap (the FSM ignores long B in SETUP)
    if (currentState == State::SETUP && btnBEvent == ButtonEvent::LONG_PRESS) {
        ble.removeSavedAddress();
        Serial.println("Saved strap address removed");
    }

    ble.update();
    // Debug print of strap connect / disconnect
    static bool wasConnected = false;
    if (ble.isConnected() != wasConnected) {
        wasConnected = ble.isConnected();
        Serial.println(wasConnected ? "BLE: strap connected" : "BLE: strap disconnected");
    }
    // Push one HR sample per second (0 = no data, keeps the time axis aligned) + debug print
    static uint32_t lastHrSampleMs = 0;
    if (now - lastHrSampleMs >= 1000) {
        lastHrSampleMs = now;
        uint16_t heartRate;
        bool hasHeartRate = SIMULATION ? sim.getHeartRate(heartRate) : ble.getHeartRate(heartRate);
        if (!hasHeartRate) {
            heartRate = 0;
        }
        hrBuffer.push(heartRate);
        // Print only on HR change so the Serial Monitor does not scroll and stays usable for input
        static uint16_t lastPrintedHr = UINT16_MAX;
        if (heartRate != lastPrintedHr) {
            lastPrintedHr = heartRate;
            Serial.print("HR: ");
            if (heartRate) {
                Serial.println(heartRate);
            } else {
                Serial.println("--");
            }
        }
        recovery.onSample();
        hrReady = recovery.isHrReady(hrBuffer);
        // Outside a workout the file is closed -> logSample writes nothing
        logger.logSample((now - logStartMs) / 1000, heartRate, fsm.getState(), hrReady);
    }

    battery.update(now);
    // Battery debug print once per measurement interval (shows garbage without the divider - pin is floating)
    static uint32_t lastBatteryPrintMs = 0;
    if (now - lastBatteryPrintMs >= BATTERY_MEASURE_INTERVAL_MS) {
        lastBatteryPrintMs = now;
        Serial.print("BAT: ");
        Serial.print(battery.getMillivolts());
        Serial.print(" mV, ");
        Serial.print(battery.getPercent());
        Serial.println(" %");
    }

}   