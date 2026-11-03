#pragma once
#include <cstdint>
#include "config.h"
#include <cstddef>

class HrBuffer {
    private:
        uint16_t samples[HR_BUFFER_SIZE] = {};
        size_t head = 0;
        size_t count = 0;

    public:
        void push(uint16_t hr);
        size_t size() const;
        bool get(size_t index, uint16_t& hr) const;
};