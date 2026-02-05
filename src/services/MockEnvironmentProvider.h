/**
 * @file MockEnvironmentProvider.h
 * @brief 模拟环境数据提供者
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef MOCK_ENVIRONMENT_PROVIDER_H
#define MOCK_ENVIRONMENT_PROVIDER_H

#include "interfaces/IEnvironmentProvider.h"

class MockEnvironmentProvider : public IEnvironmentProvider {
public:
    MockEnvironmentProvider();

    bool begin() override;
    bool update() override;
    const EnvironmentData& getData() const override;

private:
    EnvironmentData _data;
    uint32_t _lastUpdate;
    float _phase;
};

#endif // MOCK_ENVIRONMENT_PROVIDER_H
