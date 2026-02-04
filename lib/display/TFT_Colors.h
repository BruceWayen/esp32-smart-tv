/**
 * @file TFT_Colors.h
 * @brief TFT RGB565颜色定义速查表
 * @version 1.0.0
 * 
 * RGB565格式: RRRRRGGGGGGBBBBB
 * R: 红色通道(5bit, 0-31)
 * G: 绿色通道(6bit, 0-63)
 * B: 蓝色通道(5bit, 0-31)
 */

#ifndef TFT_COLORS_H
#define TFT_COLORS_H

// ==================== 基础颜色 ====================
#define COLOR_BLACK        0x0000  // 黑色 RGB(  0,  0,  0)
#define COLOR_NAVY         0x000F  // 深蓝 RGB(  0,  0,128)
#define COLOR_DARKBLUE     0x0010  // 暗蓝 RGB(  0,  0,139)
#define COLOR_BLUE         0x001F  // 蓝色 RGB(  0,  0,255)
#define COLOR_DARKGREEN    0x03E0  // 深绿 RGB(  0,128,  0)
#define COLOR_GREEN        0x07E0  // 绿色 RGB(  0,255,  0)
#define COLOR_CYAN         0x07FF  // 青色 RGB(  0,255,255)
#define COLOR_DARKRED      0x7800  // 深红 RGB(128,  0,  0)
#define COLOR_RED          0xF800  // 红色 RGB(255,  0,  0)
#define COLOR_MAGENTA      0xF81F  // 品红 RGB(255,  0,255)
#define COLOR_YELLOW       0xFFE0  // 黄色 RGB(255,255,  0)
#define COLOR_WHITE        0xFFFF  // 白色 RGB(255,255,255)

// ==================== 灰度颜色 ====================
#define COLOR_DARKGRAY     0x7BEF  // 深灰 RGB(128,128,128)
#define COLOR_GRAY         0xC618  // 灰色 RGB(192,192,192)
#define COLOR_LIGHTGRAY    0xD69A  // 浅灰 RGB(211,211,211)
#define COLOR_SILVER       0xC618  // 银色 RGB(192,192,192)
#define COLOR_MAROON       0x7800  // 褐红 RGB(128,  0,  0)
#define COLOR_OLIVE        0x7BE0  // 橄榄 RGB(128,128,  0)
#define COLOR_LIME         0x07E0  // 石灰 RGB(  0,255,  0)
#define COLOR_AQUA         0x07FF  // 水色 RGB(  0,255,255)
#define COLOR_TEAL         0x0410  // 蓝绿 RGB(  0,128,128)
#define COLOR_PURPLE       0x780F  // 紫色 RGB(128,  0,128)
#define COLOR_FUCHSIA      0xF81F  // 紫红 RGB(255,  0,255)

// ==================== 扩展颜色(常见) ====================
#define COLOR_ORANGE       0xFD20  // 橙色 RGB(255,165,  0)
#define COLOR_ORANGERED    0xFB00  // 橙红 RGB(255, 69,  0)
#define COLOR_PINK         0xFE1F  // 粉红 RGB(255,192,203)
#define COLOR_SALMON       0xFC20  // 三文鱼 RGB(255,128,114)
#define COLOR_GOLD         0xFEA0  // 金色 RGB(255,215,  0)
#define COLOR_KHAKI        0xF6E0  // 卡其 RGB(240,230,140)
#define COLOR_INDIGO       0x4810  // 靛蓝 RGB( 75,  0,130)
#define COLOR_VIOLET       0x8C1F  // 紫罗兰 RGB(238,130,238)
#define COLOR_BROWN        0xA145  // 褐色 RGB(165, 42, 42)
#define COLOR_DARKRED      0x7800  // 深红 RGB(139,  0,  0)
#define COLOR_CRIMSON      0xDC00  // 深红 RGB(220, 20, 60)

