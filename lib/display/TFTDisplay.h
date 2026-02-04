/**
 * @file TFTDisplay.h
 * @brief TFT屏幕驱动头文件 - ST7789控制器
 * @version 1.0.0
 * @date 2026-02-04
 * 
 * 2.8寸IPS屏幕驱动，320x240分辨率
 * 基于SPI接口通信
 */

#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <Arduino.h>
#include <SPI.h>

// ==================== 屏幕分辨率 ====================
#define TFT_WIDTH           240    // 屏幕宽度(像素)
#define TFT_HEIGHT          320    // 屏幕高度(像素)
#define TFT_ROTATION        1      // 0/1/2/3 旋转角度

// ==================== ST7789命令定义 ====================
#define ST7789_SWRESET     0x01   // 软件重置
#define ST7789_SLPIN       0x10   // 进入睡眠模式
#define ST7789_SLPOUT      0x11   // 退出睡眠模式
#define ST7789_PTLON       0x12   // 部分显示模式开启
#define ST7789_NORON       0x13   // 正常显示模式
#define ST7789_INVOFF      0x20   // 反色关闭
#define ST7789_INVON       0x21   // 反色打开
#define ST7789_GAMSET      0x26   // 伽玛曲线选择
#define ST7789_DISPOFF     0x28   // 显示关闭
#define ST7789_DISPON      0x29   // 显示打开
#define ST7789_CASET       0x2A   // 列地址设置
#define ST7789_RASET       0x2B   // 行地址设置
#define ST7789_RAMWR       0x2C   // 内存写入
#define ST7789_RGBSET      0x2D   // 彩色查找表
#define ST7789_RAMRD       0x2E   // 内存读取
#define ST7789_PTLAR       0x30   // 部分区域
#define ST7789_VSCRDEF     0x33   // 垂直滚动定义
#define ST7789_TEOFF       0x34   // 撕裂效应线关闭
#define ST7789_TEON        0x35   // 撕裂效应线打开
#define ST7789_MADCTL      0x36   // 内存数据访问控制
#define ST7789_IDMOFF      0x38   // 空闲模式关闭
#define ST7789_IDMON       0x39   // 空闲模式打开
#define ST7789_COLMOD      0x3A   // 接口像素格式
#define ST7789_RAMWRC      0x3C   // 内存写入继续
#define ST7789_RAMRDC      0x3E   // 内存读取继续
#define ST7789_TESCAN      0x44   // 撕裂效应扫描线
#define ST7789_DISPFUNC    0xB6   // 显示功能设置
#define ST7789_POWE1       0xD0   // 电源控制1
#define ST7789_POWE2       0xD1   // 电源控制2
#define ST7789_VCOM        0xBB   // VCOM电压
#define ST7789_GCTRL       0xB7   // 门极控制
#define ST7789_VCOMS       0xBB   // VCOM设置
#define ST7789_FRCTRL2     0xC6   // 帧率控制
#define ST7789_GCTRL2      0xB7   // 门极控制
#define ST7789_GMCTRP1     0xE0   // 伽玛正极
#define ST7789_GMCTRN1     0xE1   // 伽玛负极
#define ST7789_INVCTR      0xB4   // 反色控制

// ==================== 颜色定义 (RGB565) ====================
#define COLOR_BLACK        0x0000
#define COLOR_NAVY         0x000F
#define COLOR_DARKGREEN    0x03E0
#define COLOR_DARKCYAN     0x03EF
#define COLOR_MAROON       0x7800
#define COLOR_PURPLE       0x780F
#define COLOR_OLIVE        0x7BE0
#define COLOR_LIGHTGREY    0xC618
#define COLOR_DARKGREY     0x7BEF
#define COLOR_BLUE         0x001F
#define COLOR_GREEN        0x07E0
#define COLOR_CYAN         0x07FF
#define COLOR_RED          0xF800
#define COLOR_MAGENTA      0xF81F
#define COLOR_YELLOW       0xFFE0
#define COLOR_WHITE        0xFFFF
#define COLOR_ORANGE       0xFD20

