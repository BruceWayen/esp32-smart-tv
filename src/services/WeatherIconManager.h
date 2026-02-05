/**
 * @file WeatherIconManager.h
 * @brief 天气图标配置管理器
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef WEATHER_ICON_MANAGER_H
#define WEATHER_ICON_MANAGER_H

#include <Arduino.h>
#include "config/Config.h"

/**
 * @brief 天气图标样式定义
 */
struct WeatherIconStyle {
    String key;
    String type;
    uint16_t color;

    WeatherIconStyle()
        : key("unknown")
        , type("unknown")
        , color(0xFFFF) {}
};

class WeatherIconManager {
public:
    static WeatherIconManager& getInstance();

    /**
     * @brief 初始化配置
     */
    bool begin();

    /**
     * @brief 根据图标关键字获取样式
     * @param key 关键字
     */
    WeatherIconStyle getStyle(const String& key) const;

private:
    WeatherIconManager();
    ~WeatherIconManager() = default;
    WeatherIconManager(const WeatherIconManager&) = delete;
    WeatherIconManager& operator=(const WeatherIconManager&) = delete;

    WeatherIconStyle _icons[16];
    size_t _iconCount;
    bool _fsReady;

    void loadDefaults();
    bool loadFromFile();
    uint16_t parseColor(const String& value, uint16_t fallback) const;
};

#endif // WEATHER_ICON_MANAGER_H
