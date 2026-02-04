/**
 * @file DisplayManager.h
 * @brief 显示管理器 - 统一管理屏幕显示
 * @version 1.0.0
 * @date 2026-02-04
 * 
 * 负责屏幕的初始化、UI更新、自动调光等功能
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "display/TFTDisplay.h"
#include "services/SensorManager.h"
#include "services/ThemeManager.h"

/**
 * @brief 显示管理器类
 */
class DisplayManager {
public:
    /**
     * @brief 获取单例实例
     * @return DisplayManager& 单例引用
     */
    static DisplayManager& getInstance();
    
    /**
     * @brief 初始化显示系统
     * @return bool true成功, false失败
     */
    bool begin();
    
    /**
     * @brief 关闭显示系统
     */
    void end();
    
    /**
     * @brief 更新显示（在任务中调用）
     */
    void update();

    /**
     * @brief 更新环境数据
     * @param data 环境数据
     */
    void updateEnvironmentData(const EnvironmentData& data);

    /**
     * @brief 应用主题配置
     * @param theme 主题配置
     */
    void applyTheme(const ThemeConfig& theme);
    
    /**
     * @brief 清屏
     * @param color 背景颜色(RGB565)
     */
    void clear(uint16_t color = COLOR_BLACK);
    
    /**
     * @brief 绘制像素
     * @param x 列坐标
     * @param y 行坐标
     * @param color 颜色值(RGB565)
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    
    /**
     * @brief 填充矩形
     * @param x 左上角列
     * @param y 左上角行
     * @param width 宽度
     * @param height 高度
     * @param color 填充颜色
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    
    /**
     * @brief 绘制矩形框
     * @param x 左上角列
     * @param y 左上角行
     * @param width 宽度
     * @param height 高度
     * @param color 框线颜色
     */
    void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    
    /**
     * @brief 绘制水平线
     * @param x 起始列
     * @param y 行
     * @param length 长度
     * @param color 线条颜色
     */
    void drawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
    
    /**
     * @brief 绘制竖直线
     * @param x 列
     * @param y 起始行
     * @param length 长度
     * @param color 线条颜色
     */
    void drawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
    
    /**
     * @brief 设置背光亮度
     * @param brightness 亮度值(0-255)
     */
    void setBacklight(uint8_t brightness);
    
    /**
     * @brief 根据光照自动调整亮度
     * @param lightLevel 光照强度(lux)
     */
    void autoAdjustBrightness(float lightLevel);
    
    /**
     * @brief 屏幕息屏
     */
    void sleep();
    
    /**
     * @brief 屏幕唤醒
     */
    void wakeup();
    
    /**
     * @brief 设置屏幕旋转
     * @param rotation 旋转角度(0/1/2/3)
     */
    void setRotation(uint8_t rotation);
    
    /**
     * @brief 获取屏幕宽度
     * @return 宽度(像素)
     */
    uint16_t width() const;
    
    /**
     * @brief 获取屏幕高度
     * @return 高度(像素)
     */
    uint16_t height() const;
    
    /**
     * @brief 检查屏幕是否活跃
     * @return true 活跃, false 睡眠状态
     */
    bool isActive() const { return _isActive; }
    
    /**
     * @brief 重置息屏计时器
     */
    void resetScreenTimeout();

private:
    // 单例模式
    DisplayManager();
    ~DisplayManager() = default;
    DisplayManager(const DisplayManager&) = delete;
    DisplayManager& operator=(const DisplayManager&) = delete;
    
    // 屏幕驱动对象
    TFTDisplay _tft;
    
    // 状态变量
    bool _isActive;
    bool _isAwake;
    uint32_t _lastActivityTime;
    uint32_t _lastUpdateTime;
    uint8_t _currentBrightness;
    bool _needsRedraw;

    ThemeConfig _theme;
    EnvironmentData _envData;
    bool _hasEnvData;
    
    /**
     * @brief 绘制示例UI
     */
    void drawDemoUI();

    /**
     * @brief 绘制主界面
     */
    void drawMainUI();

    /**
     * @brief 绘制环境数据条形展示
     */
    void drawEnvironmentBars(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height);
    
    /**
     * @brief 检查是否需要息屏
     */
    void checkScreenTimeout();
};

#endif // DISPLAY_MANAGER_H
