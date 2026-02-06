/**
 * @file QWeatherProvider.h
 * @brief 和风天气数据提供者
 * @version 1.0.0
 */

#ifndef QWEATHER_PROVIDER_H
#define QWEATHER_PROVIDER_H

#include "interfaces/IWeatherProvider.h"
#include "services/QWeatherClient.h"
#include "services/UserConfig.h"

class QWeatherProvider : public IWeatherProvider {
public:
    QWeatherProvider();

    bool begin() override;
    bool update() override;
    const WeatherInfo& getData() const override;

private:
    String mapIconCodeToKey(const String& iconCode) const;

    QWeatherClient _client;
    WeatherInfo _data;
    uint32_t _lastUpdate;
};

#endif // QWEATHER_PROVIDER_H
