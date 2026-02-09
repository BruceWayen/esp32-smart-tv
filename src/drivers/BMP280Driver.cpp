/**
 * @file BMP280Driver.cpp
 * @brief BMP280气压传感器驱动实现
 * @version 1.0.0
 */

#include "drivers/BMP280Driver.h"
#include "config/Config.h"
#include <cmath>

BMP280Driver::BMP280Driver()
    : _ready(false) {
}

bool BMP280Driver::begin(TwoWire& wire, uint8_t address) {
    _ready = _bmp.begin(address, &wire);
    if (_ready) {
        _bmp.setSampling(
            Adafruit_BMP280::MODE_NORMAL,
            Adafruit_BMP280::SAMPLING_X2,
            Adafruit_BMP280::SAMPLING_X16,
            Adafruit_BMP280::FILTER_X16,
            Adafruit_BMP280::STANDBY_MS_500
        );
        DEBUG_PRINTLN("[BMP280] 传感器初始化成功");
    } else {
        DEBUG_PRINTLN("[BMP280] 传感器初始化失败");
    }
    return _ready;
}

bool BMP280Driver::readPressure(float& pressureHpa) {
    if (!_ready) {
        return false;
    }

    float pressurePa = _bmp.readPressure();
    if (pressurePa <= 0.0f || !isfinite(pressurePa)) {
        return false;
    }
    pressureHpa = pressurePa / 100.0f;
    return true;
}

bool BMP280Driver::isReady() const {
    return _ready;
}
