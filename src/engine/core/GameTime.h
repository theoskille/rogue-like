#pragma once

namespace Engine {

class GameTime {
public:
    GameTime();
    
    // Update the timer for a new frame
    void Update();
    
    // Get the time elapsed since the last frame in seconds
    float GetDeltaTime() const;
    
    // Get the total elapsed time in seconds
    float GetTotalTime() const;
    
    // Get the current frame count
    unsigned int GetFrameCount() const;
    
    // Singleton access
    static GameTime& GetInstance();
    
private:
    float deltaTime;    // Time between frames in seconds
    float totalTime;    // Total time elapsed in seconds
    unsigned int frameCount; // Number of frames since start
    
    // For internal time tracking
    double lastFrameTime;
    
    // Singleton instance
    static GameTime* instance;
};

} // namespace Engine 