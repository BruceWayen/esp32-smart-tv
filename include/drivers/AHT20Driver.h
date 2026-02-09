/**
 * @file AHT20Driver.h
 * @brief AHT20温湿度传感器驱动
 * @version 1.0.0
 */

#ifndef AHT20_DRIVER_H
#define AHT20_DRIVER_H

#include <Arduino.h>
#include <Adafruit_AHTX0.h>

class AHT20Driver {
public:
    AHT20Driver();

    bool begin(TwoWire& wire, uint8_t address);
    bool read(float& temperature, float& humidity);
    bool isReady() const;

private:
    Adafruit_AHTX0 _aht;
    bool _ready;
};

#endif // AHT20_DRIVER_H
