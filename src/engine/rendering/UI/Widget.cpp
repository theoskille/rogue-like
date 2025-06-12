#include "Widget.h"

namespace Engine {
namespace UI {

Widget::Widget(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height),
      isVisible(true), isFocused(false), isEnabled(true), isHovered(false),
      name("Widget") {
}

void Widget::Update(float deltaTime) {
    // Base implementation does nothing - override in derived classes
}

void Widget::Render() {
    // Base implementation does nothing - override in derived classes
}

void Widget::SetPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Widget::SetSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

bool Widget::ContainsPoint(int pointX, int pointY) const {
    return (pointX >= x && pointX < x + width &&
            pointY >= y && pointY < y + height);
}

bool Widget::OnKeyPressed(int key) {
    // Base implementation returns false to indicate event not handled
    return false;
}

bool Widget::OnKeyReleased(int key) {
    // Base implementation returns false to indicate event not handled
    return false;
}

bool Widget::OnMouseMoved(int mouseX, int mouseY) {
    // Update hover state
    bool wasHovered = isHovered;
    isHovered = ContainsPoint(mouseX, mouseY);
    
    // Return true if hover state changed
    return (wasHovered != isHovered);
}

bool Widget::OnMousePressed(int mouseX, int mouseY, int button) {
    // Return true if click is inside the widget
    return ContainsPoint(mouseX, mouseY);
}

bool Widget::OnMouseReleased(int mouseX, int mouseY, int button) {
    // Return true if release is inside the widget
    return ContainsPoint(mouseX, mouseY);
}

Renderer& Widget::GetRenderer() const {
    return Renderer::GetInstance();
}

} // namespace UI
} // namespace Engine 