/**
 * @file BH1750Sensor.cpp
 * @brief BH1750光照传感器驱动实现
 */

#include "BH1750Sensor.h"
#include "config/Config.h"

BH1750Sensor::BH1750Sensor() 
    : _initialized(false)
    , _lastReadTime(0)
    , _lastLight(0.0f) {
}

bool BH1750Sensor::begin() {
    DEBUG_PRINTLN("[BH1750] Initializing...");
    
    if (_bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_I2C_ADDR)) {
        _initialized = true;
        DEBUG_PRINTLN("[BH1750] Initialization successful");
        return true;
    }
    
    DEBUG_PRINTLN("[BH1750] Initialization failed!");
    _initialized = false;
    return false;
}

SensorData BH1750Sensor::read() {
    SensorData data;
    data.timestamp = millis();
    
    if (!_initialized) {
        data.status = SensorStatus::NOT_INITIALIZED;
        return data;
    }
    
    float lux = _bh1750.readLightLevel();
    
    if (lux < 0) {
        DEBUG_PRINTLN("[BH1750] Read error");
        data.status = SensorStatus::READ_ERROR;
        return data;
    }
    
    _lastLight = lux;
    _lastReadTime = data.timestamp;
    
    data.value = lux;
    data.status = SensorStatus::OK;
    
    DEBUG_PRINTF("[BH1750] Light=%.0flux\n", lux);
    
    return data;
}

float BH1750Sensor::readLightLevel() {
    SensorData data = read();
    return (data.status == SensorStatus::OK) ? data.value : _lastLight;
}

bool BH1750Sensor::isAvailable() {
    return _initialized;
}

const char* BH1750Sensor::getName() {
    return "BH1750";
}

void BH1750Sensor::reset() {
    DEBUG_PRINTLN("[BH1750] Resetting...");
    _initialized = false;
    _lastLight = 0.0f;
    _lastReadTime = 0;
    begin();
}
