/**
 * @file AHT20Sensor.cpp
 * @brief AHT20温湿度传感器驱动实现
 * @version 1.0.0
 */

#include "AHT20Sensor.h"
#include "config/Config.h"

AHT20Sensor::AHT20Sensor() 
    : _initialized(false)
    , _lastReadTime(0)
    , _lastTemp(0.0f)
    , _lastHumi(0.0f) {
}

bool AHT20Sensor::begin() {
    DEBUG_PRINTLN("[AHT20] Initializing...");
    
    if (_aht.begin()) {
        _initialized = true;
        DEBUG_PRINTLN("[AHT20] Initialization successful");
        return true;
    }
    
    DEBUG_PRINTLN("[AHT20] Initialization failed!");
    _initialized = false;
    return false;
}

SensorData AHT20Sensor::read() {
    SensorData data;
    data.timestamp = millis();
    
    if (!_initialized) {
        data.status = SensorStatus::NOT_INITIALIZED;
        return data;
    }
    
    // 读取传感器数据
    sensors_event_t humidity, temp;
    if (!_aht.getEvent(&humidity, &temp)) {
        DEBUG_PRINTLN("[AHT20] Read failed");
        data.status = SensorStatus::READ_ERROR;
        return data;
    }
    
    float temperature = temp.temperature;
    float humid = humidity.relative_humidity;
    
    // 验证数据有效性
    if (!validateData(temperature, humid)) {
        DEBUG_PRINTF("[AHT20] Invalid data: T=%.2f, H=%.2f\n", temperature, humid);
        data.status = SensorStatus::INVALID_DATA;
        return data;
    }
    
    // 保存有效数据
    _lastTemp = temperature;
    _lastHumi = humid;
    _lastReadTime = data.timestamp;
    
    // 默认返回温度值，湿度通过readHumidity()单独获取
    data.value = temperature;
    data.status = SensorStatus::OK;
    
    DEBUG_PRINTF("[AHT20] T=%.2f°C, H=%.2f%%RH\n", temperature, humid);
    
    return data;
}

float AHT20Sensor::readTemperature() {
    SensorData data = read();
    return (data.status == SensorStatus::OK) ? data.value : _lastTemp;
}

float AHT20Sensor::readHumidity() {
    if (!_initialized) {
        return 0.0f;
    }
    return _lastHumi;
}

bool AHT20Sensor::isAvailable() {
    return _initialized;
}

const char* AHT20Sensor::getName() {
    return "AHT20";
}

void AHT20Sensor::reset() {
    DEBUG_PRINTLN("[AHT20] Resetting...");
    _initialized = false;
    _lastTemp = 0.0f;
    _lastHumi = 0.0f;
    _lastReadTime = 0;
    
    // 重新初始化
    begin();
}

bool AHT20Sensor::validateData(float temp, float humi) {
    // 检查温度范围
    if (temp < TEMP_MIN_VALID || temp > TEMP_MAX_VALID) {
        return false;
    }
    
    // 检查湿度范围
    if (humi < HUMI_MIN_VALID || humi > HUMI_MAX_VALID) {
        return false;
    }
    
    return true;
}
