#pragma once

#include <Arduino.h>
#include "display/TftDriver.h"
#include "theme/ThemeTypes.h"

class DashboardRenderer
{
public:
    explicit DashboardRenderer(TftDriver &display) : _display(display) {}

    void render(const ThemeConfig &theme, uint8_t themeNumber);

private:
    TftDriver &_display;

    static uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b);
    static uint16_t blend565(uint16_t fg, uint16_t bg, uint8_t alpha);

    void renderModule(const ModuleStyle &style, uint16_t backgroundColor);
    void drawWeatherIconSlot(const String &iconPath, const ThemeConfig &theme);
};
