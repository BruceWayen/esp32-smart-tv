/**
 * @file NetworkManager.cpp
 * @brief 网络管理器实现
 * @version 1.0.0
 */

#include "NetworkManager.h"
#include "config/Config.h"
#include "services/ThemeManager.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

namespace {
AsyncWebServer server(WEB_SERVER_PORT);
}

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager()
    : _isActive(false) {
}

bool NetworkManager::begin() {
    if (!startAccessPoint()) {
        DEBUG_PRINTLN("[NetworkManager] ERROR: AP start failed");
        return false;
    }

    startWebServer();
    _isActive = true;
    return true;
}

bool NetworkManager::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    bool result = WiFi.softAP(AP_SSID, AP_PASSWORD);
    if (result) {
        DEBUG_PRINTF("[NetworkManager] AP started: %s (IP: %s)\n", AP_SSID, WiFi.softAPIP().toString().c_str());
    }
    return result;
}

void NetworkManager::startWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        String html;
        html += "<!DOCTYPE html><html><head><meta charset='utf-8'/>";
        html += "<title>ESP32 SmartTV</title></head><body>";
        html += "<h2>ESP32 SmartTV Web Config</h2>";
        html += "<p>GET /api/theme - 获取主题配置</p>";
        html += "<p>POST /api/theme - 更新主题配置(JSON)</p>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/api/theme", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = ThemeManager::getInstance().getThemeJson();
        request->send(200, "application/json", json);
    });

    server.on(
        "/api/theme",
        HTTP_POST,
        [](AsyncWebServerRequest* request) {},
        nullptr,
        [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            if (index == 0) {
                request->_tempObject = new String();
            }

            String* body = static_cast<String*>(request->_tempObject);
            body->concat(reinterpret_cast<const char*>(data), len);

            if (index + len == total) {
                String error;
                bool ok = ThemeManager::getInstance().updateThemeFromJson(*body, error);
                delete body;
                request->_tempObject = nullptr;

                if (ok) {
                    request->send(200, "application/json", ThemeManager::getInstance().getThemeJson());
                } else {
                    String response = String("{\"error\":\"") + error + "\"}";
                    request->send(400, "application/json", response);
                }
            }
        }
    );

    server.begin();
    DEBUG_PRINTF("[NetworkManager] Web server started on port %d\n", WEB_SERVER_PORT);
}
