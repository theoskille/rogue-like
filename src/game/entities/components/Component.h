#pragma once

namespace Game {

// Forward declaration
class Entity;

// Base class for all entity components
class Component {
public:
    Component() = default;
    virtual ~Component() = default;
    
    // Called when the component is added to an entity
    virtual void OnAttach(Entity* owner) { this->owner = owner; }
    
    // Called when component is removed from an entity
    virtual void OnDetach() { owner = nullptr; }
    
    // Lifecycle methods
    virtual void Start() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    
    // Get the owning entity
    Entity* GetOwner() const { return owner; }
    
private:
    Entity* owner = nullptr;
};

} // namespace Game 