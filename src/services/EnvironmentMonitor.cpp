/**
 * @file EnvironmentMonitor.cpp
 * @brief 环境温湿压监测服务实现
 * @version 1.0.0
 */

#include "services/EnvironmentMonitor.h"
#include "config/Config.h"

EnvironmentMonitor* EnvironmentMonitor::_activeInstance = nullptr;

EnvironmentMonitor::EnvironmentMonitor()
    : _timer(nullptr)
    , _sampleDue(false)
    , _timerMux(portMUX_INITIALIZER_UNLOCKED)
    , _ready(false) {
}

bool EnvironmentMonitor::begin() {
    DEBUG_PRINTLN("[Env] 初始化环境监测...");

    _i2c.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);

    bool ahtOk = _aht20.begin(_i2c.wire(), AHT20_I2C_ADDR);
    bool bmpOk = _bmp280.begin(_i2c.wire(), BMP280_I2C_ADDR);

    _ready = ahtOk && bmpOk;
    if (!_ready) {
        DEBUG_PRINTLN("[Env] 传感器初始化失败");
        return false;
    }

    _processor.reset();
    _data = EnvironmentData();

    _activeInstance = this;
    _timer = timerBegin(0, 80, true); // 80MHz / 80 = 1MHz (1us)
    timerAttachInterrupt(_timer, &EnvironmentMonitor::onTimer, true);
    timerAlarmWrite(_timer, SENSOR_SAMPLE_INTERVAL_MS * 1000ULL, true);
    timerAlarmEnable(_timer);

    _sampleDue = true;

    DEBUG_PRINTLN("[Env] 环境监测初始化完成");
    return true;
}

bool EnvironmentMonitor::update() {
    if (!_ready) {
        return false;
    }

    bool shouldSample = false;
    portENTER_CRITICAL(&_timerMux);
    if (_sampleDue) {
        _sampleDue = false;
        shouldSample = true;
    }
    portEXIT_CRITICAL(&_timerMux);

    if (!shouldSample) {
        return false;
    }

    float temperature = 0.0f;
    float humidity = 0.0f;
    float pressure = 0.0f;

    if (!_aht20.read(temperature, humidity)) {
        DEBUG_PRINTLN("[Env] AHT20读取失败");
        return false;
    }

    if (!_bmp280.readPressure(pressure)) {
        DEBUG_PRINTLN("[Env] BMP280读取失败");
        return false;
    }

    if (!_processor.processSample(temperature, humidity, pressure, _data)) {
        DEBUG_PRINTLN("[Env] 异常温度数据被丢弃");
        return false;
    }

    return true;
}

const EnvironmentData& EnvironmentMonitor::getData() const {
    return _data;
}

void IRAM_ATTR EnvironmentMonitor::onTimer() {
    if (_activeInstance == nullptr) {
        return;
    }
    portENTER_CRITICAL_ISR(&_activeInstance->_timerMux);
    _activeInstance->_sampleDue = true;
    portEXIT_CRITICAL_ISR(&_activeInstance->_timerMux);
}
