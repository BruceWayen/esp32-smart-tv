#include <Arduino.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define TFT_CS 8
#define TFT_DC 9
#define TFT_RST 10
#define TFT_MOSI 11
#define TFT_SCLK 12

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define BLACK 0x0000
#define WHITE 0xFFFF

#define THEME_SWITCH_BUTTON 0

SPIClass spi = SPIClass(HSPI);

struct TextStyle {
    int16_t x = 0;
    int16_t y = 0;
    uint8_t size = 1;
    uint16_t color = WHITE;
    String value;
};

struct ModuleStyle {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 60;
    int16_t h = 40;
    uint8_t opacity = 255;
    uint16_t color = 0x2104;
};

struct ThemeConfig {
    uint16_t backgroundColor = 0x0842;
    String backgroundImage;

    TextStyle timeText;
    TextStyle dateText;
    TextStyle tempText;
    TextStyle humidText;
    TextStyle pressureText;
    TextStyle alarmText;

    ModuleStyle timeModule;
    ModuleStyle envModule;
    ModuleStyle alarmModule;

    String weatherIcon = "/icons/weather/default.bin";
    String wifiIcon;
    String batteryIcon;
};

struct ThemeIndex {
    String activeTheme = "/themes/theme1.json";
    String themes[8];
    uint8_t themeCount = 0;
};

ThemeConfig g_theme;
ThemeIndex g_themeIndex;
uint8_t g_currentThemeIndex = 0;
unsigned long g_lastClockRefresh = 0;
unsigned long g_lastButtonTick = 0;

const uint8_t font5x7[] PROGMEM = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5F,0x00,0x00,0x00,0x07,0x00,0x07,0x00,0x14,0x7F,0x14,0x7F,0x14,0x24,0x2A,0x7F,0x2A,0x12,0x23,0x13,0x08,0x64,0x62,0x36,0x49,0x55,0x22,0x50,0x00,0x05,0x03,0x00,0x00,0x00,0x1C,0x22,0x41,0x00,0x00,0x41,0x22,0x1C,0x00,0x14,0x08,0x3E,0x08,0x14,0x08,0x08,0x3E,0x08,0x08,0x00,0x50,0x30,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x60,0x60,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x3E,0x51,0x49,0x45,0x3E,0x00,0x42,0x7F,0x40,0x00,0x42,0x61,0x51,0x49,0x46,0x21,0x41,0x45,0x4B,0x31,0x18,0x14,0x12,0x7F,0x10,0x27,0x45,0x45,0x45,0x39,0x3C,0x4A,0x49,0x49,0x30,0x01,0x71,0x09,0x05,0x03,0x36,0x49,0x49,0x49,0x36,0x06,0x49,0x49,0x29,0x1E,0x00,0x36,0x36,0x00,0x00,0x00,0x56,0x36,0x00,0x00,0x08,0x14,0x22,0x41,0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x41,0x22,0x14,0x08,0x02,0x01,0x51,0x09,0x06,0x32,0x49,0x79,0x41,0x3E,0x7E,0x11,0x11,0x11,0x7E,0x7F,0x49,0x49,0x49,0x36,0x3E,0x41,0x41,0x41,0x22,0x7F,0x41,0x41,0x22,0x1C,0x7F,0x49,0x49,0x49,0x41,0x7F,0x09,0x09,0x09,0x01,0x3E,0x41,0x49,0x49,0x7A,0x7F,0x08,0x08,0x08,0x7F,0x00,0x41,0x7F,0x41,0x00,0x20,0x40,0x41,0x3F,0x01,0x7F,0x08,0x14,0x22,0x41,0x7F,0x40,0x40,0x40,0x40,0x7F,0x02,0x0C,0x02,0x7F,0x7F,0x04,0x08,0x10,0x7F,0x3E,0x41,0x41,0x41,0x3E,0x7F,0x09,0x09,0x09,0x06,0x3E,0x41,0x51,0x21,0x5E,0x7F,0x09,0x19,0x29,0x46,0x46,0x49,0x49,0x49,0x31,0x01,0x01,0x7F,0x01,0x01,0x3F,0x40,0x40,0x40,0x3F,0x1F,0x20,0x40,0x20,0x1F,0x3F,0x40,0x38,0x40,0x3F,0x63,0x14,0x08,0x14,0x63,0x07,0x08,0x70,0x08,0x07,0x61,0x51,0x49,0x45,0x43,0x00,0x7F,0x41,0x41,0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x41,0x41,0x7F,0x00,0x04,0x02,0x01,0x02,0x04,0x40,0x40,0x40,0x40,0x40,0x00,0x01,0x02,0x04,0x00,0x20,0x54,0x54,0x54,0x78,0x7F,0x48,0x44,0x44,0x38,0x38,0x44,0x44,0x44,0x20,0x38,0x44,0x44,0x48,0x7F,0x38,0x54,0x54,0x54,0x18,0x08,0x7E,0x09,0x01,0x02,0x0C,0x52,0x52,0x52,0x3E,0x7F,0x08,0x04,0x04,0x78,0x00,0x44,0x7D,0x40,0x00,0x20,0x40,0x44,0x3D,0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x41,0x7F,0x40,0x00,0x7C,0x04,0x18,0x04,0x78,0x7C,0x08,0x04,0x04,0x78,0x38,0x44,0x44,0x44,0x38,0x7C,0x14,0x14,0x14,0x08,0x08,0x14,0x14,0x18,0x7C,0x7C,0x08,0x04,0x04,0x08,0x48,0x54,0x54,0x54,0x20,0x04,0x3F,0x44,0x40,0x20,0x3C,0x40,0x40,0x20,0x7C,0x1C,0x20,0x40,0x20,0x1C,0x3C,0x40,0x30,0x40,0x3C,0x44,0x28,0x10,0x28,0x44,0x0C,0x50,0x50,0x50,0x3C,0x44,0x64,0x54,0x4C,0x44,0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x41,0x36,0x08,0x00,0x10,0x08,0x08,0x10,0x08,0x00,0x00,0x00,0x00,0x00
};

