# TFT 2.8寸屏幕接线指南

## 📋 屏幕规格

- **型号**: GMT028-05 2.8寸IPS屏幕
- **控制器**: ST7789
- **分辨率**: 320×240像素
- **通信方式**: SPI接口
- **供电**: 3.3V

## 🔌 引脚定义

屏幕共8个引脚，从左到右标号为1-8：

| 引脚号 | 符号 | 名称 | 功能 | 对应ESP32引脚 |
|--------|------|------|------|--------------|
| 1 | GND | 电源负极 | 地线 | GND |
| 2 | VCC | 电源正极 | 3.3V电源 | 3V3 |
| 3 | SCK/SCLK | 时钟信号 | SPI时钟 | GPIO12 |
| 4 | SDA/MOSI | 数据信号 | SPI主出从入 | GPIO11 |
| 5 | RST | 复位 | 低电平复位 | GPIO17 |
| 6 | DC | 命令/数据 | 控制命令还是数据 | GPIO18 |
| 7 | CS | 片选 | SPI片选 | GPIO10 |
| 8 | BL | 背光 | PWM控制 | GPIO21 |

> **引脚号标注**: 屏幕FPC连接器上从左向右编号为1-8，对应上表

## 🎯 推荐硬件连接

### 完整连接图

```
ESP32-S3              TFT 2.8寸屏幕
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GND      ─────────→  PIN1 (GND)
3V3      ─────────→  PIN2 (VCC) 
GPIO12   ─────────→  PIN3 (SCK)
GPIO11   ─────────→  PIN4 (SDA/MOSI)
GPIO17   ─────────→  PIN5 (RST)
GPIO18   ─────────→  PIN6 (DC)
GPIO10   ─────────→  PIN7 (CS)
GPIO21   ─────────→  PIN8 (BL)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 面包板连接示例

```
ESP32-S3         连接器(FPC)
┌─────────┐      ┌─────────────────────┐
│         │      │ 1  2  3  4  5  6  7  8 │
│ GND ●───┼──────→ GND VCC SCK SDA RST DC CS BL
│ 3V3 ●───┼──────→ 2
│         │      │
│GPIO10 ●─┼──────→ 7
│GPIO11 ●─┼──────→ 4
│GPIO12 ●─┼──────→ 3
│GPIO17 ●─┼──────→ 5
│GPIO18 ●─┼──────→ 6
│GPIO21 ●─┼──────→ 8
│         │      │
└─────────┘      └─────────────────────┘
```

## ⚡ 电气规格

### 电压要求

| 项目 | 最小 | 典型 | 最大 | 单位 |
|------|------|------|------|------|
| VCC | 2.7 | 3.0 | 3.3 | V |
| 逻辑电平(HIGH) | 0.8×VCC | - | VCC | V |
| 逻辑电平(LOW) | -0.3 | - | 0.2×VCC | V |

### 功耗

| 工作模式 | 功耗 | 备注 |
|---------|------|------|
| 正常工作 | ~50-100mA | 取决于背光亮度 |
| 背光满亮 | ~100mA | 背光为主耗电源 |
| 睡眠模式 | <1mA | 显示关闭 |

## 🔧 接线步骤

### 第一步：准备材料

- ESP32-S3-DevKitC-1 开发板
- 2.8寸TFT屏幕模块
- FPC连接器/杜邦线8根（或直接焊接FPC）
- 3.3V电源（开发板提供）
- 面包板（可选）

### 第二步：连接电源

1. **接地(GND)**
   - 屏幕PIN1 → ESP32 GND
   - 确保所有设备共地

2. **电源(VCC)**
   - 屏幕PIN2 → ESP32 3V3
   - **注意**: 必须用3.3V，不能用5V！

### 第三步：连接SPI总线

按照GPIO分配连接SPI信号：

```
屏幕信号     ESP32引脚    功能
────────────────────────────
SCK(PIN3)  ← GPIO12      SPI时钟
MOSI(PIN4) ← GPIO11      数据线
CS(PIN7)   ← GPIO10      片选信号
```

### 第四步：连接控制引脚

```
屏幕信号     ESP32引脚    功能
────────────────────────────
DC(PIN6)   ← GPIO18      命令/数据选择
RST(PIN5)  ← GPIO17      硬件复位
BL(PIN8)   ← GPIO21      背光PWM
```

### 第五步：验证连接

接线完成后，检查清单：

- [ ] GND连接无误（两个引脚）
- [ ] VCC连接无误（3.3V，仅一个引脚）
- [ ] SCK、MOSI、CS三条SPI线连接正确
- [ ] DC、RST、BL控制线连接正确
- [ ] 没有虚接或短路
- [ ] 没有接反或错接

## 🎨 软件使用示例

### 基础初始化

```cpp
#include "TFTDisplay.h"
#include "config/Config.h"

TFTDisplay tft;

void setup() {
    Serial.begin(115200);
    
    // 初始化屏幕
    if (!tft.begin()) {
        Serial.println("TFT initialization failed!");
        while(1);
    }
    
    Serial.println("TFT initialized successfully!");
    
    // 清屏为白色
    tft.fillScreen(COLOR_WHITE);
}

