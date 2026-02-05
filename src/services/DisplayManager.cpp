/**
 * @file DisplayManager.cpp
 * @brief 显示管理器实现
 * @version 1.0.0
 */

#include "DisplayManager.h"
#include "config/Config.h"
#include "services/WeatherIconManager.h"

DisplayManager& DisplayManager::getInstance() {
    static DisplayManager instance;
    return instance;
}

DisplayManager::DisplayManager()
    : _isActive(false)
    , _isAwake(false)
    , _lastActivityTime(0)
    , _lastUpdateTime(0)
    , _currentBrightness(BRIGHTNESS_MAX)
    , _needsRedraw(false)
    , _theme(ThemeManager::getInstance().getTheme())
    , _hasEnvData(false)
    , _hasWeatherData(false)
    , _hasClockData(false) {
}

bool DisplayManager::begin() {
    DEBUG_PRINTLN("[DisplayManager] 正在初始化显示管理器...");
    
    // 初始化TFT屏幕
    if (!_tft.begin()) {
        DEBUG_PRINTLN("[DisplayManager] 错误：TFT屏幕初始化失败");
        _isActive = false;
        return false;
    }
    
    // 设置旋转角度
    _tft.setRotation(TFT_ROTATION);
    
    // 清屏为黑色
    _tft.fillScreen(COLOR_BLACK);
    
    // 设置初始背光
    _tft.setBacklight(BRIGHTNESS_MAX);
    _currentBrightness = BRIGHTNESS_MAX;
    
    // 标记为活跃
    _isActive = true;
    _isAwake = true;
    _lastActivityTime = millis();
    _lastUpdateTime = millis();
    _needsRedraw = true;
    
    DEBUG_PRINTLN("[DisplayManager] 显示管理器初始化完成");
    
    return true;
}

void DisplayManager::end() {
    DEBUG_PRINTLN("[DisplayManager] 正在关闭显示管理器...");
    
    _tft.end();
    _isActive = false;
    _isAwake = false;
}

void DisplayManager::update() {
    if (!_isActive) return;
    
    uint32_t currentTime = millis();
    
    // 检查息屏超时
    checkScreenTimeout();
    
    // 定期更新显示（控制刷新率）
    if (currentTime - _lastUpdateTime >= 50) {  // 20fps
        _lastUpdateTime = currentTime;
        if (_needsRedraw) {
            drawMainUI();
            _needsRedraw = false;
        }
    }
}

void DisplayManager::clear(uint16_t color) {
    if (!_isActive) return;
    
    _tft.fillScreen(color);
    resetScreenTimeout();
}

void DisplayManager::updateEnvironmentData(const EnvironmentData& data) {
    _envData = data;
    _hasEnvData = true;
    _needsRedraw = true;
}

void DisplayManager::applyTheme(const ThemeConfig& theme) {
    _theme = theme;
    _needsRedraw = true;
}

void DisplayManager::updateWeatherData(const WeatherInfo& info) {
    _weatherData = info;
    _hasWeatherData = true;
    _needsRedraw = true;
}

void DisplayManager::updateClockData(const ClockInfo& info) {
    _clockData = info;
    _hasClockData = true;
    _needsRedraw = true;
}

void DisplayManager::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (!_isActive || !_isAwake) return;
    
    _tft.drawPixel(x, y, color);
    resetScreenTimeout();
}

void DisplayManager::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if (!_isActive || !_isAwake) return;
    
    _tft.fillRect(x, y, width, height, color);
    resetScreenTimeout();
}

void DisplayManager::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if (!_isActive || !_isAwake) return;
    
    _tft.drawRect(x, y, width, height, color);
    resetScreenTimeout();
}

void DisplayManager::drawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
    if (!_isActive || !_isAwake) return;
    
    _tft.drawHLine(x, y, length, color);
    resetScreenTimeout();
}

void DisplayManager::drawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
    if (!_isActive || !_isAwake) return;
    
    _tft.drawVLine(x, y, length, color);
    resetScreenTimeout();
}

void DisplayManager::setBacklight(uint8_t brightness) {
    if (!_isActive) return;
    
    _currentBrightness = brightness;
    _tft.setBacklight(brightness);
    
    DEBUG_PRINTF("[DisplayManager] 背光亮度设置为 %d\n", brightness);
}

