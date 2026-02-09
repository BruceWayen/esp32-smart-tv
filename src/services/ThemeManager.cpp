/**
 * @file ThemeManager.cpp
 * @brief 主题配置管理器实现
 * @version 1.0.0
 */

#include "ThemeManager.h"
#include "config/Config.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

ThemeManager& ThemeManager::getInstance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager()
    : _currentTheme()
    , _themeCount(0)
    , _fsReady(false)
    , _callback(nullptr) {
}

bool ThemeManager::begin() {
    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("[ThemeManager] 错误：SPIFFS挂载失败");
        _fsReady = false;
        _currentTheme = buildDefaultTheme();
        return false;
    }

    _fsReady = true;

    if (!SPIFFS.exists("/config")) {
        SPIFFS.mkdir("/config");
    }

    if (!loadThemeList()) {
        buildDefaultThemeList();
        saveThemeList();
    }

    if (!loadTheme()) {
        _currentTheme = buildDefaultTheme();
        saveTheme();
    }

    DEBUG_PRINTLN("[ThemeManager] 主题配置加载完成");
    return true;
}

const ThemeConfig& ThemeManager::getTheme() const {
    return _currentTheme;
}

size_t ThemeManager::getThemeCount() const {
    return _themeCount;
}

const ThemeConfig& ThemeManager::getThemeByIndex(size_t index) const {
    if (_themeCount == 0) {
        return _currentTheme;
    }
    size_t safeIndex = index % _themeCount;
    return _themeList[safeIndex];
}

String ThemeManager::getThemeJson() const {
    DynamicJsonDocument doc(1536);
    doc["id"] = _currentTheme.id;
    doc["name"] = _currentTheme.name;
    doc["showSensors"] = _currentTheme.showSensors;
    doc["fontSize"] = _currentTheme.fontSize;
    doc["wallpaper"] = _currentTheme.wallpaperPath;
    doc["iconPath"] = _currentTheme.iconPath;

    JsonObject colors = doc.createNestedObject("colors");
    colors["background"] = colorToHex(_currentTheme.backgroundColor);
    colors["primary"] = colorToHex(_currentTheme.primaryColor);
    colors["accent"] = colorToHex(_currentTheme.accentColor);
    colors["secondary"] = colorToHex(_currentTheme.secondaryColor);

    JsonObject layout = doc.createNestedObject("layout");
    auto fillRect = [&](const char* key, const ThemeRect& rect) {
        JsonObject item = layout.createNestedObject(key);
        item["x"] = rect.x;
        item["y"] = rect.y;
        item["w"] = rect.w;
        item["h"] = rect.h;
    };
    fillRect("header", _currentTheme.layout.header);
    fillRect("clock", _currentTheme.layout.clock);
    fillRect("weather", _currentTheme.layout.weather);
    fillRect("sensors", _currentTheme.layout.sensors);
    fillRect("footer", _currentTheme.layout.footer);

    String output;
    serializeJsonPretty(doc, output);
    return output;
}

