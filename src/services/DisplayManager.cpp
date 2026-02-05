/**
 * @file DisplayManager.cpp
 * @brief 显示管理器实现
 * @version 1.0.0
 */

#include "DisplayManager.h"
#include "config/Config.h"

DisplayManager& DisplayManager::getInstance() {
    static DisplayManager instance;
    return instance;
}

DisplayManager::DisplayManager()
    : _isActive(false)
    , _isAwake(false)
    , _lastActivityTime(0)
    , _lastUpdateTime(0)
    , _currentBrightness(BRIGHTNESS_MAX) {
}

bool DisplayManager::begin() {
    DEBUG_PRINTLN("[DisplayManager] Initializing...");
    
    // 初始化TFT屏幕
    if (!_tft.begin()) {
        DEBUG_PRINTLN("[DisplayManager] ERROR: TFT initialization failed!");
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

    // 绘制演示界面，验证屏幕点亮与显示
    drawDemoUI();
    
    DEBUG_PRINTLN("[DisplayManager] Initialization completed");
    
    return true;
}

void DisplayManager::end() {
    DEBUG_PRINTLN("[DisplayManager] Shutting down...");
    
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
        // TODO: 更新UI内容
    }
}

void DisplayManager::clear(uint16_t color) {
    if (!_isActive) return;
    
    _tft.fillScreen(color);
    resetScreenTimeout();
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
    
    DEBUG_PRINTF("[DisplayManager] Backlight set to %d\n", brightness);
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
        DEBUG_PRINTF("[DisplayManager] Auto brightness: %.0flux → %d\n", lightLevel, brightness);
    }
}

void DisplayManager::sleep() {
    if (!_isActive || !_isAwake) return;
    
    DEBUG_PRINTLN("[DisplayManager] Display entering sleep mode...");
    
    _tft.backlight(false);
    _tft.sleep();
    _isAwake = false;
}

void DisplayManager::wakeup() {
    if (!_isActive || _isAwake) return;
    
    DEBUG_PRINTLN("[DisplayManager] Display waking up...");
    
    _tft.wakeup();
    _tft.setBacklight(_currentBrightness);
    _isAwake = true;
    _lastActivityTime = millis();
}

void DisplayManager::setRotation(uint8_t rotation) {
    if (!_isActive) return;
    
    _tft.setRotation(rotation % 4);
    
    DEBUG_PRINTF("[DisplayManager] Screen rotation set to %d\n", rotation);
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
    
    // 背景填充为黑色
    _tft.fillScreen(COLOR_BLACK);

    // 绘制三色测试条
    uint16_t barWidth = _tft.width() / 3;
    _tft.fillRect(0, 0, barWidth, _tft.height(), COLOR_RED);
    _tft.fillRect(barWidth, 0, barWidth, _tft.height(), COLOR_GREEN);
    _tft.fillRect(barWidth * 2, 0, _tft.width() - barWidth * 2, _tft.height(), COLOR_BLUE);

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
