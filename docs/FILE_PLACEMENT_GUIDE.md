# TFT屏幕驱动 - 项目结构集成指南

## 📂 完整的项目目录结构

根据你项目的现有结构，这是建议的完整目录树：

```
esp32-smart-tv/
│
├── platformio.ini                  # 项目配置（已有）
├── README.md                       # 项目说明（已有）
├── PROJECT_STRUCTURE.md            # 结构说明（已有）
├── .gitignore                      # Git配置（已有）
│
├── include/                        # 全局头文件
│   ├── config/
│   │   └── Config.h                # ⭐ 全局配置（更新）
│   └── interfaces/
│       └── ISensor.h               # 传感器接口（已有）
│
├── src/                            # 源代码
│   ├── main.cpp                    # ⭐ 主程序（更新）
│   │
│   ├── core/                       # 核心功能（预留）
│   │
│   ├── drivers/                    # 硬件驱动（预留）
│   │
│   ├── services/                   # 服务管理器
│   │   ├── SensorManager.h         # 传感器管理器（已有）
│   │   ├── SensorManager.cpp       # 传感器管理器（已有）
│   │   │
│   │   ├── DisplayManager.h        # ⭐ NEW: 显示管理器
│   │   ├── DisplayManager.cpp      # ⭐ NEW: 显示管理器
│   │   │
│   │   ├── PowerManager.h          # 电源管理（预留）
│   │   ├── NetworkManager.h        # 网络管理（预留）
│   │   ├── AudioManager.h          # 音频管理（预留）
│   │   ├── ButtonManager.h         # 按键管理（预留）
│   │   └── RTCManager.h            # RTC管理（预留）
│   │
│   ├── ui/                         # UI组件（预留）
│   │
│   └── utils/                      # 工具函数（预留）
│
├── lib/                            # 自定义库
│   ├── sensors/                    # 传感器驱动
│   │   ├── aht20/
│   │   │   ├── AHT20Sensor.h       # AHT20驱动（已有）
│   │   │   └── AHT20Sensor.cpp     # AHT20驱动（已有）
│   │   │
│   │   ├── bmp280/
│   │   │   ├── BMP280Sensor.h      # BMP280驱动（已有）
│   │   │   └── BMP280Sensor.cpp    # BMP280驱动（已有）
│   │   │
│   │   └── bh1750/
│   │       ├── BH1750Sensor.h      # BH1750驱动（已有）
│   │       └── BH1750Sensor.cpp    # BH1750驱动（已有）
│   │
│   ├── display/                    # ⭐ NEW: 显示驱动库
│   │   ├── TFTDisplay.h            # ⭐ NEW: TFT驱动头文件
│   │   ├── TFTDisplay.cpp          # ⭐ NEW: TFT驱动实现
│   │   └── TFT_Colors.h            # ⭐ NEW: 颜色定义速查表
│   │
│   ├── audio/                      # 音频驱动（待实现）
│   │   ├── microphone/
│   │   ├── speaker/
│   │   └── vad/
│   │
│   ├── power/                      # 电源管理（待实现）
│   │   ├── battery/
│   │   └── charging/
│   │
│   ├── rtc/                        # RTC驱动（待实现）
│   │   ├── ds3231/
│   │   └── alarm/
│   │
│   └── network/                    # 网络驱动（待实现）
│       ├── wifi/
│       └── api/
│
├── data/                           # 数据文件
│   ├── sounds/                     # 音频文件（WAV格式）
│   ├── themes/                     # 主题配置（JSON格式）
│   └── fonts/                      # 字体文件
│
├── test/                           # 单元测试
│
├── docs/                           # 文档
│   ├── QUICK_START.md              # 快速开始（已有）
│   ├── DEVELOPMENT.md              # 开发指南（已有）
│   ├── TROUBLESHOOTING.md          # 故障排除（待实现）
│   │
│   ├── DISPLAY.md                  # ⭐ NEW: 显示系统说明
│   ├── TFT_WIRING_GUIDE.md         # ⭐ NEW: TFT接线指南
│   ├── TFT_INTEGRATION_GUIDE.md    # ⭐ NEW: 集成指南
│   └── TFT_QUICK_REFERENCE.md      # ⭐ NEW: 快速参考卡
│
└── .vscode/                        # VSCode配置
    ├── settings.json
    └── extensions.json
```

## 📥 文件放置详细说明

### 第一步：创建新目录

```bash
# 在项目根目录下执行
mkdir -p lib/display
mkdir -p docs/display_docs
```

### 第二步：放置源代码文件

#### 1️⃣ 驱动层文件 → `lib/display/`

