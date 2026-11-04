#pragma once
#include <cstdint>
#include "config.h"
#include "HrBuffer.h"

class RecoveryAlgorithm {
    public:
        bool findPeak(const HrBuffer& hrBuffer, uint16_t& peakHr, uint16_t& peakIndex) const;

        bool isPeakConfirmed(const HrBuffer& hrBuffer, uint16_t peakHr) const;

        bool linearFit(const float* x, const float* y, size_t n, float& slope, float& intercept) const;

        bool fitExponential(const HrBuffer& hrBuffer, uint16_t peakIndex, float& a, float& tau, float c) const;

        float fitError(const HrBuffer& hrBuffer, uint16_t peakIndex, float a, float tau, float c) const;
        
        bool findBestC(const HrBuffer& hrBuffer, uint16_t peakIndex, float& bestC, float& bestA, float& bestTau) const;

        bool recentSlope(const HrBuffer& hrBuffer, float& slope) const;

        void startRest();
        void onSample();

        bool isHrReady(const HrBuffer& hrBuffer) const;
    private:
        uint32_t secondsSinceRestStart = 0;

};