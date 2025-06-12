#include "TestStates.h"
#include <iostream>
#include <cmath>
#include "../input/InputHandler.h"

namespace Engine {

// MenuState implementation
void MenuState::Enter() {
    std::cout << "Entering Menu State" << std::endl;
    animationTime = 0.0f;
    isPaused = false;
}

void MenuState::Exit() {
    std::cout << "Exiting Menu State" << std::endl;
}

void MenuState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update animation time
    animationTime += deltaTime;
    
    // Check for confirm action to transition to game state
    if (InputHandler::GetInstance().IsActionJustPressed(InputAction::CONFIRM)) {
        StateManager::GetInstance().ChangeState(std::make_unique<GameplayState>());
    }
}

void MenuState::Render() {
    // Draw title and instructions
    ClearBackground(RAYWHITE);
    
    // Animated title
    float scale = 1.0f + 0.1f * sin(animationTime * 2.0f);
    
    DrawText("ROGUE-LIKE GAME", 
             GetScreenWidth() / 2 - MeasureText("ROGUE-LIKE GAME", 40) / 2, 
             100, 
             40, 
             MAROON);
    
    // Draw menu options
    DrawText("PRESS SPACE TO START GAME", 
             GetScreenWidth() / 2 - MeasureText("PRESS SPACE TO START GAME", 20) / 2, 
             300, 
             20, 
             BLACK);
             
    DrawText("WASD - MOVE", 
             GetScreenWidth() / 2 - MeasureText("WASD - MOVE", 20) / 2, 
             350, 
             20, 
             GRAY);
             
    if (isPaused) {
        DrawText("PAUSED", 
                 GetScreenWidth() / 2 - MeasureText("PAUSED", 30) / 2, 
                 250, 
                 30, 
                 RED);
    }
}

void MenuState::Pause() {
    std::cout << "Pausing Menu State" << std::endl;
    isPaused = true;
}

void MenuState::Resume() {
    std::cout << "Resuming Menu State" << std::endl;
    isPaused = false;
}

// GameplayState implementation
void GameplayState::Enter() {
    std::cout << "Entering Gameplay State" << std::endl;
    playerX = GetScreenWidth() / 2;
    playerY = GetScreenHeight() / 2;
    animationTime = 0.0f;
    isPaused = false;
}

void GameplayState::Exit() {
    std::cout << "Exiting Gameplay State" << std::endl;
}

void GameplayState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update animation time
    animationTime += deltaTime;
    
    // Handle player movement using input actions
    InputHandler& input = InputHandler::GetInstance();
    
    // Calculate movement speed
    float moveSpeed = 200.0f * deltaTime;
    
    // Apply movement based on input actions
    if (input.IsActionPressed(InputAction::MOVE_UP)) playerY -= moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_DOWN)) playerY += moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_LEFT)) playerX -= moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_RIGHT)) playerX += moveSpeed;
    
    // Keep player within screen bounds
    playerX = playerX < 25 ? 25 : (playerX > GetScreenWidth() - 25 ? GetScreenWidth() - 25 : playerX);
    playerY = playerY < 25 ? 25 : (playerY > GetScreenHeight() - 25 ? GetScreenHeight() - 25 : playerY);
    
    // Check for cancel action to go back to menu
    if (input.IsActionJustPressed(InputAction::CANCEL)) {
        StateManager::GetInstance().ChangeState(std::make_unique<MenuState>());
    }
    
    // Handle attack action
    if (input.IsActionJustPressed(InputAction::ATTACK)) {
        std::cout << "Player attacked!" << std::endl;
    }
    
    // Handle item use action
    if (input.IsActionJustPressed(InputAction::USE_ITEM)) {
        std::cout << "Player used an item!" << std::endl;
    }
}

void GameplayState::Render() {
    ClearBackground(BLACK);
    
    // Draw game title
    DrawText("GAMEPLAY", 
             GetScreenWidth() / 2 - MeasureText("GAMEPLAY", 40) / 2, 
             30, 
             40, 
             WHITE);
    
    // Draw player (as a simple circle)
    DrawCircle(playerX, playerY, 25, RED);
    
    // Draw instructions
    DrawText("Move with WASD, ESC to return to Menu", 
             GetScreenWidth() / 2 - MeasureText("Move with WASD, ESC to return to Menu", 20) / 2, 
             GetScreenHeight() - 60, 
             20, 
             WHITE);
             
    DrawText("E to attack, Q to use item", 
             GetScreenWidth() / 2 - MeasureText("E to attack, Q to use item", 20) / 2, 
             GetScreenHeight() - 30, 
             20, 
             WHITE);
             
    if (isPaused) {
        DrawText("PAUSED", 
                 GetScreenWidth() / 2 - MeasureText("PAUSED", 30) / 2, 
                 GetScreenHeight() / 2, 
                 30, 
                 RED);
    }
}

void GameplayState::Pause() {
    std::cout << "Pausing Gameplay State" << std::endl;
    isPaused = true;
}

void GameplayState::Resume() {
    std::cout << "Resuming Gameplay State" << std::endl;
    isPaused = false;
}

} // namespace Engine 