/**
 * @file SettingsManager.cpp
 * @brief 设备设置管理器实现
 * @version 1.0.0
 */

#include "SettingsManager.h"

SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

SettingsManager::SettingsManager()
    : _settings(buildDefaultSettings())
    , _fsReady(false) {
}

bool SettingsManager::begin() {
    if (!LittleFS.begin(true)) {
        DEBUG_PRINTLN("[SettingsManager] 错误：LittleFS挂载失败");
        _fsReady = false;
        _settings = buildDefaultSettings();
        return false;
    }

    _fsReady = true;

    if (!LittleFS.exists("/config")) {
        LittleFS.mkdir("/config");
    }

    if (!load()) {
        _settings = buildDefaultSettings();
        save();
    }

    DEBUG_PRINTLN("[SettingsManager] 设备设置加载完成");
    return true;
}

const DeviceSettings& SettingsManager::getSettings() const {
    return _settings;
}

String SettingsManager::getSettingsJson(bool includeSecrets) const {
    DynamicJsonDocument doc(1536);

    JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = _settings.wifi.ssid;
    wifi["hasPassword"] = _settings.wifi.password.length() > 0;
    if (includeSecrets) {
        wifi["password"] = _settings.wifi.password;
    }

    JsonObject region = doc.createNestedObject("region");
    region["timezoneOffsetSec"] = _settings.region.timezoneOffsetSec;
    region["city"] = _settings.region.city;
    region["locale"] = _settings.region.locale;

    JsonArray alarms = doc.createNestedArray("alarms");
    for (size_t i = 0; i < MAX_ALARMS; ++i) {
        const AlarmConfig& alarm = _settings.alarms[i];
        JsonObject item = alarms.createNestedObject();
        item["id"] = alarm.id;
        item["hour"] = alarm.hour;
        item["minute"] = alarm.minute;
        item["enabled"] = alarm.enabled;
        item["repeatMask"] = alarm.repeatMask;
        item["label"] = alarm.label;
    }

    String output;
    serializeJsonPretty(doc, output);
    return output;
}

bool SettingsManager::updateFromJson(const String& json, String& error) {
    DynamicJsonDocument doc(1536);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        error = String("JSON解析失败: ") + err.c_str();
        return false;
    }

    if (doc.containsKey("wifi")) {
        JsonObject wifi = doc["wifi"].as<JsonObject>();
        if (wifi.containsKey("ssid")) {
            _settings.wifi.ssid = wifi["ssid"].as<String>();
        }
        if (wifi.containsKey("password")) {
            _settings.wifi.password = wifi["password"].as<String>();
        }
    }

    if (doc.containsKey("region")) {
        JsonObject region = doc["region"].as<JsonObject>();
        if (region.containsKey("timezoneOffsetSec")) {
            _settings.region.timezoneOffsetSec = region["timezoneOffsetSec"].as<int32_t>();
        }
        if (region.containsKey("city")) {
            _settings.region.city = region["city"].as<String>();
        }
        if (region.containsKey("locale")) {
            _settings.region.locale = region["locale"].as<String>();
        }
    }

    if (doc.containsKey("alarms")) {
        JsonArray alarms = doc["alarms"].as<JsonArray>();
        size_t index = 0;
        for (JsonVariant item : alarms) {
            if (index >= MAX_ALARMS) {
                break;
            }
            AlarmConfig& alarm = _settings.alarms[index];
            if (item.containsKey("id")) {
                alarm.id = item["id"].as<uint8_t>();
            } else {
                alarm.id = index + 1;
            }
            if (item.containsKey("hour")) {
                alarm.hour = clampU8(item["hour"].as<int>(), 0, 23);
            }
            if (item.containsKey("minute")) {
                alarm.minute = clampU8(item["minute"].as<int>(), 0, 59);
            }
            if (item.containsKey("enabled")) {
                alarm.enabled = item["enabled"].as<bool>();
            }
            if (item.containsKey("repeatMask")) {
                alarm.repeatMask = item["repeatMask"].as<uint8_t>();
            } else if (item.containsKey("days")) {
                alarm.repeatMask = parseRepeatMask(item["days"]);
            }
            if (item.containsKey("label")) {
                alarm.label = item["label"].as<String>();
            }
            ++index;
        }
    }

    ensureDefaults();
    return save();
}

