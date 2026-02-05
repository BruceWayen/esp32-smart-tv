/**
 * @file ButtonManager.cpp
 * @brief 按键管理器实现
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "ButtonManager.h"
#include "config/Config.h"

ButtonManager& ButtonManager::getInstance() {
    static ButtonManager instance;
    return instance;
}

ButtonManager::ButtonManager()
    : _callback(nullptr) {
    _buttons[0] = {BUTTON_1_PIN, true, 0, 0, false};
    _buttons[1] = {BUTTON_2_PIN, true, 0, 0, false};
}

bool ButtonManager::begin() {
    DEBUG_PRINTLN("[Button] 正在初始化按键...");
    for (auto& button : _buttons) {
        pinMode(button.pin, INPUT_PULLUP);
        button.lastLevel = digitalRead(button.pin);
        button.lastChangeTime = millis();
        button.pressStart = 0;
        button.longPressFired = false;
    }
    DEBUG_PRINTLN("[Button] 按键初始化完成");
    return true;
}

void ButtonManager::update() {
    handleButton(_buttons[0], 1);
    handleButton(_buttons[1], 2);
}

void ButtonManager::setButtonCallback(ButtonCallback callback) {
    _callback = callback;
}

void ButtonManager::handleButton(ButtonState& state, int id) {
    bool level = digitalRead(state.pin);
    uint32_t now = millis();

    if (level != state.lastLevel) {
        state.lastChangeTime = now;
        state.lastLevel = level;
    }

    if (now - state.lastChangeTime < BUTTON_DEBOUNCE_MS) {
        return;
    }

    if (!level && state.pressStart == 0) {
        state.pressStart = now;
        state.longPressFired = false;
    }

    if (!level && !state.longPressFired && (now - state.pressStart >= BUTTON_LONG_PRESS_MS)) {
        state.longPressFired = true;
        if (_callback) {
            _callback(id, true);
        }
    }

    if (level && state.pressStart != 0) {
        if (!state.longPressFired) {
            if (_callback) {
                _callback(id, false);
            }
        }
        state.pressStart = 0;
        state.longPressFired = false;
    }
}
