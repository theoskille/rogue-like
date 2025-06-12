#include "EntityTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include <iostream>

namespace Game {

EntityTestState::EntityTestState() {
    std::cout << "EntityTestState created" << std::endl;
}

EntityTestState::~EntityTestState() {
    // Clear all entities
    entities.clear();
    std::cout << "EntityTestState destroyed" << std::endl;
}

void EntityTestState::Enter() {
    std::cout << "Entering Entity Test State" << std::endl;
    isPaused = false;
    
    // Create entities
    CreatePlayer();
    CreateObstacles();
    
    // Start all entities
    for (auto& entity : entities) {
        entity->Start();
    }
}

void EntityTestState::Exit() {
    std::cout << "Exiting Entity Test State" << std::endl;
    
    // Clear all entities when exiting
    entities.clear();
}

void EntityTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update all entities
    for (auto& entity : entities) {
        entity->Update(deltaTime);
    }
    
    // Check for exit (ESC key)
    if (Engine::InputHandler::GetInstance().IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void EntityTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear the background
    renderer.ClearBackground(RAYWHITE);
    
    // Render all entities
    for (auto& entity : entities) {
        entity->Render();
    }
    
    // Draw instructions
    renderer.DrawTextCentered("ENTITY COMPONENT SYSTEM TEST", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             30, 
                             BLACK);
    
    renderer.DrawTextCentered("Use WASD to move the player", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 60, 
                             20, 
                             DARKGRAY);
                             
    renderer.DrawTextCentered("Press ESC to exit", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 30, 
                             20, 
                             DARKGRAY);
    
    // Draw pause message if paused
    if (isPaused) {
        renderer.DrawTextCentered("PAUSED", 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() / 2, 
                                 40, 
                                 RED);
    }
}

void EntityTestState::Pause() {
    std::cout << "Pausing Entity Test State" << std::endl;
    isPaused = true;
}

void EntityTestState::Resume() {
    std::cout << "Resuming Entity Test State" << std::endl;
    isPaused = false;
}

void EntityTestState::CreatePlayer() {
    // Create player entity
    auto player = std::make_unique<Entity>("Player");
    
    // Get window size
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    float centerX = renderer.GetScreenWidth() / 2.0f;
    float centerY = renderer.GetScreenHeight() / 2.0f;
    
    // Add transform component
    auto& transform = player->AddComponent<TransformComponent>(centerX, centerY);
    
    // Add render component
    auto& render = player->AddComponent<RenderComponent>(RenderComponent::RenderShape::CIRCLE, RED, 25.0f);
    
    // Add input component
    auto& input = player->AddComponent<InputComponent>(300.0f);
    
    // Add to entities list
    entities.push_back(std::move(player));
}

void EntityTestState::CreateObstacles() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    float width = renderer.GetScreenWidth();
    float height = renderer.GetScreenHeight();
    
    // Create some obstacle entities at different positions
    const int numObstacles = 5;
    
    for (int i = 0; i < numObstacles; i++) {
        auto obstacle = std::make_unique<Entity>("Obstacle" + std::to_string(i));
        
        // Random position (avoiding center where player spawns)
        float x = (i % 2 == 0) ? width * 0.25f + (i * 50) : width * 0.75f - (i * 50);
        float y = height * 0.3f + (i * 60);
        
        // Add transform component
        auto& transform = obstacle->AddComponent<TransformComponent>(x, y);
        
        // Add render component (rectangles with different colors)
        Engine::RColor obstacleColor;
        switch (i % 4) {
            case 0: obstacleColor = BLUE; break;
            case 1: obstacleColor = GREEN; break;
            case 2: obstacleColor = PURPLE; break;
            case 3: obstacleColor = ORANGE; break;
        }
        
        auto& render = obstacle->AddComponent<RenderComponent>(
            RenderComponent::RenderShape::RECTANGLE, 
            obstacleColor, 
            40.0f
        );
        
        // Add to entities list
        entities.push_back(std::move(obstacle));
    }
}

} // namespace Game 