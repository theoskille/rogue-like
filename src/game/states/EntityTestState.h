#pragma once

#include "../../engine/core/StateManager.h"
#include "../entities/Entity.h"
#include "../entities/components/TransformComponent.h"
#include "../entities/components/RenderComponent.h"
#include "../entities/components/InputComponent.h"
#include <vector>
#include <memory>

namespace Game {

class EntityTestState : public Engine::GameState {
public:
    EntityTestState();
    ~EntityTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "EntityTestState"; }
    
private:
    std::vector<std::unique_ptr<Entity>> entities;
    bool isPaused = false;
    
    // Create different entity types
    void CreatePlayer();
    void CreateObstacles();
};

} // namespace Game 