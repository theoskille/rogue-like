#pragma once

#include <string>
#include "StateManager.h"
#include "GameTime.h"
#include "../input/InputHandler.h"
#include "../rendering/Renderer.h"

namespace Engine {

class Application {
public:
    // Constructor with default window settings
    Application(int width = 1024, int height = 768, const std::string& title = "Rogue-Like");
    
    // Destructor
    ~Application();
    
    // Initialization
    bool Initialize();
    
    // Main application loop
    void Run();
    
    // Cleanup
    void Shutdown();
    
    // Window properties
    int GetScreenWidth() const;
    int GetScreenHeight() const;
    
    // Singleton access
    static Application& GetInstance();
    
private:
    // Window properties
    int screenWidth;
    int screenHeight;
    std::string windowTitle;
    
    // Application state
    bool isRunning;
    
    // Game systems
    StateManager stateManager;
    GameTime gameTime;
    InputHandler inputHandler;
    Renderer renderer;
    
    // Singleton instance
    static Application* instance;
};

} // namespace Engine 