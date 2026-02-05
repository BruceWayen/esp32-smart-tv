/**
 * @file ButtonManager.h
 * @brief 按键管理器 - 支持短按/长按回调
 * @version 1.0.0
 * @date 2026-02-04
 */

#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

typedef void (*ButtonCallback)(int buttonId, bool isLongPress);

class ButtonManager {
public:
    static ButtonManager& getInstance();

    bool begin();
    void update();
    void setButtonCallback(ButtonCallback callback);

private:
    ButtonManager();
    ~ButtonManager() = default;
    ButtonManager(const ButtonManager&) = delete;
    ButtonManager& operator=(const ButtonManager&) = delete;

    struct ButtonState {
        uint8_t pin;
        bool lastLevel;
        uint32_t lastChangeTime;
        uint32_t pressStart;
        bool longPressFired;
    };

    ButtonState _buttons[2];
    ButtonCallback _callback;

    void handleButton(ButtonState& state, int id);
};

#endif // BUTTON_MANAGER_H
