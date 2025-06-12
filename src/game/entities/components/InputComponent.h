#pragma once

#include "Component.h"
#include "../Entity.h"
#include "../../../engine/input/InputHandler.h"
#include "TransformComponent.h"

namespace Game {

// Handles player input for entity movement
class InputComponent : public Component {
public:
    InputComponent(float moveSpeed = 200.0f) : moveSpeed(moveSpeed) {}
    
    void Update(float deltaTime) override {
        auto* entity = GetOwner();
        if (!entity || !entity->HasComponent<TransformComponent>()) {
            return;
        }
        
        auto& transform = entity->GetComponent<TransformComponent>();
        Engine::InputHandler& input = Engine::InputHandler::GetInstance();
        
        // Calculate movement
        float dx = 0.0f, dy = 0.0f;
        
        if (input.IsActionPressed(Engine::InputAction::MOVE_UP)) {
            dy -= moveSpeed * deltaTime;
        }
        if (input.IsActionPressed(Engine::InputAction::MOVE_DOWN)) {
            dy += moveSpeed * deltaTime;
        }
        if (input.IsActionPressed(Engine::InputAction::MOVE_LEFT)) {
            dx -= moveSpeed * deltaTime;
        }
        if (input.IsActionPressed(Engine::InputAction::MOVE_RIGHT)) {
            dx += moveSpeed * deltaTime;
        }
        
        // Apply movement
        if (dx != 0.0f || dy != 0.0f) {
            transform.Move(dx, dy);
        }
    }
    
    // Getters and setters
    float GetMoveSpeed() const { return moveSpeed; }
    void SetMoveSpeed(float speed) { moveSpeed = speed; }
    
private:
    float moveSpeed;
};

} // namespace Game 