#include "TestStates.h"
#include <iostream>
#include <cmath>
#include "../input/InputHandler.h"
#include "../rendering/Renderer.h"

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
    Renderer& renderer = Renderer::GetInstance();
    
    // Clear the background
    renderer.ClearBackground(RAYWHITE);
    
    // Animated title - for now we just render it without animation
    renderer.DrawTextCentered("ROGUE-LIKE GAME", 
                             renderer.GetScreenWidth() / 2, 
                             100, 
                             40, 
                             MAROON);
    
    // Draw menu options
    renderer.DrawTextCentered("PRESS SPACE TO START GAME", 
                             renderer.GetScreenWidth() / 2, 
                             300, 
                             20, 
                             BLACK);
                             
    renderer.DrawTextCentered("WASD - MOVE", 
                             renderer.GetScreenWidth() / 2, 
                             350, 
                             20, 
                             GRAY);
    
    // Draw buttons
    renderer.DrawButton(renderer.GetScreenWidth() / 2 - 100, 400, 200, 40, "START GAME", LIGHTGRAY, BLACK);
    renderer.DrawButton(renderer.GetScreenWidth() / 2 - 100, 460, 200, 40, "OPTIONS", LIGHTGRAY, BLACK);
    renderer.DrawButton(renderer.GetScreenWidth() / 2 - 100, 520, 200, 40, "QUIT", LIGHTGRAY, BLACK);
    
    // Display pause message if paused
    if (isPaused) {
        renderer.DrawTextCentered("PAUSED", 
                                 renderer.GetScreenWidth() / 2, 
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
    Renderer& renderer = Renderer::GetInstance();
    playerX = renderer.GetScreenWidth() / 2;
    playerY = renderer.GetScreenHeight() / 2;
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
    Renderer& renderer = Renderer::GetInstance();
    
    // Calculate movement speed
    float moveSpeed = 200.0f * deltaTime;
    
    // Apply movement based on input actions
    if (input.IsActionPressed(InputAction::MOVE_UP)) playerY -= moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_DOWN)) playerY += moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_LEFT)) playerX -= moveSpeed;
    if (input.IsActionPressed(InputAction::MOVE_RIGHT)) playerX += moveSpeed;
    
    // Keep player within screen bounds
    playerX = playerX < 25 ? 25 : (playerX > renderer.GetScreenWidth() - 25 ? renderer.GetScreenWidth() - 25 : playerX);
    playerY = playerY < 25 ? 25 : (playerY > renderer.GetScreenHeight() - 25 ? renderer.GetScreenHeight() - 25 : playerY);
    
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
    Renderer& renderer = Renderer::GetInstance();
    
    // Clear the background
    renderer.ClearBackground(BLACK);
    
    // Draw game title
    renderer.DrawTextCentered("GAMEPLAY", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             40, 
                             WHITE);
    
    // Draw placeholder battlefield (just draw directly without passing a pointer)
    // We'll implement this properly when we have the Battlefield class
    // renderer.DrawBattlefield(*(Battlefield*)nullptr);
    
    // Draw player (as a simple circle)
    renderer.DrawCircle(playerX, playerY, 25, RED);
    
    // Draw instructions
    renderer.DrawTextCentered("Move with WASD, ESC to return to Menu", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 60, 
                             20, 
                             WHITE);
                             
    renderer.DrawTextCentered("E to attack, Q to use item", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 30, 
                             20, 
                             WHITE);
    
    // Display pause message if paused
    if (isPaused) {
        renderer.DrawTextCentered("PAUSED", 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() / 2, 
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