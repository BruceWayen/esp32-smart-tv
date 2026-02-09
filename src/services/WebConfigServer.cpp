/**
 * @file WebConfigServer.cpp
 * @brief Web配置服务器实现
 * @version 1.0.0
 */

#include "WebConfigServer.h"
#include "config/Config.h"
#include "services/ThemeManager.h"
#include "services/QWeatherClient.h"
#include "services/UserConfig.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>

static QWeatherClient gWeatherClient;

WebConfigServer& WebConfigServer::getInstance() {
    static WebConfigServer instance;
    return instance;
}

WebConfigServer::WebConfigServer()
    : _server(WEB_SERVER_PORT) {}

bool WebConfigServer::begin() {
    DEBUG_PRINTLN("[Web] 初始化Web配置服务");

    if (!UserConfig::getInstance().begin()) {
        DEBUG_PRINTLN("[Web] 用户配置初始化失败");
    }

    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("[Web] SPIFFS挂载失败");
    }

    // 同时开启AP+STA，便于手机配置
    WiFi.mode(WIFI_AP_STA);
    bool apOk = WiFi.softAP(AP_SSID, AP_PASSWORD);
    if (apOk) {
        DEBUG_PRINTF("[Web] AP热点已启动: %s\n", AP_SSID);
    } else {
        DEBUG_PRINTLN("[Web] AP热点启动失败");
    }

    const UserConfigData& config = UserConfig::getInstance().getData();
    // 如果已有WiFi配置，则尝试自动连接
    if (!config.wifiSsid.isEmpty()) {
        DEBUG_PRINTF("[Web] 连接WiFi: %s\n", config.wifiSsid.c_str());
        WiFi.begin(config.wifiSsid.c_str(), config.wifiPassword.c_str());
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
            delay(200);
        }
        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINTF("[Web] WiFi连接成功: %s\n", WiFi.localIP().toString().c_str());
        } else {
            DEBUG_PRINTLN("[Web] WiFi连接失败");
        }
    }

    gWeatherClient.begin();

    setupRoutes();
    _server.begin();
    DEBUG_PRINTLN("[Web] Web服务器已启动");
    return true;
}

