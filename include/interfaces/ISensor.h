/**
 * @file ISensor.h
 * @brief 传感器接口抽象类
 * @version 1.0.0
 * 
 * 定义传感器的通用接口，所有传感器驱动必须实现此接口
 * 便于扩展和替换不同的传感器硬件
 */

#ifndef I_SENSOR_H
#define I_SENSOR_H

#include <Arduino.h>

/**
 * @brief 传感器状态枚举
 */
enum class SensorStatus {
    OK,                 // 正常
    NOT_INITIALIZED,    // 未初始化
    READ_ERROR,         // 读取错误
    INVALID_DATA,       // 数据无效
    TIMEOUT,            // 超时
    HARDWARE_ERROR      // 硬件错误
};

/**
 * @brief 传感器数据结构
 */
struct SensorData {
    float value;            // 传感器值
    uint32_t timestamp;     // 时间戳(ms)
    SensorStatus status;    // 状态
    
    SensorData() : value(0.0f), timestamp(0), status(SensorStatus::NOT_INITIALIZED) {}
};

/**
 * @brief 传感器接口抽象类
 */
class ISensor {
public:
    virtual ~ISensor() = default;
    
    /**
     * @brief 初始化传感器
     * @return true 成功, false 失败
     */
    virtual bool begin() = 0;
    
    /**
     * @brief 读取传感器数据
     * @return SensorData 传感器数据
     */
    virtual SensorData read() = 0;
    
    /**
     * @brief 检查传感器是否可用
     * @return true 可用, false 不可用
     */
    virtual bool isAvailable() = 0;
    
    /**
     * @brief 获取传感器名称
     * @return const char* 传感器名称
     */
    virtual const char* getName() = 0;
    
    /**
     * @brief 重置传感器
     */
    virtual void reset() = 0;
};

#endif // I_SENSOR_H
