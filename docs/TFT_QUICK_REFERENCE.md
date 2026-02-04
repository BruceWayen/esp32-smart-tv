# TFT 2.8寸屏幕接线参考卡

## 🎯 快速对照表

### 引脚映射

```
TFT屏幕          ESP32-S3          功能说明
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PIN1 GND    ←→  GND             地线(电源负极)
PIN2 VCC    ←→  3V3             电源(+3.3V)
PIN3 SCK    ←→  GPIO12          SPI时钟
PIN4 SDA    ←→  GPIO11          SPI数据(MOSI)
PIN5 RST    ←→  GPIO17          硬件复位
PIN6 DC     ←→  GPIO18          命令/数据选择
PIN7 CS     ←→  GPIO10          片选信号
PIN8 BL     ←→  GPIO21          背光PWM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 📐 物理接线示意

### 方式1：FPC连接器接线

```
TFT屏幕FPC (从左到右看，标号为1-8)

┌──────────────────────────┐
│ ① ② ③ ④ ⑤ ⑥ ⑦ ⑧ │  屏幕面向你
└──────────────────────────┘

接线对照:
①  → GND
②  → 3V3
③  → GPIO12
④  → GPIO11
⑤  → GPIO17
⑥  → GPIO18
⑦  → GPIO10
⑧  → GPIO21
```

### 方式2：杜邦线接线

```
ESP32-S3正面视图          TFT屏幕FPC接线
┌───────────────┐        ┌─────────────┐
│ ●●●●● GND    │◄───┐   │ 1 GND       │
│ ●●●●● 3V3    │◄───├──→ 2 VCC       │
│ GPIO21     ●●│───┐│   │ 3 SCK       │
│ GPIO18     ●●│─┐ ││   │ 4 SDA       │
│ GPIO17     ●●│─┤ ││   │ 5 RST       │
│ GPIO12     ●●│─┤ │└──→ 6 DC        │
│ GPIO11     ●●│─┤ └───→ 7 CS        │
│ GPIO10     ●●│─┴──────→ 8 BL        │
└───────────────┘       └─────────────┘
```

## 🔌 面包板连接示意

```
     GND   3V3   GPIO10 GPIO11 GPIO12 GPIO17 GPIO18 GPIO21
      │     │      │      │      │      │      │      │
    ┌─┴─────┴──┬───┴──┬───┴──┬───┴──┬───┴──┬───┴──┬───┴──┐
    │           │      │      │      │      │      │      │
    │     ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●●
    │     ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●●
    │     ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●● ●●●●●
    │
    │     TFT屏幕FPC (从右向左插入)
    │     ┌─────────────────────────┐
    └────→ 1GND 2VCC 3 4 5 6 7 8   │
          └─────────────────────────┘
                    │  │  │ │ │ │ │
                    │  │  │ │ │ │ └─→ GPIO21(BL)
                    │  │  │ │ │ └───→ GPIO18(DC)
                    │  │  │ │ └─────→ GPIO17(RST)
                    │  │  │ └───────→ GPIO11(SDA)
                    │  │  └─────────→ GPIO12(SCK)
                    │  └───────────→ 3V3(VCC)
                    └─────────────→ GND
```

## ✅ 接线检查清单

逐项检查确保无误：

```
□ 电源连接
  □ PIN1(GND) → ESP32 GND
  □ PIN2(VCC) → ESP32 3V3
  □ VCC和GND之间无短路

□ SPI总线(3条必须线)
  □ PIN3(SCK) → GPIO12
  □ PIN4(SDA) → GPIO11  
  □ PIN7(CS)  → GPIO10

□ 控制信号(3条)
  □ PIN5(RST) → GPIO17
  □ PIN6(DC)  → GPIO18
  □ PIN8(BL)  → GPIO21

□ 物理检查
  □ 所有接线紧固，无虚接
  □ 无焊接点滑脱
  □ 杜邦线接头接触良好
  □ 无交叉或缠绕
