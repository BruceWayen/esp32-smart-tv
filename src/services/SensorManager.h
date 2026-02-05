/**
 * @file SensorManager.h
 * @brief 传感器管理器 - 统一管理所有传感器
 * @version 1.0.0
 * 
 * 负责传感器的初始化、数据采集、滤波处理
 * 实现观察者模式，支持数据变化回调
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "interfaces/ISensor.h"
#include "models/EnvironmentData.h"

/**
 * @brief 数据变化回调函数类型
 */
typedef void (*DataCallback)(const EnvironmentData& data);

/**
 * @brief 传感器管理器类
 */
class SensorManager {
public:
    /**
     * @brief 获取单例实例
     * @return SensorManager& 单例引用
     */
    static SensorManager& getInstance();
    
    /**
     * @brief 初始化所有传感器
     * @return bool true成功, false失败
     */
    bool begin();
    
    /**
     * @brief 更新传感器数据（在loop中调用）
     */
    void update();
    
    /**
     * @brief 获取当前环境数据
     * @return const EnvironmentData& 环境数据引用
     */
    const EnvironmentData& getData() const;
    
    /**
     * @brief 注册数据变化回调
     * @param callback 回调函数
     */
    void setDataCallback(DataCallback callback);
    
    /**
     * @brief 获取温度
     * @return float 温度值(℃)
     */
    float getTemperature() const;
    
    /**
     * @brief 获取湿度
     * @return float 湿度值(%RH)
     */
    float getHumidity() const;
    
    /**
     * @brief 获取气压
     * @return float 气压值(hPa)
     */
    float getPressure() const;
    
    /**
     * @brief 获取光照强度
     * @return float 光照值(lx)
     */
    float getLightLevel() const;
    
    /**
     * @brief 检查所有传感器状态
     * @return bool true全部正常, false有异常
     */
    bool checkStatus();

private:
    // 单例模式
    SensorManager();
    ~SensorManager() = default;
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;
    
    // 滤波缓冲区
    static const int FILTER_SIZE = 5;
    float _tempBuffer[FILTER_SIZE];
    float _humiBuffer[FILTER_SIZE];
    float _pressBuffer[FILTER_SIZE];
    int _bufferIndex;
    
    // 当前数据
    EnvironmentData _currentData;
    
    // 上次采样时间
    uint32_t _lastSampleTime;
    uint32_t _lastLightSampleTime;
    
    // 数据回调
    DataCallback _dataCallback;
    
    /**
     * @brief 移动平均滤波
     * @param buffer 缓冲区
     * @param newValue 新值
     * @return float 滤波后的值
     */
    float movingAverage(float* buffer, float newValue);
    
    /**
     * @brief 采样环境数据
     */
    void sampleEnvironment();
    
    /**
     * @brief 采样光照数据
     */
    void sampleLight();
};

#endif // SENSOR_MANAGER_H
