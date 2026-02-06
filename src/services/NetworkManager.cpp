/**
 * @file NetworkManager.cpp
 * @brief 网络管理器实现
 * @version 1.0.0
 */

#include "NetworkManager.h"

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager()
    : _settingsMgr(SettingsManager::getInstance())
    , _server(WEB_SERVER_PORT)
    , _apMode(false)
    , _lastReconnectAttempt(0) {
}

bool NetworkManager::begin() {
    _settingsMgr.begin();

    WiFi.mode(WIFI_STA);
    bool connected = connectToWifi();
    if (!connected) {
        startAccessPoint();
    }

    startWebServer();

    DEBUG_PRINTLN("[NetworkManager] 网络初始化完成");
    return connected;
}

void NetworkManager::reconnectIfNeeded() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    uint32_t now = millis();
    if (now - _lastReconnectAttempt < WIFI_RECONNECT_INTERVAL_MS) {
        return;
    }
    _lastReconnectAttempt = now;
    if (!connectToWifi()) {
        if (!_apMode) {
            startAccessPoint();
        }
    }
}

bool NetworkManager::connectToWifi() {
    const DeviceSettings& settings = _settingsMgr.getSettings();
    if (settings.wifi.ssid.length() == 0) {
        DEBUG_PRINTLN("[NetworkManager] 未配置WiFi，进入AP模式");
        return false;
    }

    DEBUG_PRINTF("[NetworkManager] 连接WiFi: %s\n", settings.wifi.ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifi.ssid.c_str(), settings.wifi.password.c_str());

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
        delay(200);
    }

    if (WiFi.status() == WL_CONNECTED) {
        _apMode = false;
        DEBUG_PRINTF("[NetworkManager] WiFi连接成功，IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }

    DEBUG_PRINTLN("[NetworkManager] WiFi连接失败");
    return false;
}

void NetworkManager::startAccessPoint() {
    DEBUG_PRINTF("[NetworkManager] 启动AP模式: %s\n", AP_SSID);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    _apMode = true;
    IPAddress ip = WiFi.softAPIP();
    DEBUG_PRINTF("[NetworkManager] AP IP: %s\n", ip.toString().c_str());
}

void NetworkManager::startWebServer() {
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/html; charset=utf-8", buildSettingsPage());
    });

    _server.on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest* request) {
        String json = _settingsMgr.getSettingsJson(false);
        request->send(200, "application/json", json);
    });

    auto* settingsHandler = new AsyncCallbackJsonWebHandler("/api/settings", [this](AsyncWebServerRequest* request, JsonVariant& json) {
        handleSettingsUpdate(json, request);
    });
    _server.addHandler(settingsHandler);

    auto* wifiHandler = new AsyncCallbackJsonWebHandler("/api/wifi", [this](AsyncWebServerRequest* request, JsonVariant& json) {
        handleWifiUpdate(json, request);
    });
    _server.addHandler(wifiHandler);

    _server.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "application/json", "{\"error\":\"Not Found\"}");
    });

    _server.begin();
}

