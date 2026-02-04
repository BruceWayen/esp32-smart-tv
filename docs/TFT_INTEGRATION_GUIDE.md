# TFT屏幕驱动集成指南

## 📦 文件清单

本次交付的TFT屏幕驱动包含以下文件：

### 驱动层文件
```
lib/display/
├── TFTDisplay.h          # TFT驱动头文件
└── TFTDisplay.cpp        # TFT驱动实现
```

### 管理层文件
```
src/services/
├── DisplayManager.h      # 显示管理器头文件
└── DisplayManager.cpp    # 显示管理器实现
```

### 文档文件
```
docs/
└── TFT_WIRING_GUIDE.md   # 接线和使用指南
```

## 🔧 集成步骤

### 第一步：拷贝文件到项目

1. **创建目录**
   ```bash
   mkdir -p lib/display
   mkdir -p src/services
   ```

2. **拷贝驱动文件**
   - 将 `TFTDisplay.h` 和 `TFTDisplay.cpp` 放入 `lib/display/`
   - 将 `DisplayManager.h` 和 `DisplayManager.cpp` 放入 `src/services/`

3. **拷贝文档**
   - 将 `TFT_WIRING_GUIDE.md` 放入 `docs/`

### 第二步：更新Config.h

在 `include/config/Config.h` 中，确保这些定义存在（已有）：

```cpp
// SPI屏幕引脚（ILI9341）已改为ST7789
#define TFT_CS_PIN          10
#define TFT_MOSI_PIN        11
#define TFT_SCLK_PIN        12
#define TFT_RST_PIN         17
#define TFT_DC_PIN          18
#define TFT_BL_PIN          21
#define TFT_MISO_PIN        -1      // 不使用

// 屏幕配置
#define TFT_WIDTH           240
#define TFT_HEIGHT          320
#define TFT_SPI_FREQ_HZ     40000000
#define TFT_ROTATION        1

// 背光PWM配置
#define BL_PWM_CHANNEL      0
#define BL_PWM_FREQ_HZ      5000
#define BL_PWM_RESOLUTION   8

// 自动调光阈值
#define LIGHT_LEVEL_VERY_BRIGHT 500
#define LIGHT_LEVEL_BRIGHT      200
#define LIGHT_LEVEL_NORMAL      50
#define LIGHT_LEVEL_DIM         10

// 亮度等级
#define BRIGHTNESS_MAX      255
#define BRIGHTNESS_HIGH     178
#define BRIGHTNESS_MEDIUM   102
#define BRIGHTNESS_LOW      26
#define BRIGHTNESS_OFF      0

// 息屏超时
#define SCREEN_TIMEOUT_MS   30000
```

### 第三步：更新platformio.ini

检查 `platformio.ini` 中是否包含必要的库（如果没有，添加）：

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

lib_deps = 
    ; 已有的库...
    ; SPI驱动由Arduino框架内置提供
```

> 注意：ST7789驱动使用原生SPI库，无需额外依赖

### 第四步：更新main.cpp

在 `src/main.cpp` 中添加屏幕管理器的初始化：

```cpp
#include <Arduino.h>
#include "config/Config.h"
#include "services/SensorManager.h"
#include "services/DisplayManager.h"    // ← 添加这一行

// ==================== 全局对象 ====================
SensorManager& sensorMgr = SensorManager::getInstance();
DisplayManager& displayMgr = DisplayManager::getInstance();  // ← 添加这一行

// ==================== 显示任务 ====================

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

void setup() {
    // ... 现有代码 ...
    
    // 初始化显示管理器
    DEBUG_PRINTLN("[Setup] Initializing display...");
    if (!displayMgr.begin()) {
        DEBUG_PRINTLN("[Setup] ERROR: Display initialization failed!");
    }
    
    // 创建显示任务
    xTaskCreatePinnedToCore(
        displayTask,
        "DisplayTask",
        4096,
        NULL,
        3,
        &displayTaskHandle,
        1  // CPU核心1
    );
    
    // ... 其他代码 ...
}

