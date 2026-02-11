#pragma once

#include <Arduino.h>

struct TextStyle
{
    int16_t x = 0;
    int16_t y = 0;
    uint8_t size = 1;
    uint16_t color = 0xFFFF;
    String value;
};

struct ModuleStyle
{
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 60;
    int16_t h = 40;
    uint8_t opacity = 255;
    uint16_t color = 0x2104;
};

struct ThemeConfig
{
    uint16_t backgroundColor = 0x0842;
    String backgroundImage;

    TextStyle timeText;
    TextStyle dateText;
    TextStyle tempText;
    TextStyle humidText;
    TextStyle pressureText;
    TextStyle alarmText;

    ModuleStyle timeModule;
    ModuleStyle envModule;
    ModuleStyle alarmModule;

    String weatherIcon = "/icons/weather/default.bin";
    String wifiIcon;
    String batteryIcon;
};

struct ThemeIndex
{
    String activeTheme = "/themes/theme1.json";
    String themes[8];
    uint8_t themeCount = 0;
};
