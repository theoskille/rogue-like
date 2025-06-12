#pragma once

#include "Widget.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace Engine {
namespace UI {

// UIManager handles all UI widgets and input routing
class UIManager {
public:
    // Singleton pattern
    static UIManager& GetInstance();
    
    // Add a widget to the manager
    void AddWidget(std::shared_ptr<Widget> widget);
    
    // Remove a widget by pointer
    void RemoveWidget(Widget* widget);
    
    // Remove a widget by name
    void RemoveWidgetByName(const std::string& name);
    
    // Get a widget by name
    std::shared_ptr<Widget> GetWidgetByName(const std::string& name);
    
    // Clear all widgets
    void Clear();
    
    // Update all widgets
    void Update(float deltaTime);
    
    // Render all widgets
    void Render();
    
    // Process input events
    bool OnKeyPressed(int key);
    bool OnKeyReleased(int key);
    bool OnMouseMoved(int x, int y);
    bool OnMousePressed(int x, int y, int button);
    bool OnMouseReleased(int x, int y, int button);
    
    // Focus management
    void SetFocusedWidget(Widget* widget);
    void ClearFocus();
    Widget* GetFocusedWidget() const { return focusedWidget; }
    
    // Navigate focus using keyboard
    void FocusNext();
    void FocusPrevious();
    
private:
    UIManager();
    ~UIManager();
    
    // Singleton instance
    static UIManager* instance;
    
    // List of widgets
    std::vector<std::shared_ptr<Widget>> widgets;
    
    // Map of widget names to widgets for quick lookup
    std::unordered_map<std::string, std::shared_ptr<Widget>> widgetMap;
    
    // Currently focused widget
    Widget* focusedWidget;
    
    // Get index of widget in the list
    int GetWidgetIndex(Widget* widget) const;
};

} // namespace UI
} // namespace Engine 