| 文件名 | 来源 | 目标位置 | 说明 |
|--------|------|---------|------|
| TFTDisplay.h | 交付文件 | `lib/display/TFTDisplay.h` | 驱动头文件 |
| TFTDisplay.cpp | 交付文件 | `lib/display/TFTDisplay.cpp` | 驱动实现 |
| TFT_Colors.h | 交付文件 | `lib/display/TFT_Colors.h` | 颜色定义 |

```bash
# 复制命令
cp TFTDisplay.h lib/display/
cp TFTDisplay.cpp lib/display/
cp TFT_Colors.h lib/display/
```

#### 2️⃣ 管理层文件 → `src/services/`

| 文件名 | 来源 | 目标位置 | 说明 |
|--------|------|---------|------|
| DisplayManager.h | 交付文件 | `src/services/DisplayManager.h` | 管理器头文件 |
| DisplayManager.cpp | 交付文件 | `src/services/DisplayManager.cpp` | 管理器实现 |

```bash
# 复制命令
cp DisplayManager.h src/services/
cp DisplayManager.cpp src/services/
```

### 第三步：放置文档文件

#### 文档 → `docs/`

| 文件名 | 来源 | 目标位置 | 优先级 |
|--------|------|---------|--------|
| TFT_QUICK_REFERENCE.md | 交付文件 | `docs/TFT_QUICK_REFERENCE.md` | ⭐⭐⭐⭐⭐ |
| TFT_WIRING_GUIDE.md | 交付文件 | `docs/TFT_WIRING_GUIDE.md` | ⭐⭐⭐⭐ |
| TFT_INTEGRATION_GUIDE.md | 交付文件 | `docs/TFT_INTEGRATION_GUIDE.md` | ⭐⭐⭐⭐ |
| TFT_COMPLETE_GUIDE.md | 交付文件 | `docs/TFT_COMPLETE_GUIDE.md` | ⭐⭐⭐ |
| README_DELIVERY.txt | 交付文件 | `docs/README_DELIVERY.txt` | ⭐⭐ |

```bash
# 复制命令
cp TFT_QUICK_REFERENCE.md docs/
cp TFT_WIRING_GUIDE.md docs/
cp TFT_INTEGRATION_GUIDE.md docs/
cp TFT_COMPLETE_GUIDE.md docs/
cp README_DELIVERY.txt docs/
```

## 🔧 需要更新的文件

### 1. 更新 `include/config/Config.h`

确保以下配置存在（已有，但需要验证）：

```cpp
// ==================== SPI屏幕引脚配置 ====================
// 已有正确配置
#define TFT_CS_PIN          10
#define TFT_MOSI_PIN        11
#define TFT_SCLK_PIN        12
#define TFT_RST_PIN         17
#define TFT_DC_PIN          18
#define TFT_BL_PIN          21
#define TFT_MISO_PIN        -1      // 不使用

// ==================== 屏幕配置 ====================
#define TFT_WIDTH           240
#define TFT_HEIGHT          320
#define TFT_SPI_FREQ_HZ     40000000    // 40MHz
#define TFT_ROTATION        1           // 0/1/2/3 横竖屏

// ==================== 背光PWM配置 ====================
#define BL_PWM_CHANNEL              0
#define BL_PWM_FREQ_HZ              5000        // 5kHz，人眼不可见频闪
#define BL_PWM_RESOLUTION           8           // 8位分辨率(0-255)

// ==================== 自动调光阈值（单位：lux）====================
#define LIGHT_LEVEL_VERY_BRIGHT     500
#define LIGHT_LEVEL_BRIGHT          200
#define LIGHT_LEVEL_NORMAL          50
#define LIGHT_LEVEL_DIM             10

// ==================== 亮度等级（0-255）====================
#define BRIGHTNESS_MAX              255     // 100%
#define BRIGHTNESS_HIGH             178     // 70%
#define BRIGHTNESS_MEDIUM           102     // 40%
#define BRIGHTNESS_LOW              26      // 10%
#define BRIGHTNESS_OFF              0       // 息屏

// ==================== 息屏超时 ====================
#define SCREEN_TIMEOUT_MS           30000   // 30秒无操作息屏

// ==================== 功能开关 ====================
#define ENABLE_SERIAL_DEBUG         true    // 串口调试输出
// ... 其他开关 ...
```

### 2. 更新 `src/main.cpp`

添加DisplayManager的初始化和任务：