// ==================== 浅色系 ====================
#define COLOR_LIGHTRED     0xFD0F  // 浅红 RGB(255,127,127)
#define COLOR_LIGHTGREEN   0x8FE0  // 浅绿 RGB(144,238,144)
#define COLOR_LIGHTBLUE    0xAEDF  // 浅蓝 RGB(173,216,230)
#define COLOR_LIGHTCYAN    0xE0FF  // 浅青 RGB(224,255,255)
#define COLOR_LIGHTYELLOW  0xFFFC  // 浅黄 RGB(255,255,224)
#define COLOR_LIGHTGOLDEN  0xFEEC  // 浅金 RGB(250,250,210)
#define COLOR_LIGHTPINK    0xFE1F  // 浅粉 RGB(255,182,193)
#define COLOR_LIGHTCORAL   0xF410  // 浅珊瑚 RGB(240,128,128)
#define COLOR_LIGHTSALMON  0xFD2D  // 浅三文鱼 RGB(255,160,122)
#define COLOR_LIGHTSEAGRN  0x2595  // 浅海绿 RGB( 32,178,170)
#define COLOR_LIGHTSKYBLUE 0x9D8F  // 浅天蓝 RGB(135,206,250)
#define COLOR_LIGHTSLATEGR 0x7453  // 浅板岩灰 RGB(119,136,153)
#define COLOR_LIGHTSTEELBLU 0xB63B // 浅钢蓝 RGB(176,196,222)

// ==================== 深色系 ====================
#define COLOR_DARKBLUE     0x0010  // 深蓝 RGB(  0,  0,139)
#define COLOR_DARKCYAN     0x03EF  // 深青 RGB(  0,139,139)
#define COLOR_DARKGOLDENR  0xB5A0  // 深金棕 RGB(184,134, 11)
#define COLOR_DARKGRAY     0x7BEF  // 深灰 RGB(169,169,169)
#define COLOR_DARKGREEN    0x03E0  // 深绿 RGB(  0,100,  0)
#define COLOR_DARKKHAKI    0xBEE0  // 深卡其 RGB(189,183,107)
#define COLOR_DARKMAGENTA  0x8800  // 深品红 RGB(139,  0,139)
#define COLOR_DARKOLIVEGREEN 0x5360 // 深橄榄绿 RGB( 85,107, 47)
#define COLOR_DARKORANGE   0xFC40  // 深橙 RGB(255,140,  0)
#define COLOR_DARKORCHID   0x9880  // 深兰花 RGB(153, 50,204)
#define COLOR_DARKRED      0x7800  // 深红 RGB(139,  0,  0)
#define COLOR_DARKSALMON   0xEA6D  // 深三文鱼 RGB(233,150,122)
#define COLOR_DARKSEAGREEN 0x8DF1  // 深海绿 RGB(143,188,143)
#define COLOR_DARKSLATEBLUE 0x482E // 深板岩蓝 RGB( 72, 61,139)
#define COLOR_DARKSLATEGRAY 0x2A4D // 深板岩灰 RGB( 47, 79, 79)
#define COLOR_DARKTURQUOISE 0x067A // 深绿松石 RGB(  0,206,209)
#define COLOR_DARKVIOLET   0x901F  // 深紫 RGB(148,  0,211)

// ==================== Web标准颜色 ====================
#define COLOR_ALICEBLUE      0xF7DF // Alice蓝 RGB(240,248,255)
#define COLOR_ANTIQUEWHITE   0xFF5A // 古董白 RGB(250,235,215)
#define COLOR_AQUAMARINE     0x7FFF // 绿宝石 RGB(127,255,212)
#define COLOR_AZURE          0xF0FF // 蔚蓝 RGB(240,255,255)
#define COLOR_BEIGE          0xF7BB // 米色 RGB(245,245,220)
#define COLOR_BISQUE         0xFF78 // 浅陶土 RGB(255,228,196)
#define COLOR_BLANCHEDALMOND 0xFF5C // 漂白杏仁 RGB(255,235,205)
#define COLOR_BLUEVIOLET     0x895F // 蓝紫 RGB(138, 43,226)
#define COLOR_BURLYWOOD      0xDD80 // 实木 RGB(222,184,135)
#define COLOR_CADETBLUE      0x5CF4 // 军校蓝 RGB( 95,158,160)
#define COLOR_CHARTREUSE     0x7FE0 // 黄绿 RGB(127,255,  0)
#define COLOR_CHOCOLATE      0xD320 // 巧克力 RGB(210,105, 30)
#define COLOR_CORAL          0xFB60 // 珊瑚 RGB(255,127, 80)
#define COLOR_CORNFLOWERBLUE 0x64BD // 矢车菊蓝 RGB(100,149,237)
#define COLOR_CORNSILK       0xFFDB // 玉米丝 RGB(255,248,220)
#define COLOR_DARKCYAN       0x03EF // 深青 RGB(  0,139,139)

