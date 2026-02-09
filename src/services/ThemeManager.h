/**
 * @file ThemeManager.h
 * @brief 主题配置管理器 - 读取/保存主题配置
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <config/Config.h>

/**
 * @brief 主题矩形布局
 */
struct ThemeRect {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;

    ThemeRect()
        : x(0)
        , y(0)
        , w(0)
        , h(0) {}

    ThemeRect(uint16_t xVal, uint16_t yVal, uint16_t wVal, uint16_t hVal)
        : x(xVal)
        , y(yVal)
        , w(wVal)
        , h(hVal) {}
};

/**
 * @brief 主题布局定义
 */
struct ThemeLayout {
    ThemeRect header;
    ThemeRect clock;
    ThemeRect weather;
    ThemeRect sensors;
    ThemeRect footer;

    ThemeLayout() = default;
};

struct ThemeConfig {
    uint8_t id;
    String name;
    uint16_t backgroundColor;
    uint16_t primaryColor;
    uint16_t accentColor;
    uint16_t secondaryColor;
    bool showSensors;
    uint8_t fontSize;
    String wallpaperPath;
    String iconPath;
    ThemeLayout layout;

    ThemeConfig()
        : id(1)
        , name("Default")
        , backgroundColor(0x0000)
        , primaryColor(0x001F)
        , accentColor(0x07E0)
        , secondaryColor(0x7BEF)
        , showSensors(true)
        , fontSize(2)
        , wallpaperPath("/themes/theme_1.webp")
        , iconPath("/icons/weather/{code}.bmp")
        , layout() {}
};

typedef void (*ThemeChangeCallback)(const ThemeConfig& theme);

class ThemeManager {
public:
    static ThemeManager& getInstance();

    bool begin();
    const ThemeConfig& getTheme() const;
    size_t getThemeCount() const;
    const ThemeConfig& getThemeByIndex(size_t index) const;
    String getThemeJson() const;
    bool updateThemeFromJson(const String& json, String& error);
    bool resetToDefaultTheme();
    bool reloadThemeFromFile();
    bool nextTheme();
    bool previousTheme();
    bool setThemeById(uint8_t id);
    void setThemeChangeCallback(ThemeChangeCallback callback);

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    ThemeConfig _currentTheme;
    ThemeConfig _themeList[MAX_THEMES];
    size_t _themeCount;
    bool _fsReady;
    ThemeChangeCallback _callback;

    bool loadTheme();
    bool saveTheme();
    bool loadThemeList();
    bool saveThemeList();
    ThemeConfig buildDefaultTheme() const;
    void buildDefaultThemeList();
    uint16_t parseColor(const String& value, uint16_t fallback) const;
    uint16_t parseColor(uint32_t value, uint16_t fallback) const;
    String colorToHex(uint16_t color) const;
    ThemeRect parseRect(const JsonVariant& value, const ThemeRect& fallback) const;
};

#endif // THEME_MANAGER_H
