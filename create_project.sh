#!/bin/bash

# 创建项目目录结构
mkdir -p {src,include,lib,data,test,docs}
mkdir -p src/{core,drivers,services,ui,utils}
mkdir -p include/{config,interfaces}
mkdir -p lib/{sensors,display,audio,power,rtc,network}
mkdir -p data/{sounds,themes,fonts}

# 创建子目录
mkdir -p lib/sensors/{aht20,bmp280,bh1750}
mkdir -p lib/display/{ili9341,ui_components}
mkdir -p lib/audio/{microphone,speaker,vad}
mkdir -p lib/power/{battery,charging}
mkdir -p lib/rtc/{ds3231,alarm}
mkdir -p lib/network/{wifi,api}

echo "项目目录结构创建完成！"
tree -L 3 -I 'test_*'

