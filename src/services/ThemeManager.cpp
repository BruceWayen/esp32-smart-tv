/**
 * @file ThemeManager.cpp
 * @brief 主题配置管理器实现
 * @version 1.0.0
 */

#include "ThemeManager.h"
#include "config/Config.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

ThemeManager& ThemeManager::getInstance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager()
    : _currentTheme()
    , _fsReady(false)
    , _callback(nullptr) {
}

bool ThemeManager::begin() {
    if (!LittleFS.begin(true)) {
        DEBUG_PRINTLN("[ThemeManager] ERROR: LittleFS mount failed");
        _fsReady = false;
        _currentTheme = buildDefaultTheme();
        return false;
    }

    _fsReady = true;

    if (!LittleFS.exists("/config")) {
        LittleFS.mkdir("/config");
    }

    if (!loadTheme()) {
        _currentTheme = buildDefaultTheme();
        saveTheme();
    }

    DEBUG_PRINTLN("[ThemeManager] Theme loaded");
    return true;
}

const ThemeConfig& ThemeManager::getTheme() const {
    return _currentTheme;
}

String ThemeManager::getThemeJson() const {
    DynamicJsonDocument doc(512);
    doc["id"] = _currentTheme.id;
    doc["name"] = _currentTheme.name;
    doc["showSensors"] = _currentTheme.showSensors;
    doc["wallpaper"] = _currentTheme.wallpaperPath;

    JsonObject colors = doc.createNestedObject("colors");
    colors["background"] = colorToHex(_currentTheme.backgroundColor);
    colors["primary"] = colorToHex(_currentTheme.primaryColor);
    colors["accent"] = colorToHex(_currentTheme.accentColor);
    colors["secondary"] = colorToHex(_currentTheme.secondaryColor);

    String output;
    serializeJsonPretty(doc, output);
    return output;
}

bool ThemeManager::updateThemeFromJson(const String& json, String& error) {
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        error = String("JSON解析失败: ") + err.c_str();
        return false;
    }

    ThemeConfig updated = _currentTheme;
    if (doc.containsKey("id")) {
        updated.id = doc["id"].as<uint8_t>();
    }
    if (doc.containsKey("name")) {
        updated.name = doc["name"].as<String>();
    }
    if (doc.containsKey("showSensors")) {
        updated.showSensors = doc["showSensors"].as<bool>();
    }
    if (doc.containsKey("wallpaper")) {
        updated.wallpaperPath = doc["wallpaper"].as<String>();
    }

    if (doc.containsKey("colors")) {
        JsonObject colors = doc["colors"];
        if (colors.containsKey("background")) {
            updated.backgroundColor = parseColor(colors["background"].as<String>(), updated.backgroundColor);
        }
        if (colors.containsKey("primary")) {
            updated.primaryColor = parseColor(colors["primary"].as<String>(), updated.primaryColor);
        }
        if (colors.containsKey("accent")) {
            updated.accentColor = parseColor(colors["accent"].as<String>(), updated.accentColor);
        }
        if (colors.containsKey("secondary")) {
            updated.secondaryColor = parseColor(colors["secondary"].as<String>(), updated.secondaryColor);
        }
    }

    _currentTheme = updated;

    if (!saveTheme()) {
        error = "保存主题配置失败";
        return false;
    }

    if (_callback) {
        _callback(_currentTheme);
    }

    return true;
}

void ThemeManager::setThemeChangeCallback(ThemeChangeCallback callback) {
    _callback = callback;
}

bool ThemeManager::loadTheme() {
    if (!_fsReady || !LittleFS.exists(THEME_CONFIG_FILE)) {
        return false;
    }

    File file = LittleFS.open(THEME_CONFIG_FILE, "r");
    if (!file) {
        return false;
    }

    String json = file.readString();
    file.close();

    String error;
    return updateThemeFromJson(json, error);
}

bool ThemeManager::saveTheme() {
    if (!_fsReady) {
        return false;
    }

    File file = LittleFS.open(THEME_CONFIG_FILE, "w");
    if (!file) {
        return false;
    }

    String json = getThemeJson();
    file.print(json);
    file.close();
    return true;
}

ThemeConfig ThemeManager::buildDefaultTheme() const {
    ThemeConfig theme;
    theme.id = DEFAULT_THEME_ID;
    theme.name = "Classic Dark";
    theme.backgroundColor = 0x0000;
    theme.primaryColor = 0x001F;
    theme.accentColor = 0x07E0;
    theme.secondaryColor = 0x39E7;
    theme.showSensors = true;
    theme.wallpaperPath = "/themes/theme_1.webp";
    return theme;
}

uint16_t ThemeManager::parseColor(const String& value, uint16_t fallback) const {
    if (value.length() == 0) {
        return fallback;
    }

    String color = value;
    color.replace("#", "");
    color.replace("0x", "");

    if (color.length() != 6) {
        return fallback;
    }

    uint32_t rgb = strtoul(color.c_str(), nullptr, 16);
    return parseColor(rgb, fallback);
}

uint16_t ThemeManager::parseColor(uint32_t value, uint16_t fallback) const {
    if (value > 0xFFFFFF) {
        return fallback;
    }

    uint8_t r = (value >> 16) & 0xFF;
    uint8_t g = (value >> 8) & 0xFF;
    uint8_t b = value & 0xFF;
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

String ThemeManager::colorToHex(uint16_t color) const {
    uint8_t r = (color >> 8) & 0xF8;
    uint8_t g = (color >> 3) & 0xFC;
    uint8_t b = (color << 3) & 0xF8;
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", r, g, b);
    return String(buffer);
}
