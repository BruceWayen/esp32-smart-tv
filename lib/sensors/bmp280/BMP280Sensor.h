/**
 * @file BMP280Sensor.h
 * @brief BMP280气压传感器驱动
 * @version 1.0.0
 */

#ifndef BMP280_SENSOR_H
#define BMP280_SENSOR_H

#include "interfaces/ISensor.h"
#include <Adafruit_BMP280.h>

class BMP280Sensor : public ISensor {
public:
    BMP280Sensor();
    ~BMP280Sensor() override = default;
    
    bool begin() override;
    SensorData read() override;
    float readPressure();  // 返回气压(hPa)
    float readTemperature();  // 返回温度(℃)
    bool isAvailable() override;
    const char* getName() override;
    void reset() override;

private:
    Adafruit_BMP280 _bmp;
    bool _initialized;
    uint32_t _lastReadTime;
    float _lastPress;
    float _lastTemp;
    
    bool validateData(float press);
};

#endif
