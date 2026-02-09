/**
 * @file AHT20Driver.cpp
 * @brief AHT20温湿度传感器驱动实现
 * @version 1.0.0
 */

#include "drivers/AHT20Driver.h"
#include "config/Config.h"

AHT20Driver::AHT20Driver()
    : _ready(false) {
}

bool AHT20Driver::begin(TwoWire& wire, uint8_t address) {
    _ready = _aht.begin(&wire, address);
    if (_ready) {
        DEBUG_PRINTLN("[AHT20] 传感器初始化成功");
    } else {
        DEBUG_PRINTLN("[AHT20] 传感器初始化失败");
    }
    return _ready;
}

bool AHT20Driver::read(float& temperature, float& humidity) {
    if (!_ready) {
        return false;
    }

    sensors_event_t humidityEvent;
    sensors_event_t tempEvent;
    if (!_aht.getEvent(&humidityEvent, &tempEvent)) {
        return false;
    }

    temperature = tempEvent.temperature;
    humidity = humidityEvent.relative_humidity;
    return true;
}

bool AHT20Driver::isReady() const {
    return _ready;
}
