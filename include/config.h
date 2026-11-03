#pragma once
#include <cstdint>

//Display
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t SCREEN_I2C_ADDRESS = 0x3C;

//Pin
constexpr uint8_t LED_PIN = 1;
constexpr uint8_t BUTTON_PIN_A = 2;
constexpr uint8_t BUTTON_PIN_B = 3;
constexpr uint8_t SDA_PIN = 6;
constexpr uint8_t SCL_PIN = 7;

//LEDS
constexpr uint8_t NUM_LEDS = 1;   // jedna 5mm LED s čipem WS2811
constexpr uint8_t LED_BRIGHTNESS = 64; // 0–255, 5mm LED na plný jas oslňuje

//Buttons
constexpr uint32_t DEBOUNCE_DELAY_MS = 50;
constexpr uint32_t LONG_PRESS_MS = 3000;
constexpr uint32_t MIN_REST_TIME_MS = 120000; // 2 minutes
constexpr uint32_t MAX_REST_TIME_MS = 240000; // 4 minutes

//Heart Rate
constexpr uint32_t HR_MAX_AGE_MS = 3000;

//Buffer
constexpr uint32_t HR_BUFFER_SIZE = 300; // 60 s pred stiskem A + max. 4 min pauzy

//Recovery Algorithm
constexpr uint8_t HR_PEAK_DROP_BMP = 3;
constexpr uint8_t HR_PEAK_HOLD_SAMPLES = 5;
constexpr uint8_t HR_C_SEARCH_RANGE = 40;
constexpr uint8_t HR_SLOPE_WINDOW_SAMPLES = 20;
constexpr uint8_t HR_PEAK_LOOKBACK_SAMPLES = 60;