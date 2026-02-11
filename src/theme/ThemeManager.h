#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "ThemeTypes.h"

class ThemeManager
{
public:
    bool begin();

    bool switchToNextTheme();
    bool reloadActiveTheme();
    void tickMockClock();

    const ThemeConfig &theme() const { return _theme; }
    uint8_t currentThemeNumber() const { return _currentThemeIndex + 1; }

private:
    ThemeConfig _theme;
    ThemeIndex _themeIndex;
    uint8_t _currentThemeIndex = 0;

    bool readJson(const char *path, DynamicJsonDocument &doc);
    bool loadThemeIndex();
    bool loadTheme(const String &path);
    void saveThemeIndex();

    void setDefaultThemeData();

    static uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b);
    static uint16_t parseColor(const String &hex, uint16_t fallback);
    static void loadTextStyle(JsonObject obj, TextStyle &style);
    static void loadModuleStyle(JsonObject obj, ModuleStyle &style);
};
