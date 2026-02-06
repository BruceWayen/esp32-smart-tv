/**
 * @file WebConfigServer.h
 * @brief Web配置服务器
 * @version 1.0.0
 */

#ifndef WEB_CONFIG_SERVER_H
#define WEB_CONFIG_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class WebConfigServer {
public:
    static WebConfigServer& getInstance();

    bool begin();

private:
    WebConfigServer();
    WebConfigServer(const WebConfigServer&) = delete;
    WebConfigServer& operator=(const WebConfigServer&) = delete;

    void setupRoutes();
    void handleWifiUpdate(AsyncWebServerRequest* request, const String& body);
    void handleAlarmUpdate(AsyncWebServerRequest* request, const String& body);
    void handleCityLookup(AsyncWebServerRequest* request);
    void handleCitySelect(AsyncWebServerRequest* request, const String& body);
    void handleWeatherNow(AsyncWebServerRequest* request);
    void handleWeatherDaily(AsyncWebServerRequest* request);

    AsyncWebServer _server;
};

#endif // WEB_CONFIG_SERVER_H
