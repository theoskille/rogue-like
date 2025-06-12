#include "InputHandler.h"
#include "/opt/homebrew/include/raylib.h"
#include <iostream>

namespace Engine {

// Initialize static member
InputHandler* InputHandler::instance = nullptr;

InputHandler::InputHandler() : eventSystem(&EventSystem::GetInstance()) {
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple InputHandler instances created!" << std::endl;
    }
    
    // Set up default key bindings
    BindKey(KEY_W, InputAction::MOVE_UP);
    BindKey(KEY_S, InputAction::MOVE_DOWN);
    BindKey(KEY_A, InputAction::MOVE_LEFT);
    BindKey(KEY_D, InputAction::MOVE_RIGHT);
    BindKey(KEY_SPACE, InputAction::CONFIRM);
    BindKey(KEY_ESCAPE, InputAction::CANCEL);
    BindKey(KEY_TAB, InputAction::MENU);
    BindKey(KEY_E, InputAction::ATTACK);
    BindKey(KEY_Q, InputAction::USE_ITEM);
}

InputHandler::~InputHandler() {
    // Clean up the singleton instance
    if (instance == this) {
        instance = nullptr;
    }
}

void InputHandler::Update() {
    // Process all bound actions
    for (const auto& [action, keyCode] : actionKeyMap) {
        ProcessKey(keyCode, action);
    }
}

bool InputHandler::IsActionPressed(InputAction action) const {
    auto it = actionKeyMap.find(action);
    if (it != actionKeyMap.end()) {
        return IsKeyDown(it->second);
    }
    return false;
}

bool InputHandler::IsActionJustPressed(InputAction action) const {
    auto it = actionKeyMap.find(action);
    if (it != actionKeyMap.end()) {
        return IsKeyPressed(it->second);
    }
    return false;
}

bool InputHandler::IsActionJustReleased(InputAction action) const {
    auto it = actionKeyMap.find(action);
    if (it != actionKeyMap.end()) {
        return IsKeyReleased(it->second);
    }
    return false;
}

void InputHandler::BindKey(int keyCode, InputAction action) {
    // Remove old binding for this action if it exists
    auto it = actionKeyMap.find(action);
    if (it != actionKeyMap.end()) {
        keyActionMap.erase(it->second);
    }
    
    // Set new binding
    actionKeyMap[action] = keyCode;
    keyActionMap[keyCode] = action;
}

int InputHandler::GetKeyForAction(InputAction action) const {
    auto it = actionKeyMap.find(action);
    if (it != actionKeyMap.end()) {
        return it->second;
    }
    return -1; // No key bound
}

void InputHandler::ProcessKey(int keyCode, InputAction action) {
    // Check for key press
    if (IsKeyPressed(keyCode)) {
        // Create and publish key press event
        Event event(EventType::KEY_PRESSED);
        event.SetData("action", static_cast<int>(action));
        event.SetData("key_code", keyCode);
        eventSystem->Publish(event);
    }
    
    // Check for key release
    if (IsKeyReleased(keyCode)) {
        // Create and publish key release event
        Event event(EventType::KEY_RELEASED);
        event.SetData("action", static_cast<int>(action));
        event.SetData("key_code", keyCode);
        eventSystem->Publish(event);
    }
}

InputHandler& InputHandler::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: InputHandler instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static InputHandler defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 