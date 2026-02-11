/**
 * GMT028-05 2.8" TFT LCD Driver for ESP32-S3
 * Controller: ST7789
 * Resolution: 240x320
 * Interface: SPI
 *
 * 接线说明 (请根据实际情况修改引脚定义):
 * TFT -> ESP32-S3
 * GND -> GND
 * VCC -> 3.3V
 * SCK -> GPIO12
 * SDA -> GPIO11
 * RST -> GPIO10
 * DC  -> GPIO9
 * CS  -> GPIO8
 */

#include <Arduino.h>
#include <SPI.h>

// =============== 引脚定义 (请根据实际接线修改) ===============
#define TFT_CS 8    // 片选
#define TFT_DC 9    // 数据/命令选择
#define TFT_RST 10  // 复位
#define TFT_MOSI 11 // SDA数据线
#define TFT_SCLK 12 // SCK时钟线

// 注意: 7脚屏幕没有背光控制引脚，背光应该是常亮的

// =============== 屏幕参数 ===============
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// =============== 颜色定义 (标准RGB565格式) ===============
// 根据color_diagnostic.cpp第一轮测试(0x00模式)显示正确，使用标准定义
#define BLACK 0x0000   // 黑色
#define RED 0xF800     // 红色 
#define GREEN 0x07E0   // 绿色
#define BLUE 0x001F    // 蓝色
#define YELLOW 0xFFE0  // 黄色
#define CYAN 0x07FF    // 青色
#define MAGENTA 0xF81F // 品红
#define WHITE 0xFFFF   // 白色

// =============== 显示方向 ===============
#define PORTRAIT 0
#define LANDSCAPE 1

// SPI设置
SPIClass spi = SPIClass(HSPI);

// =============== 辅助函数 ===============
template <typename T>
void swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

// =============== 底层函数 ===============

void writeCommand(uint8_t cmd)
{
    digitalWrite(TFT_DC, LOW); // 命令模式
    digitalWrite(TFT_CS, LOW); // 选中
    spi.transfer(cmd);
    digitalWrite(TFT_CS, HIGH); // 取消选中
}

void writeData(uint8_t data)
{
    digitalWrite(TFT_DC, HIGH); // 数据模式
    digitalWrite(TFT_CS, LOW);  // 选中
    spi.transfer(data);
    digitalWrite(TFT_CS, HIGH); // 取消选中
}

void writeData16(uint16_t data)
{
    digitalWrite(TFT_DC, HIGH); // 数据模式
    digitalWrite(TFT_CS, LOW);  // 选中
    spi.transfer16(data);
    digitalWrite(TFT_CS, HIGH); // 取消选中
}

// =============== 屏幕初始化 ===============

