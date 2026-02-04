# 快速开始指南

## 🚀 5分钟快速上手

### 第一步：硬件连接

#### 最小系统（仅传感器测试）

只需连接3个传感器即可开始测试：

```
ESP32-S3          AHT20+BMP280模块
GPIO8  (SDA) ──→  SDA
GPIO9  (SCL) ──→  SCL
3.3V        ──→  VCC
GND         ──→  GND

ESP32-S3          BH1750模块  
GPIO8  (SDA) ──→  SDA (共用)
GPIO9  (SCL) ──→  SCL (共用)
3.3V        ──→  VCC
GND         ──→  GND
```

**重要提示**：
- 如果模块没有自带上拉电阻，需要在SDA和SCL各加一个4.7kΩ电阻到3.3V
- 确保所有GND连接到一起（共地）

### 第二步：软件安装

1. **安装VSCode**
   - 下载：https://code.visualstudio.com/
   - 安装并启动

2. **安装PlatformIO**
   - VSCode左侧扩展图标
   - 搜索 "PlatformIO IDE"
   - 点击安装（约5-10分钟）

3. **打开项目**
   - VSCode → 文件 → 打开文件夹
   - 选择 `esp32-smart-tv` 目录
   - 等待PlatformIO自动下载依赖库（首次约5-15分钟）

### 第三步：编译上传

1. **连接开发板**
   - 使用USB-C线连接ESP32-S3到电脑
   - 驱动会自动安装（Windows可能需要手动安装CP210x驱动）

2. **编译**
   - 点击底部状态栏的 ✓（Build）图标
   - 等待编译完成（首次约3-5分钟）

3. **上传**
   - 点击底部状态栏的 →（Upload）图标
   - 看到 "SUCCESS" 即上传成功

4. **查看输出**
   - 点击底部状态栏的 🔌（Serial Monitor）图标
   - 波特率选择 115200
   - 观察串口输出

### 第四步：验证功能

如果一切正常，你会在串口看到：

```
========================================
  ESP32-S3 Smart Desktop TV
  Version: 1.0.0
========================================

[SensorManager] Initializing...
[SensorManager] Scanning I2C bus...
  Found device at 0x23
  Found device at 0x38
  Found device at 0x76
  Total devices found: 3

[AHT20] Initializing...
[AHT20] Initialization successful
[BMP280] Initializing...
[BMP280] Initialization successful
[BH1750] Initializing...
[BH1750] Initialization successful

[SensorManager] T=25.30°C, H=55.20%RH, P=1013.5hPa
[SensorManager] Light=350lux
```

## 🔧 常见问题快速解决

### ❌ 编译失败

**问题**：Library not found
**解决**：等待PlatformIO自动下载库，或手动清理重新编译：
```bash
pio lib install
pio run --target clean
pio run
```

### ❌ 上传失败

**问题**：Failed to connect
**解决**：
1. 检查USB线是否支持数据传输（不是充电线）
2. 按住ESP32-S3的BOOT按键，然后点击上传
3. 上传开始后松开BOOT按键

### ❌ 串口无输出

**问题**：Serial monitor blank
**解决**：
1. 确认波特率设置为115200
2. 点击RST按键重启开发板
3. 检查USB连接是否稳定

### ❌ I2C设备扫描不到

**问题**：No devices found
**解决**：
1. 检查VCC是否接3.3V（不是5V！）
2. 检查SDA/SCL是否接反
3. 用万用表测量VCC引脚（应为3.3V）
4. 添加4.7kΩ上拉电阻

## 📝 下一步

现在你已经完成了基础功能测试，可以继续：

1. **添加屏幕显示**
   - 查看 `docs/DISPLAY_GUIDE.md`
   
2. **添加WiFi功能**
   - 查看 `docs/WIFI_GUIDE.md`
   
3. **添加语音功能**
   - 查看 `docs/AUDIO_GUIDE.md`

## 🎯 测试代码示例

如果想单独测试某个传感器，可以修改 `src/main.cpp`：

```cpp
void loop() {
    // 读取温湿度
    float temp = sensorMgr.getTemperature();
    float humi = sensorMgr.getHumidity();
    Serial.printf("T=%.2f°C, H=%.2f%%RH\n", temp, humi);
    
    // 读取气压
    float press = sensorMgr.getPressure();
    Serial.printf("P=%.1fhPa\n", press);
    
    // 读取光照
    float light = sensorMgr.getLightLevel();
    Serial.printf("Light=%.0flux\n", light);
    
    delay(2000);  // 2秒更新一次
}
```

## 💡 提示

- 串口输出会自动显示传感器初始化状态
- 温度范围：-40°C ~ 85°C
- 湿度范围：0 ~ 100%RH
- 气压范围：300 ~ 1100 hPa
- 光照范围：0 ~ 65535 lux

---

**遇到问题？** 查看 `docs/TROUBLESHOOTING.md` 或提交Issue
