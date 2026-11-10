#include "BatteryMonitor.h"

// Jeden bod vybíjecí křivky LiPol: napětí baterie → zbývající %
struct BatteryPoint {
    uint16_t millivolts;
    uint8_t percent;
};

// Body seřazené od nejvyššího napětí; přibližné hodnoty z typických křivek LiPol
constexpr BatteryPoint BATTERY_CURVE[] = {
    {4200, 100},
    {4060,  90},
    {3980,  80},
    {3920,  70},
    {3870,  60},
    {3820,  50},
    {3790,  40},
    {3770,  30},
    {3740,  20},
    {3680,  10},
    {3450,   5},
    {3300,   0},
};
constexpr size_t BATTERY_CURVE_SIZE = sizeof(BATTERY_CURVE) / sizeof(BATTERY_CURVE[0]);

uint16_t BatteryMonitor::measure() const {
    uint32_t sum = 0;
    for (int i=0; i<BATTERY_SAMPLES; ++i) {
        sum += analogReadMilliVolts(BATTERY_PIN);
    }
    sum /= BATTERY_SAMPLES;
    return sum * BATTERY_DIVIDER_RATIO;
}

void BatteryMonitor::begin() {
    millivolts = measure();
    lastMeasureMs = millis();
}

void BatteryMonitor::update(uint32_t now) {
    if (now - lastMeasureMs >= BATTERY_MEASURE_INTERVAL_MS) {
        millivolts = measure();
        lastMeasureMs = now;
    }
}

uint16_t BatteryMonitor::getMillivolts() const {
    return millivolts;
}

uint8_t BatteryMonitor::getPercent() const {
    // Mimo tabulku: plná / prázdná
    if (millivolts >= BATTERY_CURVE[0].millivolts) {
        return 100;
    }
    if (millivolts <= BATTERY_CURVE[BATTERY_CURVE_SIZE - 1].millivolts) {
        return 0;
    }
    
    for (size_t i = 0; i < BATTERY_CURVE_SIZE - 1; ++i) {
        const BatteryPoint& high = BATTERY_CURVE[i];
        const BatteryPoint& low = BATTERY_CURVE[i + 1];
        if (millivolts >= low.millivolts) {
            // Lineární interpolace: kolik mV nad low → úměrně kolik % nad low
            return low.percent + (millivolts - low.millivolts) * (high.percent - low.percent)
                                 / (high.millivolts - low.millivolts);
        }
    }
    return 0; 
}