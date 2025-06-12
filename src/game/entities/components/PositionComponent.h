#pragma once

#include "Component.h"

namespace Game {

// Component that handles entity position on the battlefield
class PositionComponent : public Component {
public:
    PositionComponent();
    ~PositionComponent() override = default;
    
    // Lifecycle methods
    void Start() override;
    
    // Get the current position (0-7 for an 8-tile battlefield)
    int GetPosition() const { return position; }
    
    // Set the position directly
    void SetPosition(int newPosition);
    
    // Move forward (toward higher position)
    bool MoveForward(int steps = 1);
    
    // Move backward (toward lower position)
    bool MoveBackward(int steps = 1);
    
    // Check if entity is at the leftmost position
    bool IsAtLeftEdge() const { return position <= 0; }
    
    // Check if entity is at the rightmost position
    bool IsAtRightEdge() const { return position >= maxPosition; }
    
    // Check if entity can move to a specific position
    bool CanMoveTo(int targetPosition) const;
    
    // Get the number of tiles between this entity and another
    int GetDistanceTo(const PositionComponent& other) const;
    
    // Check if this entity is within range of another
    bool IsWithinRange(const PositionComponent& other, int range) const;
    
    // Get the direction to another entity (1 = forward, -1 = backward)
    int GetDirectionTo(const PositionComponent& other) const;
    
    // Get the max position (default is 7 for 8-tile battlefield)
    int GetMaxPosition() const { return maxPosition; }
    
    // Set the battlefield size
    void SetBattlefieldSize(int size);
    
private:
    // Current position on the battlefield (0-7 for default 8-tile battlefield)
    int position;
    
    // Maximum position (battlefield size - 1)
    int maxPosition;
    
    // Validate a position is within bounds
    bool IsValidPosition(int pos) const { return pos >= 0 && pos <= maxPosition; }
};

} // namespace Game 