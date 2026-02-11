#include "ThemeManager.h"

uint16_t ThemeManager::rgbTo565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t ThemeManager::parseColor(const String &hex, uint16_t fallback)
{
    if (hex.length() != 7 || hex[0] != '#')
        return fallback;

    long value = strtol(hex.substring(1).c_str(), nullptr, 16);
    return rgbTo565((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

void ThemeManager::loadTextStyle(JsonObject obj, TextStyle &style)
{
    if (obj["x"].is<int>()) style.x = obj["x"].as<int>();
    if (obj["y"].is<int>()) style.y = obj["y"].as<int>();
    if (obj["size"].is<int>()) style.size = obj["size"].as<int>();
    if (obj["color"].is<const char *>()) style.color = parseColor(obj["color"].as<String>(), style.color);
    if (obj["value"].is<const char *>()) style.value = obj["value"].as<String>();
}

void ThemeManager::loadModuleStyle(JsonObject obj, ModuleStyle &style)
{
    if (obj["x"].is<int>()) style.x = obj["x"].as<int>();
    if (obj["y"].is<int>()) style.y = obj["y"].as<int>();
    if (obj["w"].is<int>()) style.w = obj["w"].as<int>();
    if (obj["h"].is<int>()) style.h = obj["h"].as<int>();
    if (obj["opacity"].is<int>()) style.opacity = constrain(obj["opacity"].as<int>(), 0, 255);
    if (obj["color"].is<const char *>()) style.color = parseColor(obj["color"].as<String>(), style.color);
}

bool ThemeManager::readJson(const char *path, DynamicJsonDocument &doc)
{
    File file = SPIFFS.open(path, "r");
    if (!file)
    {
        Serial.printf("[主题] ❌ 打开配置失败: %s\n", path);
        return false;
    }

    auto err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.printf("[主题] ❌ 解析 JSON 失败: %s, 错误: %s\n", path, err.c_str());
        return false;
    }
    return true;
}

bool ThemeManager::loadThemeIndex()
{
    DynamicJsonDocument doc(2048);
    if (!readJson("/theme_config.json", doc))
        return false;

    _themeIndex.activeTheme = doc["activeTheme"] | "/themes/theme1.json";
    _themeIndex.themeCount = 0;

    JsonArray arr = doc["themes"].as<JsonArray>();
    for (JsonVariant v : arr)
    {
        if (_themeIndex.themeCount >= 8)
            break;
        _themeIndex.themes[_themeIndex.themeCount++] = v.as<String>();
    }

    if (_themeIndex.themeCount == 0)
    {
        _themeIndex.themes[0] = _themeIndex.activeTheme;
        _themeIndex.themeCount = 1;
    }

    _currentThemeIndex = 0;
    for (uint8_t i = 0; i < _themeIndex.themeCount; i++)
    {
        if (_themeIndex.themes[i] == _themeIndex.activeTheme)
        {
            _currentThemeIndex = i;
            break;
        }
    }

    Serial.printf("[主题] ✅ 主题索引加载完成, active=%s, count=%d\n", _themeIndex.activeTheme.c_str(), _themeIndex.themeCount);
    return true;
}

bool ThemeManager::loadTheme(const String &path)
{
    DynamicJsonDocument doc(4096);
    if (!readJson(path.c_str(), doc))
        return false;

    _theme = ThemeConfig();

    JsonObject background = doc["background"];
    if (!background.isNull())
    {
        _theme.backgroundColor = parseColor(background["color"] | "#0B1328", _theme.backgroundColor);
        _theme.backgroundImage = background["image"] | "";
    }

    JsonObject texts = doc["text"];
    if (!texts.isNull())
    {
        loadTextStyle(texts["time"], _theme.timeText);
        loadTextStyle(texts["date"], _theme.dateText);
        loadTextStyle(texts["temp"], _theme.tempText);
        loadTextStyle(texts["humidity"], _theme.humidText);
        loadTextStyle(texts["pressure"], _theme.pressureText);
        loadTextStyle(texts["alarm"], _theme.alarmText);
    }

    JsonObject modules = doc["modules"];
    if (!modules.isNull())
    {
        loadModuleStyle(modules["time"], _theme.timeModule);
        loadModuleStyle(modules["environment"], _theme.envModule);
        loadModuleStyle(modules["alarm"], _theme.alarmModule);
    }

    JsonObject images = doc["images"];
    if (!images.isNull())
    {
        _theme.weatherIcon = images["weatherIcon"] | _theme.weatherIcon;
        _theme.wifiIcon = images["wifiIcon"] | "";
        _theme.batteryIcon = images["batteryIcon"] | "";
    }

    Serial.printf("[主题] ✅ 主题配置加载成功: %s\n", path.c_str());
    return true;
}

void ThemeManager::saveThemeIndex()
{
    DynamicJsonDocument doc(2048);
    doc["activeTheme"] = _themeIndex.themes[_currentThemeIndex];
    JsonArray arr = doc.createNestedArray("themes");
    for (uint8_t i = 0; i < _themeIndex.themeCount; i++)
        arr.add(_themeIndex.themes[i]);

    File file = SPIFFS.open("/theme_config.json", "w");
    if (!file)
    {
        Serial.println("[主题] ❌ 保存主题索引失败");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();
}

void ThemeManager::setDefaultThemeData()
{
    _theme.timeText = {70, 64, 4, 0xFFFF, "14:30"};
    _theme.dateText = {70, 104, 2, rgbTo565(0xA7, 0xB2, 0xC7), "2026-10-24"};
    _theme.tempText = {20, 170, 2, 0xFFFF, "TEMP 26C"};
    _theme.humidText = {20, 194, 2, 0xFFFF, "HUM 45%"};
    _theme.pressureText = {20, 218, 2, 0xFFFF, "PRES 1013"};
    _theme.alarmText = {14, 276, 2, rgbTo565(0xF8, 0xD5, 0x74), "TODO 15:00"};

    _theme.timeModule = {52, 42, 138, 94, 210, rgbTo565(0x1F, 0x2B, 0x46)};
    _theme.envModule = {12, 152, 216, 84, 220, rgbTo565(0x25, 0x32, 0x50)};
    _theme.alarmModule = {8, 262, 224, 44, 210, rgbTo565(0x3A, 0x32, 0x20)};
}

bool ThemeManager::begin()
{
    setDefaultThemeData();

    if (!loadThemeIndex())
    {
        // 索引文件缺失时的容错回退
        Serial.println("[主题] ⚠️ 未找到 theme_config.json, 使用内置 6 主题列表");
        _themeIndex.themeCount = 6;
        _themeIndex.themes[0] = "/themes/theme1.json";
        _themeIndex.themes[1] = "/themes/theme2.json";
        _themeIndex.themes[2] = "/themes/theme3.json";
        _themeIndex.themes[3] = "/themes/theme4.json";
        _themeIndex.themes[4] = "/themes/theme5.json";
        _themeIndex.themes[5] = "/themes/theme6.json";
        _themeIndex.activeTheme = _themeIndex.themes[0];
        _currentThemeIndex = 0;
    }

    if (!loadTheme(_themeIndex.activeTheme))
    {
        Serial.println("[主题] ⚠️ 当前主题加载失败，使用默认样式");
        return false;
    }
    return true;
}

bool ThemeManager::switchToNextTheme()
{
    if (_themeIndex.themeCount == 0)
        return false;

    _currentThemeIndex = (_currentThemeIndex + 1) % _themeIndex.themeCount;
    _themeIndex.activeTheme = _themeIndex.themes[_currentThemeIndex];

    if (!loadTheme(_themeIndex.activeTheme))
        return false;

    saveThemeIndex();
    Serial.printf("[主题] 🔁 已切换到第 %d 套主题: %s\n", _currentThemeIndex + 1, _themeIndex.activeTheme.c_str());
    return true;
}

bool ThemeManager::reloadActiveTheme()
{
    if (!loadThemeIndex())
        return false;

    bool ok = loadTheme(_themeIndex.activeTheme);
    if (ok)
        Serial.println("[主题] ♻️ 已从 SPIFFS 重新加载主题");
    return ok;
}

void ThemeManager::tickMockClock()
{
    static uint8_t minute = 30;
    static uint8_t hour = 14;

    minute++;
    if (minute >= 60)
    {
        minute = 0;
        hour = (hour + 1) % 24;
    }

    char buff[6];
    snprintf(buff, sizeof(buff), "%02d:%02d", hour, minute);
    _theme.timeText.value = String(buff);
}
