/**
 * @file EnvironmentDataProcessor.h
 * @brief 环境数据处理层 - 采集/滤波/单位转换
 * @version 1.0.0
 */

#ifndef ENVIRONMENT_DATA_PROCESSOR_H
#define ENVIRONMENT_DATA_PROCESSOR_H

#include <Arduino.h>
#include <array>
#include "config/Config.h"
#include "models/EnvironmentData.h"

class EnvironmentDataProcessor {
public:
    EnvironmentDataProcessor();

    bool processSample(float temperature, float humidity, float pressureHpa, EnvironmentData& output);
    void reset();

private:
    struct MovingAverage {
        std::array<float, SENSOR_FILTER_WINDOW_SIZE> values{};
        size_t count = 0;
        size_t index = 0;

        void reset();
        void add(float value);
        float average() const;
        bool ready() const;
    };

    MovingAverage _tempFilter;
    MovingAverage _humiFilter;
    MovingAverage _pressFilter;
};

#endif // ENVIRONMENT_DATA_PROCESSOR_H
