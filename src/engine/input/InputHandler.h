#pragma once

#include <unordered_map>
#include <string>
#include "../core/EventSystem.h"

namespace Engine {

// Input actions used throughout the game
enum class InputAction {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    CONFIRM,
    CANCEL,
    MENU,
    ATTACK,
    USE_ITEM
};

// Input handler translates raw input to game actions
class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    
    // Process input and generate events
    void Update();
    
    // Check if an action is currently pressed
    bool IsActionPressed(InputAction action) const;
    
    // Check if an action was just pressed this frame
    bool IsActionJustPressed(InputAction action) const;
    
    // Check if an action was just released this frame
    bool IsActionJustReleased(InputAction action) const;
    
    // Rebind a key to an action
    void BindKey(int keyCode, InputAction action);
    
    // Get the key bound to an action
    int GetKeyForAction(InputAction action) const;
    
    // Singleton access
    static InputHandler& GetInstance();
    
private:
    // Maps action to key code
    std::unordered_map<InputAction, int> actionKeyMap;
    
    // Maps key code to action
    std::unordered_map<int, InputAction> keyActionMap;
    
    // Reference to the event system for publishing input events
    EventSystem* eventSystem;
    
    // Process a specific key and potentially generate events
    void ProcessKey(int keyCode, InputAction action);
    
    // Singleton instance
    static InputHandler* instance;
};

} // namespace Engine 