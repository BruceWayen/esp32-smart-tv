# ESP32-S3 智能桌面小电视

基于ESP32-S3-DevKitC-1开发板的多功能智能桌面设备

## 📋 项目简介

这是一个集成环境监测、语音交互、自动调光、双供电等功能的智能桌面小电视项目。

### 核心功能
1. ✅ **环境监测** - AHT20+BMP280实时显示温湿度和气压
2. ✅ **智能调光** - BH1750光照传感器自动调节屏幕亮度
3. 🔨 **语音交互** - INMP441麦克风+MAX98357A功放，本地唤醒+云端识别
4. 🔨 **双供电** - TP4056+MP1584EN，USB/电池自动切换
5. 🔨 **屏幕显示** - ILI9341 2.8寸彩屏，320×240分辨率
6. 🔨 **精准计时** - DS3231高精度RTC，断网防漂移
7. 🔨 **WiFi配置** - Web端配置，AP热点模式
8. 🔨 **按键控制** - 2个独立按键，短按/长按多功能
9. 🔨 **主题定制** - JSON配置文件，Web端可视化修改
10. 🔨 **闹钟功能** - 自定义语音，按键/语音双控制

> ✅ = 已完成  🔨 = 开发中  📝 = 规划中

## 🛠️ 硬件清单

| 器件 | 型号 | 数量 | 用途 |
|------|------|------|------|
| 主控板 | ESP32-S3-DevKitC-1 (N8R16) | 1 | 核心控制器 |
| 温湿压传感器 | AHT20+BMP280集成模块 | 1 | 环境监测 |
| 光照传感器 | BH1750 | 1 | 自动调光 |
| 显示屏 | ILI9341 2.8寸 SPI屏 | 1 | UI显示 |
| 麦克风 | INMP441数字麦克风 | 1 | 语音采集 |
| 功放 | MAX98357A I2S功放 | 1 | 音频输出 |
| 喇叭 | 2W 8Ω扬声器 | 1 | 声音播放 |
| 充电模块 | TP4056 | 1 | 电池充电 |
| 降压模块 | MP1584EN | 1 | 电源稳压 |
| 锂电池 | 3.7V 2000mAh | 1 | 备用电源 |
| RTC模块 | DS3231 | 1 | 时钟计时 |
| 按键 | 轻触开关 | 2 | 功能控制 |

**总成本：约 ¥150-200**

## 📌 引脚定义

### I2C总线（共用）
- **SDA**: GPIO8
- **SCL**: GPIO9
- 设备地址：
  - AHT20: 0x38
  - BMP280: 0x76
  - BH1750: 0x23
  - DS3231: 0x68

### SPI屏幕（ILI9341）
- **CS**: GPIO10
- **MOSI**: GPIO11
- **SCK**: GPIO12
- **RST**: GPIO17
- **DC**: GPIO18
- **BL**: GPIO21 (PWM背光)

### I2S音频
- **麦克风（INMP441）**
  - SCK: GPIO4
  - WS: GPIO5
  - SD: GPIO6
- **功放（MAX98357A）**
  - BCLK: GPIO15
  - LRC: GPIO16
  - DOUT: GPIO7

### 其他
- **按键1**: GPIO13
- **按键2**: GPIO14
- **RTC中断**: GPIO2
- **电池检测**: GPIO1 (ADC)

## 📁 项目结构

```
esp32-smart-tv/
├── platformio.ini          # PlatformIO配置
├── README.md               # 项目说明
├── include/                # 头文件目录
│   ├── config/             # 配置文件
│   │   └── Config.h        # 全局配置
│   └── interfaces/         # 接口定义
│       └── ISensor.h       # 传感器接口
├── src/                    # 源代码目录
│   ├── main.cpp            # 主程序
│   ├── core/               # 核心功能
│   ├── drivers/            # 硬件驱动
│   ├── services/           # 服务管理器
│   │   ├── SensorManager   # 传感器管理
│   │   ├── DisplayManager  # 显示管理
│   │   ├── PowerManager    # 电源管理
│   │   ├── NetworkManager  # 网络管理
│   │   ├── AudioManager    # 音频管理
│   │   ├── ButtonManager   # 按键管理
│   │   └── RTCManager      # RTC管理
│   ├── ui/                 # UI组件
│   └── utils/              # 工具函数
├── lib/                    # 自定义库
│   ├── sensors/            # 传感器驱动
│   │   ├── aht20/
│   │   ├── bmp280/
│   │   └── bh1750/
│   ├── display/            # 显示驱动
│   ├── audio/              # 音频驱动
│   ├── power/              # 电源驱动
│   ├── rtc/                # RTC驱动
│   └── network/            # 网络驱动
├── data/                   # 数据文件
│   ├── sounds/             # 音频文件
│   ├── themes/             # 主题配置
│   └── fonts/              # 字体文件
└── test/                   # 测试代码
```

