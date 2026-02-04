/**
 * @file BH1750Sensor.h
 * @brief BH1750光照传感器驱动
 * @version 1.0.0
 */

#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include "interfaces/ISensor.h"
#include <BH1750.h>

class BH1750Sensor : public ISensor {
public:
    BH1750Sensor();
    ~BH1750Sensor() override = default;
    
    bool begin() override;
    SensorData read() override;
    float readLightLevel();  // 返回光照强度(lux)
    bool isAvailable() override;
    const char* getName() override;
    void reset() override;

private:
    BH1750 _bh1750;
    bool _initialized;
    uint32_t _lastReadTime;
    float _lastLight;
};

#endif