void DisplayManager::autoAdjustBrightness(float lightLevel) {
    if (!_isActive || !_isAwake) return;
    
    uint8_t brightness;
    
    // 根据光照强度自动调整亮度
    if (lightLevel > LIGHT_LEVEL_VERY_BRIGHT) {
        brightness = BRIGHTNESS_MAX;
    } else if (lightLevel > LIGHT_LEVEL_BRIGHT) {
        brightness = BRIGHTNESS_HIGH;
    } else if (lightLevel > LIGHT_LEVEL_NORMAL) {
        brightness = BRIGHTNESS_MEDIUM;
    } else if (lightLevel > LIGHT_LEVEL_DIM) {
        brightness = BRIGHTNESS_LOW;
    } else {
        brightness = BRIGHTNESS_OFF;
    }
    
    // 只在亮度变化时才更新
    if (brightness != _currentBrightness) {
        setBacklight(brightness);
        DEBUG_PRINTF("[DisplayManager] 自动亮度: %.0flux → %d\n", lightLevel, brightness);
    }
}

void DisplayManager::sleep() {
    if (!_isActive || !_isAwake) return;
    
    DEBUG_PRINTLN("[DisplayManager] 屏幕进入休眠模式...");
    
    _tft.backlight(false);
    _tft.sleep();
    _isAwake = false;
}

void DisplayManager::wakeup() {
    if (!_isActive || _isAwake) return;
    
    DEBUG_PRINTLN("[DisplayManager] 屏幕唤醒中...");
    
    _tft.wakeup();
    _tft.setBacklight(_currentBrightness);
    _isAwake = true;
    _lastActivityTime = millis();
}

void DisplayManager::setRotation(uint8_t rotation) {
    if (!_isActive) return;
    
    _tft.setRotation(rotation % 4);
    
    DEBUG_PRINTF("[DisplayManager] 屏幕旋转角度设置为 %d\n", rotation);
}

uint16_t DisplayManager::width() const {
    return _tft.width();
}

uint16_t DisplayManager::height() const {
    return _tft.height();
}

void DisplayManager::resetScreenTimeout() {
    _lastActivityTime = millis();
    
    // 如果屏幕在睡眠，唤醒它
    if (!_isAwake) {
        wakeup();
    }
}

void DisplayManager::checkScreenTimeout() {
    if (!_isActive || !_isAwake) return;
    
    uint32_t idleTime = millis() - _lastActivityTime;
    
    // 检查息屏超时
    if (idleTime > SCREEN_TIMEOUT_MS) {
        sleep();
    }
}

void DisplayManager::drawDemoUI() {
    if (!_isActive || !_isAwake) return;
    
    // 绘制标题栏
    _tft.fillRect(0, 0, _tft.width(), 30, COLOR_BLUE);
    
    // 绘制状态栏
    _tft.fillRect(0, _tft.height() - 20, _tft.width(), 20, COLOR_GRAY);
    
    // 绘制中心演示
    uint16_t centerX = _tft.width() / 2;
    uint16_t centerY = _tft.height() / 2;
    
    // 绘制矩形
    _tft.drawRect(centerX - 50, centerY - 40, 100, 80, COLOR_GREEN);
    
    // 绘制十字线
    _tft.drawHLine(centerX - 30, centerY, 60, COLOR_RED);
    _tft.drawVLine(centerX, centerY - 30, 60, COLOR_RED);
}

void DisplayManager::drawMainUI() {
    if (!_isActive || !_isAwake) return;

    _tft.fillScreen(_theme.backgroundColor);

    ThemeLayout layout = _theme.layout;
    if (layout.header.w == 0 || layout.header.h == 0) {
        layout.header = ThemeRect(0, 0, _tft.width(), 36);
        layout.clock = ThemeRect(12, 6, 150, 24);
        layout.weather = ThemeRect(_tft.width() - 120, 42, 110, 80);
        layout.sensors = ThemeRect(12, 58, _tft.width() - 150, 130);
        layout.footer = ThemeRect(0, static_cast<uint16_t>(_tft.height() - 30), _tft.width(), 30);
    }

    _tft.fillRect(layout.header.x, layout.header.y, layout.header.w, layout.header.h, _theme.primaryColor);
    _tft.drawHLine(layout.header.x, layout.header.y + layout.header.h, layout.header.w, _theme.accentColor);

    drawClockSection(layout);
    drawWeatherSection(layout);
    drawSensorSection(layout);
    drawFooterSection(layout);
}

