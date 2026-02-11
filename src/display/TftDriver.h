#pragma once

#include <Arduino.h>
#include <SPI.h>

class TftDriver
{
public:
    static constexpr int16_t WIDTH = 240;
    static constexpr int16_t HEIGHT = 320;

    TftDriver(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint8_t mosiPin, uint8_t sclkPin);

    void begin();

    void fillScreen(uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void drawText(int16_t x, int16_t y, const String &text, uint16_t color, uint8_t size);

private:
    uint8_t _cs;
    uint8_t _dc;
    uint8_t _rst;
    uint8_t _mosi;
    uint8_t _sclk;
    SPIClass _spi;

    void tftInit();
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void drawChar5x7(int16_t x, int16_t y, char c, uint16_t color, uint8_t size);
};
