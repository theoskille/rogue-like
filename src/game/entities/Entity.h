#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include "components/Component.h"

namespace Game {

// Base class for all game entities
class Entity {
public:
    Entity(const std::string& name = "Entity");
    virtual ~Entity();
    
    // Component management
    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);
    
    template<typename T>
    T& GetComponent();
    
    template<typename T>
    bool HasComponent() const;
    
    template<typename T>
    void RemoveComponent();
    
    // Lifecycle methods
    virtual void Start();
    virtual void Update(float deltaTime);
    virtual void Render();
    
    // Entity properties
    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }
    
private:
    std::string name;
    bool isActive = true;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

// Template implementation

template<typename T, typename... Args>
T& Entity::AddComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    // Check if component already exists
    auto typeIndex = std::type_index(typeid(T));
    if (components.find(typeIndex) != components.end()) {
        return static_cast<T&>(*components[typeIndex]);
    }
    
    // Create and store component
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T& componentRef = *component;
    
    // Add to component map
    components[typeIndex] = std::move(component);
    
    // Notify component it was attached
    componentRef.OnAttach(this);
    
    return componentRef;
}

template<typename T>
T& Entity::GetComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    auto typeIndex = std::type_index(typeid(T));
    auto it = components.find(typeIndex);
    
    if (it == components.end()) {
        throw std::runtime_error("Component not found on entity");
    }
    
    return static_cast<T&>(*it->second);
}

template<typename T>
bool Entity::HasComponent() const {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    auto typeIndex = std::type_index(typeid(T));
    return components.find(typeIndex) != components.end();
}

template<typename T>
void Entity::RemoveComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    auto typeIndex = std::type_index(typeid(T));
    auto it = components.find(typeIndex);
    
    if (it != components.end()) {
        it->second->OnDetach();
        components.erase(it);
    }
}

} // namespace Game 