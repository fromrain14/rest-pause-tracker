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
uint32_t logStartMs = 0; // čas startu tréninku pro sloupec t v logu

BatteryMonitor battery;

void setup() {
    btnA.begin();
    btnB.begin();
    Serial.begin(115200);
    // USB Serial na C3: po resetu chvíli trvá, než se monitor znovu připojí → počkat max. 3 s
    while (!Serial && millis() < 3000) {
        delay(10);
    }
    Serial.println("Build: " __DATE__ " " __TIME__); // ověření, že na desce běží poslední nahraný firmware
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
    // Příkazy ze Serialu se čtou vždy (DUMP funguje i s pásem)
    sim.update();
    if (sim.getDumpRequest()) {
        logger.dumpLastWorkout();
    }
    // Simulace: příkaz ze Serialu přebije fyzické tlačítko (fyzická tlačítka fungují dál)
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

    
    // Výsledek algoritmu, počítá se 1× za sekundu po novém vzorku (viz níže)
    static bool hrReady = false;
    fsm.update(btnAEvent, btnBEvent, hrReady, now);
    displayUI.draw(fsm, now, battery.getPercent());
    ledController.updateLEDs(fsm);
    if (currentState != fsm.getState()) {
        Serial.print("State changed to: ");
        Serial.println(fsm.printState(fsm.getState()));
        // Vstup do pauzy (jen z READY po stisku A) → algoritmus počítá od nuly
        if (fsm.getState() == State::REST_RED) {
            recovery.startRest();
            hrReady = false;
        }
        // Start tréninku → nový log soubor; konec tréninku → zavřít log
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

    // Dlouhé B v SETUP → zapomenout uložený pás (FSM na dlouhé B v SETUP nereaguje)
    if (currentState == State::SETUP && btnBEvent == ButtonEvent::LONG_PRESS) {
        ble.removeSavedAddress();
        Serial.println("Saved strap address removed");
    }

    ble.update();
    // Ladicí výpis připojení / odpojení pásu
    static bool wasConnected = false;
    if (ble.isConnected() != wasConnected) {
        wasConnected = ble.isConnected();
        Serial.println(wasConnected ? "BLE: strap connected" : "BLE: strap disconnected");
    }
    // Vzorek tepu do bufferu 1× za sekundu (0 = bez dat, aby seděla časová osa) + ladicí výpis
    static uint32_t lastHrSampleMs = 0;
    if (now - lastHrSampleMs >= 1000) {
        lastHrSampleMs = now;
        uint16_t heartRate;
        bool hasHeartRate = SIMULATION ? sim.getHeartRate(heartRate) : ble.getHeartRate(heartRate);
        if (!hasHeartRate) {
            heartRate = 0;
        }
        hrBuffer.push(heartRate);
        // Ladicí výpis jen při změně tepu, ať Serial Monitor neroluje a dá se psát
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
        // Mimo trénink je soubor zavřený → logSample nic nezapíše
        logger.logSample((now - logStartMs) / 1000, heartRate, fsm.getState(), hrReady);
    }

    battery.update(now);
    // Ladicí výpis baterie 1× za interval měření (bez zapojeného děliče ukazuje nesmysly — pin visí ve vzduchu)
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