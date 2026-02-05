# 主题布局与数据接口说明

本文件用于说明主题布局配置、天气/环境数据接口占位，以及图标映射配置的使用方式，便于后续替换成真实接口与素材。

## 1. 主题配置（/config/theme.json）

`theme.json` 保存当前正在使用的主题配置。字段说明如下：

| 字段 | 说明 |
| --- | --- |
| id | 主题ID |
| name | 主题名称 |
| showSensors | 是否显示传感器区域 |
| wallpaper | 对应的主题参考图路径（data/themes/） |
| colors | 主题配色 |
| layout | 主题布局（像素单位） |

布局包含 5 个矩形区域：`header`（顶部标题）、`clock`（时间）、`weather`（天气）、`sensors`（环境数据）、`footer`（底部栏）。每个矩形包含 `x/y/w/h`。

## 2. 主题列表（/config/themes.json）

`themes.json` 存储可切换的主题列表，按键短按可在列表中前后切换。建议将新的主题布局方案追加到该文件，以便通过按键立即生效。

## 3. 天气图标配置（/config/weather_icons.json）

天气图标采用配置映射的方式占位，格式如下：

```json
{
  "icons": [
    { "key": "sunny", "type": "sun", "color": "#FFD200" }
  ]
}
```

- `key`：天气接口返回的图标关键字。
- `type`：图标渲染类型（sun/cloud/rain/snow/unknown）。
- `color`：图标渲染颜色（RGB16会由程序自动转换）。

当你提供真实图标文件时，可以继续使用 `key` 字段来做映射；图标渲染逻辑可进一步扩展为位图渲染。

## 4. 数据接口占位

当前桌面所需的温湿度、气压、天气等数据来源均通过接口占位：

- 环境数据接口：`IEnvironmentProvider`
- 天气数据接口：`IWeatherProvider`

目前提供 `MockEnvironmentProvider` 与 `MockWeatherProvider` 作为模拟数据源，后续对接真实传感器或天气接口时，只需替换实现类即可。

## 5. 按键切换逻辑

- 按键1短按：切换到下一套主题
- 按键2短按：切换到上一套主题

按键行为可在 `ButtonManager` 中扩展（长按、组合键等）。
