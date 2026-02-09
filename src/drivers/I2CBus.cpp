/**
 * @file I2CBus.cpp
 * @brief I2C通信驱动封装实现
 * @version 1.0.0
 */

#include "drivers/I2CBus.h"
#include "config/Config.h"

I2CBus::I2CBus()
    : _wire(&Wire)
    , _ready(false) {
}

bool I2CBus::begin(uint8_t sdaPin, uint8_t sclPin, uint32_t frequency) {
    _wire->begin(sdaPin, sclPin, frequency);
    _ready = true;
    DEBUG_PRINTF("[I2C] 总线初始化: SDA=%d SCL=%d FREQ=%lu\n", sdaPin, sclPin, frequency);
    return _ready;
}

TwoWire& I2CBus::wire() {
    return *_wire;
}

bool I2CBus::isReady() const {
    return _ready;
}
