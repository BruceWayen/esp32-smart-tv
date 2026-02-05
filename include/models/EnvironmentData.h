/**
 * @file EnvironmentData.h
 * @brief 环境数据模型定义
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef ENVIRONMENT_DATA_H
#define ENVIRONMENT_DATA_H

#include <Arduino.h>

/**
 * @brief 环境数据结构体
 */
struct EnvironmentData {
    float temperature;      // 温度(℃)
    float humidity;         // 湿度(%RH)
    float pressure;         // 气压(hPa)
    float lightLevel;       // 光照(lx)
    uint32_t timestamp;     // 时间戳(ms)

    EnvironmentData()
        : temperature(0.0f)
        , humidity(0.0f)
        , pressure(0.0f)
        , lightLevel(0.0f)
        , timestamp(0) {}
};

#endif // ENVIRONMENT_DATA_H
