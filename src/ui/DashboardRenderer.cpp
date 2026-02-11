#include "DashboardRenderer.h"

uint16_t DashboardRenderer::rgbTo565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t DashboardRenderer::blend565(uint16_t fg, uint16_t bg, uint8_t alpha)
{
    uint8_t fr = ((fg >> 11) & 0x1F) << 3;
    uint8_t fgG = ((fg >> 5) & 0x3F) << 2;
    uint8_t fb = (fg & 0x1F) << 3;

    uint8_t br = ((bg >> 11) & 0x1F) << 3;
    uint8_t bgG = ((bg >> 5) & 0x3F) << 2;
    uint8_t bb = (bg & 0x1F) << 3;

    uint8_t r = ((uint16_t)fr * alpha + (uint16_t)br * (255 - alpha)) / 255;
    uint8_t g = ((uint16_t)fgG * alpha + (uint16_t)bgG * (255 - alpha)) / 255;
    uint8_t b = ((uint16_t)fb * alpha + (uint16_t)bb * (255 - alpha)) / 255;

    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void DashboardRenderer::renderModule(const ModuleStyle &style, uint16_t backgroundColor)
{
    uint16_t color = blend565(style.color, backgroundColor, style.opacity);
    _display.fillRect(style.x, style.y, style.w, style.h, color);
    _display.drawRect(style.x, style.y, style.w, style.h, blend565(0xFFFF, color, 25));
}

void DashboardRenderer::drawWeatherIconSlot(const String &iconPath, const ThemeConfig &theme)
{
    // 第一阶段仅渲染占位框与文件名，用户后续可替换为真实图标解码。
    const int16_t x = theme.envModule.x + theme.envModule.w - 64;
    const int16_t y = theme.envModule.y + 8;
    const int16_t w = 56;
    const int16_t h = 22;

    uint16_t bg = blend565(rgbTo565(0x3A, 0x4A, 0x6A), theme.backgroundColor, 190);
    _display.fillRect(x, y, w, h, bg);
    _display.drawRect(x, y, w, h, blend565(0xFFFF, bg, 35));

    String name = iconPath;
    int slash = name.lastIndexOf('/');
    if (slash >= 0)
        name = name.substring(slash + 1);
    if (name.length() > 8)
        name = name.substring(0, 8);
    if (name.length() == 0)
        name = "icon";

    _display.drawText(x + 4, y + 7, name, rgbTo565(0xD8, 0xE6, 0xFF), 1);
}

void DashboardRenderer::render(const ThemeConfig &theme, uint8_t themeNumber)
{
    _display.fillScreen(theme.backgroundColor);

    renderModule(theme.timeModule, theme.backgroundColor);
    renderModule(theme.envModule, theme.backgroundColor);
    renderModule(theme.alarmModule, theme.backgroundColor);

    drawWeatherIconSlot(theme.weatherIcon, theme);

    _display.drawText(theme.timeText.x, theme.timeText.y, theme.timeText.value, theme.timeText.color, theme.timeText.size);
    _display.drawText(theme.dateText.x, theme.dateText.y, theme.dateText.value, theme.dateText.color, theme.dateText.size);
    _display.drawText(theme.tempText.x, theme.tempText.y, theme.tempText.value, theme.tempText.color, theme.tempText.size);
    _display.drawText(theme.humidText.x, theme.humidText.y, theme.humidText.value, theme.humidText.color, theme.humidText.size);
    _display.drawText(theme.pressureText.x, theme.pressureText.y, theme.pressureText.value, theme.pressureText.color, theme.pressureText.size);
    _display.drawText(theme.alarmText.x, theme.alarmText.y, theme.alarmText.value, theme.alarmText.color, theme.alarmText.size);

    // 修复原先 "THEME:" + String(...) 触发的运算符报错，使用 String 显式构造。
    _display.drawText(8, 8, String("THEME:") + String(themeNumber), rgbTo565(0x68, 0xB0, 0xFF), 1);
}
