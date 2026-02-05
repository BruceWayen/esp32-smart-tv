/**
 * @file IEnvironmentProvider.h
 * @brief 环境数据提供接口
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef I_ENVIRONMENT_PROVIDER_H
#define I_ENVIRONMENT_PROVIDER_H

#include <Arduino.h>
#include "models/EnvironmentData.h"

/**
 * @brief 环境数据提供者接口
 */
class IEnvironmentProvider {
public:
    virtual ~IEnvironmentProvider() = default;

    /**
     * @brief 初始化数据源
     */
    virtual bool begin() = 0;

    /**
     * @brief 更新数据
     * @return true 数据有更新
     */
    virtual bool update() = 0;

    /**
     * @brief 获取当前数据
     */
    virtual const EnvironmentData& getData() const = 0;
};

#endif // I_ENVIRONMENT_PROVIDER_H