bool SettingsManager::save() {
    if (!_fsReady) {
        return false;
    }

    DynamicJsonDocument doc(1536);
    JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = _settings.wifi.ssid;
    wifi["password"] = _settings.wifi.password;

    JsonObject region = doc.createNestedObject("region");
    region["timezoneOffsetSec"] = _settings.region.timezoneOffsetSec;
    region["city"] = _settings.region.city;
    region["locale"] = _settings.region.locale;

    JsonArray alarms = doc.createNestedArray("alarms");
    for (size_t i = 0; i < MAX_ALARMS; ++i) {
        const AlarmConfig& alarm = _settings.alarms[i];
        JsonObject item = alarms.createNestedObject();
        item["id"] = alarm.id;
        item["hour"] = alarm.hour;
        item["minute"] = alarm.minute;
        item["enabled"] = alarm.enabled;
        item["repeatMask"] = alarm.repeatMask;
        item["label"] = alarm.label;
    }

    File file = LittleFS.open(DEVICE_SETTINGS_FILE, "w");
    if (!file) {
        DEBUG_PRINTLN("[SettingsManager] 错误：无法写入配置文件");
        return false;
    }
    if (serializeJsonPretty(doc, file) == 0) {
        DEBUG_PRINTLN("[SettingsManager] 错误：写入配置失败");
        file.close();
        return false;
    }
    file.close();
    return true;
}

DeviceSettings SettingsManager::buildDefaultSettings() const {
    DeviceSettings settings;
    settings.wifi.ssid = "";
    settings.wifi.password = "";
    settings.region.timezoneOffsetSec = NTP_TIMEZONE_OFFSET_SEC;
    settings.region.city = "北京";
    settings.region.locale = "zh-CN";
    for (size_t i = 0; i < MAX_ALARMS; ++i) {
        settings.alarms[i].id = i + 1;
        settings.alarms[i].hour = 7 + i;
        settings.alarms[i].minute = 30;
        settings.alarms[i].enabled = false;
        settings.alarms[i].repeatMask = 0;
        settings.alarms[i].label = String("闹钟") + String(i + 1);
    }
    return settings;
}

bool SettingsManager::load() {
    if (!_fsReady) {
        return false;
    }

    if (!LittleFS.exists(DEVICE_SETTINGS_FILE)) {
        return false;
    }

    File file = LittleFS.open(DEVICE_SETTINGS_FILE, "r");
    if (!file) {
        return false;
    }

    DynamicJsonDocument doc(1536);
    DeserializationError err = deserializeJson(doc, file);
    file.close();
    if (err) {
        DEBUG_PRINTLN("[SettingsManager] 错误：配置文件解析失败");
        return false;
    }

    _settings = buildDefaultSettings();

    if (doc.containsKey("wifi")) {
        JsonObject wifi = doc["wifi"].as<JsonObject>();
        _settings.wifi.ssid = wifi["ssid"].as<String>();
        _settings.wifi.password = wifi["password"].as<String>();
    }

    if (doc.containsKey("region")) {
        JsonObject region = doc["region"].as<JsonObject>();
        _settings.region.timezoneOffsetSec = region["timezoneOffsetSec"].as<int32_t>();
        _settings.region.city = region["city"].as<String>();
        _settings.region.locale = region["locale"].as<String>();
    }

    if (doc.containsKey("alarms")) {
        JsonArray alarms = doc["alarms"].as<JsonArray>();
        size_t index = 0;
        for (JsonVariant item : alarms) {
            if (index >= MAX_ALARMS) {
                break;
            }
            AlarmConfig& alarm = _settings.alarms[index];
            alarm.id = item["id"].as<uint8_t>();
            alarm.hour = clampU8(item["hour"].as<int>(), 0, 23);
            alarm.minute = clampU8(item["minute"].as<int>(), 0, 59);
            alarm.enabled = item["enabled"].as<bool>();
            alarm.repeatMask = item["repeatMask"].as<uint8_t>();
            alarm.label = item["label"].as<String>();
            ++index;
        }
    }

    ensureDefaults();
    return true;
}

void SettingsManager::ensureDefaults() {
    if (_settings.region.city.length() == 0) {
        _settings.region.city = "北京";
    }
    if (_settings.region.locale.length() == 0) {
        _settings.region.locale = "zh-CN";
    }
    for (size_t i = 0; i < MAX_ALARMS; ++i) {
        AlarmConfig& alarm = _settings.alarms[i];
        if (alarm.id == 0) {
            alarm.id = i + 1;
        }
        if (alarm.label.length() == 0) {
            alarm.label = String("闹钟") + String(i + 1);
        }
    }
}

uint8_t SettingsManager::clampU8(int value, uint8_t minVal, uint8_t maxVal) const {
    if (value < minVal) {
        return minVal;
    }
    if (value > maxVal) {
        return maxVal;
    }
    return static_cast<uint8_t>(value);
}

uint8_t SettingsManager::parseRepeatMask(const JsonVariant& value) const {
    uint8_t mask = 0;
    if (!value.is<JsonArray>()) {
        return mask;
    }
    JsonArray days = value.as<JsonArray>();
    size_t index = 0;
    for (JsonVariant item : days) {
        if (index >= 7) {
            break;
        }
        if (item.as<bool>()) {
            mask |= (1 << index);
        }
        ++index;
    }
    return mask;
}

