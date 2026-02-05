/**
 * @file MockEnvironmentProvider.cpp
 * @brief 模拟环境数据提供者实现
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "MockEnvironmentProvider.h"
#include "config/Config.h"
#include <math.h>

MockEnvironmentProvider::MockEnvironmentProvider()
    : _data()
    , _lastUpdate(0)
    , _phase(0.0f) {}

bool MockEnvironmentProvider::begin() {
    DEBUG_PRINTLN("[MockEnv] 使用模拟环境数据");
    _lastUpdate = 0;
    _phase = 0.0f;
    return true;
}

bool MockEnvironmentProvider::update() {
    uint32_t now = millis();
    if (now - _lastUpdate < SENSOR_SAMPLE_INTERVAL_MS) {
        return false;
    }
    _lastUpdate = now;
    _phase += 0.1f;

    _data.temperature = 24.0f + 2.0f * sin(_phase);
    _data.humidity = 50.0f + 10.0f * cos(_phase * 0.8f);
    _data.pressure = 1010.0f + 5.0f * sin(_phase * 0.6f);
    _data.lightLevel = 120.0f + 60.0f * sin(_phase * 1.2f);
    _data.timestamp = now;

    DEBUG_PRINTF("[MockEnv] T=%.1f℃ H=%.1f%% P=%.1fhPa L=%.0flux\n",
                 _data.temperature, _data.humidity, _data.pressure, _data.lightLevel);
    return true;
}

const EnvironmentData& MockEnvironmentProvider::getData() const {
    return _data;
}
