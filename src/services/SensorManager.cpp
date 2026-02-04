/**
 * @file SensorManager.cpp
 * @brief 传感器管理器实现
 * @version 1.0.0
 */

#include "SensorManager.h"
#include "config/Config.h"
#include "sensors/aht20/AHT20Sensor.h"
#include "sensors/bmp280/BMP280Sensor.h"
#include "sensors/bh1750/BH1750Sensor.h"
#include <Wire.h>

// 静态传感器对象
static AHT20Sensor aht20;
static BMP280Sensor bmp280;
static BH1750Sensor bh1750;

SensorManager& SensorManager::getInstance() {
    static SensorManager instance;
    return instance;
}

SensorManager::SensorManager()
    : _bufferIndex(0)
    , _lastSampleTime(0)
    , _lastLightSampleTime(0)
    , _dataCallback(nullptr) {
    
    // 初始化滤波缓冲区
    for (int i = 0; i < FILTER_SIZE; i++) {
        _tempBuffer[i] = 0.0f;
        _humiBuffer[i] = 0.0f;
        _pressBuffer[i] = 0.0f;
    }
}

bool SensorManager::begin() {
    DEBUG_PRINTLN("[SensorManager] Initializing...");
    
    // 初始化I2C总线
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_FREQ_HZ);
    delay(100);
    
    // 扫描I2C设备（调试用）
    #if ENABLE_SERIAL_DEBUG
    DEBUG_PRINTLN("[SensorManager] Scanning I2C bus...");
    int deviceCount = 0;
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            DEBUG_PRINTF("  Found device at 0x%02X\n", addr);
            deviceCount++;
        }
    }
    DEBUG_PRINTF("  Total devices found: %d\n", deviceCount);
    #endif
    
    // 初始化各传感器
    bool aht20OK = aht20.begin();
    delay(50);
    
    bool bmp280OK = bmp280.begin();
    delay(50);
    
    bool bh1750OK = bh1750.begin();
    delay(50);
    
    // 检查初始化结果
    if (!aht20OK) {
        DEBUG_PRINTLN("[SensorManager] WARNING: AHT20 init failed");
    }
    if (!bmp280OK) {
        DEBUG_PRINTLN("[SensorManager] WARNING: BMP280 init failed");
    }
    if (!bh1750OK) {
        DEBUG_PRINTLN("[SensorManager] WARNING: BH1750 init failed");
    }
    
    // 只要有一个传感器初始化成功就返回true
    bool result = (aht20OK || bmp280OK || bh1750OK);
    
    if (result) {
        DEBUG_PRINTLN("[SensorManager] Initialization completed");
    } else {
        DEBUG_PRINTLN("[SensorManager] Initialization FAILED - No sensors available!");
    }
    
    return result;
}

void SensorManager::update() {
    uint32_t currentTime = millis();
    
    // 定时采样环境数据
    if (currentTime - _lastSampleTime >= SENSOR_SAMPLE_INTERVAL_MS) {
        _lastSampleTime = currentTime;
        sampleEnvironment();
        
        // 触发回调
        if (_dataCallback) {
            _dataCallback(_currentData);
        }
    }
    
    // 定时采样光照数据
    if (currentTime - _lastLightSampleTime >= LIGHT_SAMPLE_INTERVAL_MS) {
        _lastLightSampleTime = currentTime;
        sampleLight();
    }
}

void SensorManager::sampleEnvironment() {
    // 读取温湿度
    if (aht20.isAvailable()) {
        float temp = aht20.readTemperature();
        float humi = aht20.readHumidity();
        
        // 移动平均滤波
        _currentData.temperature = movingAverage(_tempBuffer, temp);
        _currentData.humidity = movingAverage(_humiBuffer, humi);
    }
    
    // 读取气压
    if (bmp280.isAvailable()) {
        float press = bmp280.readPressure();
        _currentData.pressure = movingAverage(_pressBuffer, press);
    }
    
    _currentData.timestamp = millis();
    
    DEBUG_PRINTF("[SensorManager] T=%.2f°C, H=%.2f%%RH, P=%.1fhPa\n",
                 _currentData.temperature, 
                 _currentData.humidity,
                 _currentData.pressure);
}

void SensorManager::sampleLight() {
    if (bh1750.isAvailable()) {
        SensorData data = bh1750.read();
        if (data.status == SensorStatus::OK) {
            _currentData.lightLevel = data.value;
            DEBUG_PRINTF("[SensorManager] Light=%.0flux\n", _currentData.lightLevel);
        }
    }
}

float SensorManager::movingAverage(float* buffer, float newValue) {
    // 添加新值到缓冲区
    buffer[_bufferIndex] = newValue;
    _bufferIndex = (_bufferIndex + 1) % FILTER_SIZE;
    
    // 计算平均值
    float sum = 0.0f;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    
    return sum / FILTER_SIZE;
}

const EnvironmentData& SensorManager::getData() const {
    return _currentData;
}

void SensorManager::setDataCallback(DataCallback callback) {
    _dataCallback = callback;
}

float SensorManager::getTemperature() const {
    return _currentData.temperature;
}

float SensorManager::getHumidity() const {
    return _currentData.humidity;
}

float SensorManager::getPressure() const {
    return _currentData.pressure;
}

float SensorManager::getLightLevel() const {
    return _currentData.lightLevel;
}

bool SensorManager::checkStatus() {
    bool allOK = true;
    
    if (!aht20.isAvailable()) {
        DEBUG_PRINTLN("[SensorManager] AHT20 not available");
        allOK = false;
    }
    
    if (!bmp280.isAvailable()) {
        DEBUG_PRINTLN("[SensorManager] BMP280 not available");
        allOK = false;
    }
    
    if (!bh1750.isAvailable()) {
        DEBUG_PRINTLN("[SensorManager] BH1750 not available");
        allOK = false;
    }
    
    return allOK;
}