// ==================== 显示模式 ====================
enum class DisplayMode {
    NORMAL = 0x00,      // 正常显示
    INVERT = 0x01       // 反色显示
};

// ==================== 像素格式 ====================
enum class PixelFormat {
    RGB565 = 0x05,      // 16位RGB565
    RGB444 = 0x03,      // 12位RGB444
    RGB888 = 0x07       // 24位RGB888
};

/**
 * @brief TFT显示驱动类
 */
class TFTDisplay {
public:
    /**
     * @brief 初始化屏幕
     * @return true 成功, false 失败
     */
    bool begin();
    
    /**
     * @brief 关闭屏幕
     */
    void end();
    
    /**
     * @brief 设置显示窗口
     * @param x0 起始列
     * @param y0 起始行
     * @param x1 结束列
     * @param y1 结束行
     */
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
    /**
     * @brief 写入颜色数据
     * @param color RGB565颜色值
     * @param count 像素数量
     */
    void writeColor(uint16_t color, uint32_t count = 1);
    
    /**
     * @brief 写入颜色数据（原始字节）
     * @param data 数据指针
     * @param len 数据长度
     */
    void writeData(const uint8_t* data, uint32_t len);
    
    /**
     * @brief 填充矩形区域
     * @param x 左上角列
     * @param y 左上角行
     * @param width 宽度
     * @param height 高度
     * @param color 填充颜色
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    
    /**
     * @brief 清屏
     * @param color 背景颜色
     */
    void fillScreen(uint16_t color);
    
    /**
     * @brief 绘制像素
     * @param x 列
     * @param y 行
     * @param color 像素颜色
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    
    /**
     * @brief 绘制水平线
     * @param x 起始列
     * @param y 行
     * @param length 长度
     * @param color 线条颜色
     */
    void drawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
    
    /**
     * @brief 绘制竖直线
     * @param x 列
     * @param y 起始行
     * @param length 长度
     * @param color 线条颜色
     */
    void drawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
    
    /**
     * @brief 绘制矩形
     * @param x 左上角列
     * @param y 左上角行
     * @param width 宽度
     * @param height 高度
     * @param color 框线颜色
     */
    void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    
    /**
     * @brief 设置像素格式
     * @param format 像素格式
     */
    void setPixelFormat(PixelFormat format);
    
    /**
     * @brief 设置显示模式
     * @param mode 显示模式
     */
    void setDisplayMode(DisplayMode mode);
    
    /**
     * @brief 设置屏幕旋转
     * @param rotation 旋转角度 (0/1/2/3)
     */
    void setRotation(uint8_t rotation);
    
    /**
     * @brief 打开背光
     */
    void backlight(bool on = true);
    
    /**
     * @brief 设置背光亮度
     * @param brightness 亮度值(0-255)
     */
    void setBacklight(uint8_t brightness);
    
    /**
     * @brief 进入睡眠模式
     */
    void sleep();
    
    /**
     * @brief 唤醒显示
     */
    void wakeup();
    
    /**
     * @brief 设置显示打开/关闭
     * @param on true 打开, false 关闭
     */
    void display(bool on = true);
    
    /**
     * @brief 获取屏幕宽度
     * @return 宽度(像素)
     */
    uint16_t width() const { return _width; }
    
    /**
     * @brief 获取屏幕高度
     * @return 高度(像素)
     */
    uint16_t height() const { return _height; }

private:
    uint16_t _width;
    uint16_t _height;
    uint8_t _rotation;
    
    /**
     * @brief 发送命令
     * @param cmd 命令字节
     */
    void writeCommand(uint8_t cmd);
    
    /**
     * @brief 发送数据
     * @param data 数据字节
     */
    void writeDataByte(uint8_t data);
    
    /**
     * @brief 初始化ST7789
     */
    void initST7789();
    
    /**
     * @brief 重置屏幕
     */
    void reset();
    
    /**
     * @brief 设置地址窗口
     * @param x0 起始列
     * @param y0 起始行
     * @param x1 结束列
     * @param y1 结束行
     */
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

#endif // TFT_DISPLAY_H
