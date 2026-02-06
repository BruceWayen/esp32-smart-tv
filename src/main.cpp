/**
 * @file main.cpp
 * @brief ESP32-S3智能桌面小电视 - 主程序
 * @version 1.0.0
 * @date 2026-02-04
 * 
 * 项目功能：
 * 1. 环境监测（温湿度、气压）
 * 2. 光照自动调光
 * 3. 语音交互
 * 4. 双供电管理
 * 5. 屏幕显示
 * 6. RTC精准计时
 * 7. WiFi配置
 * 8. 按键控制
 * 9. 主题自定义
 * 10. 闹钟功能
 */

#include <Arduino.h>
#include "config/Config.h"
#include "services/DesktopDataService.h"
#include "services/DisplayManager.h"
#include "services/ButtonManager.h"
#include "services/ThemeManager.h"
#include "services/WeatherIconManager.h"
#include "services/WebConfigServer.h"

// ==================== 全局对象 ====================
DesktopDataService& dataService = DesktopDataService::getInstance();
DisplayManager& displayMgr = DisplayManager::getInstance();
ThemeManager& themeMgr = ThemeManager::getInstance();
WeatherIconManager& iconMgr = WeatherIconManager::getInstance();
WebConfigServer& webServer = WebConfigServer::getInstance();
// PowerManager& powerMgr = PowerManager::getInstance();
// AudioManager& audioMgr = AudioManager::getInstance();
ButtonManager& buttonMgr = ButtonManager::getInstance();
// RTCManager& rtcMgr = RTCManager::getInstance();

// ==================== 任务句柄 ====================
TaskHandle_t dataTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t audioTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;

// ==================== 回调函数 ====================

/**
 * @brief 传感器数据更新回调
 */
void onSensorDataUpdate(const EnvironmentData& data) {
    DEBUG_PRINTF("[Main] 环境更新: T=%.2f℃, H=%.2f%%RH, P=%.1fhPa, L=%.0flux\n",
                 data.temperature, data.humidity, data.pressure, data.lightLevel);
    
    displayMgr.updateEnvironmentData(data);
    
    // TODO: 根据光照调整亮度
    // displayMgr.autoAdjustBrightness(data.lightLevel);
}

/**
 * @brief 按键按下回调
 */
void onButtonPressed(int buttonId, bool isLongPress) {
    DEBUG_PRINTF("[Main] 按键%d %s\n", buttonId, isLongPress ? "长按" : "短按");

    if (!isLongPress) {
        if (buttonId == 1) {
            themeMgr.nextTheme();
        } else if (buttonId == 2) {
            themeMgr.previousTheme();
        }
    }
}

/**
 * @brief 天气数据更新回调
 */
void onWeatherDataUpdate(const WeatherInfo& data) {
    DEBUG_PRINTF("[Main] 天气更新: %s %.1f℃ 风速%.1fm/s\n",
                 data.condition.c_str(), data.temperature, data.windSpeed);
    displayMgr.updateWeatherData(data);
}

/**
 * @brief 时间数据更新回调
 */
void onClockUpdate(const ClockInfo& data) {
    displayMgr.updateClockData(data);
}

/**
 * @brief 闹钟触发回调
 */
void onAlarmTriggered(int alarmId) {
    DEBUG_PRINTF("[Main] 闹钟%d已触发\n", alarmId);
    
    // TODO: 播放闹钟音
}

// ==================== FreeRTOS任务 ====================

/**
 * @brief 传感器采集任务
 */
void dataTask(void* parameter) {
    DEBUG_PRINTLN("[Task] 数据任务启动");
    
    while (true) {
        dataService.update();
        vTaskDelay(pdMS_TO_TICKS(100));  // 100ms
    }
}

/**
 * @brief 显示更新任务
 */
void displayTask(void* parameter) {
    DEBUG_PRINTLN("[Task] 显示任务启动");
    
    while (true) {
        displayMgr.update();
        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms，20fps
    }
}

/**
 * @brief 音频处理任务
 */
void audioTask(void* parameter) {
    DEBUG_PRINTLN("[Task] 音频任务启动");
    
    while (true) {
        // TODO: 音频处理
        // audioMgr.update();
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms
    }
}

/**
 * @brief 按键扫描任务
 */
