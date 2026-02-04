# 开发指南

## 📐 软件架构设计

### 1. 分层架构

```
┌─────────────────────────────────────┐
│         应用层 (Application)         │
│  main.cpp - 主程序入口和任务调度    │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│         服务层 (Services)            │
│  SensorManager, DisplayManager,     │
│  PowerManager, NetworkManager...    │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│        驱动层 (Drivers/Lib)          │
│  AHT20Sensor, BMP280Sensor,         │
│  BH1750Sensor, DisplayDriver...     │
└─────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────┐
│         硬件层 (Hardware)            │
│  I2C, SPI, I2S, GPIO, ADC...        │
└─────────────────────────────────────┘
```

### 2. 设计模式

#### 单例模式（Singleton）
所有Manager类使用单例模式，确保全局唯一实例：

```cpp
class SensorManager {
public:
    static SensorManager& getInstance() {
        static SensorManager instance;
        return instance;
    }
    
private:
    SensorManager() = default;
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;
};
```

#### 接口抽象（Interface）
使用纯虚函数定义接口，便于扩展和测试：

```cpp
class ISensor {
public:
    virtual bool begin() = 0;
    virtual SensorData read() = 0;
    virtual bool isAvailable() = 0;
    // ...
};
```

#### 观察者模式（Observer）
使用回调函数实现数据变化通知：

```cpp
typedef void (*DataCallback)(const EnvironmentData& data);

class SensorManager {
    void setDataCallback(DataCallback callback);
private:
    DataCallback _dataCallback;
};
```

### 3. 模块职责划分

| 模块 | 职责 | 依赖 |
|------|------|------|
| SensorManager | 传感器数据采集、滤波 | ISensor接口 |
| DisplayManager | 屏幕渲染、UI更新 | TFT_eSPI库 |
| PowerManager | 电源管理、电量监测 | ADC |
| NetworkManager | WiFi连接、Web服务器 | WiFi, AsyncWebServer |
| AudioManager | 音频录放、VAD检测 | I2S驱动 |
| ButtonManager | 按键检测、防抖处理 | GPIO中断 |
| RTCManager | 时钟同步、闹钟管理 | DS3231驱动 |

## 🔨 开发流程

### 阶段1：基础功能（第1-2周）

1. **传感器驱动开发**
   - [ ] AHT20温湿度传感器
   - [ ] BMP280气压传感器
   - [ ] BH1750光照传感器
   - [ ] 数据滤波算法

2. **显示系统开发**
   - [ ] TFT_eSPI配置
   - [ ] 基础UI框架
   - [ ] PWM背光控制
   - [ ] 自动调光算法

### 阶段2：高级功能（第3-4周）

3. **电源管理**
   - [ ] ADC电池电压检测
   - [ ] 电量百分比计算
   - [ ] 低电保护

4. **网络功能**
   - [ ] WiFi连接管理
   - [ ] Web配置界面
   - [ ] NTP时间同步

### 阶段3：交互功能（第5-6周）

5. **语音交互**
   - [ ] I2S音频驱动
   - [ ] VAD语音检测
   - [ ] 云端ASR对接
   - [ ] TTS语音播放

6. **按键和闹钟**
   - [ ] 按键中断处理
   - [ ] DS3231 RTC驱动
   - [ ] 闹钟逻辑

## 🧪 测试流程

### 单元测试

在 `test/` 目录下创建测试文件：

```cpp
// test/test_sensor.cpp
#include <unity.h>
#include "sensors/aht20/AHT20Sensor.h"

void test_sensor_init() {
    AHT20Sensor sensor;
    TEST_ASSERT_TRUE(sensor.begin());
}

void test_sensor_read() {
    AHT20Sensor sensor;
    sensor.begin();
    SensorData data = sensor.read();
    TEST_ASSERT_EQUAL(SensorStatus::OK, data.status);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_init);
    RUN_TEST(test_sensor_read);
    UNITY_END();
}

void loop() {}
```

运行测试：
```bash
pio test
```

### 集成测试

创建专门的测试任务：

```cpp
void testTask(void* parameter) {
    // 测试传感器读取
    sensorMgr.update();
    EnvironmentData data = sensorMgr.getData();
    assert(data.temperature > -40 && data.temperature < 85);
    
    // 测试显示更新
    displayMgr.update();
    
    vTaskDelete(NULL);
}
```

