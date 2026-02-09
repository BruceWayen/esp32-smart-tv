/**
 * @file I2CBus.h
 * @brief I2C通信驱动封装
 * @version 1.0.0
 */

#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <Arduino.h>
#include <Wire.h>

class I2CBus {
public:
    I2CBus();

    bool begin(uint8_t sdaPin, uint8_t sclPin, uint32_t frequency);
    TwoWire& wire();
    bool isReady() const;

private:
    TwoWire* _wire;
    bool _ready;
};

#endif // I2C_BUS_H
