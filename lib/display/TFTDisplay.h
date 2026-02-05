/**
 * @file TFTDisplay.h
 * @brief TFT屏幕驱动头文件 - ILI9341控制器
 * @version 1.0.0
 * @date 2026-02-04
 * 
 * 2.8寸TFT屏幕驱动，320x240分辨率
 * 7脚SPI接口（仅写）
 */

#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <Arduino.h>
#include <SPI.h>

// ==================== ILI9341命令定义 ====================
#define ILI9341_SWRESET     0x01   // 软件重置
#define ILI9341_SLPIN       0x10   // 进入睡眠
#define ILI9341_SLPOUT      0x11   // 退出睡眠
#define ILI9341_DISPOFF     0x28   // 显示关闭
#define ILI9341_DISPON      0x29   // 显示开启
#define ILI9341_CASET       0x2A   // 列地址设置
#define ILI9341_RASET       0x2B   // 行地址设置
#define ILI9341_RAMWR       0x2C   // 内存写入
#define ILI9341_MADCTL      0x36   // 内存访问控制
#define ILI9341_PIXFMT      0x3A   // 像素格式
#define ILI9341_FRMCTR1     0xB1   // 帧率控制
#define ILI9341_DFUNCTR     0xB6   // 显示功能控制
#define ILI9341_PWCTR1      0xC0   // 电源控制1
#define ILI9341_PWCTR2      0xC1   // 电源控制2
#define ILI9341_VMCTR1      0xC5   // VCOM控制1
#define ILI9341_VMCTR2      0xC7   // VCOM控制2
#define ILI9341_GMCTRP1     0xE0   // 伽玛正极
#define ILI9341_GMCTRN1     0xE1   // 伽玛负极

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
    RGB565 = 0x55,      // 16位RGB565
    RGB666 = 0x66,      // 18位RGB666
    RGB888 = 0x77       // 24位RGB888
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
     * @brief 绘制单个字符
     * @param x 起始列
     * @param y 起始行
     * @param c 字符
     * @param color 字体颜色
     * @param bg 背景颜色
     * @param size 字体放大倍数
     */
    void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size = 1);

    /**
     * @brief 绘制字符串
     * @param x 起始列
     * @param y 起始行
     * @param text 文本内容
     * @param color 字体颜色
     * @param bg 背景颜色
     * @param size 字体放大倍数
     */
    void drawText(uint16_t x, uint16_t y, const String& text, uint16_t color, uint16_t bg, uint8_t size = 1);

    /**
     * @brief 计算文本宽度
     * @param text 文本内容
     * @param size 字体放大倍数
     */
    uint16_t measureTextWidth(const String& text, uint8_t size = 1) const;
    
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
     * @brief 初始化ILI9341
     */
    void initILI9341();
    
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
