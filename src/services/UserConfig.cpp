/**
 * @file UserConfig.cpp
 * @brief 用户配置存储实现
 * @version 1.0.0
 */

#include "UserConfig.h"
#include "config/Config.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

UserConfig& UserConfig::getInstance() {
    static UserConfig instance;
    return instance;
}

UserConfig::UserConfig()
    : _data()
    , _fsReady(false) {}

bool UserConfig::begin() {
    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("[配置] SPIFFS挂载失败");
        _fsReady = false;
        return false;
    }

    _fsReady = true;
    if (!SPIFFS.exists("/config")) {
        SPIFFS.mkdir("/config");
    }

    if (!load()) {
        DEBUG_PRINTLN("[配置] 未找到配置文件，已使用默认值");
        save();
    }

    return true;
}

const UserConfigData& UserConfig::getData() const {
    return _data;
}

bool UserConfig::updateWifi(const String& ssid, const String& password) {
    _data.wifiSsid = ssid;
    _data.wifiPassword = password;
    return save();
}

bool UserConfig::updateCity(const String& cityId, const String& cityName) {
    _data.cityId = cityId;
    _data.cityName = cityName;
    return save();
}

bool UserConfig::updateAlarms(const std::vector<AlarmConfig>& alarms) {
    _data.alarms = alarms;
    return save();
}

bool UserConfig::load() {
    if (!_fsReady) {
        return false;
    }

    if (!SPIFFS.exists(USER_CONFIG_FILE)) {
        return false;
    }

    File file = SPIFFS.open(USER_CONFIG_FILE, "r");
    if (!file) {
        DEBUG_PRINTLN("[配置] 配置文件打开失败");
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, file);
    file.close();
    if (err) {
        DEBUG_PRINTLN("[配置] 配置解析失败");
        return false;
    }

    _data.wifiSsid = doc["wifi"]["ssid"] | "";
    _data.wifiPassword = doc["wifi"]["password"] | "";
    _data.cityId = doc["city"]["id"] | "";
    _data.cityName = doc["city"]["name"] | "";

    _data.alarms.clear();
    JsonArray alarmArray = doc["alarms"].as<JsonArray>();
    for (JsonVariant alarmItem : alarmArray) {
        AlarmConfig alarm;
        alarm.hour = alarmItem["hour"] | 0;
        alarm.minute = alarmItem["minute"] | 0;
        alarm.enabled = alarmItem["enabled"] | false;
        alarm.label = alarmItem["label"] | "";
        _data.alarms.push_back(alarm);
    }

    DEBUG_PRINTLN("[配置] 用户配置加载完成");
    return true;
}

bool UserConfig::save() {
    if (!_fsReady) {
        return false;
    }

    DynamicJsonDocument doc(2048);
    JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = _data.wifiSsid;
    wifi["password"] = _data.wifiPassword;

    JsonObject city = doc.createNestedObject("city");
    city["id"] = _data.cityId;
    city["name"] = _data.cityName;

    JsonArray alarms = doc.createNestedArray("alarms");
    for (const auto& alarm : _data.alarms) {
        JsonObject item = alarms.createNestedObject();
        item["hour"] = alarm.hour;
        item["minute"] = alarm.minute;
        item["enabled"] = alarm.enabled;
        item["label"] = alarm.label;
    }

    File file = SPIFFS.open(USER_CONFIG_FILE, "w");
    if (!file) {
        DEBUG_PRINTLN("[配置] 配置保存失败");
        return false;
    }

    serializeJsonPretty(doc, file);
    file.close();

    DEBUG_PRINTLN("[配置] 用户配置已保存");
    return true;
}
