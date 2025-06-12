#include "Renderer.h"
#include <iostream>

namespace Engine {

// Initialize static member
Renderer* Renderer::instance = nullptr;

Renderer::Renderer(int width, int height, const std::string& title)
    : screenWidth(width), screenHeight(height), windowTitle(title), initialized(false) {
    
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple Renderer instances created!" << std::endl;
    }
}

Renderer::~Renderer() {
    // Clean up the singleton instance
    if (instance == this) {
        instance = nullptr;
    }
    
    // Ensure we're properly shut down
    if (initialized) {
        Shutdown();
    }
}

bool Renderer::Initialize() {
    // Avoid double initialization
    if (initialized) {
        std::cerr << "Warning: Renderer already initialized!" << std::endl;
        return true;
    }
    
    // Initialize Raylib window
    ::InitWindow(screenWidth, screenHeight, windowTitle.c_str());
    ::SetTargetFPS(60);
    
    if (!::IsWindowReady()) {
        std::cerr << "Failed to initialize Raylib window!" << std::endl;
        return false;
    }
    
    initialized = true;
    return true;
}

void Renderer::Shutdown() {
    if (!initialized) {
        return;
    }
    
    ::CloseWindow();
    initialized = false;
}

void Renderer::BeginFrame() {
    if (!initialized) {
        std::cerr << "Warning: BeginFrame called without initialization!" << std::endl;
        return;
    }
    
    ::BeginDrawing();
}

void Renderer::EndFrame() {
    if (!initialized) {
        std::cerr << "Warning: EndFrame called without initialization!" << std::endl;
        return;
    }
    
    ::EndDrawing();
}

void Renderer::DrawText(const std::string& text, int x, int y, int fontSize, RColor color) {
    ::DrawText(text.c_str(), x, y, fontSize, color);
}

void Renderer::DrawTextCentered(const std::string& text, int x, int y, int fontSize, RColor color) {
    int textWidth = ::MeasureText(text.c_str(), fontSize);
    int textX = x - textWidth / 2;
    ::DrawText(text.c_str(), textX, y, fontSize, color);
}

void Renderer::DrawRect(int x, int y, int width, int height, RColor color) {
    ::DrawRectangle(x, y, width, height, color);
}

void Renderer::DrawRectLines(int x, int y, int width, int height, RColor color) {
    ::DrawRectangleLines(x, y, width, height, color);
}

void Renderer::DrawCircle(int x, int y, int radius, RColor color) {
    ::DrawCircle(x, y, radius, color);
}

void Renderer::ClearBackground(RColor color) {
    ::ClearBackground(color);
}

void Renderer::DrawButton(int x, int y, int width, int height, const std::string& text, RColor bgColor, RColor textColor) {
    // Draw button rectangle
    DrawRect(x, y, width, height, bgColor);
    DrawRectLines(x, y, width, height, BLACK);
    
    // Draw button text centered
    int textWidth = ::MeasureText(text.c_str(), 20);
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - 20) / 2;
    DrawText(text, textX, textY, 20, textColor);
}

bool Renderer::IsButtonPressed(int x, int y, int width, int height) {
    // Check if mouse is inside button
    Vector2 mousePos = ::GetMousePosition();
    bool mouseInside = (mousePos.x >= x && mousePos.x <= x + width &&
                         mousePos.y >= y && mousePos.y <= y + height);
    
    // Return true if mouse is inside and left button was pressed
    return mouseInside && ::IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Renderer::DrawBattlefield(const Battlefield& /* battlefield */) {
    // This is a placeholder since we haven't implemented Battlefield yet
    // Will be implemented later when we have the Battlefield class
    DrawRect(100, 300, 600, 100, LIGHTGRAY);
    DrawRectLines(100, 300, 600, 100, BLACK);
    
    // Draw battlefield positions
    for (int i = 0; i < 8; ++i) {
        int x = 100 + i * 75;
        DrawRectLines(x, 300, 75, 100, BLACK);
        DrawText(std::to_string(i), x + 35, 340, 20, DARKGRAY);
    }
}

int Renderer::GetScreenWidth() const {
    return screenWidth;
}

int Renderer::GetScreenHeight() const {
    return screenHeight;
}

void Renderer::SetTargetFPS(int fps) {
    ::SetTargetFPS(fps);
}

Renderer& Renderer::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: Renderer instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static Renderer defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 