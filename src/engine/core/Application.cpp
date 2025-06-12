#include "Application.h"
#include "/opt/homebrew/include/raylib.h"
#include <iostream>
#include "TestStates.h"

namespace Engine {

// Initialize static member
Application* Application::instance = nullptr;

Application::Application(int width, int height, const std::string& title)
    : screenWidth(width), screenHeight(height), windowTitle(title), isRunning(false),
      renderer(width, height, title) {
    
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple Application instances created!" << std::endl;
    }
}

Application::~Application() {
    // Clean up the singleton instance
    if (instance == this) {
        instance = nullptr;
    }
}

bool Application::Initialize() {
    // Initialize renderer
    if (!renderer.Initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }
    
    // Start with the menu state
    stateManager.PushState(std::make_unique<MenuState>());
    
    isRunning = true;
    std::cout << "Application initialized successfully." << std::endl;
    return true;
}

void Application::Run() {
    if (!isRunning) {
        std::cerr << "Application not initialized. Call Initialize() first." << std::endl;
        return;
    }
    
    // Main game loop
    while (!WindowShouldClose() && isRunning) {
        // Update game time
        gameTime.Update();
        
        // Process input
        inputHandler.Update();
        
        // Update the current state
        stateManager.Update(gameTime.GetDeltaTime());
        
        // Render frame
        renderer.BeginFrame();
        
        // Render the current state
        stateManager.Render();
        
        renderer.EndFrame();
        
        // Check if we should exit (no states left)
        if (stateManager.IsEmpty()) {
            isRunning = false;
        }
    }
}

void Application::Shutdown() {
    isRunning = false;
    renderer.Shutdown();
    std::cout << "Application shut down." << std::endl;
}

int Application::GetScreenWidth() const {
    return screenWidth;
}

int Application::GetScreenHeight() const {
    return screenHeight;
}

Application& Application::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: Application instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static Application defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 