String NetworkManager::buildSettingsPage() const {
    return R"rawliteral(
<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>ESP32 SmartTV 设置</title>
  <style>
    :root {
      color-scheme: light;
      font-family: "PingFang SC", "Microsoft Yahei", sans-serif;
      background: #f4f6fb;
      color: #1f2a44;
    }
    body {
      margin: 0;
      padding: 24px;
    }
    .container {
      max-width: 860px;
      margin: 0 auto;
    }
    h1 {
      margin-bottom: 12px;
    }
    .card {
      background: #fff;
      border-radius: 16px;
      box-shadow: 0 10px 30px rgba(15, 23, 42, 0.08);
      padding: 20px 24px;
      margin-bottom: 16px;
    }
    .card h2 {
      margin-top: 0;
    }
    .grid {
      display: grid;
      gap: 12px;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    }
    label {
      display: block;
      font-size: 14px;
      margin-bottom: 6px;
      color: #4b5563;
    }
    input, select {
      width: 100%;
      padding: 8px 10px;
      border-radius: 10px;
      border: 1px solid #d8dde6;
      font-size: 14px;
    }
    button {
      background: #2563eb;
      color: #fff;
      border: none;
      border-radius: 10px;
      padding: 10px 14px;
      cursor: pointer;
      font-size: 14px;
      margin-top: 8px;
    }
    button.secondary {
      background: #64748b;
    }
    .alarm {
      border: 1px dashed #d1d5db;
      padding: 12px;
      border-radius: 12px;
      margin-bottom: 12px;
    }
    .days {
      display: grid;
      grid-template-columns: repeat(7, minmax(30px, 1fr));
      gap: 4px;
      margin-top: 6px;
    }
    .status {
      font-size: 13px;
      color: #10b981;
      margin-top: 6px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>智能桌面小电视 · Web设置</h1>

    <div class="card">
      <h2>WiFi配置</h2>
      <div class="grid">
        <div>
          <label for="ssid">WiFi名称</label>
          <input id="ssid" placeholder="输入SSID" />
        </div>
        <div>
          <label for="password">WiFi密码</label>
          <input id="password" type="password" placeholder="输入密码" />
        </div>
      </div>
      <button id="saveWifi">保存并连接</button>
      <div id="wifiStatus" class="status"></div>
    </div>

    <div class="card">
      <h2>区域与时间</h2>
      <div class="grid">
        <div>
          <label for="city">城市</label>
          <input id="city" placeholder="例如：北京" />
        </div>
        <div>
          <label for="locale">语言区域</label>
          <input id="locale" placeholder="例如：zh-CN" />
        </div>
        <div>
          <label for="timezone">时区偏移（小时）</label>
          <input id="timezone" type="number" step="0.5" />
        </div>
      </div>
      <button id="saveRegion" class="secondary">保存区域设置</button>
      <div id="regionStatus" class="status"></div>
    </div>

    <div class="card">
      <h2>闹钟配置</h2>
      <div id="alarmList"></div>
      <button id="saveAlarms" class="secondary">保存闹钟</button>
      <div id="alarmStatus" class="status"></div>
    </div>
  </div>

<script>
const dayLabels = ["一", "二", "三", "四", "五", "六", "日"];
let cachedSettings = null;

function createAlarmRow(alarm, index) {
  const wrapper = document.createElement("div");
  wrapper.className = "alarm";
  wrapper.innerHTML = `
    <div class="grid">
      <div>
        <label>闹钟名称</label>
        <input data-field="label" value="${alarm.label || ""}" />
      </div>
      <div>
        <label>时间</label>
        <input data-field="time" type="time" value="${String(alarm.hour).padStart(2, "0")}:${String(alarm.minute).padStart(2, "0")}" />
      </div>
      <div>
        <label>启用</label>
        <select data-field="enabled">
          <option value="true" ${alarm.enabled ? "selected" : ""}>开启</option>
          <option value="false" ${!alarm.enabled ? "selected" : ""}>关闭</option>
        </select>
      </div>
    </div>
    <div class="days">
      ${dayLabels.map((label, dayIndex) => {
        const checked = (alarm.repeatMask & (1 << dayIndex)) !== 0;
        return `<label><input type="checkbox" data-day="${dayIndex}" ${checked ? "checked" : ""} />${label}</label>`;
      }).join("")}
    </div>
  `;
  wrapper.dataset.index = index;
  return wrapper;
}

async function loadSettings() {
  const res = await fetch("/api/settings");
  const data = await res.json();
  cachedSettings = data;

  document.getElementById("ssid").value = data.wifi?.ssid || "";
  document.getElementById("password").value = "";
  document.getElementById("city").value = data.region?.city || "";
  document.getElementById("locale").value = data.region?.locale || "";
  document.getElementById("timezone").value = (data.region?.timezoneOffsetSec || 0) / 3600;

  const alarmList = document.getElementById("alarmList");
  alarmList.innerHTML = "";
  (data.alarms || []).forEach((alarm, index) => {
    alarmList.appendChild(createAlarmRow(alarm, index));
  });
}

function collectAlarms() {
  const alarms = [];
  document.querySelectorAll(".alarm").forEach((alarmNode) => {
    const index = parseInt(alarmNode.dataset.index, 10);
    const label = alarmNode.querySelector("[data-field='label']").value;
    const time = alarmNode.querySelector("[data-field='time']").value;
    const enabled = alarmNode.querySelector("[data-field='enabled']").value === "true";
    const [hour, minute] = time.split(":").map(val => parseInt(val, 10));
    let repeatMask = 0;
    alarmNode.querySelectorAll("input[type='checkbox']").forEach((checkbox) => {
      const dayIndex = parseInt(checkbox.dataset.day, 10);
      if (checkbox.checked) {
        repeatMask |= (1 << dayIndex);
      }
    });
    alarms.push({
      id: index + 1,
      label,
      hour,
      minute,
      enabled,
      repeatMask
    });
  });
  return alarms;
}

async function saveWifi() {
  const payload = {
    wifi: {
      ssid: document.getElementById("ssid").value.trim(),
      password: document.getElementById("password").value
    }
  };
  const res = await fetch("/api/wifi", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });
  const data = await res.json();
  document.getElementById("wifiStatus").textContent = data.message || "已发送";
}

async function saveRegion() {
  const payload = {
    region: {
      city: document.getElementById("city").value.trim(),
      locale: document.getElementById("locale").value.trim(),
      timezoneOffsetSec: Math.round(parseFloat(document.getElementById("timezone").value || "0") * 3600)
    }
  };
  const res = await fetch("/api/settings", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });
  const data = await res.json();
  document.getElementById("regionStatus").textContent = data.message || "已保存";
}

async function saveAlarms() {
  const payload = {
    alarms: collectAlarms()
  };
  const res = await fetch("/api/settings", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });
  const data = await res.json();
  document.getElementById("alarmStatus").textContent = data.message || "已保存";
}

window.addEventListener("load", () => {
  loadSettings();
  document.getElementById("saveWifi").addEventListener("click", saveWifi);
  document.getElementById("saveRegion").addEventListener("click", saveRegion);
  document.getElementById("saveAlarms").addEventListener("click", saveAlarms);
});
</script>
</body>
</html>
)rawliteral";
}

void NetworkManager::handleWifiUpdate(const JsonVariant& data, AsyncWebServerRequest* request) {
    if (!data.is<JsonObject>()) {
        request->send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    String json;
    serializeJson(data, json);
    String error;
    if (!_settingsMgr.updateFromJson(json, error)) {
        String message = String("{\"error\":\"") + error + "\"}";
        request->send(400, "application/json", message);
        return;
    }

    connectToWifi();
    request->send(200, "application/json", "{\"ok\":true,\"message\":\"WiFi配置已保存，正在连接\"}");
}

void NetworkManager::handleSettingsUpdate(const JsonVariant& data, AsyncWebServerRequest* request) {
    if (!data.is<JsonObject>()) {
        request->send(400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }

    String json;
    serializeJson(data, json);
    String error;
    if (!_settingsMgr.updateFromJson(json, error)) {
        String message = String("{\"error\":\"") + error + "\"}";
        request->send(400, "application/json", message);
        return;
    }

    request->send(200, "application/json", "{\"ok\":true,\"message\":\"设置已保存\"}");
}

