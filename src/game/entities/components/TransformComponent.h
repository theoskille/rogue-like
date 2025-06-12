#pragma once

#include "Component.h"

namespace Game {

// Handles entity position and movement
class TransformComponent : public Component {
public:
    TransformComponent(float x = 0.0f, float y = 0.0f) 
        : x(x), y(y), rotation(0.0f), scaleX(1.0f), scaleY(1.0f) {}
    
    // Getters
    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetRotation() const { return rotation; }
    float GetScaleX() const { return scaleX; }
    float GetScaleY() const { return scaleY; }
    
    // Setters
    void SetX(float newX) { x = newX; }
    void SetY(float newY) { y = newY; }
    void SetPosition(float newX, float newY) { x = newX; y = newY; }
    void SetRotation(float newRotation) { rotation = newRotation; }
    void SetScale(float newScale) { scaleX = scaleY = newScale; }
    void SetScale(float newScaleX, float newScaleY) { scaleX = newScaleX; scaleY = newScaleY; }
    
    // Movement helpers
    void Move(float deltaX, float deltaY) { x += deltaX; y += deltaY; }
    void Rotate(float deltaRotation) { rotation += deltaRotation; }
    
    // Override Update for movement based on velocity
    void Update(float deltaTime) override {
        if (velocityX != 0.0f || velocityY != 0.0f) {
            x += velocityX * deltaTime;
            y += velocityY * deltaTime;
        }
    }
    
    // Velocity setters
    void SetVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
    void SetVelocityX(float vx) { velocityX = vx; }
    void SetVelocityY(float vy) { velocityY = vy; }
    
    // Velocity getters
    float GetVelocityX() const { return velocityX; }
    float GetVelocityY() const { return velocityY; }
    
private:
    float x, y;            // Position
    float rotation;        // Rotation angle in degrees
    float scaleX, scaleY;  // Scale factors
    float velocityX = 0.0f, velocityY = 0.0f;  // Movement velocity
};

} // namespace Game 