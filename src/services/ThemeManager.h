/**
 * @file ThemeManager.h
 * @brief 主题配置管理器 - 读取/保存主题配置
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <Arduino.h>

struct ThemeConfig {
    uint8_t id;
    String name;
    uint16_t backgroundColor;
    uint16_t primaryColor;
    uint16_t accentColor;
    uint16_t secondaryColor;
    bool showSensors;
    String wallpaperPath;

    ThemeConfig()
        : id(1)
        , name("Default")
        , backgroundColor(0x0000)
        , primaryColor(0x001F)
        , accentColor(0x07E0)
        , secondaryColor(0x7BEF)
        , showSensors(true)
        , wallpaperPath("/themes/theme_1.webp") {}
};

typedef void (*ThemeChangeCallback)(const ThemeConfig& theme);

class ThemeManager {
public:
    static ThemeManager& getInstance();

    bool begin();
    const ThemeConfig& getTheme() const;
    String getThemeJson() const;
    bool updateThemeFromJson(const String& json, String& error);
    void setThemeChangeCallback(ThemeChangeCallback callback);

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    ThemeConfig _currentTheme;
    bool _fsReady;
    ThemeChangeCallback _callback;

    bool loadTheme();
    bool saveTheme();
    ThemeConfig buildDefaultTheme() const;
    uint16_t parseColor(const String& value, uint16_t fallback) const;
    uint16_t parseColor(uint32_t value, uint16_t fallback) const;
    String colorToHex(uint16_t color) const;
};

#endif // THEME_MANAGER_H
