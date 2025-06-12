#pragma once

#include <string>
#include "/opt/homebrew/include/raylib.h"

namespace Engine {

// Forward declarations
class Battlefield;

// Color aliases for easier usage
using RColor = Color;

// Renderer class wraps around Raylib rendering
class Renderer {
public:
    Renderer(int width = 1024, int height = 768, const std::string& title = "Rogue-Like");
    ~Renderer();
    
    // Initialization and shutdown
    bool Initialize();
    void Shutdown();
    
    // Frame operations
    void BeginFrame();
    void EndFrame();
    
    // Basic drawing functions
    void DrawText(const std::string& text, int x, int y, int fontSize, RColor color);
    void DrawTextCentered(const std::string& text, int x, int y, int fontSize, RColor color);
    void DrawRect(int x, int y, int width, int height, RColor color);
    void DrawRectLines(int x, int y, int width, int height, RColor color);
    void DrawCircle(int x, int y, int radius, RColor color);
    void ClearBackground(RColor color);
    
    // UI elements
    void DrawButton(int x, int y, int width, int height, const std::string& text, RColor bgColor, RColor textColor);
    bool IsButtonPressed(int x, int y, int width, int height);
    
    // Game-specific drawing
    void DrawBattlefield(const Battlefield& battlefield);
    
    // Window properties
    int GetScreenWidth() const;
    int GetScreenHeight() const;
    void SetTargetFPS(int fps);
    
    // Singleton access
    static Renderer& GetInstance();
    
private:
    int screenWidth;
    int screenHeight;
    std::string windowTitle;
    bool initialized;
    
    // Singleton instance
    static Renderer* instance;
};

} // namespace Engine 