void TFT_Init()
{
    // 硬件复位
    digitalWrite(TFT_RST, LOW);
    delay(100);
    digitalWrite(TFT_RST, HIGH);
    delay(120);

    // ST7789 初始化序列
    writeCommand(0x3A); // 接口像素格式
    writeData(0x05);    // 16位色(RGB565)

    writeCommand(0xC5); // VCOM设置
    writeData(0x1A);

    writeCommand(0x36); // 内存访问控制(屏幕方向)
    writeData(0x00);    // 正常竖屏显示，RGB模式
    // 其他方向设置:
    // 0x00 - 竖屏 (0度)
    // 0xA0 - 横屏 (90度)
    // 0xC0 - 竖屏倒转 (180度)
    // 0x60 - 横屏倒转 (270度)

    // Porch设置
    writeCommand(0xB2);
    writeData(0x05);
    writeData(0x05);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    // Gate Control
    writeCommand(0xB7);
    writeData(0x05);

    // VCOM设置
    writeCommand(0xBB);
    writeData(0x3F);

    // 电源控制
    writeCommand(0xC0);
    writeData(0x2C);

    // VDV和VRH使能
    writeCommand(0xC2);
    writeData(0x01);

    // VRH设置
    writeCommand(0xC3);
    writeData(0x0F);

    // VDV设置
    writeCommand(0xC4);
    writeData(0x20);

    // 帧率控制
    writeCommand(0xC6);
    writeData(0x01); // 111Hz

    // 电源控制
    writeCommand(0xD0);
    writeData(0xA4);
    writeData(0xA1);

    writeCommand(0xE8);
    writeData(0x03);

    // 均衡时间控制
    writeCommand(0xE9);
    writeData(0x09);
    writeData(0x09);
    writeData(0x08);

    // Gamma设置 - 正极性
    writeCommand(0xE0);
    writeData(0xD0);
    writeData(0x05);
    writeData(0x09);
    writeData(0x09);
    writeData(0x08);
    writeData(0x14);
    writeData(0x28);
    writeData(0x33);
    writeData(0x3F);
    writeData(0x07);
    writeData(0x13);
    writeData(0x14);
    writeData(0x28);
    writeData(0x30);

    // Gamma设置 - 负极性
    writeCommand(0xE1);
    writeData(0xD0);
    writeData(0x05);
    writeData(0x09);
    writeData(0x09);
    writeData(0x08);
    writeData(0x03);
    writeData(0x24);
    writeData(0x32);
    writeData(0x32);
    writeData(0x3B);
    writeData(0x14);
    writeData(0x13);
    writeData(0x28);
    writeData(0x2F);

    writeCommand(0x20); // 反显开启（Display Inversion ON）
    writeCommand(0x11); // 退出睡眠模式
    delay(120);
    writeCommand(0x29); // 开启显示
}

// =============== 设置显示窗口 ===============

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // 列地址设置
    writeCommand(0x2A);
    writeData16((x0 << 8) | (x0 & 0xFF));
    writeData16((x1 << 8) | (x1 & 0xFF));

    // 行地址设置
    writeCommand(0x2B);
    writeData16((y0 << 8) | (y0 & 0xFF));
    writeData16((y1 << 8) | (y1 & 0xFF));

    // 写内存命令
    writeCommand(0x2C);
}

// =============== 绘图函数 ===============

void fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);

    for (uint32_t i = 0; i < (uint32_t)TFT_WIDTH * TFT_HEIGHT; i++)
    {
        spi.transfer16(color);
    }

    digitalWrite(TFT_CS, HIGH);
}

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x >= TFT_WIDTH || y < 0 || y >= TFT_HEIGHT)
        return;

    setAddrWindow(x, y, x, y);
    writeData16(color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT)
        return;
    if (x + w > TFT_WIDTH)
        w = TFT_WIDTH - x;
    if (y + h > TFT_HEIGHT)
        h = TFT_HEIGHT - y;

    setAddrWindow(x, y, x + w - 1, y + h - 1);

    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);

    for (uint32_t i = 0; i < (uint32_t)w * h; i++)
    {
        spi.transfer16(color);
    }

    digitalWrite(TFT_CS, HIGH);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1)
    {
        swap(x0, x1);
        swap(y0, y1);
    }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            drawPixel(y0, x0, color);
        }
        else
        {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

// =============== 主程序 ===============

void setup()
{
    Serial.begin(115200);
    Serial.println("GMT028-05 TFT Display Test");

    // 初始化引脚
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);

    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_RST, HIGH);

    // 初始化SPI
    spi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    spi.setFrequency(40000000); // 40MHz SPI时钟
    spi.setDataMode(SPI_MODE0);
    spi.setBitOrder(MSBFIRST);

    // 初始化屏幕
    TFT_Init();

    Serial.println("TFT initialized!");
}

void loop()
{
    // 测试程序：循环显示不同颜色
    Serial.println("红色");
    fillScreen(RED);
    delay(1000);

    Serial.println("绿色");
    fillScreen(GREEN);
    delay(1000);

    Serial.println("蓝色");
    fillScreen(BLUE);
    delay(1000);

    Serial.println("白色");
    fillScreen(WHITE);
    delay(1000);

    Serial.println("黑色");
    fillScreen(BLACK);
    delay(1000);
    Serial.println("青色");
    fillScreen(CYAN);
    delay(1000);
    Serial.println("粉色");
    fillScreen(MAGENTA);
    delay(1000);
}