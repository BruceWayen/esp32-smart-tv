/**
 * @file SettingsManager.h
 * @brief 设备设置管理器 - WiFi/闹钟/区域配置
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "config/Config.h"

struct WifiConfig {
    String ssid;
    String password;

    WifiConfig() : ssid(""), password("") {}
};

struct RegionConfig {
    int32_t timezoneOffsetSec;
    String city;
    String locale;

    RegionConfig()
        : timezoneOffsetSec(NTP_TIMEZONE_OFFSET_SEC)
        , city("北京")
        , locale("zh-CN") {}
};

struct AlarmConfig {
    uint8_t id;
    uint8_t hour;
    uint8_t minute;
    bool enabled;
    uint8_t repeatMask; // bit0~bit6: Mon~Sun
    String label;

    AlarmConfig()
        : id(0)
        , hour(7)
        , minute(30)
        , enabled(false)
        , repeatMask(0)
        , label("闹钟") {}
};

struct DeviceSettings {
    WifiConfig wifi;
    RegionConfig region;
    AlarmConfig alarms[MAX_ALARMS];
};

class SettingsManager {
public:
    static SettingsManager& getInstance();

    bool begin();
    const DeviceSettings& getSettings() const;
    String getSettingsJson(bool includeSecrets) const;
    bool updateFromJson(const String& json, String& error);
    bool save();

private:
    SettingsManager();
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    DeviceSettings _settings;
    bool _fsReady;

    DeviceSettings buildDefaultSettings() const;
    bool load();
    void ensureDefaults();
    uint8_t clampU8(int value, uint8_t minVal, uint8_t maxVal) const;
    uint8_t parseRepeatMask(const JsonVariant& value) const;
};

#endif // SETTINGS_MANAGER_H