## 🚀 快速开始

### 环境准备

1. **安装VSCode**
   ```bash
   # 下载：https://code.visualstudio.com/
   ```

2. **安装PlatformIO插件**
   - VSCode扩展市场搜索 "PlatformIO IDE"
   - 点击安装

3. **克隆项目**
   ```bash
   git clone <your-repo-url>
   cd esp32-smart-tv
   ```

### 编译上传

1. **打开项目**
   - VSCode -> File -> Open Folder -> 选择项目目录

2. **编译**
   - 点击底部状态栏的 ✓ 图标
   - 或按 `Ctrl+Alt+B`

3. **上传**
   - 连接ESP32-S3开发板到电脑
   - 点击底部状态栏的 → 图标
   - 或按 `Ctrl+Alt+U`

4. **查看串口输出**
   - 点击底部状态栏的 🔌 图标
   - 或按 `Ctrl+Alt+S`

## 🔧 配置说明

所有配置集中在 `include/config/Config.h` 文件中：

### 传感器配置
```cpp
#define SENSOR_SAMPLE_INTERVAL_MS   2000    // 采样间隔2秒
#define SENSOR_FILTER_WINDOW_SIZE   5       // 滤波窗口
```

### 显示配置
```cpp
#define TFT_SPI_FREQ_HZ             40000000    // SPI时钟40MHz
#define BRIGHTNESS_MAX              255         // 最大亮度
```

### WiFi配置
```cpp
#define AP_SSID                     "ESP32-SmartTV"
#define AP_PASSWORD                 "12345678"
```

### 功能开关
```cpp
#define ENABLE_SERIAL_DEBUG         true    // 串口调试
#define ENABLE_WIFI                 true    // WiFi功能
#define ENABLE_VOICE                true    // 语音功能
```

## 📖 开发指南

### 添加新传感器

1. 继承 `ISensor` 接口
2. 实现必要的方法
3. 在 `SensorManager` 中注册

示例：
```cpp
class NewSensor : public ISensor {
    bool begin() override;
    SensorData read() override;
    // ...
};
```

### 添加新功能

1. 在 `src/services/` 创建管理器类
2. 实现单例模式
3. 在 `main.cpp` 中初始化

### 调试技巧

1. **使用串口调试**
   ```cpp
   DEBUG_PRINTLN("Debug message");
   DEBUG_PRINTF("Value: %d\n", value);
   ```

2. **I2C设备扫描**
   - 上传代码后查看串口输出
   - 会自动扫描并显示所有I2C设备地址

3. **查看内存使用**
   ```cpp
   Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
   ```

## 🐛 常见问题

### 1. 编译错误：库未找到
**解决**: PlatformIO会自动下载依赖库，首次编译需要等待

### 2. 上传失败
**解决**: 
- 检查USB连接
- 确认COM口选择正确
- 按住BOOT按键再上传

### 3. I2C设备扫描不到
**解决**:
- 检查接线（VCC、GND、SDA、SCL）
- 确认供电电压为3.3V
- 添加4.7kΩ上拉电阻

### 4. 屏幕不显示
**解决**:
- 检查SPI接线
- 降低SPI时钟频率（改为20MHz）
- 确认背光引脚有PWM输出

## 📚 参考资料

- [ESP32-S3技术手册](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_cn.pdf)
- [PlatformIO文档](https://docs.platformio.org/)
- [Arduino-ESP32文档](https://docs.espressif.com/projects/arduino-esp32/)
- [TFT_eSPI库文档](https://github.com/Bodmer/TFT_eSPI)

## 📄 许可证

MIT License

## 👥 贡献

欢迎提交Issue和Pull Request！

## 📞 联系方式

- 作者：Your Name
- 邮箱：your.email@example.com
- 项目地址：https://github.com/yourusername/esp32-smart-tv

---

**Happy Coding! 🎉**
