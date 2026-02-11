#include <Arduino.h>
#include <SPIFFS.h>

#include "display/TftDriver.h"
#include "theme/ThemeManager.h"
#include "ui/DashboardRenderer.h"

namespace
{
constexpr uint8_t TFT_CS = 8;
constexpr uint8_t TFT_DC = 9;
constexpr uint8_t TFT_RST = 10;
constexpr uint8_t TFT_MOSI = 11;
constexpr uint8_t TFT_SCLK = 12;
constexpr uint8_t THEME_SWITCH_BUTTON = 0;

TftDriver g_display(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);
ThemeManager g_themeManager;
DashboardRenderer g_renderer(g_display);

unsigned long g_lastButtonTick = 0;
unsigned long g_lastClockRefreshTick = 0;

void renderCurrentTheme()
{
    g_renderer.render(g_themeManager.theme(), g_themeManager.currentThemeNumber());
}
} // namespace

void setup()
{
    Serial.begin(115200);
    Serial.println("[系统] 启动 ESP32-S3 智能桌面主题程序");

    pinMode(THEME_SWITCH_BUTTON, INPUT_PULLUP);

    g_display.begin();

    if (!SPIFFS.begin(true))
    {
        Serial.println("[文件系统] ❌ SPIFFS 挂载失败");
    }
    else
    {
        Serial.println("[文件系统] ✅ SPIFFS 挂载成功");
    }

    g_themeManager.begin();
    renderCurrentTheme();

    Serial.println("[提示] GPIO0短按切换主题，串口输入 n 切换、r 重载");
}

void loop()
{
    if (digitalRead(THEME_SWITCH_BUTTON) == LOW && (millis() - g_lastButtonTick) > 350)
    {
        g_lastButtonTick = millis();
        if (g_themeManager.switchToNextTheme())
        {
            renderCurrentTheme();
        }
    }

    if (Serial.available())
    {
        char c = Serial.read();
        if (c == 'n' || c == 'N')
        {
            if (g_themeManager.switchToNextTheme())
                renderCurrentTheme();
        }
        else if (c == 'r' || c == 'R')
        {
            if (g_themeManager.reloadActiveTheme())
                renderCurrentTheme();
        }
    }

    // 静态数据演示：每 10 秒更新时间文本（便于看到配置和刷新流程）
    if ((millis() - g_lastClockRefreshTick) > 10000)
    {
        g_lastClockRefreshTick = millis();
        g_themeManager.tickMockClock();
        renderCurrentTheme();
    }
}
