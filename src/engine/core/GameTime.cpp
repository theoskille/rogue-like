#include "GameTime.h"
#include "/opt/homebrew/include/raylib.h"
#include <iostream>

namespace Engine {

// Initialize static member
GameTime* GameTime::instance = nullptr;

GameTime::GameTime() 
    : deltaTime(0.0f), totalTime(0.0f), frameCount(0), lastFrameTime(0.0) {
    
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple GameTime instances created!" << std::endl;
    }
    
    // Initialize time
    lastFrameTime = GetTime();
}

void GameTime::Update() {
    // Get current time
    double currentTime = GetTime();
    
    // Calculate delta time
    deltaTime = static_cast<float>(currentTime - lastFrameTime);
    
    // Update total time
    totalTime += deltaTime;
    
    // Save current time for next frame
    lastFrameTime = currentTime;
    
    // Increment frame counter
    frameCount++;
}

float GameTime::GetDeltaTime() const {
    return deltaTime;
}

float GameTime::GetTotalTime() const {
    return totalTime;
}

unsigned int GameTime::GetFrameCount() const {
    return frameCount;
}

GameTime& GameTime::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: GameTime instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static GameTime defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 