/**
 * @file UserConfig.h
 * @brief 用户配置存储 - WiFi/闹钟/城市信息
 * @version 1.0.0
 */

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include <Arduino.h>
#include <vector>

struct AlarmConfig {
    uint8_t hour;
    uint8_t minute;
    bool enabled;
    String label;
};

struct UserConfigData {
    String wifiSsid;
    String wifiPassword;
    String cityId;
    String cityName;
    std::vector<AlarmConfig> alarms;
};

class UserConfig {
public:
    static UserConfig& getInstance();

    bool begin();
    const UserConfigData& getData() const;

    bool updateWifi(const String& ssid, const String& password);
    bool updateCity(const String& cityId, const String& cityName);
    bool updateAlarms(const std::vector<AlarmConfig>& alarms);

    bool save();

private:
    UserConfig();
    UserConfig(const UserConfig&) = delete;
    UserConfig& operator=(const UserConfig&) = delete;

    bool load();

    UserConfigData _data;
    bool _fsReady;
};

#endif // USER_CONFIG_H
