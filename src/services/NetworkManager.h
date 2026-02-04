/**
 * @file NetworkManager.h
 * @brief 网络管理器 - WiFi与Web配置
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>

class NetworkManager {
public:
    static NetworkManager& getInstance();

    bool begin();

private:
    NetworkManager();
    ~NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    bool startAccessPoint();
    void startWebServer();

    bool _isActive;
};

#endif // NETWORK_MANAGER_H
