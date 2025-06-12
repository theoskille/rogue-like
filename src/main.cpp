#include "engine/core/Application.h"
#include "engine/core/EventSystem.h"
#include "engine/input/InputHandler.h"
#include <iostream>

// Function to handle key press events
void OnKeyPressed(const Engine::Event& event) {
    int keyCode = event.GetIntData("key_code");
    int actionId = event.GetIntData("action");
    
    // Convert action ID to enum for readability
    Engine::InputAction action = static_cast<Engine::InputAction>(actionId);
    
    // Print action information
    std::string actionName;
    switch (action) {
        case Engine::InputAction::MOVE_UP:    actionName = "MOVE_UP"; break;
        case Engine::InputAction::MOVE_DOWN:  actionName = "MOVE_DOWN"; break;
        case Engine::InputAction::MOVE_LEFT:  actionName = "MOVE_LEFT"; break;
        case Engine::InputAction::MOVE_RIGHT: actionName = "MOVE_RIGHT"; break;
        case Engine::InputAction::CONFIRM:    actionName = "CONFIRM"; break;
        case Engine::InputAction::CANCEL:     actionName = "CANCEL"; break;
        case Engine::InputAction::MENU:       actionName = "MENU"; break;
        case Engine::InputAction::ATTACK:     actionName = "ATTACK"; break;
        case Engine::InputAction::USE_ITEM:   actionName = "USE_ITEM"; break;
        default: actionName = "UNKNOWN"; break;
    }
    
    std::cout << "Key pressed: " << keyCode << " (Action: " << actionName << ")" << std::endl;
}

// Function to handle window resize events
void OnWindowResize(const Engine::Event& event) {
    int width = event.GetIntData("width");
    int height = event.GetIntData("height");
    std::cout << "Window resized to: " << width << "x" << height << std::endl;
}

int main() {
    // Create the event system
    Engine::EventSystem eventSystem;
    
    // Subscribe to events
    eventSystem.Subscribe(Engine::EventType::KEY_PRESSED, OnKeyPressed);
    eventSystem.Subscribe(Engine::EventType::WINDOW_RESIZE, OnWindowResize);
    
    // Test publishing events
    Engine::Event resizeEvent(Engine::EventType::WINDOW_RESIZE);
    resizeEvent.SetData("width", 1024);
    resizeEvent.SetData("height", 768);
    eventSystem.Publish(resizeEvent);
    
    // Create and initialize the application
    Engine::Application app(800, 600, "Rogue-Like Game");
    
    if (!app.Initialize()) {
        return 1;
    }
    
    // Run the main game loop
    app.Run();
    
    // Clean up
    app.Shutdown();
    
    return 0;
} 