```cpp
#include <Arduino.h>
#include "config/Config.h"
#include "services/SensorManager.h"
#include "services/DisplayManager.h"              // ⭐ 添加这一行

// ==================== 全局对象 ====================
SensorManager& sensorMgr = SensorManager::getInstance();
DisplayManager& displayMgr = DisplayManager::getInstance();  // ⭐ 添加这一行

// ==================== 任务句柄 ====================
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;            // ⭐ 添加这一行
TaskHandle_t audioTaskHandle = NULL;

// ==================== 回调函数 ====================

/**
 * @brief 传感器数据更新回调
 */
void onSensorDataUpdate(const EnvironmentData& data) {
    DEBUG_PRINTF("[Main] Sensor update: T=%.2f°C, H=%.2f%%RH, P=%.1fhPa, L=%.0flux\n",
                 data.temperature, data.humidity, data.pressure, data.lightLevel);
    
    // ⭐ 新增：根据光照自动调整亮度
    #if ENABLE_AUTO_BRIGHTNESS
    displayMgr.autoAdjustBrightness(data.lightLevel);
    #endif
}

// ... 其他回调函数 ...

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
 * @brief 显示更新任务 ⭐ 新增任务
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
        vTaskDelay(pdMS_TO_TICKS(10));
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
    
    // ⭐ 2. 初始化显示管理器（新增）
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
        sensorTask,
        "SensorTask",
        4096,
        NULL,
        2,
        &sensorTaskHandle,
        0
    );
    
    // ⭐ 显示任务（优先级3，Core 1）- 新增
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
        
        // ⭐ 检查显示器状态（新增）
        DEBUG_PRINTF("  Display: %s\n", displayMgr.isActive() ? "ACTIVE" : "INACTIVE");
        
        DEBUG_PRINTLN("");
    }
    
    delay(1000);  // 主循环1秒延时
}
```

## 📋 文件清单验证

### 源代码文件 (4个)

```
✓ lib/display/TFTDisplay.h
✓ lib/display/TFTDisplay.cpp
✓ lib/display/TFT_Colors.h
✓ src/services/DisplayManager.h
✓ src/services/DisplayManager.cpp
```

### 文档文件 (5个)

```
✓ docs/TFT_QUICK_REFERENCE.md
✓ docs/TFT_WIRING_GUIDE.md
✓ docs/TFT_INTEGRATION_GUIDE.md
✓ docs/TFT_COMPLETE_GUIDE.md
✓ docs/README_DELIVERY.txt
```

### 需要修改的文件 (2个)

```
✓ include/config/Config.h (验证配置)
✓ src/main.cpp (添加DisplayManager初始化)
```

## 🔍 集成检查清单

完成文件放置后，逐项检查：

### 文件检查
- [ ] `lib/display/` 目录下有3个文件
- [ ] `src/services/` 目录下有DisplayManager.h和.cpp
- [ ] `docs/` 目录下有5个Markdown文档
- [ ] `include/config/Config.h` 中有TFT相关配置
- [ ] `src/main.cpp` 中添加了DisplayManager初始化

### 编译检查
- [ ] 项目编译无错误
- [ ] 编译无TFT相关警告
- [ ] 代码大小合理

### 运行检查
- [ ] 串口输出显示Display初始化成功
- [ ] 屏幕正确显示
- [ ] 背光正常工作

## 🚀 下一步

完成文件放置后：

1. **编译验证**
   ```bash
   pio run --target clean
   pio run
   ```

2. **上传测试**
   ```bash
   pio run -t upload
   pio device monitor  # 波特率115200
   ```

3. **验证输出**
   看到以下日志表示成功：
   ```
   [TFT] Initializing TFT display...
   [TFT] TFT display initialized successfully
   [DisplayManager] Initializing...
   [DisplayManager] Initialization completed
   ```

## 📚 快速参考

### 导入头文件的方式

在任何需要使用显示的文件中：

```cpp
// 使用DisplayManager（推荐）
#include "services/DisplayManager.h"
DisplayManager& displayMgr = DisplayManager::getInstance();

// 或者直接使用TFTDisplay驱动（底层）
#include "display/TFTDisplay.h"
TFTDisplay tft;
```

### 包含关系

```
src/main.cpp
    ↓
src/services/DisplayManager.h/cpp
    ↓
lib/display/TFTDisplay.h/cpp
    ↓
lib/display/TFT_Colors.h
    ↓
Arduino SPI库（内置）
```

## 🎯 推荐的文件结构验证

完成所有操作后，你的项目结构应该像这样：

```bash
esp32-smart-tv/
├── include/config/Config.h (已验证TFT配置)
├── src/
│   ├── main.cpp (已添加DisplayManager)
│   └── services/
│       ├── SensorManager.h/cpp
│       └── DisplayManager.h/cpp ✓ NEW
├── lib/
│   ├── sensors/
│   │   ├── aht20/
│   │   ├── bmp280/
│   │   └── bh1750/
│   └── display/ ✓ NEW
│       ├── TFTDisplay.h ✓ NEW
│       ├── TFTDisplay.cpp ✓ NEW
│       └── TFT_Colors.h ✓ NEW
└── docs/
    ├── QUICK_START.md
    ├── DEVELOPMENT.md
    └── TFT_* (5个新文档) ✓ NEW
```

---

**完成以上步骤后，你的项目就完全集成了TFT屏幕驱动系统！** 🎉
