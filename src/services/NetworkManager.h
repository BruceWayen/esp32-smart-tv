/**
 * @file NetworkManager.h
 * @brief 网络管理器 - WiFi连接与Web配置
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include "config/Config.h"
#include "services/SettingsManager.h"

class NetworkManager {
public:
    static NetworkManager& getInstance();

    bool begin();
    void reconnectIfNeeded();

private:
    NetworkManager();
    ~NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    SettingsManager& _settingsMgr;
    AsyncWebServer _server;
    bool _apMode;
    uint32_t _lastReconnectAttempt;

    bool connectToWifi();
    void startAccessPoint();
    void startWebServer();
    String buildSettingsPage() const;
    void handleWifiUpdate(const JsonVariant& data, AsyncWebServerRequest* request);
    void handleSettingsUpdate(const JsonVariant& data, AsyncWebServerRequest* request);
};

#endif // NETWORK_MANAGER_H
