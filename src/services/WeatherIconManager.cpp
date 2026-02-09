/**
 * @file WeatherIconManager.cpp
 * @brief 天气图标配置管理器实现
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "WeatherIconManager.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

WeatherIconManager& WeatherIconManager::getInstance() {
    static WeatherIconManager instance;
    return instance;
}

WeatherIconManager::WeatherIconManager()
    : _iconCount(0)
    , _fsReady(false) {}

bool WeatherIconManager::begin() {
    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("[WeatherIcon] 错误：SPIFFS挂载失败，使用默认图标");
        _fsReady = false;
        loadDefaults();
        return false;
    }

    _fsReady = true;

    if (!loadFromFile()) {
        loadDefaults();
    }

    DEBUG_PRINTF("[WeatherIcon] 图标配置加载完成，共%d个\n", _iconCount);
    return true;
}

WeatherIconStyle WeatherIconManager::getStyle(const String& key) const {
    for (size_t i = 0; i < _iconCount; i++) {
        if (_icons[i].key == key) {
            return _icons[i];
        }
    }
    return WeatherIconStyle();
}

void WeatherIconManager::loadDefaults() {
    _iconCount = 0;
    WeatherIconStyle sun;
    sun.key = "sunny";
    sun.type = "sun";
    sun.color = 0xFFE0;
    _icons[_iconCount++] = sun;

    WeatherIconStyle cloud;
    cloud.key = "cloudy";
    cloud.type = "cloud";
    cloud.color = 0xC618;
    _icons[_iconCount++] = cloud;

    WeatherIconStyle rain;
    rain.key = "rain";
    rain.type = "rain";
    rain.color = 0x07FF;
    _icons[_iconCount++] = rain;

    WeatherIconStyle snow;
    snow.key = "snow";
    snow.type = "snow";
    snow.color = 0xFFFF;
    _icons[_iconCount++] = snow;

    WeatherIconStyle unknown;
    unknown.key = "unknown";
    unknown.type = "unknown";
    unknown.color = 0xFFFF;
    _icons[_iconCount++] = unknown;
}

bool WeatherIconManager::loadFromFile() {
    if (!_fsReady || !SPIFFS.exists(WEATHER_ICON_CONFIG_FILE)) {
        return false;
    }

    File file = SPIFFS.open(WEATHER_ICON_CONFIG_FILE, "r");
    if (!file) {
        return false;
    }

    String json = file.readString();
    file.close();

    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        DEBUG_PRINTF("[WeatherIcon] 配置解析失败: %s\n", err.c_str());
        return false;
    }

    JsonArray list = doc["icons"].as<JsonArray>();
    if (list.isNull()) {
        return false;
    }

    _iconCount = 0;
    for (JsonVariant item : list) {
        if (_iconCount >= 16) {
            break;
        }
        WeatherIconStyle style;
        style.key = item["key"].as<String>();
        style.type = item["type"].as<String>();
        style.color = parseColor(item["color"].as<String>(), 0xFFFF);
        if (style.key.length() == 0) {
            continue;
        }
        if (style.type.length() == 0) {
            style.type = "unknown";
        }
        _icons[_iconCount++] = style;
    }

    return _iconCount > 0;
}

uint16_t WeatherIconManager::parseColor(const String& value, uint16_t fallback) const {
    if (value.length() == 0) {
        return fallback;
    }
    String color = value;
    color.replace("#", "");
    color.replace("0x", "");
    if (color.length() != 6) {
        return fallback;
    }
    uint32_t rgb = strtoul(color.c_str(), nullptr, 16);
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
