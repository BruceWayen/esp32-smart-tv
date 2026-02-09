/**
 * @file BMP280Driver.h
 * @brief BMP280气压传感器驱动
 * @version 1.0.0
 */

#ifndef BMP280_DRIVER_H
#define BMP280_DRIVER_H

#include <Arduino.h>
#include <Adafruit_BMP280.h>

class BMP280Driver {
public:
    BMP280Driver();

    bool begin(TwoWire& wire, uint8_t address);
    bool readPressure(float& pressureHpa);
    bool isReady() const;

private:
    Adafruit_BMP280 _bmp;
    bool _ready;
};

#endif // BMP280_DRIVER_H
