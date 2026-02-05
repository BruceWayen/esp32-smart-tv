/**
 * @file ClockInfo.h
 * @brief 时间数据模型定义
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef CLOCK_INFO_H
#define CLOCK_INFO_H

#include <Arduino.h>

/**
 * @brief 时钟数据结构体
 */
struct ClockInfo {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    String dateText;

    ClockInfo()
        : hour(0)
        , minute(0)
        , second(0)
        , dateText("1970-01-01") {}
};

#endif // CLOCK_INFO_H
