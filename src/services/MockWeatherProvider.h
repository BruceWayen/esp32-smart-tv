/**
 * @file MockWeatherProvider.h
 * @brief 模拟天气数据提供者
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef MOCK_WEATHER_PROVIDER_H
#define MOCK_WEATHER_PROVIDER_H

#include "interfaces/IWeatherProvider.h"

class MockWeatherProvider : public IWeatherProvider {
public:
    MockWeatherProvider();

    bool begin() override;
    bool update() override;
    const WeatherInfo& getData() const override;

private:
    WeatherInfo _data;
    uint32_t _lastUpdate;
    uint8_t _stateIndex;
};

#endif // MOCK_WEATHER_PROVIDER_H
