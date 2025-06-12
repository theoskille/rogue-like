#pragma once

#include "Widget.h"
#include <vector>
#include <functional>
#include <memory>

namespace Engine {
namespace UI {

// Forward declaration
class MenuItem;

// Menu class for navigation and selection
class Menu : public Widget {
public:
    Menu(int x, int y, int width, int height);
    ~Menu() override = default;
    
    // Add a menu item
    void AddItem(const std::string& text, std::function<void()> callback = nullptr);
    
    // Remove a menu item by index
    void RemoveItem(size_t index);
    
    // Clear all items
    void Clear();
    
    // Get the number of items
    size_t GetItemCount() const { return items.size(); }
    
    // Get the selected item index
    int GetSelectedIndex() const { return selectedIndex; }
    
    // Set the selected item index
    void SetSelectedIndex(int index);
    
    // Select the next item
    void SelectNext();
    
    // Select the previous item
    void SelectPrevious();
    
    // Activate the currently selected item
    void ActivateSelected();
    
    // Lifecycle methods
    void Update(float deltaTime) override;
    void Render() override;
    
    // Input event handlers
    bool OnKeyPressed(int key) override;
    
    // Set menu appearance
    void SetItemHeight(int height) { itemHeight = height; }
    void SetItemPadding(int padding) { itemPadding = padding; }
    void SetBackgroundColor(RColor color) { backgroundColor = color; }
    void SetSelectedBackgroundColor(RColor color) { selectedBackgroundColor = color; }
    void SetTextColor(RColor color) { textColor = color; }
    void SetSelectedTextColor(RColor color) { selectedTextColor = color; }
    
private:
    // Menu items
    std::vector<std::shared_ptr<MenuItem>> items;
    
    // Currently selected item
    int selectedIndex;
    
    // Appearance properties
    int itemHeight;
    int itemPadding;
    RColor backgroundColor;
    RColor selectedBackgroundColor;
    RColor textColor;
    RColor selectedTextColor;
    
    // Whether the menu is vertical or horizontal
    bool isVertical;
};

// Class for a single menu item
class MenuItem {
public:
    MenuItem(const std::string& text, std::function<void()> callback = nullptr);
    
    // Get the item text
    const std::string& GetText() const { return text; }
    
    // Set the item text
    void SetText(const std::string& newText) { text = newText; }
    
    // Execute the callback when the item is activated
    void Activate();
    
    // Set the callback
    void SetCallback(std::function<void()> newCallback) { callback = newCallback; }
    
private:
    std::string text;
    std::function<void()> callback;
};

} // namespace UI
} // namespace Engine 