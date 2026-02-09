/**
 * @file EnvironmentDataProcessor.cpp
 * @brief 环境数据处理层实现
 * @version 1.0.0
 */

#include "services/EnvironmentDataProcessor.h"
#include <algorithm>
#include <cmath>

EnvironmentDataProcessor::EnvironmentDataProcessor() {
    reset();
}

void EnvironmentDataProcessor::reset() {
    _tempFilter.reset();
    _humiFilter.reset();
    _pressFilter.reset();
}

void EnvironmentDataProcessor::MovingAverage::reset() {
    values.fill(0.0f);
    count = 0;
    index = 0;
}

void EnvironmentDataProcessor::MovingAverage::add(float value) {
    values[index] = value;
    index = (index + 1) % values.size();
    if (count < values.size()) {
        count++;
    }
}

float EnvironmentDataProcessor::MovingAverage::average() const {
    if (count == 0) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        sum += values[i];
    }
    return sum / static_cast<float>(count);
}

bool EnvironmentDataProcessor::MovingAverage::ready() const {
    return count == values.size();
}

bool EnvironmentDataProcessor::processSample(float temperature, float humidity, float pressureHpa, EnvironmentData& output) {
    if (!isfinite(temperature) || !isfinite(humidity) || !isfinite(pressureHpa)) {
        return false;
    }

    if (temperature < TEMP_MIN_VALID || temperature > TEMP_MAX_VALID) {
        return false;
    }

    humidity = std::clamp(humidity, HUMI_MIN_VALID, HUMI_MAX_VALID);
    pressureHpa = std::clamp(pressureHpa, PRESS_MIN_VALID, PRESS_MAX_VALID);

    _tempFilter.add(temperature);
    _humiFilter.add(humidity);
    _pressFilter.add(pressureHpa);

    output.temperature = _tempFilter.average();
    output.humidity = _humiFilter.average();
    output.pressure = _pressFilter.average();
    output.timestamp = millis();
    return true;
}