void loop() {
    // ... 现有代码 ...
}
```

## 🧪 测试步骤

### 测试1：屏幕颜色测试

在 `src/main.cpp` 的 `setup()` 函数中添加：

```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // 初始化显示
    if (!displayMgr.begin()) {
        Serial.println("Display init failed!");
        while(1);
    }
    
    // 颜色测试
    Serial.println("Testing colors...");
    
    displayMgr.clear(COLOR_RED);
    delay(1000);
    
    displayMgr.clear(COLOR_GREEN);
    delay(1000);
    
    displayMgr.clear(COLOR_BLUE);
    delay(1000);
    
    displayMgr.clear(COLOR_WHITE);
    delay(1000);
    
    displayMgr.clear(COLOR_BLACK);
    Serial.println("Color test completed!");
}

void loop() {
    // 保持运行
    delay(1000);
}
```

### 测试2：图形绘制测试

```cpp
void drawTestShapes() {
    // 清屏
    displayMgr.clear(COLOR_BLACK);
    delay(500);
    
    // 绘制矩形
    displayMgr.drawRect(10, 10, 100, 80, COLOR_RED);
    delay(500);
    
    // 绘制填充矩形
    displayMgr.fillRect(130, 10, 80, 80, COLOR_GREEN);
    delay(500);
    
    // 绘制水平线
    displayMgr.drawHLine(10, 120, 200, COLOR_BLUE);
    delay(500);
    
    // 绘制竖直线
    displayMgr.drawVLine(120, 150, 100, COLOR_YELLOW);
    delay(500);
    
    // 绘制像素点阵
    for (int x = 0; x < 50; x++) {
        for (int y = 0; y < 50; y++) {
            if ((x + y) % 10 == 0) {
                displayMgr.drawPixel(50 + x, 250 + y, COLOR_MAGENTA);
            }
        }
    }
    delay(1000);
}
```

### 测试3：背光控制测试

```cpp
void backlightTest() {
    displayMgr.clear(COLOR_BLUE);
    
    // 逐级增亮
    Serial.println("Brightness increasing...");
    for (int i = 0; i <= 255; i += 15) {
        displayMgr.setBacklight(i);
        delay(100);
    }
    
    // 逐级降暗
    Serial.println("Brightness decreasing...");
    for (int i = 255; i >= 0; i -= 15) {
        displayMgr.setBacklight(i);
        delay(100);
    }
    
    // 恢复正常亮度
    displayMgr.setBacklight(BRIGHTNESS_MAX);
}
```

### 测试4：屏幕旋转测试

```cpp
void rotationTest() {
    uint16_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
    
    for (int rot = 0; rot < 4; rot++) {
        displayMgr.setRotation(rot);
        displayMgr.clear(colors[rot]);
        
        // 显示当前旋转角度
        Serial.printf("Rotation: %d (W=%d, H=%d)\n", 
                     rot, displayMgr.width(), displayMgr.height());
        
        delay(2000);
    }
}
```

## 📊 集成检查清单

在编译和上传前，确保完成以下检查：

### 文件检查
- [ ] TFTDisplay.h 和 .cpp 在 lib/display/ 目录
- [ ] DisplayManager.h 和 .cpp 在 src/services/ 目录
- [ ] Config.h 中有TFT相关的配置定义
- [ ] main.cpp 中包含了DisplayManager初始化

### 硬件检查
- [ ] ESP32-S3 与 TFT屏幕正确接线（见 TFT_WIRING_GUIDE.md）
- [ ] VCC接3.3V，GND正确接地
- [ ] SPI三条线(SCK/MOSI/CS)接对
- [ ] RST、DC、BL三条控制线接对
- [ ] 无虚接或短路

### 编译检查
- [ ] 编译无错误信息
- [ ] 编译无警告（或仅有library相关的预期警告）
- [ ] 代码大小合理（应在2MB以内）

### 上传和运行检查
- [ ] 上传成功，ESP32-S3正常重启
- [ ] 串口输出显示TFT初始化成功
- [ ] 屏幕亮起（有背光）
- [ ] 屏幕显示正确的颜色/内容

## 🎨 常用API速查表

### 初始化和基本控制
```cpp
displayMgr.begin();              // 初始化
displayMgr.end();                // 关闭
displayMgr.clear(COLOR_BLACK);   // 清屏
```

### 绘制图形
```cpp
displayMgr.drawPixel(x, y, color);                    // 像素
displayMgr.drawHLine(x, y, length, color);            // 水平线
displayMgr.drawVLine(x, y, length, color);            // 竖直线
displayMgr.drawRect(x, y, width, height, color);      // 矩形框
displayMgr.fillRect(x, y, width, height, color);      // 填充矩形
```

### 屏幕控制
```cpp
displayMgr.setBacklight(brightness);           // 设置亮度(0-255)
displayMgr.autoAdjustBrightness(lightLevel);   // 自动调光
displayMgr.sleep();                            // 睡眠模式
displayMgr.wakeup();                           // 唤醒
displayMgr.setRotation(rotation);              // 设置旋转(0/1/2/3)
```

### 状态查询
```cpp
displayMgr.width();              // 屏幕宽度
displayMgr.height();             // 屏幕高度
displayMgr.isActive();           // 是否活跃
displayMgr.resetScreenTimeout(); // 重置息屏计时
```

## 📈 性能优化建议

### 1. 刷新率优化

如果屏幕闪烁或刷新速度慢，可以调整：

```cpp
// 在DisplayManager::update()中
if (currentTime - _lastUpdateTime >= 16) {  // 改为约60fps
    _lastUpdateTime = currentTime;
}
```

### 2. SPI速度优化

在Config.h中调整：

```cpp
#define TFT_SPI_FREQ_HZ  80000000  // 提升到80MHz（如果稳定的话）
```

### 3. 背光PWM优化

```cpp
#define BL_PWM_FREQ_HZ   10000    // 提升到10kHz，人眼完全察觉不到闪烁
```

## 🐛 故障排除

### 屏幕无显示

**日志信息**：
```
[TFT] Initializing TFT display...
[TFT] Resetting display...
[TFT] Initializing ST7789 controller...
```

**排查步骤**：
1. 检查串口是否输出初始化日志
2. 用万用表测量VCC(应为3.3V)和GND
3. 检查GPIO引脚是否有输出（示波器或LED）
4. 尝试降低SPI频率到10MHz重新编译

### 屏幕显示花屏

**原因**：SPI通信错误或时序问题

**解决**：
```cpp
// 在Config.h中降低SPI频率
#define TFT_SPI_FREQ_HZ  20000000  // 降到20MHz
```

### 颜色显示不对

**检查**：
- 是否使用了正确的RGB565颜色值
- 是否调用了 `setDisplayMode()` 改变了显示模式

### 背光不工作

**检查**：
- BL引脚是否接GPIO21
- LED驱动是否初始化：
  ```cpp
  ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ_HZ, BL_PWM_RESOLUTION);
  ledcAttachPin(TFT_BL_PIN, BL_PWM_CHANNEL);
  ```

## 📚 扩展功能

### 添加文字显示

当前驱动还不支持文字显示。可以添加：

```cpp
// 在TFTDisplay中添加
void drawChar(uint16_t x, uint16_t y, char c, uint16_t color);
void drawString(uint16_t x, uint16_t y, const char* str, uint16_t color);
```

### 添加图片显示

```cpp
// 显示RGB565原始图片数据
void drawBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
                const uint16_t* data);
```

### 添加动画支持

```cpp
class Animation {
    virtual void update(float dt) = 0;
    virtual void draw(DisplayManager& display) = 0;
};
```

## 📞 常见问题

**Q: SPI总线与其他设备冲突**  
A: TFT使用GPIO10(CS)单独片选，不会影响其他SPI设备，但要避免共享MOSI/SCK/MISO引脚。

**Q: 如何降低功耗**  
A: 使用 `displayMgr.sleep()` 进入睡眠模式，功耗会降至<1mA。

**Q: 支持哪些颜色**  
A: RGB565格式，支持65536种颜色。详见TFTDisplay.h中的COLOR定义。

**Q: 能否使用其他SPI设备**  
A: 可以，但要注意时分复用。建议使用DMA或中断确保数据正确。

---

**现在你已经掌握了TFT屏幕的完整集成方式！开始测试吧！** 🚀