void WebConfigServer::setupRoutes() {
    // 静态资源托管
    _server.serveStatic("/", SPIFFS, "/web/").setDefaultFile("index.html");

    _server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest* request) {
        const UserConfigData& config = UserConfig::getInstance().getData();
        DynamicJsonDocument doc(2048);
        JsonObject wifi = doc.createNestedObject("wifi");
        wifi["ssid"] = config.wifiSsid;
        wifi["password"] = config.wifiPassword;

        JsonObject city = doc.createNestedObject("city");
        city["id"] = config.cityId;
        city["name"] = config.cityName;

        JsonArray alarms = doc.createNestedArray("alarms");
        for (const auto& alarm : config.alarms) {
            JsonObject item = alarms.createNestedObject();
            item["hour"] = alarm.hour;
            item["minute"] = alarm.minute;
            item["enabled"] = alarm.enabled;
            item["label"] = alarm.label;
        }

        String response;
        serializeJsonPretty(doc, response);
        request->send(200, "application/json", response);
    });

    _server.on("/api/wifi", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String body = request->arg("plain");
        handleWifiUpdate(request, body);
    });

    _server.on("/api/alarms", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String body = request->arg("plain");
        handleAlarmUpdate(request, body);
    });

    _server.on("/api/city/lookup", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleCityLookup(request);
    });

    _server.on("/api/city/select", HTTP_POST, [this](AsyncWebServerRequest* request) {
        String body = request->arg("plain");
        handleCitySelect(request, body);
    });

    _server.on("/api/weather/now", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleWeatherNow(request);
    });

    _server.on("/api/weather/7d", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleWeatherDaily(request);
    });

    _server.on("/api/theme", HTTP_GET, [](AsyncWebServerRequest* request) {
        String response = ThemeManager::getInstance().getThemeJson();
        request->send(200, "application/json", response);
    });

    _server.on("/api/theme", HTTP_POST, [](AsyncWebServerRequest* request) {
        String body = request->arg("plain");
        if (body.length() == 0) {
            request->send(400, "application/json", "{\"error\":\"缺少主题配置内容\"}");
            return;
        }
        String error;
        if (!ThemeManager::getInstance().updateThemeFromJson(body, error)) {
            String payload = String("{\"error\":\"") + error + "\"}";
            request->send(400, "application/json", payload);
            return;
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    _server.on("/api/theme/preset", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (!ThemeManager::getInstance().resetToDefaultTheme()) {
            request->send(500, "application/json", "{\"error\":\"生成默认主题失败\"}");
            return;
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    _server.on(
        "/api/theme/upload",
        HTTP_POST,
        [](AsyncWebServerRequest* request) {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        },
        [](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* data, size_t len, bool final) {
            (void)filename;
            String targetPath = THEME_CONFIG_FILE;
            if (request->hasParam("path", true)) {
                targetPath = request->getParam("path", true)->value();
            }
            if (!targetPath.startsWith("/")) {
                targetPath = "/" + targetPath;
            }
            if (index == 0) {
                request->_tempFile = SPIFFS.open(targetPath, "w");
            }
            if (request->_tempFile) {
                request->_tempFile.write(data, len);
            }
            if (final) {
                if (request->_tempFile) {
                    request->_tempFile.close();
                }
                if (targetPath == THEME_CONFIG_FILE) {
                    ThemeManager::getInstance().reloadThemeFromFile();
                }
            }
        });
}

void WebConfigServer::handleWifiUpdate(AsyncWebServerRequest* request, const String& body) {
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        request->send(400, "application/json", "{\"error\":\"JSON解析失败\"}");
        return;
    }

    String ssid = doc["ssid"] | "";
    String password = doc["password"] | "";

    if (ssid.isEmpty()) {
        request->send(400, "application/json", "{\"error\":\"SSID不能为空\"}");
        return;
    }

    UserConfig::getInstance().updateWifi(ssid, password);
    DEBUG_PRINTF("[Web] 更新WiFi配置: %s\n", ssid.c_str());

    WiFi.begin(ssid.c_str(), password.c_str());
    request->send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebConfigServer::handleAlarmUpdate(AsyncWebServerRequest* request, const String& body) {
    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        request->send(400, "application/json", "{\"error\":\"JSON解析失败\"}");
        return;
    }

    std::vector<AlarmConfig> alarms;
    JsonArray array = doc["alarms"].as<JsonArray>();
    for (JsonVariant item : array) {
        AlarmConfig alarm;
        alarm.hour = item["hour"] | 0;
        alarm.minute = item["minute"] | 0;
        alarm.enabled = item["enabled"] | false;
        alarm.label = item["label"] | "";
        alarms.push_back(alarm);
    }

    UserConfig::getInstance().updateAlarms(alarms);
    DEBUG_PRINTLN("[Web] 闹钟配置已更新");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebConfigServer::handleCityLookup(AsyncWebServerRequest* request) {
    if (!request->hasParam("keyword")) {
        request->send(400, "application/json", "{\"error\":\"缺少keyword参数\"}");
        return;
    }

    // keyword是拼音结果
    String keyword = request->getParam("keyword")->value();
    String response;

    if (!gWeatherClient.fetchCityLookup(keyword, response)) {
        request->send(500, "application/json", "{\"error\":\"查询失败\"}");
        return;
    }

    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
        request->send(500, "application/json", "{\"error\":\"解析失败\"}");
        return;
    }

    DynamicJsonDocument output(2048);
    output["code"] = doc["code"] | "";
    JsonArray list = output.createNestedArray("locations");
    JsonArray locations = doc["location"].as<JsonArray>();
    for (JsonVariant item : locations) {
        JsonObject entry = list.createNestedObject();
        entry["id"] = item["id"] | "";
        entry["name"] = item["name"] | "";
    }

    String payload;
    serializeJsonPretty(output, payload);
    request->send(200, "application/json", payload);
}

void WebConfigServer::handleCitySelect(AsyncWebServerRequest* request, const String& body) {
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        request->send(400, "application/json", "{\"error\":\"JSON解析失败\"}");
        return;
    }

    String cityId = doc["id"] | "";
    String cityName = doc["name"] | "";

    if (cityId.isEmpty()) {
        request->send(400, "application/json", "{\"error\":\"城市ID不能为空\"}");
        return;
    }

    UserConfig::getInstance().updateCity(cityId, cityName);
    DEBUG_PRINTF("[Web] 城市已选择: %s(%s)\n", cityName.c_str(), cityId.c_str());
    request->send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebConfigServer::handleWeatherNow(AsyncWebServerRequest* request) {
    const UserConfigData& config = UserConfig::getInstance().getData();
    if (config.cityId.isEmpty()) {
        request->send(400, "application/json", "{\"error\":\"未选择城市\"}");
        return;
    }

    String response;
    if (!gWeatherClient.fetchNowWeather(config.cityId, response)) {
        request->send(500, "application/json", "{\"error\":\"获取失败\"}");
        return;
    }

    request->send(200, "application/json", response);
}

void WebConfigServer::handleWeatherDaily(AsyncWebServerRequest* request) {
    const UserConfigData& config = UserConfig::getInstance().getData();
    if (config.cityId.isEmpty()) {
        request->send(400, "application/json", "{\"error\":\"未选择城市\"}");
        return;
    }

    String response;
    if (!gWeatherClient.fetchDailyWeather(config.cityId, response)) {
        request->send(500, "application/json", "{\"error\":\"获取失败\"}");
        return;
    }

    request->send(200, "application/json", response);
}
