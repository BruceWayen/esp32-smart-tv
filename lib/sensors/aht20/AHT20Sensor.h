/**
 * @file AHT20Sensor.h
 * @brief AHT20温湿度传感器驱动
 * @version 1.0.0
 */

#ifndef AHT20_SENSOR_H
#define AHT20_SENSOR_H

#include "interfaces/ISensor.h"
#include <Adafruit_AHTX0.h>

/**
 * @brief AHT20传感器类
 */
class AHT20Sensor : public ISensor {
public:
    /**
     * @brief 构造函数
     */
    AHT20Sensor();
    
    /**
     * @brief 析构函数
     */
    ~AHT20Sensor() override = default;
    
    /**
     * @brief 初始化传感器
     * @return true 成功, false 失败
     */
    bool begin() override;
    
    /**
     * @brief 读取温度
     * @return SensorData 温度数据(℃)
     */
    SensorData read() override;
    
    /**
     * @brief 读取温度
     * @return float 温度值(℃)
     */
    float readTemperature();
    
    /**
     * @brief 读取湿度
     * @return float 湿度值(%RH)
     */
    float readHumidity();
    
    /**
     * @brief 检查传感器是否可用
     * @return true 可用, false 不可用
     */
    bool isAvailable() override;
    
    /**
     * @brief 获取传感器名称
     * @return const char* 传感器名称
     */
    const char* getName() override;
    
    /**
     * @brief 重置传感器
     */
    void reset() override;

private:
    Adafruit_AHTX0 _aht;        // AHT20对象
    bool _initialized;           // 初始化标志
    uint32_t _lastReadTime;      // 上次读取时间
    float _lastTemp;             // 上次温度值
    float _lastHumi;             // 上次湿度值
    
    /**
     * @brief 验证数据有效性
     * @param temp 温度
     * @param humi 湿度
     * @return true 有效, false 无效
     */
    bool validateData(float temp, float humi);
};

#endif // AHT20_SENSOR_H
