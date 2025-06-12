#include "Menu.h"
#include "../../input/InputHandler.h"

namespace Engine {
namespace UI {

// MenuItem implementation
MenuItem::MenuItem(const std::string& text, std::function<void()> callback)
    : text(text), callback(callback) {
}

void MenuItem::Activate() {
    if (callback) {
        callback();
    }
}

// Menu implementation
Menu::Menu(int x, int y, int width, int height)
    : Widget(x, y, width, height),
      selectedIndex(0),
      itemHeight(30),
      itemPadding(10),
      backgroundColor(LIGHTGRAY),
      selectedBackgroundColor(DARKGRAY),
      textColor(BLACK),
      selectedTextColor(WHITE),
      isVertical(true) {
    name = "Menu";
}

void Menu::AddItem(const std::string& text, std::function<void()> callback) {
    items.push_back(std::make_shared<MenuItem>(text, callback));
}

void Menu::RemoveItem(size_t index) {
    if (index < items.size()) {
        items.erase(items.begin() + index);
        
        // Adjust selected index if needed
        if (selectedIndex >= static_cast<int>(items.size())) {
            selectedIndex = static_cast<int>(items.size()) - 1;
            if (selectedIndex < 0) selectedIndex = 0;
        }
    }
}

void Menu::Clear() {
    items.clear();
    selectedIndex = 0;
}

void Menu::SetSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(items.size())) {
        selectedIndex = index;
    }
}

void Menu::SelectNext() {
    if (!items.empty()) {
        selectedIndex = (selectedIndex + 1) % items.size();
    }
}

void Menu::SelectPrevious() {
    if (!items.empty()) {
        selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
    }
}

void Menu::ActivateSelected() {
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) {
        items[selectedIndex]->Activate();
    }
}

void Menu::Update(float deltaTime) {
    // Base update behavior
    Widget::Update(deltaTime);
    
    // Menu-specific update logic could go here
}

void Menu::Render() {
    if (!isVisible) return;
    
    Renderer& renderer = GetRenderer();
    
    // Draw menu background
    renderer.DrawRect(x, y, width, height, backgroundColor);
    renderer.DrawRectLines(x, y, width, height, BLACK);
    
    // Draw menu items
    for (size_t i = 0; i < items.size(); i++) {
        int itemY = y + (isVertical ? (i * (itemHeight + itemPadding)) : 0);
        int itemX = x + (isVertical ? 0 : (i * (width / items.size())));
        int itemWidth = isVertical ? width : (width / items.size());
        
        // Draw item background
        RColor bgColor = (i == static_cast<size_t>(selectedIndex)) ? selectedBackgroundColor : backgroundColor;
        renderer.DrawRect(itemX, itemY, itemWidth, itemHeight, bgColor);
        
        // Draw item text
        RColor txtColor = (i == static_cast<size_t>(selectedIndex)) ? selectedTextColor : textColor;
        renderer.DrawTextCentered(
            items[i]->GetText().c_str(),
            itemX + itemWidth / 2,
            itemY + itemHeight / 2,
            16,
            txtColor
        );
    }
}

bool Menu::OnKeyPressed(int key) {
    // Handle menu navigation
    InputHandler& input = InputHandler::GetInstance();
    
    if (input.IsActionJustPressed(InputAction::MOVE_UP) && isVertical) {
        SelectPrevious();
        return true;
    }
    
    if (input.IsActionJustPressed(InputAction::MOVE_DOWN) && isVertical) {
        SelectNext();
        return true;
    }
    
    if (input.IsActionJustPressed(InputAction::MOVE_LEFT) && !isVertical) {
        SelectPrevious();
        return true;
    }
    
    if (input.IsActionJustPressed(InputAction::MOVE_RIGHT) && !isVertical) {
        SelectNext();
        return true;
    }
    
    if (input.IsActionJustPressed(InputAction::CONFIRM)) {
        ActivateSelected();
        return true;
    }
    
    return false;
}

} // namespace UI
} // namespace Engine 