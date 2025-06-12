#pragma once

#include "Component.h"
#include "../Entity.h"
#include "../../../engine/rendering/Renderer.h"
#include "TransformComponent.h"

namespace Game {

// Handles entity rendering
class RenderComponent : public Component {
public:
    enum class RenderShape {
        CIRCLE,
        RECTANGLE
    };
    
    RenderComponent(RenderShape shape = RenderShape::CIRCLE, 
                    Engine::RColor color = RED,
                    float size = 20.0f)
        : shape(shape), color(color), size(size) {}
    
    // Render override
    void Render() override {
        // Get the transform component for position
        auto* entity = GetOwner();
        if (!entity || !entity->HasComponent<TransformComponent>()) {
            return;
        }
        
        auto& transform = entity->GetComponent<TransformComponent>();
        Engine::Renderer& renderer = Engine::Renderer::GetInstance();
        
        // Render based on shape type
        if (shape == RenderShape::CIRCLE) {
            renderer.DrawCircle(
                static_cast<int>(transform.GetX()),
                static_cast<int>(transform.GetY()),
                size * transform.GetScaleX(),
                color
            );
        } 
        else if (shape == RenderShape::RECTANGLE) {
            float halfWidth = size * transform.GetScaleX() / 2.0f;
            float halfHeight = size * transform.GetScaleY() / 2.0f;
            
            renderer.DrawRect(
                static_cast<int>(transform.GetX() - halfWidth),
                static_cast<int>(transform.GetY() - halfHeight),
                static_cast<int>(size * transform.GetScaleX()),
                static_cast<int>(size * transform.GetScaleY()),
                color
            );
        }
    }
    
    // Shape setters and getters
    void SetShape(RenderShape newShape) { shape = newShape; }
    RenderShape GetShape() const { return shape; }
    
    // Color setters and getters
    void SetColor(const Engine::RColor& newColor) { color = newColor; }
    const Engine::RColor& GetColor() const { return color; }
    
    // Size setters and getters
    void SetSize(float newSize) { size = newSize; }
    float GetSize() const { return size; }
    
private:
    RenderShape shape;
    Engine::RColor color;
    float size;
};

} // namespace Game 