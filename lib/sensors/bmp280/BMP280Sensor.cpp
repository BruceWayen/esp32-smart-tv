/**
 * @file BMP280Sensor.cpp
 * @brief BMP280气压传感器驱动实现
 */

#include "BMP280Sensor.h"
#include "config/Config.h"

BMP280Sensor::BMP280Sensor() 
    : _initialized(false)
    , _lastReadTime(0)
    , _lastPress(0.0f)
    , _lastTemp(0.0f) {
}

bool BMP280Sensor::begin() {
    DEBUG_PRINTLN("[BMP280] Initializing...");
    
    if (_bmp.begin(BMP280_I2C_ADDR)) {
        // 配置传感器
        _bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // 正常模式
                        Adafruit_BMP280::SAMPLING_X2,       // 温度过采样×2
                        Adafruit_BMP280::SAMPLING_X16,      // 气压过采样×16
                        Adafruit_BMP280::FILTER_X16,        // IIR滤波系数16
                        Adafruit_BMP280::STANDBY_MS_500);   // 待机时间500ms
        
        _initialized = true;
        DEBUG_PRINTLN("[BMP280] Initialization successful");
        return true;
    }
    
    DEBUG_PRINTLN("[BMP280] Initialization failed!");
    _initialized = false;
    return false;
}

SensorData BMP280Sensor::read() {
    SensorData data;
    data.timestamp = millis();
    
    if (!_initialized) {
        data.status = SensorStatus::NOT_INITIALIZED;
        return data;
    }
    
    float pressure = _bmp.readPressure() / 100.0f;  // Pa转hPa
    
    if (!validateData(pressure)) {
        DEBUG_PRINTF("[BMP280] Invalid data: P=%.2f\n", pressure);
        data.status = SensorStatus::INVALID_DATA;
        return data;
    }
    
    _lastPress = pressure;
    _lastTemp = _bmp.readTemperature();
    _lastReadTime = data.timestamp;
    
    data.value = pressure;
    data.status = SensorStatus::OK;
    
    DEBUG_PRINTF("[BMP280] P=%.1fhPa, T=%.2f°C\n", pressure, _lastTemp);
    
    return data;
}

float BMP280Sensor::readPressure() {
    SensorData data = read();
    return (data.status == SensorStatus::OK) ? data.value : _lastPress;
}

float BMP280Sensor::readTemperature() {
    if (!_initialized) {
        return 0.0f;
    }
    return _lastTemp;
}

bool BMP280Sensor::isAvailable() {
    return _initialized;
}

const char* BMP280Sensor::getName() {
    return "BMP280";
}

void BMP280Sensor::reset() {
    DEBUG_PRINTLN("[BMP280] Resetting...");
    _initialized = false;
    _lastPress = 0.0f;
    _lastTemp = 0.0f;
    _lastReadTime = 0;
    begin();
}

bool BMP280Sensor::validateData(float press) {
    return (press >= PRESS_MIN_VALID && press <= PRESS_MAX_VALID);
}
