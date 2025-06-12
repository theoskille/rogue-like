#include "PositionComponent.h"
#include <algorithm>
#include <iostream>

namespace Game {

// Helper function to clamp a value between min and max
int ClampValue(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

PositionComponent::PositionComponent() : position(0), maxPosition(7) {
    // Default to position 0 on an 8-tile battlefield
}

void PositionComponent::Start() {
    // No special initialization needed for now
}

void PositionComponent::SetPosition(int newPosition) {
    // Validate and set the position
    if (IsValidPosition(newPosition)) {
        position = newPosition;
    } else {
        std::cerr << "Warning: Attempted to set invalid position " << newPosition 
                  << ". Valid range is 0-" << maxPosition << std::endl;
        // Clamp to valid range
        position = ClampValue(newPosition, 0, maxPosition);
    }
}

bool PositionComponent::MoveForward(int steps) {
    // Check if movement is possible
    int targetPosition = position + steps;
    if (CanMoveTo(targetPosition)) {
        position = targetPosition;
        return true;
    }
    return false;
}

bool PositionComponent::MoveBackward(int steps) {
    // Moving backward is just moving forward with negative steps
    return MoveForward(-steps);
}

bool PositionComponent::CanMoveTo(int targetPosition) const {
    // Check if the target position is valid
    return IsValidPosition(targetPosition);
    
    // Note: In a full implementation, we would also check for collisions
    // with other entities, terrain obstacles, etc.
    // This will be handled in the Battlefield class later.
}

int PositionComponent::GetDistanceTo(const PositionComponent& other) const {
    // Calculate absolute distance between positions
    return std::abs(position - other.GetPosition());
}

bool PositionComponent::IsWithinRange(const PositionComponent& other, int range) const {
    // Check if other entity is within the specified range
    return GetDistanceTo(other) <= range;
}

int PositionComponent::GetDirectionTo(const PositionComponent& other) const {
    // Calculate direction: 1 = forward, -1 = backward, 0 = same position
    int otherPos = other.GetPosition();
    
    if (position < otherPos) return 1;      // Other is forward
    else if (position > otherPos) return -1; // Other is backward
    else return 0;                          // Same position
}

void PositionComponent::SetBattlefieldSize(int size) {
    // Update battlefield size (minimum size is 2)
    if (size < 2) {
        std::cerr << "Warning: Minimum battlefield size is 2. Using 2 instead of " 
                  << size << std::endl;
        size = 2;
    }
    
    maxPosition = size - 1;
    
    // Ensure current position is still valid
    if (position > maxPosition) {
        std::cerr << "Warning: Current position " << position 
                  << " is out of bounds after resize. Adjusting to " 
                  << maxPosition << std::endl;
        position = maxPosition;
    }
}

} // namespace Game 