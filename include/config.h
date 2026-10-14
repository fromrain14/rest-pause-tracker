#pragma once
#include <cstdint>

//Display
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;

//Pin
constexpr uint8_t LED_PIN = 1;
constexpr uint8_t BUTTON_PIN_A = 2;
constexpr uint8_t BUTTON_PIN_B = 3;
constexpr uint8_t SDA_PIN = 6;
constexpr uint8_t SCL_PIN = 7;

//LEDS
constexpr uint8_t NUM_LEDS = 21;

//Buttons
constexpr uint32_t DEBOUNCE_DELAY_MS = 50;
constexpr uint32_t LONG_PRESS_MS = 3000;
constexpr uint32_t MIN_REST_TIME_MS = 120000; // 2 minutes
constexpr uint32_t MAX_REST_TIME_MS = 240000; // 4 minutes