// ==================== 快速RGB颜色生成宏 ====================
/**
 * @brief RGB565颜色生成宏
 * @param r 红色值(0-255)
 * @param g 绿色值(0-255)
 * @param b 蓝色值(0-255)
 * @return uint16_t RGB565颜色值
 * 
 * 使用示例:
 * uint16_t myColor = RGB565(255, 128, 64);
 */
#define RGB565(r, g, b) \
    (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

// ==================== 颜色对照表(文本版) ====================
/*
基础颜色对照表:

黑色系:
  COLOR_BLACK        = 0x0000  RGB(  0,  0,  0)

红色系:
  COLOR_DARKRED      = 0x7800  RGB(128,  0,  0)
  COLOR_RED          = 0xF800  RGB(255,  0,  0)
  COLOR_LIGHTRED     = 0xFD0F  RGB(255,127,127)
  COLOR_CRIMSON      = 0xDC00  RGB(220, 20, 60)

绿色系:
  COLOR_DARKGREEN    = 0x03E0  RGB(  0,128,  0)
  COLOR_GREEN        = 0x07E0  RGB(  0,255,  0)
  COLOR_LIGHTGREEN   = 0x8FE0  RGB(144,238,144)
  COLOR_LIME         = 0x07E0  RGB(  0,255,  0)

蓝色系:
  COLOR_NAVY         = 0x000F  RGB(  0,  0,128)
  COLOR_DARKBLUE     = 0x0010  RGB(  0,  0,139)
  COLOR_BLUE         = 0x001F  RGB(  0,  0,255)
  COLOR_LIGHTBLUE    = 0xAEDF  RGB(173,216,230)

黄色系:
  COLOR_YELLOW       = 0xFFE0  RGB(255,255,  0)
  COLOR_LIGHTYELLOW  = 0xFFFC  RGB(255,255,224)
  COLOR_GOLD         = 0xFEA0  RGB(255,215,  0)
  COLOR_KHAKI        = 0xF6E0  RGB(240,230,140)

橙色系:
  COLOR_ORANGE       = 0xFD20  RGB(255,165,  0)
  COLOR_ORANGERED    = 0xFB00  RGB(255, 69,  0)
  COLOR_DARKORANGE   = 0xFC40  RGB(255,140,  0)

紫色系:
  COLOR_PURPLE       = 0x780F  RGB(128,  0,128)
  COLOR_MAGENTA      = 0xF81F  RGB(255,  0,255)
  COLOR_VIOLET       = 0x8C1F  RGB(238,130,238)
  COLOR_INDIGO       = 0x4810  RGB( 75,  0,130)

灰色系:
  COLOR_GRAY         = 0xC618  RGB(192,192,192)
  COLOR_DARKGRAY     = 0x7BEF  RGB(128,128,128)
  COLOR_LIGHTGRAY    = 0xD69A  RGB(211,211,211)

特殊:
  COLOR_WHITE        = 0xFFFF  RGB(255,255,255)
  COLOR_CYAN         = 0x07FF  RGB(  0,255,255)
  COLOR_PINK         = 0xFE1F  RGB(255,192,203)
  COLOR_BROWN        = 0xA145  RGB(165, 42, 42)
*/

// ==================== 使用示例 ====================
/*
// 方式1: 使用预定义颜色常数
displayMgr.clear(COLOR_BLACK);
displayMgr.fillRect(10, 10, 100, 100, COLOR_RED);

// 方式2: 使用RGB宏生成自定义颜色
uint16_t customColor = RGB565(200, 150, 100);
displayMgr.drawRect(10, 10, 100, 100, customColor);

// 方式3: 直接使用RGB565值
displayMgr.drawPixel(50, 50, 0xF800);  // 红色

// 方式4: 使用变量存储颜色
uint16_t palette[4] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
for (int i = 0; i < 4; i++) {
    displayMgr.fillRect(i*60, 0, 60, 240, palette[i]);
}
*/

#endif // TFT_COLORS_H