bool ThemeManager::updateThemeFromJson(const String& json, String& error) {
    DynamicJsonDocument doc(1536);
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
    if (doc.containsKey("fontSize")) {
        updated.fontSize = doc["fontSize"].as<uint8_t>();
    }
    if (doc.containsKey("wallpaper")) {
        updated.wallpaperPath = doc["wallpaper"].as<String>();
    }
    if (doc.containsKey("iconPath")) {
        updated.iconPath = doc["iconPath"].as<String>();
    }
    if (updated.fontSize == 0) {
        updated.fontSize = 2;
    } else if (updated.fontSize > 4) {
        updated.fontSize = 4;
    }

    if (doc.containsKey("layout")) {
        JsonObject layout = doc["layout"];
        if (layout.containsKey("header")) {
            updated.layout.header = parseRect(layout["header"], updated.layout.header);
        }
        if (layout.containsKey("clock")) {
            updated.layout.clock = parseRect(layout["clock"], updated.layout.clock);
        }
        if (layout.containsKey("weather")) {
            updated.layout.weather = parseRect(layout["weather"], updated.layout.weather);
        }
        if (layout.containsKey("sensors")) {
            updated.layout.sensors = parseRect(layout["sensors"], updated.layout.sensors);
        }
        if (layout.containsKey("footer")) {
            updated.layout.footer = parseRect(layout["footer"], updated.layout.footer);
        }
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

bool ThemeManager::nextTheme() {
    if (_themeCount == 0) {
        return false;
    }
    size_t nextIndex = 0;
    for (size_t i = 0; i < _themeCount; i++) {
        if (_themeList[i].id == _currentTheme.id) {
            nextIndex = (i + 1) % _themeCount;
            break;
        }
    }
    _currentTheme = _themeList[nextIndex];
    bool saved = saveTheme();
    if (saved && _callback) {
        _callback(_currentTheme);
    }
    return saved;
}

bool ThemeManager::resetToDefaultTheme() {
    _currentTheme = buildDefaultTheme();
    bool saved = saveTheme();
    if (saved && _callback) {
        _callback(_currentTheme);
    }
    return saved;
}

bool ThemeManager::reloadThemeFromFile() {
    bool loaded = loadTheme();
    if (loaded && _callback) {
        _callback(_currentTheme);
    }
    return loaded;
}

bool ThemeManager::previousTheme() {
    if (_themeCount == 0) {
        return false;
    }
    size_t prevIndex = 0;
    for (size_t i = 0; i < _themeCount; i++) {
        if (_themeList[i].id == _currentTheme.id) {
            prevIndex = (i + _themeCount - 1) % _themeCount;
            break;
        }
    }
    _currentTheme = _themeList[prevIndex];
    bool saved = saveTheme();
    if (saved && _callback) {
        _callback(_currentTheme);
    }
    return saved;
}

bool ThemeManager::setThemeById(uint8_t id) {
    for (size_t i = 0; i < _themeCount; i++) {
        if (_themeList[i].id == id) {
            _currentTheme = _themeList[i];
            bool saved = saveTheme();
            if (saved && _callback) {
                _callback(_currentTheme);
            }
            return saved;
        }
    }
    return false;
}

void ThemeManager::setThemeChangeCallback(ThemeChangeCallback callback) {
    _callback = callback;
}

bool ThemeManager::loadTheme() {
    if (!_fsReady || !SPIFFS.exists(THEME_CONFIG_FILE)) {
        return false;
    }

    File file = SPIFFS.open(THEME_CONFIG_FILE, "r");
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

    File file = SPIFFS.open(THEME_CONFIG_FILE, "w");
    if (!file) {
        return false;
    }

    String json = getThemeJson();
    file.print(json);
    file.close();
    return true;
}

bool ThemeManager::loadThemeList() {
    if (!_fsReady || !SPIFFS.exists(THEMES_CONFIG_FILE)) {
        return false;
    }

    File file = SPIFFS.open(THEMES_CONFIG_FILE, "r");
    if (!file) {
        return false;
    }

    String json = file.readString();
    file.close();

    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        DEBUG_PRINTF("[ThemeManager] 主题列表解析失败: %s\n", err.c_str());
        return false;
    }

    JsonArray list = doc["themes"].as<JsonArray>();
    if (list.isNull()) {
        return false;
    }

    _themeCount = 0;
    for (JsonVariant item : list) {
        if (_themeCount >= MAX_THEMES) {
            break;
        }
        ThemeConfig theme = buildDefaultTheme();
        theme.id = item["id"] | theme.id;
        theme.name = item["name"].as<String>();
        if (theme.name.length() == 0) {
            theme.name = String("Theme-") + theme.id;
        }
        theme.showSensors = item["showSensors"] | theme.showSensors;
        theme.fontSize = item["fontSize"] | theme.fontSize;
        theme.wallpaperPath = item["wallpaper"].as<String>();
        if (theme.wallpaperPath.length() == 0) {
            theme.wallpaperPath = "/themes/theme_1.webp";
        }
        theme.iconPath = item["iconPath"].as<String>();
        if (theme.iconPath.length() == 0) {
            theme.iconPath = "/icons/weather/{code}.bmp";
        }

        if (item.containsKey("colors")) {
            JsonObject colors = item["colors"];
            theme.backgroundColor = parseColor(colors["background"].as<String>(), theme.backgroundColor);
            theme.primaryColor = parseColor(colors["primary"].as<String>(), theme.primaryColor);
            theme.accentColor = parseColor(colors["accent"].as<String>(), theme.accentColor);
            theme.secondaryColor = parseColor(colors["secondary"].as<String>(), theme.secondaryColor);
        }

        if (item.containsKey("layout")) {
            JsonObject layout = item["layout"];
            theme.layout.header = parseRect(layout["header"], theme.layout.header);
            theme.layout.clock = parseRect(layout["clock"], theme.layout.clock);
            theme.layout.weather = parseRect(layout["weather"], theme.layout.weather);
            theme.layout.sensors = parseRect(layout["sensors"], theme.layout.sensors);
            theme.layout.footer = parseRect(layout["footer"], theme.layout.footer);
        }

        _themeList[_themeCount++] = theme;
    }

    if (_themeCount == 0) {
        return false;
    }

    DEBUG_PRINTF("[ThemeManager] 主题列表加载完成，共%d套\n", _themeCount);
    return true;
}

bool ThemeManager::saveThemeList() {
    if (!_fsReady) {
        return false;
    }

    DynamicJsonDocument doc(4096);
    JsonArray list = doc.createNestedArray("themes");
    for (size_t i = 0; i < _themeCount; i++) {
        const ThemeConfig& theme = _themeList[i];
        JsonObject item = list.createNestedObject();
        item["id"] = theme.id;
        item["name"] = theme.name;
        item["showSensors"] = theme.showSensors;
        item["fontSize"] = theme.fontSize;
        item["wallpaper"] = theme.wallpaperPath;
        item["iconPath"] = theme.iconPath;

        JsonObject colors = item.createNestedObject("colors");
        colors["background"] = colorToHex(theme.backgroundColor);
        colors["primary"] = colorToHex(theme.primaryColor);
        colors["accent"] = colorToHex(theme.accentColor);
        colors["secondary"] = colorToHex(theme.secondaryColor);

        JsonObject layout = item.createNestedObject("layout");
        auto writeRect = [&](const char* key, const ThemeRect& rect) {
            JsonObject rectObj = layout.createNestedObject(key);
            rectObj["x"] = rect.x;
            rectObj["y"] = rect.y;
            rectObj["w"] = rect.w;
            rectObj["h"] = rect.h;
        };
        writeRect("header", theme.layout.header);
        writeRect("clock", theme.layout.clock);
        writeRect("weather", theme.layout.weather);
        writeRect("sensors", theme.layout.sensors);
        writeRect("footer", theme.layout.footer);
    }

    File file = SPIFFS.open(THEMES_CONFIG_FILE, "w");
    if (!file) {
        return false;
    }

    String output;
    serializeJsonPretty(doc, output);
    file.print(output);
    file.close();
    return true;
}

ThemeConfig ThemeManager::buildDefaultTheme() const {
    ThemeConfig theme;
    theme.id = DEFAULT_THEME_ID;
    theme.name = "Landscape Custom";
    theme.backgroundColor = 0x0A12;
    theme.primaryColor = 0x1B1C;
    theme.accentColor = 0xFD20;
    theme.secondaryColor = 0x4A49;
    theme.showSensors = true;
    theme.fontSize = 3;
    theme.wallpaperPath = "/themes/theme_1.webp";
    theme.iconPath = "/icons/weather/{code}.bmp";
    theme.layout.header = ThemeRect(0, 0, TFT_WIDTH, 28);
    theme.layout.clock = ThemeRect(20, 36, 170, 60);
    theme.layout.weather = ThemeRect(200, 36, 108, 80);
    theme.layout.sensors = ThemeRect(12, 126, 296, 72);
    theme.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    return theme;
}

void ThemeManager::buildDefaultThemeList() {
    _themeCount = 0;

    ThemeConfig theme1 = buildDefaultTheme();
    theme1.id = 1;
    theme1.name = "Landscape Custom";
    theme1.wallpaperPath = "/themes/theme_1.webp";
    _themeList[_themeCount++] = theme1;

    ThemeConfig theme2 = buildDefaultTheme();
    theme2.id = 2;
    theme2.name = "Aurora Blue";
    theme2.backgroundColor = 0x0011;
    theme2.primaryColor = 0x0A5F;
    theme2.accentColor = 0x07FF;
    theme2.secondaryColor = 0x5AEB;
    theme2.fontSize = 3;
    theme2.wallpaperPath = "/themes/theme_2.webp";
    theme2.iconPath = "/icons/weather/{code}.bmp";
    theme2.layout.header = ThemeRect(0, 0, TFT_WIDTH, 30);
    theme2.layout.clock = ThemeRect(18, 34, 172, 60);
    theme2.layout.weather = ThemeRect(198, 34, 110, 80);
    theme2.layout.sensors = ThemeRect(14, 126, 296, 72);
    theme2.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    _themeList[_themeCount++] = theme2;

    ThemeConfig theme3 = buildDefaultTheme();
    theme3.id = 3;
    theme3.name = "Sunset";
    theme3.backgroundColor = 0x2004;
    theme3.primaryColor = 0xF960;
    theme3.accentColor = 0xFD20;
    theme3.secondaryColor = 0x49C8;
    theme3.fontSize = 3;
    theme3.wallpaperPath = "/themes/theme_3.webp";
    theme3.iconPath = "/icons/weather/{code}.bmp";
    theme3.layout.header = ThemeRect(0, 0, TFT_WIDTH, 28);
    theme3.layout.clock = ThemeRect(20, 36, 170, 60);
    theme3.layout.weather = ThemeRect(200, 36, 108, 80);
    theme3.layout.sensors = ThemeRect(12, 126, 296, 72);
    theme3.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    _themeList[_themeCount++] = theme3;

    ThemeConfig theme4 = buildDefaultTheme();
    theme4.id = 4;
    theme4.name = "Forest";
    theme4.backgroundColor = 0x0200;
    theme4.primaryColor = 0x03E0;
    theme4.accentColor = 0x7FE0;
    theme4.secondaryColor = 0x2A69;
    theme4.fontSize = 3;
    theme4.wallpaperPath = "/themes/theme_4.webp";
    theme4.iconPath = "/icons/weather/{code}.bmp";
    theme4.layout.header = ThemeRect(0, 0, TFT_WIDTH, 28);
    theme4.layout.clock = ThemeRect(20, 36, 170, 60);
    theme4.layout.weather = ThemeRect(200, 36, 108, 80);
    theme4.layout.sensors = ThemeRect(12, 126, 296, 72);
    theme4.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    _themeList[_themeCount++] = theme4;

    ThemeConfig theme5 = buildDefaultTheme();
    theme5.id = 5;
    theme5.name = "Minimal Light";
    theme5.backgroundColor = 0xFFFF;
    theme5.primaryColor = 0xC618;
    theme5.accentColor = 0x001F;
    theme5.secondaryColor = 0x7BEF;
    theme5.fontSize = 3;
    theme5.wallpaperPath = "/themes/theme_5.webp";
    theme5.iconPath = "/icons/weather/{code}.bmp";
    theme5.layout.header = ThemeRect(0, 0, TFT_WIDTH, 28);
    theme5.layout.clock = ThemeRect(20, 36, 170, 60);
    theme5.layout.weather = ThemeRect(200, 36, 108, 80);
    theme5.layout.sensors = ThemeRect(12, 126, 296, 72);
    theme5.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    _themeList[_themeCount++] = theme5;

    ThemeConfig theme6 = buildDefaultTheme();
    theme6.id = 6;
    theme6.name = "Purple Night";
    theme6.backgroundColor = 0x1008;
    theme6.primaryColor = 0x780F;
    theme6.accentColor = 0xF81F;
    theme6.secondaryColor = 0x318C;
    theme6.fontSize = 3;
    theme6.wallpaperPath = "/themes/theme_6.webp";
    theme6.iconPath = "/icons/weather/{code}.bmp";
    theme6.layout.header = ThemeRect(0, 0, TFT_WIDTH, 28);
    theme6.layout.clock = ThemeRect(20, 36, 170, 60);
    theme6.layout.weather = ThemeRect(200, 36, 108, 80);
    theme6.layout.sensors = ThemeRect(12, 126, 296, 72);
    theme6.layout.footer = ThemeRect(0, 210, TFT_WIDTH, 30);
    _themeList[_themeCount++] = theme6;
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

ThemeRect ThemeManager::parseRect(const JsonVariant& value, const ThemeRect& fallback) const {
    if (value.isNull()) {
        return fallback;
    }
    ThemeRect rect = fallback;
    rect.x = value["x"] | rect.x;
    rect.y = value["y"] | rect.y;
    rect.w = value["w"] | rect.w;
    rect.h = value["h"] | rect.h;
    return rect;
}
