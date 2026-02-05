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
#include "services/SensorManager.h"
#include "services/DisplayManager.h"
#include "services/PowerManager.h"
#include "services/NetworkManager.h"
#include "services/AudioManager.h"
#include "services/ButtonManager.h"
#include "services/RTCManager.h"

// ==================== 全局对象 ====================
SensorManager& sensorMgr = SensorManager::getInstance();
DisplayManager& displayMgr = DisplayManager::getInstance();
// PowerManager& powerMgr = PowerManager::getInstance();
// NetworkManager& networkMgr = NetworkManager::getInstance();
// AudioManager& audioMgr = AudioManager::getInstance();
// ButtonManager& buttonMgr = ButtonManager::getInstance();
// RTCManager& rtcMgr = RTCManager::getInstance();

// ==================== 任务句柄 ====================
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t audioTaskHandle = NULL;

// ==================== 回调函数 ====================

/**
 * @brief 传感器数据更新回调
 */
void onSensorDataUpdate(const EnvironmentData& data) {
    DEBUG_PRINTF("[Main] Sensor update: T=%.2f°C, H=%.2f%%RH, P=%.1fhPa, L=%.0flux\n",
                 data.temperature, data.humidity, data.pressure, data.lightLevel);
    
    // TODO: 更新显示
    // displayMgr.updateEnvironmentData(data);
    
    // TODO: 根据光照调整亮度
    // displayMgr.autoAdjustBrightness(data.lightLevel);
}

/**
 * @brief 按键按下回调
 */
void onButtonPressed(int buttonId, bool isLongPress) {
    DEBUG_PRINTF("[Main] Button %d %s\n", buttonId, isLongPress ? "long pressed" : "pressed");
    
    // TODO: 处理按键逻辑
}

/**
 * @brief 闹钟触发回调
 */
void onAlarmTriggered(int alarmId) {
    DEBUG_PRINTF("[Main] Alarm %d triggered\n", alarmId);
    
    // TODO: 播放闹钟音
}

// ==================== FreeRTOS任务 ====================

/**
 * @brief 传感器采集任务
 */
void sensorTask(void* parameter) {
    DEBUG_PRINTLN("[Task] Sensor task started");
    
    while (true) {
        sensorMgr.update();
        vTaskDelay(pdMS_TO_TICKS(100));  // 100ms
    }
}

/**
 * @brief 显示更新任务
 */
void displayTask(void* parameter) {
    DEBUG_PRINTLN("[Task] Display task started");
    
    while (true) {
        displayMgr.update();
        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms，20fps
    }
}

/**
 * @brief 音频处理任务
 */
void audioTask(void* parameter) {
    DEBUG_PRINTLN("[Task] Audio task started");
    
    while (true) {
        // TODO: 音频处理
        // audioMgr.update();
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms
    }
}

// ==================== Arduino标准函数 ====================

void setup() {
    // 串口初始化
    Serial.begin(115200);
    delay(1000);
    
    DEBUG_PRINTLN("\n\n");
    DEBUG_PRINTLN("========================================");
    DEBUG_PRINTLN("  ESP32-S3 Smart Desktop TV");
    DEBUG_PRINTLN("  Version: " SYSTEM_VERSION);
    DEBUG_PRINTLN("  Build: " __DATE__ " " __TIME__);
    DEBUG_PRINTLN("========================================\n");
    
    // 1. 初始化传感器管理器
    DEBUG_PRINTLN("[Setup] Initializing sensors...");
    if (!sensorMgr.begin()) {
        DEBUG_PRINTLN("[Setup] WARNING: Sensor initialization failed!");
    }
    sensorMgr.setDataCallback(onSensorDataUpdate);
    
    // 2. 初始化显示管理器
    DEBUG_PRINTLN("[Setup] Initializing display...");
    if (!displayMgr.begin()) {
        DEBUG_PRINTLN("[Setup] ERROR: Display initialization failed!");
    }
    
    // 3. 初始化电源管理器
    // DEBUG_PRINTLN("[Setup] Initializing power...");
    // if (!powerMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: Power manager initialization failed!");
    // }
    
    // 4. 初始化按键管理器
    // DEBUG_PRINTLN("[Setup] Initializing buttons...");
    // buttonMgr.begin();
    // buttonMgr.setButtonCallback(onButtonPressed);
    
    // 5. 初始化RTC管理器
    // DEBUG_PRINTLN("[Setup] Initializing RTC...");
    // if (!rtcMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: RTC initialization failed!");
    // }
    // rtcMgr.setAlarmCallback(onAlarmTriggered);
    
    // 6. 初始化网络管理器
    #if ENABLE_WIFI
    // DEBUG_PRINTLN("[Setup] Initializing network...");
    // if (!networkMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: Network initialization failed!");
    // }
    #endif
    
    // 7. 初始化音频管理器
    #if ENABLE_VOICE
    // DEBUG_PRINTLN("[Setup] Initializing audio...");
    // if (!audioMgr.begin()) {
    //     DEBUG_PRINTLN("[Setup] WARNING: Audio initialization failed!");
    // }
    #endif
    
    // 创建FreeRTOS任务
    DEBUG_PRINTLN("[Setup] Creating tasks...");
    
    // 传感器任务（优先级2，Core 0）
    xTaskCreatePinnedToCore(
        sensorTask,           // 任务函数
        "SensorTask",         // 任务名称
        4096,                 // 栈大小
        NULL,                 // 参数
        2,                    // 优先级
        &sensorTaskHandle,    // 任务句柄
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
    
    DEBUG_PRINTLN("[Setup] Initialization completed!\n");
}

void loop() {
    // 主循环保持空闲，任务由FreeRTOS调度
    
    // 定期检查系统状态
    static uint32_t lastStatusCheck = 0;
    if (millis() - lastStatusCheck > 60000) {  // 每分钟检查一次
        lastStatusCheck = millis();
        
        DEBUG_PRINTLN("\n[Status] System health check:");
        DEBUG_PRINTF("  Free heap: %d bytes\n", ESP.getFreeHeap());
        DEBUG_PRINTF("  Min free heap: %d bytes\n", ESP.getMinFreeHeap());
        DEBUG_PRINTF("  Uptime: %lu seconds\n", millis() / 1000);
        
        // 检查传感器状态
        bool sensorsOK = sensorMgr.checkStatus();
        DEBUG_PRINTF("  Sensors: %s\n", sensorsOK ? "OK" : "ERROR");
        
        DEBUG_PRINTLN("");
    }
    
    delay(1000);  // 主循环1秒延时
}