void DisplayManager::drawEnvironmentBars(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) {
    if (height < 60) return;

    uint16_t barWidth = width - 20;
    uint16_t barHeight = (height - 20) / 3;
    uint16_t gap = 6;

    auto drawBar = [&](uint16_t y, float value, float minValue, float maxValue) {
        float clamped = value;
        if (clamped < minValue) clamped = minValue;
        if (clamped > maxValue) clamped = maxValue;
        float ratio = (clamped - minValue) / (maxValue - minValue);
        uint16_t filled = static_cast<uint16_t>(barWidth * ratio);

        _tft.drawRect(startX, y, barWidth, barHeight, _theme.secondaryColor);
        _tft.fillRect(startX + 2, y + 2, filled > 4 ? filled - 4 : 0, barHeight - 4, _theme.accentColor);
    };

    drawBar(startY, _envData.temperature, TEMP_MIN_VALID, TEMP_MAX_VALID);
    drawBar(startY + barHeight + gap, _envData.humidity, HUMI_MIN_VALID, HUMI_MAX_VALID);
    drawBar(startY + (barHeight + gap) * 2, _envData.pressure, PRESS_MIN_VALID, PRESS_MAX_VALID);
}

void DisplayManager::drawClockSection(const ThemeLayout& layout) {
    String timeText = "--:--";
    String dateText = "----/--/--";
    if (_hasClockData) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", _clockData.hour, _clockData.minute);
        timeText = buffer;
        dateText = _clockData.dateText;
    }

    uint16_t timeColor = _theme.accentColor;
    uint16_t dateColor = _theme.secondaryColor;
    uint8_t timeSize = 2;

    uint16_t timeWidth = _tft.measureTextWidth(timeText, timeSize);
    uint16_t timeX = layout.clock.x;
    uint16_t timeY = layout.clock.y;
    if (timeWidth < layout.clock.w) {
        timeX = layout.clock.x + (layout.clock.w - timeWidth) / 2;
    }
    _tft.drawText(timeX, timeY, timeText, timeColor, _theme.primaryColor, timeSize);

    uint16_t dateY = timeY + 18;
    _tft.drawText(layout.clock.x, dateY, dateText, dateColor, _theme.primaryColor, 1);
}

void DisplayManager::drawWeatherSection(const ThemeLayout& layout) {
    uint16_t weatherX = layout.weather.x;
    uint16_t weatherY = layout.weather.y;
    uint16_t weatherW = layout.weather.w;
    uint16_t weatherH = layout.weather.h;

    _tft.drawRect(weatherX, weatherY, weatherW, weatherH, _theme.secondaryColor);

    String condition = _hasWeatherData ? _weatherData.condition : "未知";
    String tempText = _hasWeatherData ? String(_weatherData.temperature, 1) + "C" : "--.-C";
    String windText = _hasWeatherData ? _weatherData.windDir + " " + String(_weatherData.windSpeed, 1) + "m/s" : "风速--";

    uint16_t iconSize = (weatherH > 50) ? 32 : 24;
    drawWeatherIcon(weatherX + 6, weatherY + 6, iconSize, _hasWeatherData ? _weatherData.iconKey : "unknown");

    uint16_t textX = weatherX + iconSize + 12;
    _tft.drawText(textX, weatherY + 6, tempText, _theme.accentColor, _theme.backgroundColor, 1);
    _tft.drawText(textX, weatherY + 20, condition, _theme.secondaryColor, _theme.backgroundColor, 1);
    _tft.drawText(textX, weatherY + 34, windText, _theme.secondaryColor, _theme.backgroundColor, 1);
    drawWindIndicator(weatherX + weatherW - 20, weatherY + weatherH - 20, 12, _hasWeatherData ? _weatherData.windDir : "无风");
}

void DisplayManager::drawSensorSection(const ThemeLayout& layout) {
    if (!_theme.showSensors) {
        return;
    }

    _tft.drawRect(layout.sensors.x, layout.sensors.y, layout.sensors.w, layout.sensors.h, _theme.secondaryColor);

    if (!_hasEnvData) {
        drawCenteredText(layout.sensors.x, layout.sensors.y, layout.sensors.w, layout.sensors.h, "传感器未就绪", _theme.secondaryColor, 1);
        return;
    }

    uint16_t barStartX = layout.sensors.x + 8;
    uint16_t barStartY = layout.sensors.y + 22;
    uint16_t barWidth = layout.sensors.w - 16;
    uint16_t barHeight = layout.sensors.h - 30;

    _tft.drawText(layout.sensors.x + 8, layout.sensors.y + 4, "环境数据", _theme.accentColor, _theme.backgroundColor, 1);
    drawEnvironmentBars(barStartX, barStartY, barWidth, barHeight);

    String tempText = String("T ") + String(_envData.temperature, 1) + "C";
    String humiText = String("H ") + String(_envData.humidity, 1) + "%";
    String pressText = String("P ") + String(_envData.pressure, 1) + "hPa";
    _tft.drawText(barStartX, layout.sensors.y + layout.sensors.h - 14, tempText, _theme.secondaryColor, _theme.backgroundColor, 1);
    _tft.drawText(barStartX + 70, layout.sensors.y + layout.sensors.h - 14, humiText, _theme.secondaryColor, _theme.backgroundColor, 1);
    _tft.drawText(barStartX + 140, layout.sensors.y + layout.sensors.h - 14, pressText, _theme.secondaryColor, _theme.backgroundColor, 1);
}

