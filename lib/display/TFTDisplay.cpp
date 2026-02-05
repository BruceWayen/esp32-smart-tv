/**
 * @file TFTDisplay.cpp
 * @brief TFT屏幕驱动实现 - ST7789控制器
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "TFTDisplay.h"
#include "config/Config.h"

// ==================== 成员变量初始化 ====================

bool TFTDisplay::begin() {
    DEBUG_PRINTLN("[TFT] Initializing TFT display...");
    
    // 初始化SPI总线
    SPI.begin(TFT_SCLK_PIN, TFT_MISO_PIN, TFT_MOSI_PIN, TFT_CS_PIN);
    SPI.setFrequency(TFT_SPI_FREQ_HZ);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    
    // 设置GPIO引脚为输出模式
    pinMode(TFT_CS_PIN, OUTPUT);
    pinMode(TFT_DC_PIN, OUTPUT);
    pinMode(TFT_RST_PIN, OUTPUT);
    pinMode(TFT_BL_PIN, OUTPUT);
    
    // 初始化背光PWM
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ_HZ, BL_PWM_RESOLUTION);
    ledcAttachPin(TFT_BL_PIN, BL_PWM_CHANNEL);
    ledcWrite(BL_PWM_CHANNEL, 0);  // 初始关闭背光
    
    // 设置屏幕尺寸
    _width = TFT_WIDTH;
    _height = TFT_HEIGHT;
    _rotation = TFT_ROTATION;
    
    // 重置屏幕
    reset();
    delay(150);
    
    // 初始化ST7789控制器
    initST7789();
    
    // 打开背光
    backlight(true);
    setBacklight(BRIGHTNESS_MAX);
    
    DEBUG_PRINTLN("[TFT] TFT display initialized successfully");
    
    return true;
}

void TFTDisplay::end() {
    backlight(false);
    sleep();
}

void TFTDisplay::reset() {
    DEBUG_PRINTLN("[TFT] Resetting display...");
    
    // RST低电平复位
    digitalWrite(TFT_RST_PIN, HIGH);
    delay(10);
    digitalWrite(TFT_RST_PIN, LOW);
    delay(50);
    digitalWrite(TFT_RST_PIN, HIGH);
    delay(150);
}

void TFTDisplay::initST7789() {
    DEBUG_PRINTLN("[TFT] Initializing ST7789 controller...");

    // 像素格式 RGB565
    writeCommand(ST7789_COLMOD);
    writeDataByte(0x05);

    // VCOM设置
    writeCommand(0xC5);
    writeDataByte(0x1A);

    // 内存数据访问控制（默认方向，旋转后会再设置）
    writeCommand(ST7789_MADCTL);
    writeDataByte(0x00);

    // Porch Setting
    writeCommand(0xB2);
    writeDataByte(0x05);
    writeDataByte(0x05);
    writeDataByte(0x00);
    writeDataByte(0x33);
    writeDataByte(0x33);

    // Gate Control
    writeCommand(0xB7);
    writeDataByte(0x05);

    // VCOM
    writeCommand(ST7789_VCOMS);
    writeDataByte(0x3F);

    // Power control
    writeCommand(0xC0);
    writeDataByte(0x2C);

    // VDV and VRH Command Enable
    writeCommand(0xC2);
    writeDataByte(0x01);

    // VRH Set
    writeCommand(0xC3);
    writeDataByte(0x0F);

    // VDV Set
    writeCommand(0xC4);
    writeDataByte(0x20);

    // Frame Rate Control in Normal Mode
    writeCommand(ST7789_FRCTRL2);
    writeDataByte(0x01);

    // Power Control 1
    writeCommand(ST7789_POWE1);
    writeDataByte(0xA4);
    writeDataByte(0xA1);

    // Power Control 1
    writeCommand(0xE8);
    writeDataByte(0x03);

    // Equalize time control
    writeCommand(0xE9);
    writeDataByte(0x09);
    writeDataByte(0x09);
    writeDataByte(0x08);

    // Gamma positive
    writeCommand(ST7789_GMCTRP1);
    writeDataByte(0xD0);
    writeDataByte(0x05);
    writeDataByte(0x09);
    writeDataByte(0x09);
    writeDataByte(0x08);
    writeDataByte(0x14);
    writeDataByte(0x28);
    writeDataByte(0x33);
    writeDataByte(0x3F);
    writeDataByte(0x07);
    writeDataByte(0x13);
    writeDataByte(0x14);
    writeDataByte(0x28);
    writeDataByte(0x30);

    // Gamma negative
    writeCommand(ST7789_GMCTRN1);
    writeDataByte(0xD0);
    writeDataByte(0x05);
    writeDataByte(0x09);
    writeDataByte(0x09);
    writeDataByte(0x08);
    writeDataByte(0x03);
    writeDataByte(0x24);
    writeDataByte(0x32);
    writeDataByte(0x32);
    writeDataByte(0x3B);
    writeDataByte(0x14);
    writeDataByte(0x13);
    writeDataByte(0x28);
    writeDataByte(0x2F);

    // 反色关闭
    writeCommand(ST7789_INVOFF);

    // 退出睡眠模式
    writeCommand(ST7789_SLPOUT);
    delay(120);

    // 显示打开
    writeCommand(ST7789_DISPON);
    delay(10);
}

void TFTDisplay::writeCommand(uint8_t cmd) {
    digitalWrite(TFT_DC_PIN, LOW);   // 命令模式
    digitalWrite(TFT_CS_PIN, LOW);   // 片选低
    
    SPI.write(cmd);
    
    digitalWrite(TFT_CS_PIN, HIGH);  // 片选高
}

void TFTDisplay::writeDataByte(uint8_t data) {
    digitalWrite(TFT_DC_PIN, HIGH);  // 数据模式
    digitalWrite(TFT_CS_PIN, LOW);   // 片选低
    
    SPI.write(data);
    
    digitalWrite(TFT_CS_PIN, HIGH);  // 片选高
}

void TFTDisplay::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // 列地址设置 (CASET)
    writeCommand(ST7789_CASET);
    writeDataByte(x0 >> 8);
    writeDataByte(x0);
    writeDataByte(x1 >> 8);
    writeDataByte(x1);
    
    // 行地址设置 (RASET)
    writeCommand(ST7789_RASET);
    writeDataByte(y0 >> 8);
    writeDataByte(y0);
    writeDataByte(y1 >> 8);
    writeDataByte(y1);
    
    // 内存写入命令
    writeCommand(ST7789_RAMWR);
}

void TFTDisplay::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    setAddrWindow(x0, y0, x1, y1);
}

void TFTDisplay::writeData(const uint8_t* data, uint32_t len) {
    digitalWrite(TFT_DC_PIN, HIGH);  // 数据模式
    digitalWrite(TFT_CS_PIN, LOW);   // 片选低
    
    SPI.writeBytes((uint8_t*)data, len);
    
    digitalWrite(TFT_CS_PIN, HIGH);  // 片选高
}

void TFTDisplay::writeColor(uint16_t color, uint32_t count) {
    uint8_t colorHigh = color >> 8;
    uint8_t colorLow = color & 0xFF;
    
    digitalWrite(TFT_DC_PIN, HIGH);  // 数据模式
    digitalWrite(TFT_CS_PIN, LOW);   // 片选低
    
    for (uint32_t i = 0; i < count; i++) {
        SPI.write(colorHigh);
        SPI.write(colorLow);
    }
    
    digitalWrite(TFT_CS_PIN, HIGH);  // 片选高
}

void TFTDisplay::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if (x + width > _width) width = _width - x;
    if (y + height > _height) height = _height - y;
    
    setAddrWindow(x, y, x + width - 1, y + height - 1);
    
    uint32_t pixelCount = (uint32_t)width * height;
    writeColor(color, pixelCount);
}

void TFTDisplay::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

void TFTDisplay::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= _width || y >= _height) return;
    
    setAddrWindow(x, y, x, y);
    writeColor(color, 1);
}

void TFTDisplay::drawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
    if (y >= _height) return;
    if (x + length > _width) length = _width - x;
    
    setAddrWindow(x, y, x + length - 1, y);
    writeColor(color, length);
}

void TFTDisplay::drawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
    if (x >= _width) return;
    if (y + length > _height) length = _height - y;
    
    setAddrWindow(x, y, x, y + length - 1);
    writeColor(color, length);
}

void TFTDisplay::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    // 上边
    drawHLine(x, y, width, color);
    // 下边
    drawHLine(x, y + height - 1, width, color);
    // 左边
    drawVLine(x, y, height, color);
    // 右边
    drawVLine(x + width - 1, y, height, color);
}

void TFTDisplay::setPixelFormat(PixelFormat format) {
    writeCommand(ST7789_COLMOD);
    writeDataByte((uint8_t)format);
    delay(10);
}

void TFTDisplay::setDisplayMode(DisplayMode mode) {
    if (mode == DisplayMode::INVERT) {
        writeCommand(ST7789_INVON);
    } else {
        writeCommand(ST7789_INVOFF);
    }
    delay(10);
}

void TFTDisplay::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    
    uint8_t madctl = 0x00;
    
    switch (_rotation) {
        case 0:  // 正常
            madctl = 0x00;
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 1:  // 90度
            madctl = 0xA0;
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
        case 2:  // 180度
            madctl = 0xC0;
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 3:  // 270度
            madctl = 0x60;
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
    }
    
    writeCommand(ST7789_MADCTL);
    writeDataByte(madctl);
    delay(10);
    
    DEBUG_PRINTF("[TFT] Rotation set to %d\n", _rotation);
}

void TFTDisplay::backlight(bool on) {
    if (on) {
        ledcWrite(BL_PWM_CHANNEL, 255);
    } else {
        ledcWrite(BL_PWM_CHANNEL, 0);
    }
}

void TFTDisplay::setBacklight(uint8_t brightness) {
    // brightness: 0-255, 映射到PWM占空比
    ledcWrite(BL_PWM_CHANNEL, brightness);
    
    DEBUG_PRINTF("[TFT] Backlight brightness set to %d\n", brightness);
}

void TFTDisplay::sleep() {
    writeCommand(ST7789_SLPIN);
    delay(5);
    
    DEBUG_PRINTLN("[TFT] Display entered sleep mode");
}

void TFTDisplay::wakeup() {
    writeCommand(ST7789_SLPOUT);
    delay(120);
    
    DEBUG_PRINTLN("[TFT] Display woke up");
}

void TFTDisplay::display(bool on) {
    if (on) {
        writeCommand(ST7789_DISPON);
    } else {
        writeCommand(ST7789_DISPOFF);
    }
    delay(10);
    
    DEBUG_PRINTF("[TFT] Display turned %s\n", on ? "ON" : "OFF");
}
