/**
 * @file MockWeatherProvider.cpp
 * @brief 模拟天气数据提供者实现
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "MockWeatherProvider.h"

struct WeatherState {
    const char* condition;
    const char* iconKey;
    float temp;
    float humidity;
    float windSpeed;
    const char* windDir;
};

static const WeatherState kWeatherStates[] = {
    {"晴朗", "sunny", 26.5f, 45.0f, 2.2f, "东北风"},
    {"多云", "cloudy", 24.8f, 55.0f, 1.8f, "东风"},
    {"小雨", "rain", 22.3f, 70.0f, 3.5f, "南风"},
    {"小雪", "snow", 0.8f, 80.0f, 2.0f, "北风"}
};

MockWeatherProvider::MockWeatherProvider()
    : _data()
    , _lastUpdate(0)
    , _stateIndex(0) {}

bool MockWeatherProvider::begin() {
    DEBUG_PRINTLN("[MockWeather] 使用模拟天气数据");
    _lastUpdate = 0;
    _stateIndex = 0;
    return true;
}

bool MockWeatherProvider::update() {
    uint32_t now = millis();
    if (now - _lastUpdate < 8000) {
        return false;
    }
    _lastUpdate = now;

    const WeatherState& state = kWeatherStates[_stateIndex];
    _data.condition = state.condition;
    _data.iconKey = state.iconKey;
    _data.temperature = state.temp;
    _data.humidity = state.humidity;
    _data.windSpeed = state.windSpeed;
    _data.windDir = state.windDir;
    _data.timestamp = now;

    _stateIndex = (_stateIndex + 1) % (sizeof(kWeatherStates) / sizeof(kWeatherStates[0]));

    DEBUG_PRINTF("[MockWeather] %s %.1f℃ 风速%.1fm/s\n",
                 _data.condition.c_str(), _data.temperature, _data.windSpeed);
    return true;
}

const WeatherInfo& MockWeatherProvider::getData() const {
    return _data;
}
