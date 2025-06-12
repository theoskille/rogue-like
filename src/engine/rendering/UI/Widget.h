#pragma once

#include <string>
#include "../Renderer.h"

namespace Engine {
namespace UI {

// Base class for all UI elements
class Widget {
public:
    Widget(int x, int y, int width, int height);
    virtual ~Widget() = default;
    
    // Lifecycle methods
    virtual void Update(float deltaTime);
    virtual void Render();
    
    // Position and size getters/setters
    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    
    void SetPosition(int newX, int newY);
    void SetSize(int newWidth, int newHeight);
    
    // Visibility control
    bool IsVisible() const { return isVisible; }
    void SetVisible(bool visible) { isVisible = visible; }
    
    // Focus control
    bool IsFocused() const { return isFocused; }
    void SetFocused(bool focused) { isFocused = focused; }
    
    // Enable/disable control
    bool IsEnabled() const { return isEnabled; }
    void SetEnabled(bool enabled) { isEnabled = enabled; }
    
    // Check if a point is inside the widget
    bool ContainsPoint(int pointX, int pointY) const;
    
    // Get the widget's name
    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    
    // Handle input events
    virtual bool OnKeyPressed(int key);
    virtual bool OnKeyReleased(int key);
    virtual bool OnMouseMoved(int x, int y);
    virtual bool OnMousePressed(int x, int y, int button);
    virtual bool OnMouseReleased(int x, int y, int button);
    
protected:
    // Position and size
    int x;
    int y;
    int width;
    int height;
    
    // State flags
    bool isVisible;
    bool isFocused;
    bool isEnabled;
    bool isHovered;
    
    // Widget identifier
    std::string name;
    
    // Helper to get renderer reference
    Renderer& GetRenderer() const;
};

} // namespace UI
} // namespace Engine 