void buttonTask(void* parameter) {
    DEBUG_PRINTLN("[Task] 按键任务启动");

    while (true) {
        buttonMgr.update();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ==================== Arduino标准函数 ====================

void setup() {
    // 串口初始化
    Serial.begin(115200);
    delay(1000);
    
    DEBUG_PRINTLN("\n\n");
    DEBUG_PRINTLN("========================================");
    DEBUG_PRINTLN("  ESP32-S3 智能桌面小电视");
    DEBUG_PRINTLN("  Version: " SYSTEM_VERSION);
    DEBUG_PRINTLN("  Build: " __DATE__ " " __TIME__);
    DEBUG_PRINTLN("========================================\n");
    
    // 1. 初始化桌面数据服务
    DEBUG_PRINTLN("[Setup] 初始化桌面数据服务...");
    if (!dataService.begin()) {
        DEBUG_PRINTLN("[Setup] 警告：桌面数据服务初始化失败");
    }
    dataService.setEnvironmentCallback(onSensorDataUpdate);
    dataService.setWeatherCallback(onWeatherDataUpdate);
    dataService.setClockCallback(onClockUpdate);
    
    // 2. 初始化主题管理器
    DEBUG_PRINTLN("[Setup] 初始化主题管理器...");
    themeMgr.setThemeChangeCallback([](const ThemeConfig& theme) {
        displayMgr.applyTheme(theme);
    });
    themeMgr.begin();

    // 3. 初始化天气图标配置
    DEBUG_PRINTLN("[Setup] 初始化天气图标配置...");
    iconMgr.begin();

    DEBUG_PRINTLN("[Setup] 初始化显示管理器...");
    if (!displayMgr.begin()) {
        DEBUG_PRINTLN("[Setup] 错误：显示初始化失败");
    }
    
    // 3. 初始化电源管理器
    // DEBUG_PRINTLN("[Setup] Initializing power...");
    // if (!powerMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: Power manager initialization failed!");
    // }
    
    // 4. 初始化按键管理器
    DEBUG_PRINTLN("[Setup] 初始化按键...");
    buttonMgr.begin();
    buttonMgr.setButtonCallback(onButtonPressed);
    
    // 5. 初始化RTC管理器
    // DEBUG_PRINTLN("[Setup] Initializing RTC...");
    // if (!rtcMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: RTC initialization failed!");
    // }
    // rtcMgr.setAlarmCallback(onAlarmTriggered);
    
    // 6. 初始化Web配置服务
    #if ENABLE_WEB_CONFIG
    DEBUG_PRINTLN("[Setup] 初始化Web配置服务...");
    if (!webServer.begin()) {
        DEBUG_PRINTLN("[Setup] 警告：Web配置服务启动失败");
    }
    #endif
    
    // 7. 初始化音频管理器
    #if ENABLE_VOICE
    // DEBUG_PRINTLN("[Setup] Initializing audio...");
    // if (!audioMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: Audio initialization failed!");
    // }
    #endif
    
    // 创建FreeRTOS任务
    DEBUG_PRINTLN("[Setup] 创建任务...");
    
    // 数据任务（优先级2，Core 0）
    xTaskCreatePinnedToCore(
        dataTask,             // 任务函数
        "DataTask",           // 任务名称
        4096,                 // 栈大小
        NULL,                 // 参数
        2,                    // 优先级
        &dataTaskHandle,      // 任务句柄
        0                     // CPU核心
    );
    
    // 显示任务（优先级3，Core 1）
    xTaskCreatePinnedToCore(
        displayTask,
        "DisplayTask",
        4096,
        NULL,
        3,
        &displayTaskHandle,
        1
    );

    // 按键任务（优先级2，Core 1）
    xTaskCreatePinnedToCore(
        buttonTask,
        "ButtonTask",
        2048,
        NULL,
        2,
        &buttonTaskHandle,
        1
    );
    
    // 音频任务（优先级3，Core 0）
    #if ENABLE_VOICE
    // xTaskCreatePinnedToCore(
    //     audioTask,
    //     "AudioTask",
    //     8192,
    //     NULL,
    //     3,
    //     &audioTaskHandle,
    //     0
    // );
    #endif
    
    DEBUG_PRINTLN("[Setup] 初始化完成！\n");
}

void loop() {
    // 主循环保持空闲，任务由FreeRTOS调度
    
    // 定期检查系统状态
    static uint32_t lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 60000) {  // 每分钟检查一次
        lastStatusCheck = millis();
        
        DEBUG_PRINTLN("\n[状态] 系统健康检查:");
        DEBUG_PRINTF("  剩余堆内存: %d bytes\n", ESP.getFreeHeap());
        DEBUG_PRINTF("  最小剩余堆: %d bytes\n", ESP.getMinFreeHeap());
        DEBUG_PRINTF("  运行时间: %lu 秒\n", millis() / 1000);
        
        // 检查传感器状态
        DEBUG_PRINTLN("  数据服务: 运行中");
        
        DEBUG_PRINTLN("");
    }
    
    delay(1000);  // 主循环1秒延时
}
