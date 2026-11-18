#pragma once
#include <cstdint>
#include <cstddef>

//Display
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t SCREEN_I2C_ADDRESS = 0x3C;

//Pin
constexpr uint8_t LED_PIN = 5;    // D3 = GPIO 5 (D10 is damaged on this board). Pin numbers are always GPIO, not the Dx label
constexpr uint8_t BUTTON_PIN_A = 2;
constexpr uint8_t BUTTON_PIN_B = 3;
constexpr uint8_t SDA_PIN = 6;
constexpr uint8_t SCL_PIN = 7;

//LEDS
constexpr uint8_t NUM_LEDS = 1;   // a single 5 mm LED with a WS2811 chip
constexpr uint8_t LED_BRIGHTNESS = 64; // 0-255, a 5 mm LED at full brightness is blinding

//Buttons
constexpr uint32_t DEBOUNCE_DELAY_MS = 50;
constexpr uint32_t LONG_PRESS_MS = 3000;
constexpr uint32_t MIN_REST_TIME_MS = 120000; // 2 minutes
constexpr uint32_t MAX_REST_TIME_MS = 240000; // 4 minutes

//Heart Rate
constexpr uint32_t HR_MAX_AGE_MS = 3000;

//Buffer
constexpr uint32_t HR_BUFFER_SIZE = 300; // 60 s before pressing A + max. 4 min rest

//Recovery Algorithm
constexpr uint8_t HR_PEAK_DROP_BMP = 3;
constexpr uint8_t HR_PEAK_HOLD_SAMPLES = 5;
constexpr uint8_t HR_C_SEARCH_RANGE = 40;
constexpr uint8_t HR_SLOPE_WINDOW_SAMPLES = 20;
constexpr uint8_t HR_PEAK_LOOKBACK_SAMPLES = 60;
constexpr float HR_READY_RATIO = 0.90f;
constexpr float HR_PLATEAU_SLOPE = 0.05f;

//Logger
constexpr uint8_t LOG_FLUSH_EVERY_SAMPLES = 30; // flush to flash every 30 samples (= max. 30 s of data lost on power loss)

//Battery
constexpr uint8_t BATTERY_PIN = 4;                   // D2 = GPIO 4, 2x 220k divider from BAT+
constexpr uint32_t BATTERY_MEASURE_INTERVAL_MS = 10000; // voltage changes slowly -> measure once every 10 s
constexpr uint8_t BATTERY_SAMPLES = 16;              // ADC reads per measurement (averaged to reduce noise)
constexpr uint8_t BATTERY_DIVIDER_RATIO = 2;         // 2 equal resistors -> the pin sees half the battery voltage

//Serial Simulator
constexpr bool SIMULATION = false; // true = HR and buttons also from Serial Monitor (HR,140 / BTN,A / BTN,B_LONG)
constexpr size_t SIM_LINE_SIZE = 32; // max. command length including '\0' (e.g. "BTN,B_LONG")