void loop() {
    // 填充为红色
    tft.fillScreen(COLOR_RED);
    delay(1000);
    
    // 填充为绿色
    tft.fillScreen(COLOR_GREEN);
    delay(1000);
    
    // 填充为蓝色
    tft.fillScreen(COLOR_BLUE);
    delay(1000);
}
```

### 绘制图形

```cpp
void drawShapes() {
    // 清屏
    tft.fillScreen(COLOR_BLACK);
    
    // 绘制矩形
    tft.drawRect(10, 10, 100, 50, COLOR_RED);
    
    // 填充矩形
    tft.fillRect(150, 10, 80, 80, COLOR_GREEN);
    
    // 绘制水平线
    tft.drawHLine(10, 100, 200, COLOR_BLUE);
    
    // 绘制竖直线
    tft.drawVLine(120, 150, 100, COLOR_YELLOW);
    
    // 绘制像素点
    for (int i = 0; i < 50; i++) {
        tft.drawPixel(50 + i, 250 + (i % 10), COLOR_MAGENTA);
    }
}
```

### 背光控制

```cpp
void backlightDemo() {
    // 逐级增亮
    for (int i = 0; i <= 255; i += 10) {
        tft.setBacklight(i);
        delay(50);
    }
    
    // 逐级降暗
    for (int i = 255; i >= 0; i -= 10) {
        tft.setBacklight(i);
        delay(50);
    }
}
```

### 显示控制

```cpp
void displayControl() {
    tft.fillScreen(COLOR_BLUE);
    delay(1000);
    
    // 关闭显示
    tft.display(false);
    delay(2000);
    
    // 打开显示
    tft.display(true);
    delay(1000);
    
    // 进入睡眠
    tft.sleep();
    delay(3000);
    
    // 唤醒
    tft.wakeup();
}
```

## 🐛 常见问题

### Q1: 屏幕无显示

**原因**: 通常是接线问题或初始化失败

**解决方案**:
1. 检查VCC和GND连接是否正确
2. 检查SPI三条线(SCK, MOSI, CS)是否接对
3. 检查RST和DC是否接对
4. 使用万用表测量各引脚电压
5. 串口查看初始化log信息

### Q2: 屏幕显示异常(花屏)

**原因**: SPI通信速度过快或时序问题

**解决方案**:
1. 降低SPI时钟频率（修改`Config.h`中的`TFT_SPI_FREQ_HZ`）
2. 缩短杜邦线长度，改用焊接连接
3. 添加100nF电容在VCC和GND之间去耦

### Q3: 背光不亮

**原因**: BL引脚未接或PWM配置错误

**解决方案**:
1. 检查BL(PIN8)是否接到GPIO21
2. 检查是否调用了`tft.backlight(true)`
3. 检查背光电压是否为3.3V

### Q4: 色彩显示不对

**原因**: 像素格式设置错误或显示模式反色

**解决方案**:
1. 确保像素格式为RGB565: `tft.setPixelFormat(PixelFormat::RGB565);`
2. 如需反色显示: `tft.setDisplayMode(DisplayMode::INVERT);`

### Q5: 屏幕闪烁或刷新不稳定

**原因**: 电源不稳定或SPI时序不佳

**解决方案**:
1. 增加10µF电解电容在VCC和GND之间
2. 使用高质量USB线或外接电源
3. 缩短连接线，避免干扰

## 📐 坐标系统

屏幕坐标系统如下（旋转0）：

```
(0,0)                      (239,0)
  ●──────────────────────────●
  │                          │
  │                          │
  │      320×240 IPS         │
  │                          │
  │                          │
  ●──────────────────────────●
(0,319)                  (239,319)
```

- **X轴**: 0-239 (水平)
- **Y轴**: 0-319 (竖直)
- **原点**: 左上角 (0,0)

## 🔄 屏幕旋转

支持0/90/180/270度旋转：

```cpp
tft.setRotation(0);  // 正常显示
tft.setRotation(1);  // 90度
tft.setRotation(2);  // 180度
tft.setRotation(3);  // 270度
```

旋转后屏幕尺寸变化：
- 旋转0/2: 240×320
- 旋转1/3: 320×240

## 📊 性能指标

| 指标 | 值 | 备注 |
|------|-----|------|
| 最大刷新率 | ~60Hz | 取决于SPI时钟 |
| 填充整屏耗时 | ~50ms | 40MHz SPI频率 |
| 响应时间 | <5ms | 单像素绘制 |
| 视角 | IPS 178° | 上下左右都可视 |

## 📝 集成提示

1. **在Config.h中确保引脚定义正确**
   ```cpp
   #define TFT_CS_PIN          10
   #define TFT_MOSI_PIN        11
   #define TFT_SCLK_PIN        12
   #define TFT_RST_PIN         17
   #define TFT_DC_PIN          18
   #define TFT_BL_PIN          21
   ```

2. **将驱动文件放在正确位置**
   ```
   lib/display/
   ├── TFTDisplay.h
   └── TFTDisplay.cpp
   ```

3. **在main.cpp中使用**
   ```cpp
   #include "lib/display/TFTDisplay.h"
   TFTDisplay tft;
   tft.begin();
   ```

## 🎁 配色方案

RGB565常用颜色值：

```cpp
#define COLOR_BLACK        0x0000  // 黑色
#define COLOR_BLUE         0x001F  // 蓝色
#define COLOR_GREEN        0x07E0  // 绿色
#define COLOR_RED          0xF800  // 红色
#define COLOR_WHITE        0xFFFF  // 白色
#define COLOR_YELLOW       0xFFE0  // 黄色
#define COLOR_CYAN         0x07FF  // 青色
#define COLOR_MAGENTA      0xF81F  // 品红色
```

---

**接线完成后，使用提供的示例代码测试屏幕功能！** 🎉
