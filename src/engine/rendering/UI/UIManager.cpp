#include "UIManager.h"
#include <algorithm>
#include <iostream>

namespace Engine {
namespace UI {

// Initialize static instance
UIManager* UIManager::instance = nullptr;

UIManager::UIManager() : focusedWidget(nullptr) {
}

UIManager::~UIManager() {
    Clear();
}

UIManager& UIManager::GetInstance() {
    if (instance == nullptr) {
        instance = new UIManager();
    }
    return *instance;
}

void UIManager::AddWidget(std::shared_ptr<Widget> widget) {
    if (!widget) return;
    
    // Add to list
    widgets.push_back(widget);
    
    // Add to map for name lookup
    const std::string& name = widget->GetName();
    if (!name.empty()) {
        widgetMap[name] = widget;
    }
}

void UIManager::RemoveWidget(Widget* widget) {
    if (!widget) return;
    
    // Remove from name map
    widgetMap.erase(widget->GetName());
    
    // Clear focus if needed
    if (focusedWidget == widget) {
        focusedWidget = nullptr;
    }
    
    // Remove from list
    widgets.erase(
        std::remove_if(widgets.begin(), widgets.end(),
            [widget](const std::shared_ptr<Widget>& w) {
                return w.get() == widget;
            }),
        widgets.end()
    );
}

void UIManager::RemoveWidgetByName(const std::string& name) {
    auto it = widgetMap.find(name);
    if (it != widgetMap.end()) {
        Widget* widget = it->second.get();
        RemoveWidget(widget);
    }
}

std::shared_ptr<Widget> UIManager::GetWidgetByName(const std::string& name) {
    auto it = widgetMap.find(name);
    if (it != widgetMap.end()) {
        return it->second;
    }
    return nullptr;
}

void UIManager::Clear() {
    widgets.clear();
    widgetMap.clear();
    focusedWidget = nullptr;
}

void UIManager::Update(float deltaTime) {
    // Update all widgets
    for (auto& widget : widgets) {
        if (widget->IsEnabled()) {
            widget->Update(deltaTime);
        }
    }
}

void UIManager::Render() {
    // Render all visible widgets
    for (auto& widget : widgets) {
        if (widget->IsVisible()) {
            widget->Render();
        }
    }
}

bool UIManager::OnKeyPressed(int key) {
    // If there's a focused widget, let it handle the key press first
    if (focusedWidget && focusedWidget->IsEnabled()) {
        if (focusedWidget->OnKeyPressed(key)) {
            return true;
        }
    }
    
    // Otherwise, process all widgets in reverse order (top to bottom)
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        auto& widget = *it;
        if (widget->IsEnabled() && widget.get() != focusedWidget) {
            if (widget->OnKeyPressed(key)) {
                return true;
            }
        }
    }
    
    return false;
}

bool UIManager::OnKeyReleased(int key) {
    // Similar pattern to key pressed
    if (focusedWidget && focusedWidget->IsEnabled()) {
        if (focusedWidget->OnKeyReleased(key)) {
            return true;
        }
    }
    
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        auto& widget = *it;
        if (widget->IsEnabled() && widget.get() != focusedWidget) {
            if (widget->OnKeyReleased(key)) {
                return true;
            }
        }
    }
    
    return false;
}

bool UIManager::OnMouseMoved(int x, int y) {
    bool handled = false;
    
    // Process all widgets to update hover state
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        auto& widget = *it;
        if (widget->IsEnabled()) {
            if (widget->OnMouseMoved(x, y)) {
                handled = true;
            }
        }
    }
    
    return handled;
}

bool UIManager::OnMousePressed(int x, int y, int button) {
    // Process from top to bottom
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        auto& widget = *it;
        if (widget->IsEnabled() && widget->IsVisible()) {
            if (widget->ContainsPoint(x, y)) {
                // Set focus to clicked widget
                SetFocusedWidget(widget.get());
                
                if (widget->OnMousePressed(x, y, button)) {
                    return true;
                }
            }
        }
    }
    
    // If we clicked outside any widget, clear focus
    ClearFocus();
    
    return false;
}

bool UIManager::OnMouseReleased(int x, int y, int button) {
    bool handled = false;
    
    // Process all widgets
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        auto& widget = *it;
        if (widget->IsEnabled() && widget->IsVisible()) {
            if (widget->OnMouseReleased(x, y, button)) {
                handled = true;
            }
        }
    }
    
    return handled;
}

void UIManager::SetFocusedWidget(Widget* widget) {
    // Skip if already focused
    if (focusedWidget == widget) return;
    
    // Clear focus from current widget
    if (focusedWidget) {
        focusedWidget->SetFocused(false);
    }
    
    // Set new focused widget
    focusedWidget = widget;
    
    // Apply focus to new widget
    if (focusedWidget) {
        focusedWidget->SetFocused(true);
    }
}

void UIManager::ClearFocus() {
    SetFocusedWidget(nullptr);
}

void UIManager::FocusNext() {
    // Skip if no widgets
    if (widgets.empty()) return;
    
    int currentIndex = GetWidgetIndex(focusedWidget);
    int nextIndex = (currentIndex + 1) % widgets.size();
    
    // Find next focusable widget
    for (int i = 0; i < static_cast<int>(widgets.size()); i++) {
        int index = (nextIndex + i) % widgets.size();
        auto& widget = widgets[index];
        
        if (widget->IsEnabled() && widget->IsVisible()) {
            SetFocusedWidget(widget.get());
            break;
        }
    }
}

void UIManager::FocusPrevious() {
    // Skip if no widgets
    if (widgets.empty()) return;
    
    int currentIndex = GetWidgetIndex(focusedWidget);
    int prevIndex = (currentIndex - 1 + widgets.size()) % widgets.size();
    
    // Find previous focusable widget
    for (int i = 0; i < static_cast<int>(widgets.size()); i++) {
        int index = (prevIndex - i + widgets.size()) % widgets.size();
        auto& widget = widgets[index];
        
        if (widget->IsEnabled() && widget->IsVisible()) {
            SetFocusedWidget(widget.get());
            break;
        }
    }
}

int UIManager::GetWidgetIndex(Widget* widget) const {
    if (!widget) return -1;
    
    for (size_t i = 0; i < widgets.size(); i++) {
        if (widgets[i].get() == widget) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

} // namespace UI
} // namespace Engine 