template <typename T>
void swapT(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

uint16_t blend565(uint16_t fg, uint16_t bg, uint8_t alpha)
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

uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t parseColor(const String &hex, uint16_t fallback)
{
    if (hex.length() != 7 || hex[0] != '#')
        return fallback;

    long value = strtol(hex.substring(1).c_str(), nullptr, 16);
    return rgbTo565((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

void writeCommand(uint8_t cmd)
{
    digitalWrite(TFT_DC, LOW);
    digitalWrite(TFT_CS, LOW);
    spi.transfer(cmd);
    digitalWrite(TFT_CS, HIGH);
}

void writeData(uint8_t data)
{
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    spi.transfer(data);
    digitalWrite(TFT_CS, HIGH);
}

void writeData16(uint16_t data)
{
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    spi.transfer16(data);
    digitalWrite(TFT_CS, HIGH);
}

void TFT_Init()
{
    digitalWrite(TFT_RST, LOW);
    delay(100);
    digitalWrite(TFT_RST, HIGH);
    delay(120);

    writeCommand(0x3A);
    writeData(0x05);
    writeCommand(0xC5);
    writeData(0x1A);
    writeCommand(0x36);
    writeData(0x00);

    writeCommand(0xB2);
    writeData(0x05); writeData(0x05); writeData(0x00); writeData(0x33); writeData(0x33);
    writeCommand(0xB7); writeData(0x05);
    writeCommand(0xBB); writeData(0x3F);
    writeCommand(0xC0); writeData(0x2C);
    writeCommand(0xC2); writeData(0x01);
    writeCommand(0xC3); writeData(0x0F);
    writeCommand(0xC4); writeData(0x20);
    writeCommand(0xC6); writeData(0x01);
    writeCommand(0xD0); writeData(0xA4); writeData(0xA1);
    writeCommand(0xE8); writeData(0x03);
    writeCommand(0xE9); writeData(0x09); writeData(0x09); writeData(0x08);

    writeCommand(0xE0);
    uint8_t e0[] = {0xD0,0x05,0x09,0x09,0x08,0x14,0x28,0x33,0x3F,0x07,0x13,0x14,0x28,0x30};
    for (auto b : e0) writeData(b);

    writeCommand(0xE1);
    uint8_t e1[] = {0xD0,0x05,0x09,0x09,0x08,0x03,0x24,0x32,0x32,0x3B,0x14,0x13,0x28,0x2F};
    for (auto b : e1) writeData(b);

    writeCommand(0x20);
    writeCommand(0x11);
    delay(120);
    writeCommand(0x29);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    writeCommand(0x2A);
    writeData16((x0 << 8) | (x0 & 0xFF));
    writeData16((x1 << 8) | (x1 & 0xFF));

    writeCommand(0x2B);
    writeData16((y0 << 8) | (y0 & 0xFF));
    writeData16((y1 << 8) | (y1 & 0xFF));

    writeCommand(0x2C);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT || w <= 0 || h <= 0)
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

void fillScreen(uint16_t color)
{
    fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, color);
}

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x >= TFT_WIDTH || y < 0 || y >= TFT_HEIGHT)
        return;
    setAddrWindow(x, y, x, y);
    writeData16(color);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        swapT(x0, y0);
        swapT(x1, y1);
    }
    if (x0 > x1)
    {
        swapT(x0, x1);
        swapT(y0, y1);
    }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;

    for (; x0 <= x1; x0++)
    {
        if (steep)
            drawPixel(y0, x0, color);
        else
            drawPixel(x0, y0, color);
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

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    for (int16_t y = -r; y <= r; y++)
    {
        for (int16_t x = -r; x <= r; x++)
        {
            if (x * x + y * y <= r * r)
                drawPixel(x0 + x, y0 + y, color);
        }
    }
}

void drawChar5x7(int16_t x, int16_t y, char c, uint16_t color, uint8_t size)
{
    if (c < 32 || c > 127)
        c = '?';

    uint16_t idx = (c - 32) * 5;
    for (uint8_t i = 0; i < 5; i++)
    {
        uint8_t line = pgm_read_byte(font5x7 + idx + i);
        for (uint8_t j = 0; j < 8; j++)
        {
            if (line & 0x1)
            {
                if (size == 1)
                    drawPixel(x + i, y + j, color);
                else
                    fillRect(x + i * size, y + j * size, size, size, color);
            }
            line >>= 1;
        }
    }
}

void drawText(int16_t x, int16_t y, const String &text, uint16_t color, uint8_t size)
{
    int16_t cursor = x;
    for (size_t i = 0; i < text.length(); i++)
    {
        drawChar5x7(cursor, y, text[i], color, size);
        cursor += 6 * size;
    }
}

void drawWeatherIconSlot(const String &iconPath, int16_t x, int16_t y)
{
    // 第一阶段不做图片解码，仅渲染图标占位与文件名尾部，确保主题配置可见且可切换。
    const int16_t w = 56;
    const int16_t h = 22;
    uint16_t bg = blend565(rgbTo565(0x3A, 0x4A, 0x6A), g_theme.backgroundColor, 190);
    fillRect(x, y, w, h, bg);
    drawRect(x, y, w, h, blend565(WHITE, bg, 30));

    String name = iconPath;
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    if (name.length() > 8) name = name.substring(0, 8);
    if (name.length() == 0) name = "icon";

    drawText(x + 4, y + 7, name, rgbTo565(0xD8, 0xE6, 0xFF), 1);
}

bool readJson(const char *path, DynamicJsonDocument &doc)
{
    File file = SPIFFS.open(path, "r");
    if (!file)
    {
        Serial.printf("[ERR] open failed: %s\n", path);
        return false;
    }

    auto err = deserializeJson(doc, file);
    file.close();
    if (err)
    {
        Serial.printf("[ERR] parse %s failed: %s\n", path, err.c_str());
        return false;
    }
    return true;
}

void loadTextStyle(JsonObject obj, TextStyle &style)
{
    if (obj["x"].is<int>()) style.x = obj["x"].as<int>();
    if (obj["y"].is<int>()) style.y = obj["y"].as<int>();
    if (obj["size"].is<int>()) style.size = obj["size"].as<int>();
    if (obj["color"].is<const char *>()) style.color = parseColor(obj["color"].as<String>(), style.color);
    if (obj["value"].is<const char *>()) style.value = obj["value"].as<String>();
}

void loadModuleStyle(JsonObject obj, ModuleStyle &style)
{
    if (obj["x"].is<int>()) style.x = obj["x"].as<int>();
    if (obj["y"].is<int>()) style.y = obj["y"].as<int>();
    if (obj["w"].is<int>()) style.w = obj["w"].as<int>();
    if (obj["h"].is<int>()) style.h = obj["h"].as<int>();
    if (obj["opacity"].is<int>()) style.opacity = constrain(obj["opacity"].as<int>(), 0, 255);
    if (obj["color"].is<const char *>()) style.color = parseColor(obj["color"].as<String>(), style.color);
}

bool loadThemeIndex()
{
    DynamicJsonDocument doc(2048);
    if (!readJson("/theme_config.json", doc))
        return false;

    g_themeIndex.activeTheme = doc["activeTheme"] | "/themes/theme1.json";
    g_themeIndex.themeCount = 0;
    JsonArray arr = doc["themes"].as<JsonArray>();
    for (JsonVariant v : arr)
    {
        if (g_themeIndex.themeCount >= 8)
            break;
        g_themeIndex.themes[g_themeIndex.themeCount++] = v.as<String>();
    }

    if (g_themeIndex.themeCount == 0)
    {
        g_themeIndex.themes[0] = g_themeIndex.activeTheme;
        g_themeIndex.themeCount = 1;
    }

    g_currentThemeIndex = 0;
    for (uint8_t i = 0; i < g_themeIndex.themeCount; i++)
    {
        if (g_themeIndex.themes[i] == g_themeIndex.activeTheme)
            g_currentThemeIndex = i;
    }

    Serial.printf("[THEME] active=%s count=%d\n", g_themeIndex.activeTheme.c_str(), g_themeIndex.themeCount);
    return true;
}

bool loadTheme(const String &path)
{
    DynamicJsonDocument doc(4096);
    if (!readJson(path.c_str(), doc))
        return false;

    g_theme = ThemeConfig();

    JsonObject background = doc["background"];
    if (!background.isNull())
    {
        g_theme.backgroundColor = parseColor(background["color"] | "#0B1328", g_theme.backgroundColor);
        g_theme.backgroundImage = background["image"] | "";
    }

    JsonObject texts = doc["text"];
    if (!texts.isNull())
    {
        loadTextStyle(texts["time"], g_theme.timeText);
        loadTextStyle(texts["date"], g_theme.dateText);
        loadTextStyle(texts["temp"], g_theme.tempText);
        loadTextStyle(texts["humidity"], g_theme.humidText);
        loadTextStyle(texts["pressure"], g_theme.pressureText);
        loadTextStyle(texts["alarm"], g_theme.alarmText);
    }

    JsonObject modules = doc["modules"];
    if (!modules.isNull())
    {
        loadModuleStyle(modules["time"], g_theme.timeModule);
        loadModuleStyle(modules["environment"], g_theme.envModule);
        loadModuleStyle(modules["alarm"], g_theme.alarmModule);
    }

    JsonObject images = doc["images"];
    if (!images.isNull())
    {
        g_theme.weatherIcon = images["weatherIcon"] | g_theme.weatherIcon;
        g_theme.wifiIcon = images["wifiIcon"] | "";
        g_theme.batteryIcon = images["batteryIcon"] | "";
    }

    Serial.printf("[THEME] loaded: %s\n", path.c_str());
    return true;
}

void saveThemeIndex()
{
    DynamicJsonDocument doc(2048);
    doc["activeTheme"] = g_themeIndex.themes[g_currentThemeIndex];
    JsonArray arr = doc.createNestedArray("themes");
    for (uint8_t i = 0; i < g_themeIndex.themeCount; i++) arr.add(g_themeIndex.themes[i]);

    File file = SPIFFS.open("/theme_config.json", "w");
    if (!file)
    {
        Serial.println("[ERR] cannot write theme_config.json");
        return;
    }
    serializeJsonPretty(doc, file);
    file.close();
}

void setDefaultThemeData()
{
    g_theme.timeText = {70, 64, 4, WHITE, "14:30"};
    g_theme.dateText = {70, 104, 2, rgbTo565(0xA7, 0xB2, 0xC7), "2026-10-24"};
    g_theme.tempText = {20, 170, 2, WHITE, "TEMP 26C"};
    g_theme.humidText = {20, 194, 2, WHITE, "HUM 45%"};
    g_theme.pressureText = {20, 218, 2, WHITE, "PRES 1013"};
    g_theme.alarmText = {14, 276, 2, rgbTo565(0xF8, 0xD5, 0x74), "TODO 15:00 MEETING"};

    g_theme.timeModule = {52, 42, 138, 94, 210, rgbTo565(0x1F, 0x2B, 0x46)};
    g_theme.envModule = {12, 152, 216, 84, 220, rgbTo565(0x25, 0x32, 0x50)};
    g_theme.alarmModule = {8, 262, 224, 44, 210, rgbTo565(0x3A, 0x32, 0x20)};
}

void renderModule(const ModuleStyle &style)
{
    uint16_t col = blend565(style.color, g_theme.backgroundColor, style.opacity);
    fillRect(style.x, style.y, style.w, style.h, col);
    drawRect(style.x, style.y, style.w, style.h, blend565(WHITE, col, 20));
}

void renderTheme()
{
    fillScreen(g_theme.backgroundColor);

    renderModule(g_theme.timeModule);
    renderModule(g_theme.envModule);
    renderModule(g_theme.alarmModule);

    drawWeatherIconSlot(g_theme.weatherIcon, g_theme.envModule.x + g_theme.envModule.w - 64, g_theme.envModule.y + 8);

    drawText(g_theme.timeText.x, g_theme.timeText.y, g_theme.timeText.value, g_theme.timeText.color, g_theme.timeText.size);
    drawText(g_theme.dateText.x, g_theme.dateText.y, g_theme.dateText.value, g_theme.dateText.color, g_theme.dateText.size);
    drawText(g_theme.tempText.x, g_theme.tempText.y, g_theme.tempText.value, g_theme.tempText.color, g_theme.tempText.size);
    drawText(g_theme.humidText.x, g_theme.humidText.y, g_theme.humidText.value, g_theme.humidText.color, g_theme.humidText.size);
    drawText(g_theme.pressureText.x, g_theme.pressureText.y, g_theme.pressureText.value, g_theme.pressureText.color, g_theme.pressureText.size);
    drawText(g_theme.alarmText.x, g_theme.alarmText.y, g_theme.alarmText.value, g_theme.alarmText.color, g_theme.alarmText.size);

    drawText(8, 8, "THEME:" + String(g_currentThemeIndex + 1), rgbTo565(0x68, 0xB0, 0xFF), 1);
}

void switchTheme()
{
    if (g_themeIndex.themeCount == 0)
        return;

    g_currentThemeIndex = (g_currentThemeIndex + 1) % g_themeIndex.themeCount;
    g_themeIndex.activeTheme = g_themeIndex.themes[g_currentThemeIndex];

    if (!loadTheme(g_themeIndex.activeTheme))
    {
        setDefaultThemeData();
    }
    saveThemeIndex();
    renderTheme();
    Serial.printf("[THEME] switched to %s\n", g_themeIndex.activeTheme.c_str());
}

void refreshStaticClockText()
{
    static uint8_t minute = 30;
    static uint8_t hour = 14;
    minute++;
    if (minute >= 60)
    {
        minute = 0;
        hour = (hour + 1) % 24;
    }

    char buff[6];
    snprintf(buff, sizeof(buff), "%02d:%02d", hour, minute);
    g_theme.timeText.value = String(buff);

    renderTheme();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 Smart TV Theme Loader");

    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    pinMode(THEME_SWITCH_BUTTON, INPUT_PULLUP);

    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_RST, HIGH);

    spi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    spi.setFrequency(40000000);
    spi.setDataMode(SPI_MODE0);
    spi.setBitOrder(MSBFIRST);

    TFT_Init();

    if (!SPIFFS.begin(true))
    {
        Serial.println("[ERR] SPIFFS mount failed");
    }

    setDefaultThemeData();
    if (loadThemeIndex())
    {
        if (!loadTheme(g_themeIndex.activeTheme))
            Serial.println("[WARN] active theme missing, using defaults");
    }
    else
    {
        Serial.println("[WARN] theme_config missing, using defaults");
        g_themeIndex.themeCount = 6;
        g_themeIndex.themes[0] = "/themes/theme1.json";
        g_themeIndex.themes[1] = "/themes/theme2.json";
        g_themeIndex.themes[2] = "/themes/theme3.json";
        g_themeIndex.themes[3] = "/themes/theme4.json";
        g_themeIndex.themes[4] = "/themes/theme5.json";
        g_themeIndex.themes[5] = "/themes/theme6.json";
    }

    renderTheme();
    Serial.println("Press button GPIO0 or send 'n' in serial to switch theme");
}

void loop()
{
    if (digitalRead(THEME_SWITCH_BUTTON) == LOW && millis() - g_lastButtonTick > 350)
    {
        g_lastButtonTick = millis();
        switchTheme();
    }

    if (Serial.available())
    {
        char c = Serial.read();
        if (c == 'n' || c == 'N')
            switchTheme();
        if (c == 'r' || c == 'R')
        {
            loadThemeIndex();
            loadTheme(g_themeIndex.activeTheme);
            renderTheme();
            Serial.println("[THEME] reloaded from SPIFFS");
        }
    }

    if (millis() - g_lastClockRefresh > 10000)
    {
        g_lastClockRefresh = millis();
        refreshStaticClockText();
    }
}