void DisplayManager::drawFooterSection(const ThemeLayout& layout) {
    _tft.fillRect(layout.footer.x, layout.footer.y, layout.footer.w, layout.footer.h, _theme.secondaryColor);
    _tft.drawHLine(layout.footer.x, layout.footer.y, layout.footer.w, _theme.accentColor);

    String leftText = String("主题: ") + _theme.name;
    _tft.drawText(layout.footer.x + 6, layout.footer.y + 6, leftText, _theme.backgroundColor, _theme.secondaryColor, 1);

    String rightText = _theme.wallpaperPath;
    uint16_t textWidth = _tft.measureTextWidth(rightText, 1);
    if (textWidth + 6 < layout.footer.w) {
        _tft.drawText(layout.footer.x + layout.footer.w - textWidth - 6, layout.footer.y + 6, rightText, _theme.backgroundColor, _theme.secondaryColor, 1);
    }
}

void DisplayManager::drawWeatherIcon(uint16_t x, uint16_t y, uint16_t size, const String& iconKey) {
    WeatherIconStyle style = WeatherIconManager::getInstance().getStyle(iconKey);
    uint16_t color = style.color;
    uint16_t centerX = x + size / 2;
    uint16_t centerY = y + size / 2;

    if (style.type == "sun") {
        _tft.fillRect(centerX - 6, centerY - 6, 12, 12, color);
        _tft.drawHLine(centerX - 10, centerY, 20, color);
        _tft.drawVLine(centerX, centerY - 10, 20, color);
    } else if (style.type == "cloud") {
        _tft.fillRect(x + 2, y + size / 2, size - 4, size / 3, color);
        _tft.fillRect(x + 6, y + size / 3, size / 2, size / 3, color);
    } else if (style.type == "rain") {
        _tft.fillRect(x + 2, y + size / 2, size - 4, size / 3, color);
        for (uint16_t i = 0; i < 3; i++) {
            _tft.drawVLine(x + 6 + i * 6, y + size / 2 + 8, 6, color);
        }
    } else if (style.type == "snow") {
        _tft.drawRect(x + 4, y + 4, size - 8, size - 8, color);
        _tft.drawHLine(x + 4, centerY, size - 8, color);
        _tft.drawVLine(centerX, y + 4, size - 8, color);
    } else {
        _tft.drawRect(x + 2, y + 2, size - 4, size - 4, color);
    }
}

void DisplayManager::drawWindIndicator(uint16_t x, uint16_t y, uint16_t size, const String& direction) {
    uint16_t color = _theme.accentColor;
    uint16_t half = size / 2;
    _tft.drawRect(x - half, y - half, size, size, color);

    if (direction.indexOf("北") >= 0) {
        _tft.drawVLine(x, y - half + 2, half, color);
    } else if (direction.indexOf("南") >= 0) {
        _tft.drawVLine(x, y, half - 2, color);
    } else if (direction.indexOf("东") >= 0) {
        _tft.drawHLine(x, y, half - 2, color);
    } else if (direction.indexOf("西") >= 0) {
        _tft.drawHLine(x - half + 2, y, half, color);
    }
}

void DisplayManager::drawCenteredText(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const String& text, uint16_t color, uint8_t size) {
    uint16_t textWidth = _tft.measureTextWidth(text, size);
    uint16_t textHeight = 8 * size;
    uint16_t startX = x + (width > textWidth ? (width - textWidth) / 2 : 0);
    uint16_t startY = y + (height > textHeight ? (height - textHeight) / 2 : 0);
    _tft.drawText(startX, startY, text, color, _theme.backgroundColor, size);
}
