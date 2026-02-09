/**
 * @file EnvironmentMonitor.h
 * @brief 环境温湿压监测服务（业务逻辑层）
 * @version 1.0.0
 */

#ifndef ENVIRONMENT_MONITOR_H
#define ENVIRONMENT_MONITOR_H

#include <Arduino.h>
#include "interfaces/IEnvironmentProvider.h"
#include "drivers/I2CBus.h"
#include "drivers/AHT20Driver.h"
#include "drivers/BMP280Driver.h"
#include "services/EnvironmentDataProcessor.h"

class EnvironmentMonitor : public IEnvironmentProvider {
public:
    EnvironmentMonitor();

    bool begin() override;
    bool update() override;
    const EnvironmentData& getData() const override;

private:
    static void IRAM_ATTR onTimer();

    I2CBus _i2c;
    AHT20Driver _aht20;
    BMP280Driver _bmp280;
    EnvironmentDataProcessor _processor;
    EnvironmentData _data;

    hw_timer_t* _timer;
    volatile bool _sampleDue;
    portMUX_TYPE _timerMux;
    bool _ready;

    static EnvironmentMonitor* _activeInstance;
};

#endif // ENVIRONMENT_MONITOR_H
