/**
 * @file QWeatherClient.h
 * @brief 和风天气接口客户端
 * @version 1.0.0
 */

#ifndef QWEATHER_CLIENT_H
#define QWEATHER_CLIENT_H

#include <Arduino.h>

struct CityInfo {
    String id;
    String name;
};

class QWeatherClient {
public:
    QWeatherClient();

    bool begin();

    bool fetchCityLookup(const String& locationPinyin, String& responseJson);
    bool fetchNowWeather(const String& cityId, String& responseJson);
    bool fetchDailyWeather(const String& cityId, String& responseJson);

private:
    bool ensureJwtToken();
    bool refreshJwtToken();
    bool sendGetRequest(const String& path, String& responseJson, int& statusCode);

    String base64UrlEncode(const uint8_t* data, size_t length);
    bool signJwtMessage(const String& message, String& signatureBase64Url);

    String _jwtToken;
    uint32_t _jwtExpireEpoch;
    bool _ready;
};

#endif // QWEATHER_CLIENT_H
