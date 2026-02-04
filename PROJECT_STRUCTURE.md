# 项目结构说明

```
esp32-smart-tv/
│
├── platformio.ini              # PlatformIO配置文件
├── README.md                   # 项目说明文档
├── PROJECT_STRUCTURE.md        # 本文件
├── .gitignore                  # Git忽略文件
│
├── include/                    # 全局头文件
│   ├── config/
│   │   └── Config.h            # 全局配置（引脚、参数等）
│   └── interfaces/
│       └── ISensor.h           # 传感器接口抽象类
│
├── src/                        # 源代码
│   ├── main.cpp                # 主程序入口
│   ├── core/                   # 核心功能（预留）
│   ├── drivers/                # 硬件驱动（预留）
│   ├── services/               # 服务管理器
│   │   ├── SensorManager.h     # 传感器管理器（头文件）
│   │   └── SensorManager.cpp   # 传感器管理器（实现）
│   ├── ui/                     # UI组件（预留）
│   └── utils/                  # 工具函数（预留）
│
├── lib/                        # 自定义库
│   ├── sensors/                # 传感器驱动库
│   │   ├── aht20/
│   │   │   ├── AHT20Sensor.h
│   │   │   └── AHT20Sensor.cpp
│   │   ├── bmp280/
│   │   │   ├── BMP280Sensor.h
│   │   │   └── BMP280Sensor.cpp
│   │   └── bh1750/
│   │       ├── BH1750Sensor.h
│   │       └── BH1750Sensor.cpp
│   │
│   ├── display/                # 显示驱动（待实现）
│   │   ├── ili9341/
│   │   └── ui_components/
│   │
│   ├── audio/                  # 音频驱动（待实现）
│   │   ├── microphone/
│   │   ├── speaker/
│   │   └── vad/
│   │
│   ├── power/                  # 电源管理（待实现）
│   │   ├── battery/
│   │   └── charging/
│   │
│   ├── rtc/                    # RTC驱动（待实现）
│   │   ├── ds3231/
│   │   └── alarm/
│   │
│   └── network/                # 网络驱动（待实现）
│       ├── wifi/
│       └── api/
│
├── data/                       # 数据文件
│   ├── sounds/                 # 音频文件（WAV格式）
│   ├── themes/                 # 主题配置（JSON格式）
│   └── fonts/                  # 字体文件
│
├── test/                       # 单元测试
│
├── docs/                       # 文档
│   ├── DEVELOPMENT.md          # 开发指南
│   └── QUICK_START.md          # 快速开始
│
└── .vscode/                    # VSCode配置
    ├── settings.json
    └── extensions.json
```

## 文件说明

### 核心文件

- **platformio.ini**: PlatformIO项目配置，定义开发板、依赖库等
- **src/main.cpp**: 程序入口，初始化和任务调度
- **include/config/Config.h**: 集中管理所有配置参数

### 服务管理器

- **SensorManager**: 统一管理所有传感器，提供数据采集和滤波
- **DisplayManager**: 屏幕显示管理（待实现）
- **PowerManager**: 电源管理（待实现）
- **NetworkManager**: 网络管理（待实现）
- **AudioManager**: 音频管理（待实现）
- **ButtonManager**: 按键管理（待实现）
- **RTCManager**: RTC时钟管理（待实现）

### 驱动库

每个硬件模块都有独立的驱动类：
- 继承ISensor接口（传感器）
- 实现begin(), read(), isAvailable()等方法
- 提供硬件抽象层

## 开发顺序

1. ✅ **阶段1**: 传感器驱动（AHT20, BMP280, BH1750）
2. 🔨 **阶段2**: 显示驱动和UI框架
3. 📝 **阶段3**: 电源管理和WiFi
4. 📝 **阶段4**: 音频和语音交互
5. 📝 **阶段5**: 完整功能整合

## 配置管理

所有可配置参数都在 `include/config/Config.h` 中定义：
- 硬件引脚定义
- I2C设备地址
- 采样间隔
- 滤波参数
- 功能开关

修改配置无需改动业务代码，重新编译即可。

## 依赖库

通过PlatformIO自动管理，定义在 `platformio.ini`:
- Adafruit_AHTX0: AHT20驱动
- Adafruit_BMP280: BMP280驱动
- BH1750: BH1750驱动
- TFT_eSPI: 屏幕驱动
- ArduinoJson: JSON解析
- 等等...

