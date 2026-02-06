/**
 * @file Config.h
 * @brief 全局配置文件 - 硬件引脚、系统参数、功能开关
 * @version 1.0.0
 * @date 2026-02-04
 * 
 * 本文件集中管理所有硬件配置、系统参数和功能开关
 * 修改此文件即可调整整个系统的配置，无需修改业务代码
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==================== 系统配置 ====================
#define SYSTEM_NAME "ESP32-SmartTV"
#define SYSTEM_VERSION "1.0.0"
#define DEVICE_ID "ESP32S3-001"

// ==================== 硬件引脚配置 ====================

// I2C总线配置（多设备共用）
#define I2C_SDA_PIN         8
#define I2C_SCL_PIN         9
#define I2C_FREQ_HZ         100000  // 100kHz标准模式

// SPI屏幕引脚（ILI9341）
#define TFT_CS_PIN          10
#define TFT_MOSI_PIN        11
#define TFT_SCLK_PIN        12
#define TFT_RST_PIN         17
#define TFT_DC_PIN          18
#define TFT_BL_PIN          21      // 背光PWM控制
#define TFT_MISO_PIN        -1      // 不使用

// I2S麦克风引脚（INMP441）
#define I2S_MIC_SCK_PIN     4
#define I2S_MIC_WS_PIN      5
#define I2S_MIC_SD_PIN      6
#define I2S_MIC_PORT        I2S_NUM_0

// I2S功放引脚（MAX98357A）
#define I2S_SPK_BCLK_PIN    15
#define I2S_SPK_LRC_PIN     16
#define I2S_SPK_DOUT_PIN    7
#define I2S_SPK_PORT        I2S_NUM_1

// 按键引脚
#define BUTTON_1_PIN        13
#define BUTTON_2_PIN        14

// RTC中断引脚
#define RTC_SQW_PIN         2

// 电池电压检测引脚
#define BATTERY_ADC_PIN     1

// ==================== I2C设备地址 ====================
#define AHT20_I2C_ADDR      0x38
#define BMP280_I2C_ADDR     0x76    // 或0x77，根据模块确定
#define BH1750_I2C_ADDR     0x23    // ADDR引脚接GND
#define DS3231_I2C_ADDR     0x68

// ==================== 传感器配置 ====================

// AHT20+BMP280采集配置
#define SENSOR_SAMPLE_INTERVAL_MS   2000    // 采样间隔2秒
#define SENSOR_FILTER_WINDOW_SIZE   5       // 滤波窗口大小
#define TEMP_MIN_VALID              -40.0f  // 有效温度下限
#define TEMP_MAX_VALID              85.0f   // 有效温度上限
#define HUMI_MIN_VALID              0.0f    // 有效湿度下限
#define HUMI_MAX_VALID              100.0f  // 有效湿度上限
#define PRESS_MIN_VALID             300.0f  // 有效气压下限(hPa)
#define PRESS_MAX_VALID             1100.0f // 有效气压上限(hPa)

// BH1750光照配置
#define LIGHT_SAMPLE_INTERVAL_MS    1000    // 光照采样间隔1秒
#define LIGHT_STABLE_COUNT          3       // 连续稳定次数

// ==================== 屏幕配置 ====================
#define TFT_WIDTH                   320
#define TFT_HEIGHT                  240
#define TFT_SPI_FREQ_HZ             40000000    // 40MHz
#define TFT_ROTATION                1           // 0/1/2/3 横竖屏

// 背光PWM配置
#define BL_PWM_CHANNEL              0
#define BL_PWM_FREQ_HZ              5000        // 5kHz，人眼不可见频闪
#define BL_PWM_RESOLUTION           8           // 8位分辨率(0-255)

// 自动调光阈值（单位：lux）
#define LIGHT_LEVEL_VERY_BRIGHT     500
#define LIGHT_LEVEL_BRIGHT          200
#define LIGHT_LEVEL_NORMAL          50
#define LIGHT_LEVEL_DIM             10

// 亮度等级（0-255）
#define BRIGHTNESS_MAX              255     // 100%
#define BRIGHTNESS_HIGH             178     // 70%
#define BRIGHTNESS_MEDIUM           102     // 40%
#define BRIGHTNESS_LOW              26      // 10%
#define BRIGHTNESS_OFF              0       // 息屏

// 息屏超时
#define SCREEN_TIMEOUT_MS           30000   // 30秒无操作息屏

// ==================== 音频配置 ====================

// 采样率配置
#define AUDIO_SAMPLE_RATE           44100
#define AUDIO_BITS_PER_SAMPLE       I2S_BITS_PER_SAMPLE_16BIT
#define AUDIO_DMA_BUF_COUNT         4
#define AUDIO_DMA_BUF_LEN           1024

// VAD配置
#define VAD_THRESHOLD               500     // 能量阈值
#define VAD_FRAME_MS                30      // VAD帧长30ms
#define VAD_TRIGGER_COUNT           3       // 连续3帧触发唤醒

// 录音配置
#define RECORD_DURATION_SEC         3       // 录音时长3秒
#define RECORD_BUFFER_SIZE          (AUDIO_SAMPLE_RATE * RECORD_DURATION_SEC)

// ==================== 电源配置 ====================

// 电池电压映射（3.7V锂电池）
#define BATTERY_VOLTAGE_MAX         4.20f   // 充满电压
#define BATTERY_VOLTAGE_MIN         3.00f   // 过放保护电压
#define BATTERY_VOLTAGE_LOW         3.10f   // 低电警告电压

// ADC参考电压（分压后）
#define ADC_MAX_VOLTAGE             2.45f   // 11dB衰减最大电压
#define ADC_RESOLUTION              4095    // 12位ADC
#define VOLTAGE_DIVIDER_RATIO       2.0f    // 分压比

// 电量检测间隔
#define BATTERY_CHECK_INTERVAL_MS   10000   // 10秒检测一次

// ==================== WiFi配置 ====================
#define WIFI_CONNECT_TIMEOUT_MS     10000   // WiFi连接超时
#define WIFI_RECONNECT_INTERVAL_MS  30000   // 重连间隔
#define AP_SSID                     "ESP32-SmartTV"
#define AP_PASSWORD                 "12345678"
#define WEB_SERVER_PORT             80

// ==================== NTP配置 ====================
#define NTP_SERVER                  "pool.ntp.org"
#define NTP_TIMEZONE_OFFSET_SEC     (8 * 3600)  // UTC+8 (北京时间)
#define NTP_SYNC_INTERVAL_MS        (60 * 60 * 1000)  // 每小时同步一次

// ==================== 按键配置 ====================
#define BUTTON_DEBOUNCE_MS          20      // 防抖延时
#define BUTTON_LONG_PRESS_MS        3000    // 长按判定时间

// ==================== 功能开关 ====================
#define ENABLE_SERIAL_DEBUG         true    // 串口调试输出
#define ENABLE_WIFI                 true    // WiFi功能
#define ENABLE_VOICE                true    // 语音功能
#define ENABLE_AUTO_BRIGHTNESS      true    // 自动调光
#define ENABLE_BATTERY_MONITOR      true    // 电池监测
#define ENABLE_WEB_CONFIG           true    // Web配置

// ==================== 调试配置 ====================
#if ENABLE_SERIAL_DEBUG
    #define DEBUG_PRINT(x)      Serial.print(x)
    #define DEBUG_PRINTLN(x)    Serial.println(x)
    #define DEBUG_PRINTF(...)   Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

// ==================== 主题配置 ====================
#define THEME_CONFIG_FILE           "/config/theme.json"
#define THEMES_CONFIG_FILE          "/config/themes.json"
#define WEATHER_ICON_CONFIG_FILE    "/config/weather_icons.json"
#define DEVICE_SETTINGS_FILE        "/config/device.json"
#define DEFAULT_THEME_ID            1
#define MAX_THEMES                  6

// ==================== 闹钟配置 ====================
#define MAX_ALARMS                  3       // 最多3个闹钟
#define ALARM_SOUND_FILE            "/sounds/alarm.wav"

// ==================== 云端API配置 ====================
#define ASR_API_URL                 "https://api.example.com/asr"
#define ASR_API_TIMEOUT_MS          10000
#define TTS_API_URL                 "https://api.example.com/tts"

#endif // CONFIG_H