## 📝 代码规范

### 命名约定

- **类名**: PascalCase - `SensorManager`
- **函数名**: camelCase - `readTemperature()`
- **变量名**: camelCase - `sensorData`
- **成员变量**: 下划线前缀 - `_initialized`
- **常量**: UPPER_CASE - `SENSOR_SAMPLE_INTERVAL_MS`
- **宏定义**: UPPER_CASE - `#define DEBUG_PRINT(x)`

### 注释规范

使用Doxygen风格注释：

```cpp
/**
 * @brief 读取传感器数据
 * @param filterEnabled 是否启用滤波
 * @return SensorData 传感器数据
 * @note 此函数会阻塞80ms等待测量完成
 */
SensorData read(bool filterEnabled = true);
```

### 文件组织

每个类单独一个头文件和源文件：

```
lib/sensors/aht20/
├── AHT20Sensor.h      # 类声明
└── AHT20Sensor.cpp    # 类实现
```

## 🐛 调试技巧

### 1. 使用条件编译

```cpp
#if ENABLE_SERIAL_DEBUG
    Serial.printf("[DEBUG] Value: %d\n", value);
#endif
```

### 2. 分级日志

```cpp
enum LogLevel { ERROR, WARNING, INFO, DEBUG };

void log(LogLevel level, const char* fmt, ...) {
    if (level == ERROR) Serial.print("[ERROR] ");
    // ...
}
```

### 3. 断言检查

```cpp
#define ASSERT(condition, msg) \
    if (!(condition)) { \
        Serial.printf("ASSERT FAILED: %s\n", msg); \
        while(1); \
    }

ASSERT(sensor.isAvailable(), "Sensor not available");
```

### 4. 性能分析

```cpp
class Timer {
    uint32_t _start;
public:
    Timer() : _start(micros()) {}
    ~Timer() {
        uint32_t elapsed = micros() - _start;
        Serial.printf("Elapsed: %lu us\n", elapsed);
    }
};

void someFunction() {
    Timer t;
    // ... 代码 ...
}  // 自动输出耗时
```

## 📊 性能优化

### 1. 内存优化

- 使用 `F()` 宏将字符串存储到Flash
- 避免频繁的动态内存分配
- 使用对象池模式复用对象

```cpp
// 不好的做法
String msg = "Temperature: " + String(temp);

// 好的做法
char buffer[50];
snprintf(buffer, sizeof(buffer), "Temperature: %.2f", temp);
```

### 2. CPU优化

- 使用FreeRTOS任务分离耗时操作
- 利用双核并行处理
- 使用DMA减少CPU占用

```cpp
// Core 0: 网络、音频
xTaskCreatePinnedToCore(networkTask, "Net", 4096, NULL, 2, NULL, 0);

// Core 1: 显示、UI
xTaskCreatePinnedToCore(displayTask, "Display", 4096, NULL, 3, NULL, 1);
```

### 3. 功耗优化

- 使用Light Sleep降低待机功耗
- 关闭不用的外设
- 动态调整CPU频率

```cpp
// 降低CPU频率到160MHz
setCpuFrequencyMhz(160);

// 进入Light Sleep
esp_light_sleep_start();
```

## 🔐 安全建议

1. **WiFi密码保护**
   - 不要硬编码WiFi密码
   - 使用加密存储

2. **API密钥管理**
   - 将API密钥存储在单独的配置文件
   - 使用环境变量或加密存储

3. **输入验证**
   - 验证传感器数据范围
   - 检查网络输入的合法性

4. **OTA更新**
   - 实现安全的OTA更新机制
   - 验证固件签名

## 📦 版本发布

### 版本号规则

使用语义化版本：`Major.Minor.Patch`

- **Major**: 不兼容的API修改
- **Minor**: 新增功能，向下兼容
- **Patch**: Bug修复

### 发布检查清单

- [ ] 所有测试通过
- [ ] 更新README.md
- [ ] 更新CHANGELOG.md
- [ ] 标记版本号（Git Tag）
- [ ] 生成发布说明
- [ ] 编译Release版本
- [ ] 上传固件文件

## 🤝 贡献指南

1. Fork项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

### 代码审查要点

- [ ] 符合代码规范
- [ ] 有足够的注释
- [ ] 有单元测试
- [ ] 无内存泄漏
- [ ] 无编译警告

---

**祝开发顺利！** 🚀
