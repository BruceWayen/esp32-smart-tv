/**
 * @file DesktopDataService.h
 * @brief 桌面数据服务 - 聚合环境/天气/时间数据
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef DESKTOP_DATA_SERVICE_H
#define DESKTOP_DATA_SERVICE_H

#include <Arduino.h>
#include "interfaces/IEnvironmentProvider.h"
#include "interfaces/IWeatherProvider.h"
#include "models/ClockInfo.h"

typedef void (*EnvironmentCallback)(const EnvironmentData& data);
typedef void (*WeatherCallback)(const WeatherInfo& data);
typedef void (*ClockCallback)(const ClockInfo& data);

class DesktopDataService {
public:
    static DesktopDataService& getInstance();

    bool begin();
    void update();

    void setEnvironmentCallback(EnvironmentCallback callback);
    void setWeatherCallback(WeatherCallback callback);
    void setClockCallback(ClockCallback callback);

private:
    DesktopDataService();
    ~DesktopDataService() = default;
    DesktopDataService(const DesktopDataService&) = delete;
    DesktopDataService& operator=(const DesktopDataService&) = delete;

   // IEnvironmentProvider* _envProvider;
    IWeatherProvider* _weatherProvider;
    EnvironmentCallback _envCallback;
    WeatherCallback _weatherCallback;
    ClockCallback _clockCallback;

    ClockInfo _clock;
    uint32_t _lastClockTick;
    uint32_t _bootTimestamp;

    void updateClock();
};

#endif // DESKTOP_DATA_SERVICE_H
