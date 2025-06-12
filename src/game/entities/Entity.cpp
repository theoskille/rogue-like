#include "Entity.h"
#include <iostream>

namespace Game {

Entity::Entity(const std::string& name) : name(name), isActive(true) {
    std::cout << "Entity created: " << name << std::endl;
}

Entity::~Entity() {
    // Clean up all components
    components.clear();
    std::cout << "Entity destroyed: " << name << std::endl;
}

void Entity::Start() {
    // Start all components
    for (auto& [type, component] : components) {
        if (isActive) {
            component->Start();
        }
    }
}

void Entity::Update(float deltaTime) {
    // Skip if inactive
    if (!isActive) return;
    
    // Update all components
    for (auto& [type, component] : components) {
        component->Update(deltaTime);
    }
}

void Entity::Render() {
    // Skip if inactive
    if (!isActive) return;
    
    // Render all components
    for (auto& [type, component] : components) {
        component->Render();
    }
}

} // namespace Game 