/**
 * @file QWeatherProvider.cpp
 * @brief 和风天气数据提供者实现
 * @version 1.0.0
 */

#include "QWeatherProvider.h"
#include "config/Config.h"
#include <ArduinoJson.h>
#include <WiFi.h>

QWeatherProvider::QWeatherProvider()
    : _client()
    , _data()
    , _lastUpdate(0) {}

bool QWeatherProvider::begin() {
    DEBUG_PRINTLN("[天气] 初始化和风天气数据源");
    _lastUpdate = 0;
    return _client.begin();
}

bool QWeatherProvider::update() {
    // WiFi未连接则不拉取
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    const UserConfigData& config = UserConfig::getInstance().getData();
    if (config.cityId.isEmpty()) {
        return false;
    }

    uint32_t now = millis();
    // 10分钟更新一次
    if (now - _lastUpdate < 600000) {
        return false;
    }

    _lastUpdate = now;

    String response;
    if (!_client.fetchNowWeather(config.cityId, response)) {
        DEBUG_PRINTLN("[天气] 获取实时天气失败");
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
        DEBUG_PRINTLN("[天气] 实时天气解析失败");
        return false;
    }

    if (String(doc["code"].as<const char*>()) != "200") {
        DEBUG_PRINTLN("[天气] 实时天气返回异常");
        return false;
    }

    JsonObject nowObj = doc["now"];
    _data.condition = nowObj["text"] | "未知";
    _data.iconKey = mapIconCodeToKey(nowObj["icon"] | "");
    _data.temperature = String(nowObj["temp"] | "0").toFloat();
    _data.humidity = String(nowObj["humidity"] | "0").toFloat();
    _data.windSpeed = String(nowObj["windSpeed"] | "0").toFloat();
    _data.windDir = nowObj["windDir"] | "无风";
    _data.timestamp = now;

    DEBUG_PRINTF("[天气] %s %.1f℃ 体感%.1f℃\n",
                 _data.condition.c_str(),
                 _data.temperature,
                 String(nowObj["feelsLike"] | "0").toFloat());

    return true;
}

const WeatherInfo& QWeatherProvider::getData() const {
    return _data;
}

String QWeatherProvider::mapIconCodeToKey(const String& iconCode) const {
    if (iconCode.isEmpty()) {
        return "unknown";
    }
    return iconCode;
}