```

## 🎛️ GPIO引脚功能表

| GPIO | 标号 | 功能 | 可复用 | 备注 |
|------|------|------|--------|------|
| GPIO10 | IO10 | CS片选 | 否 | SPI片选专用 |
| GPIO11 | IO11 | MOSI | 否 | SPI数据线 |
| GPIO12 | IO12 | SCK | 否 | SPI时钟线 |
| GPIO17 | IO17 | RST | 是 | 可用其他GPIO替代 |
| GPIO18 | IO18 | DC | 是 | 可用其他GPIO替代 |
| GPIO21 | IO21 | BL(PWM) | 是 | 必须支持PWM |

## 🔧 软件配置检查

确保 `include/config/Config.h` 中的定义如下：

```cpp
#define TFT_CS_PIN          10      // ✓ 检查
#define TFT_MOSI_PIN        11      // ✓ 检查
#define TFT_SCLK_PIN        12      // ✓ 检查
#define TFT_RST_PIN         17      // ✓ 检查
#define TFT_DC_PIN          18      // ✓ 检查
#define TFT_BL_PIN          21      // ✓ 检查

#define TFT_SPI_FREQ_HZ     40000000 // 40MHz
#define BL_PWM_CHANNEL      0         // PWM通道0
#define BL_PWM_FREQ_HZ      5000      // 5kHz频率
```

## 🚨 常见接线错误

### ❌ 错误1：接反VCC和GND
```
错误: VCC → GND, GND → 3V3
后果: 屏幕可能烧坏!
修复: 立即断电，检查后再接
```

### ❌ 错误2：CS和DC引脚接反
```
错误: CS → GPIO18, DC → GPIO10
后果: 屏幕不响应或显示混乱
修复: 交换两条线
```

### ❌ 错误3：用5V代替3.3V
```
错误: VCC → 5V电源
后果: 屏幕可能烧坏或损坏!
修复: 必须用3.3V
```

### ❌ 错误4：缺少GND连接
```
错误: 只连接VCC，未连接GND
后果: 屏幕无法工作
修复: 确保GND (PIN1)正确连接
```

### ❌ 错误5：SCK和MOSI接反
```
错误: SCK → GPIO11, MOSI → GPIO12
后果: 显示异常或无法通信
修复: 检查是否接反，注意SPI规范
```

## 📊 电气参数速查

### 工作条件
- 工作温度: 0°C ~ 50°C
- 存储温度: -20°C ~ 70°C
- 工作电压: 2.7V ~ 3.3V

### 电流参数
- VCC供电电流: 30-100mA (不含背光)
- 背光电流: 50-100mA @ 3.3V
- 总功耗: 80-200mW

### SPI通信
- 最大时钟频率: 100MHz
- 建议频率: 40MHz-80MHz
- 数据格式: 8bit SPI, MSB优先

## 🎬 快速测试

接线完成后，上传这个测试代码：

```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // 测试屏幕初始化
    if (!displayMgr.begin()) {
        Serial.println("ERROR: Display init failed!");
        return;
    }
    
    Serial.println("Display initialized successfully!");
    
    // 测试背光
    Serial.println("Testing backlight...");
    displayMgr.setBacklight(255);
    delay(500);
    displayMgr.setBacklight(0);
    delay(500);
    displayMgr.setBacklight(255);
    
    // 测试清屏
    Serial.println("Testing colors...");
    displayMgr.clear(COLOR_RED);
    delay(500);
    displayMgr.clear(COLOR_GREEN);
    delay(500);
    displayMgr.clear(COLOR_BLUE);
    
    Serial.println("All tests passed!");
}

void loop() {
    delay(1000);
}
```

期望输出：
```
Display initialized successfully!
Testing backlight...
Testing colors...
All tests passed!
```

## 📞 快速排查流程

```
屏幕不亮
  ├─ 检查VCC是否为3.3V
  │  └─ NO → 改接3.3V
  ├─ 检查背光是否打开
  │  └─ NO → 调用 displayMgr.backlight(true)
  └─ 检查SPI通信
     └─ 使用示波器检查GPIO12(SCK)是否有时钟信号

屏幕显示花屏
  ├─ 降低SPI频率到20MHz
  ├─ 缩短杜邦线长度
  └─ 添加去耦电容(100nF)

屏幕背光不亮
  ├─ 检查GPIO21是否接BL
  ├─ 检查PWM是否初始化
  └─ 用万用表测BL引脚电压

屏幕响应慢
  ├─ 增加SPI频率到80MHz
  ├─ 减少屏幕刷新区域
  └─ 使用DMA加速传输
```

---

**打印本卡片，接线时随时查阅！** 📋
