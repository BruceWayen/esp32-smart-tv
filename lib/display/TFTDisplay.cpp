/**
 * @file TFTDisplay.cpp
 * @brief TFT屏幕驱动实现 - ILI9341控制器
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "TFTDisplay.h"
#include "Font5x7.h"
#include "config/Config.h"

// ==================== ILI9341初始化序列 ====================
const uint8_t initCmdList[] PROGMEM = {
    ILI9341_SWRESET, 0,
    ILI9341_SLPOUT, 0,

    ILI9341_PIXFMT, 1, 0x55,    // RGB565
    ILI9341_FRMCTR1, 2, 0x00, 0x1B,
    ILI9341_DFUNCTR, 3, 0x0A, 0x82, 0x27,
    ILI9341_PWCTR1, 1, 0x23,
    ILI9341_PWCTR2, 1, 0x10,
    ILI9341_VMCTR1, 2, 0x3E, 0x28,
    ILI9341_VMCTR2, 1, 0x86,

    ILI9341_GMCTRP1, 15,
        0x0F, 0x31, 0x2B, 0x0C, 0x0E,
        0x08, 0x4E, 0xF1, 0x37, 0x07,
        0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9341_GMCTRN1, 15,
        0x00, 0x0E, 0x14, 0x03, 0x11,
        0x07, 0x31, 0xC1, 0x48, 0x08,
        0x0F, 0x0C, 0x31, 0x36, 0x0F,

    ILI9341_MADCTL, 1, 0x48,
    ILI9341_DISPON, 0,

    0xFF
};

// ==================== 成员变量初始化 ====================

bool TFTDisplay::begin() {
    DEBUG_PRINTLN("[TFT] 正在初始化ILI9341屏幕...");
    
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
    
    // 初始化ILI9341控制器
    initILI9341();
    
    // 打开背光
    backlight(true);
    setBacklight(BRIGHTNESS_MAX);
    
    DEBUG_PRINTLN("[TFT] ILI9341屏幕初始化完成");
    
    return true;
}

void TFTDisplay::end() {
    backlight(false);
    sleep();
}

void TFTDisplay::reset() {
    DEBUG_PRINTLN("[TFT] 正在重置屏幕...");
    
    // RST低电平复位
    digitalWrite(TFT_RST_PIN, HIGH);
    delay(10);
    digitalWrite(TFT_RST_PIN, LOW);
    delay(50);
    digitalWrite(TFT_RST_PIN, HIGH);
    delay(150);
}

void TFTDisplay::initILI9341() {
    DEBUG_PRINTLN("[TFT] 正在初始化ILI9341控制器...");
    
    const uint8_t* addr = initCmdList;
    uint8_t numArgs;
    uint8_t cmd;
    
    while ((cmd = pgm_read_byte(addr++)) != 0xFF) {
        numArgs = pgm_read_byte(addr++);
        
        // 发送命令
        writeCommand(cmd);
        delay(10);
        
        // 发送数据
        for (uint8_t i = 0; i < numArgs; i++) {
            writeDataByte(pgm_read_byte(addr++));
        }
    }
    
    // 等待初始化完成
    delay(150);
    
    // 显示打开
    writeCommand(ILI9341_DISPON);
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
    writeCommand(ILI9341_CASET);
    writeDataByte(x0 >> 8);
    writeDataByte(x0);
    writeDataByte(x1 >> 8);
    writeDataByte(x1);
    
    // 行地址设置 (RASET)
    writeCommand(ILI9341_RASET);
    writeDataByte(y0 >> 8);
    writeDataByte(y0);
    writeDataByte(y1 >> 8);
    writeDataByte(y1);
    
    // 内存写入命令
    writeCommand(ILI9341_RAMWR);
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
    writeCommand(ILI9341_PIXFMT);
    writeDataByte((uint8_t)format);
    delay(10);
}

void TFTDisplay::setDisplayMode(DisplayMode mode) {
    if (mode == DisplayMode::INVERT) {
        writeCommand(0x21);
    } else {
        writeCommand(0x20);
    }
    delay(10);
}

void TFTDisplay::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    
    uint8_t madctl = 0x00;
    
    switch (_rotation) {
        case 0:  // 正常
            madctl = 0x48;
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 1:  // 90度
            madctl = 0x28;
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
        case 2:  // 180度
            madctl = 0x88;
            _width = TFT_WIDTH;
            _height = TFT_HEIGHT;
            break;
        case 3:  // 270度
            madctl = 0xE8;
            _width = TFT_HEIGHT;
            _height = TFT_WIDTH;
            break;
    }

    writeCommand(ILI9341_MADCTL);
    writeDataByte(madctl);
    delay(10);

    DEBUG_PRINTF("[TFT] 屏幕旋转角度设置为 %d\n", _rotation);
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

    DEBUG_PRINTF("[TFT] 背光亮度设置为 %d\n", brightness);
}

void TFTDisplay::sleep() {
    writeCommand(ILI9341_SLPIN);
    delay(5);

    DEBUG_PRINTLN("[TFT] 屏幕进入休眠模式");
}

void TFTDisplay::wakeup() {
    writeCommand(ILI9341_SLPOUT);
    delay(120);

    DEBUG_PRINTLN("[TFT] 屏幕已唤醒");
}

void TFTDisplay::display(bool on) {
    if (on) {
        writeCommand(ILI9341_DISPON);
    } else {
        writeCommand(ILI9341_DISPOFF);
    }
    delay(10);

    DEBUG_PRINTF("[TFT] 屏幕显示已%s\n", on ? "开启" : "关闭");
}

void TFTDisplay::drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (c < 32 || c > 127) {
        c = '?';
    }
    uint16_t index = (c - 32) * 5;
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = pgm_read_byte(&Font5x7[index + col]);
        for (uint8_t row = 0; row < 8; row++) {
            uint16_t drawX = x + col * size;
            uint16_t drawY = y + row * size;
            uint16_t fillColor = (line & 0x01) ? color : bg;
            if (size == 1) {
                drawPixel(drawX, drawY, fillColor);
            } else {
                fillRect(drawX, drawY, size, size, fillColor);
            }
            line >>= 1;
        }
    }
    if (size == 1) {
        fillRect(x + 5, y, 1, 8, bg);
    } else {
        fillRect(x + 5 * size, y, size, 8 * size, bg);
    }
}

void TFTDisplay::drawText(uint16_t x, uint16_t y, const String& text, uint16_t color, uint16_t bg, uint8_t size) {
    uint16_t cursorX = x;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text.charAt(i);
        drawChar(cursorX, y, c, color, bg, size);
        cursorX += (6 * size);
    }
}

uint16_t TFTDisplay::measureTextWidth(const String& text, uint8_t size) const {
    return text.length() * 6 * size;
}
