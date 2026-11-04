#include "RecoveryAlgorithm.h"
#include "config.h"
#include <cmath>

void RecoveryAlgorithm::startRest() {
    secondsSinceRestStart = 0;
}

void RecoveryAlgorithm::onSample() {
    secondsSinceRestStart++;
}

bool RecoveryAlgorithm::findPeak(const HrBuffer& hrBuffer, uint16_t& peakHr, uint16_t& peakIndex) const {
    size_t bufferSize = hrBuffer.size();
    if (bufferSize == 0) {
        return false; // Buffer is empty
    }

    uint16_t maxHr = 0;
    peakIndex = 0;
    size_t start = 0;
    if (secondsSinceRestStart + HR_PEAK_LOOKBACK_SAMPLES < bufferSize) {
    start = bufferSize - secondsSinceRestStart - HR_PEAK_LOOKBACK_SAMPLES;
    }
    for (size_t i = start; i < bufferSize; ++i) {
        uint16_t hr;
        if (hrBuffer.get(i, hr)) {
            if (hr > maxHr) {
                maxHr = hr;
                peakIndex = i;
            }
        }
    }
    if (maxHr == 0) {
        return false; // No valid heart rate found
    }
    peakHr = maxHr;
    return true;
}
bool RecoveryAlgorithm::isPeakConfirmed(const HrBuffer& hrBuffer, uint16_t peakHr) const {
    if (hrBuffer.size() < HR_PEAK_HOLD_SAMPLES) {
        return false; // Not enough samples to confirm the peak
    }
    for (size_t i = 0; i < HR_PEAK_HOLD_SAMPLES; i++) {
        uint16_t hr;
        if (!hrBuffer.get(hrBuffer.size() - HR_PEAK_HOLD_SAMPLES + i, hr)) {
            return false; // Failed to get sample
        }
        if (hr == 0 || hr > peakHr - HR_PEAK_DROP_BMP) {
            return false; // Peak not confirmed
        }
    }
    return true; // Peak confirmed
}
bool RecoveryAlgorithm::linearFit(const float* x, const float* y, size_t n, float& slope, float& intercept) const {
    if (n < 2) {
        return false; // No data points
    }

    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumXY = 0.0f;
    float sumX2 = 0.0f;

    for (size_t i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }

    float denom = n * sumX2 - sumX * sumX;
    if (denom == 0) {
        return false; // Cannot compute slope and intercept
    }

    slope = (n * sumXY - sumX * sumY) / denom;
    intercept = (sumY - slope * sumX) / n;

    return true; // Successfully computed slope and intercept
}

bool RecoveryAlgorithm::fitExponential(const HrBuffer& hrBuffer, uint16_t peakIndex, float& a, float& tau, float c) const {
    float x[HR_BUFFER_SIZE];
    float y[HR_BUFFER_SIZE];
    size_t n = 0;
    for (size_t i = peakIndex; i<hrBuffer.size();++i) {
        uint16_t hr;
        if (hrBuffer.get(i, hr)) {
            if (!(hr == 0 || hr <= c)) {
                x[n] = i - peakIndex; // Time since peak
                y[n] = logf(hr-c); // Log of (HR - c)
                n++;
            }
        }
    }
    float slope, intercept;
    if (!linearFit(x, y, n, slope, intercept)) {
        return false; // Linear fit failed
    }
    if (slope >= 0) {
        return false; // Slope should be negative for decay
    }
    a = expf(intercept);
    tau = -1.0f / slope;
    return true;
}

float RecoveryAlgorithm::fitError(const HrBuffer& hrBuffer, uint16_t peakIndex, float a, float tau, float c) const {
    float error = 0.0f;
    for (size_t i = peakIndex; i<hrBuffer.size();++i) {
        uint16_t hr;
        if (hrBuffer.get(i, hr)) {
            if (!(hr == 0 || hr <= c)) {
                float t = i - peakIndex;  
                float predicted = a * expf(-t / tau) + c;
                error += (predicted - hr) * (predicted - hr);
            }
        }
    }
    return error;
}

bool RecoveryAlgorithm::findBestC(const HrBuffer& hrBuffer, uint16_t peakIndex, float& bestC, float& bestA, float& bestTau) const {
    uint16_t minHr = UINT16_MAX;
    for(size_t i = peakIndex; i<hrBuffer.size();++i) {
        uint16_t hr;
        if (hrBuffer.get(i, hr)) {
            if (hr != 0 && hr < minHr) {
                minHr = hr;
            }
        }
    }
    float a, tau;
     float error = INFINITY;
    for (float c = minHr - 1; c >= minHr - HR_C_SEARCH_RANGE; c -= 1.0f) {
        if (fitExponential(hrBuffer, peakIndex, a, tau, c)) {
            if(error>fitError(hrBuffer,peakIndex,a,tau,c)) {
                error = fitError(hrBuffer,peakIndex,a,tau,c);
                bestC = c;
                bestA = a;
                bestTau =tau;
            }
        }
    }
    if (error<UINT16_MAX) {
        return true;
    }
    else return false;
}

bool RecoveryAlgorithm::recentSlope(const HrBuffer& hrBuffer, float& slope) const {
    if (hrBuffer.size()<HR_SLOPE_WINDOW_SAMPLES) {
        return false;
    }
    float x[HR_SLOPE_WINDOW_SAMPLES];
    float y[HR_SLOPE_WINDOW_SAMPLES];
    size_t n = 0;
    for(size_t i = hrBuffer.size()-HR_SLOPE_WINDOW_SAMPLES; i<hrBuffer.size();++i) {
        uint16_t hr;
        if (hrBuffer.get(i, hr)) {
            if (hr == 0) {
                return false; // Invalid sample
            }
            x[n] = i - (hrBuffer.size()-HR_SLOPE_WINDOW_SAMPLES); // Time since start of window
            y[n] = hr;
            n++;
        }
    }
    float intercept;
    if (!linearFit(x, y, n, slope, intercept)) {
        return false; // Linear fit failed
    }
    return true;
}

bool RecoveryAlgorithm::isHrReady(const HrBuffer& hrBuffer) const {
    uint16_t peakHr, peakIndex;
    float bestC, bestA, bestTau, p, slope;
    uint16_t lastHr;
    if (!hrBuffer.get(hrBuffer.size() - 1, lastHr)) {
        return false; // Failed to get the last heart rate sample
    }
    if(!(findPeak(hrBuffer, peakHr, peakIndex))) {
        return false;
    }
    if(!isPeakConfirmed(hrBuffer, peakHr)) {
        return false;
    }
    if(findBestC(hrBuffer, peakIndex, bestC, bestA, bestTau)) {
        p =(peakHr - lastHr)/(peakHr - bestC);
        if (p >= HR_READY_RATIO) {
            return true; // Ready by exponential fit
        }
    }
    // Fit failed or p too low -> fallback: plateau by slope
    if(!recentSlope(hrBuffer, slope)) {
        return false;
    }
    return fabsf(slope) < HR_PLATEAU_SLOPE;
}
