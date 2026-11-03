#include "HrBuffer.h"

void HrBuffer::push(uint16_t hr) {
    samples[head] = hr;
    head++;
    count++;
    if (head >= HR_BUFFER_SIZE) {
        head = 0;
    }
    if (count > HR_BUFFER_SIZE) {
        count = HR_BUFFER_SIZE;
    }
}

size_t HrBuffer::size() const {
    return count;
}

bool HrBuffer::get(size_t index, uint16_t& hr) const {
    if (index >= count) {
        return false;
    }
    else {
        size_t oldest = (count == HR_BUFFER_SIZE) ? head : 0;
        hr = samples[(oldest + index) % HR_BUFFER_SIZE];
        return true;
    }
}