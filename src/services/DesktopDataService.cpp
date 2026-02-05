/**
 * @file DesktopDataService.cpp
 * @brief 桌面数据服务实现
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "DesktopDataService.h"
#include "MockEnvironmentProvider.h"
#include "MockWeatherProvider.h"
#include "config/Config.h"

static MockEnvironmentProvider mockEnvProvider;
static MockWeatherProvider mockWeatherProvider;

DesktopDataService& DesktopDataService::getInstance() {
    static DesktopDataService instance;
    return instance;
}

DesktopDataService::DesktopDataService()
    : _envProvider(&mockEnvProvider)
    , _weatherProvider(&mockWeatherProvider)
    , _envCallback(nullptr)
    , _weatherCallback(nullptr)
    , _clockCallback(nullptr)
    , _clock()
    , _lastClockTick(0)
    , _bootTimestamp(0) {}

bool DesktopDataService::begin() {
    _bootTimestamp = millis();
    _lastClockTick = 0;

    bool envOk = _envProvider->begin();
    bool weatherOk = _weatherProvider->begin();

    if (!envOk) {
        DEBUG_PRINTLN("[DesktopData] 警告：环境数据源初始化失败");
    }
    if (!weatherOk) {
        DEBUG_PRINTLN("[DesktopData] 警告：天气数据源初始化失败");
    }

    return envOk || weatherOk;
}

void DesktopDataService::update() {
    if (_envProvider && _envProvider->update()) {
        if (_envCallback) {
            _envCallback(_envProvider->getData());
        }
    }

    if (_weatherProvider && _weatherProvider->update()) {
        if (_weatherCallback) {
            _weatherCallback(_weatherProvider->getData());
        }
    }

    updateClock();
}

void DesktopDataService::setEnvironmentCallback(EnvironmentCallback callback) {
    _envCallback = callback;
}

void DesktopDataService::setWeatherCallback(WeatherCallback callback) {
    _weatherCallback = callback;
}

void DesktopDataService::setClockCallback(ClockCallback callback) {
    _clockCallback = callback;
}

void DesktopDataService::updateClock() {
    uint32_t now = millis();
    if (now - _lastClockTick < 1000) {
        return;
    }
    _lastClockTick = now;

    uint32_t seconds = (now - _bootTimestamp) / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    _clock.second = seconds % 60;
    _clock.minute = minutes % 60;
    _clock.hour = hours % 24;

    char dateBuffer[16];
    snprintf(dateBuffer, sizeof(dateBuffer), "2026-02-%02d", (hours % 28) + 1);
    _clock.dateText = dateBuffer;

    if (_clockCallback) {
        _clockCallback(_clock);
    }
}
