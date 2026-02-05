/**
 * @file WeatherInfo.h
 * @brief 天气数据模型定义
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef WEATHER_INFO_H
#define WEATHER_INFO_H

#include <Arduino.h>

/**
 * @brief 天气数据结构体
 */
struct WeatherInfo {
    String condition;   // 天气描述
    String iconKey;     // 图标关键字（用于配置映射）
    float temperature;  // 天气温度(℃)
    float humidity;     // 湿度(%RH)
    float windSpeed;    // 风速(m/s)
    String windDir;     // 风向描述
    uint32_t timestamp; // 数据时间戳(ms)

    WeatherInfo()
        : condition("未知")
        , iconKey("unknown")
        , temperature(0.0f)
        , humidity(0.0f)
        , windSpeed(0.0f)
        , windDir("无风")
        , timestamp(0) {}
};

#endif // WEATHER_INFO_H
