#include "PositionTestState.h"
#include "../../engine/input/InputHandler.h"
#include "../entities/components/PositionComponent.h"
#include <iostream>

namespace Game {

PositionTestState::PositionTestState() {
    std::cout << "PositionTestState created" << std::endl;
}

PositionTestState::~PositionTestState() {
    std::cout << "PositionTestState destroyed" << std::endl;
}

void PositionTestState::Enter() {
    std::cout << "Entering Position Test State" << std::endl;
    isPaused = false;
    
    // Create player entity
    playerEntity = std::make_unique<Entity>("Player");
    auto& playerPos = playerEntity->AddComponent<PositionComponent>();
    playerPos.SetPosition(2);  // Place player at position 2
    
    // Create enemy entity
    enemyEntity = std::make_unique<Entity>("Enemy");
    auto& enemyPos = enemyEntity->AddComponent<PositionComponent>();
    enemyPos.SetPosition(5);  // Place enemy at position 5
    
    // Start the entities
    playerEntity->Start();
    enemyEntity->Start();
    
    std::cout << "Test entities created with position components" << std::endl;
}

void PositionTestState::Exit() {
    std::cout << "Exiting Position Test State" << std::endl;
    playerEntity.reset();
    enemyEntity.reset();
}

void PositionTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Get position component references
    auto& playerPos = playerEntity->GetComponent<PositionComponent>();
    auto& enemyPos = enemyEntity->GetComponent<PositionComponent>();
    
    // Toggle action mode
    if (input.IsActionJustPressed(Engine::InputAction::ATTACK)) {
        currentMode = (currentMode == ActionMode::MOVE) 
                       ? ActionMode::RANGE_TEST 
                       : ActionMode::MOVE;
    }
    
    if (currentMode == ActionMode::MOVE) {
        // Handle player movement
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
            playerPos.MoveForward();
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
            playerPos.MoveBackward();
        }
        
        // Handle enemy movement
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
            enemyPos.MoveBackward();
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
            enemyPos.MoveForward();
        }
    } else {
        // Range test mode
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
            rangeToTest = std::min(rangeToTest + 1, battlefieldSize - 1);
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
            rangeToTest = std::max(rangeToTest - 1, 1);
        }
    }
    
    // Exit on cancel
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void PositionTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("POSITION COMPONENT TEST", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             30, 
                             BLACK);
    
    // Draw mode indicator
    std::string modeText = (currentMode == ActionMode::MOVE) 
                          ? "Mode: MOVEMENT" 
                          : "Mode: RANGE TEST";
    renderer.DrawText(modeText.c_str(), 50, 80, 20, DARKGRAY);
    
    // Draw battlefield
    RenderBattlefield(renderer.GetScreenWidth() / 2 - (battlefieldSize * tileWidth) / 2, 150);
    
    // Draw entity information
    RenderEntityInfo(50, 300);
    
    // Draw controls help
    RenderControls(renderer.GetScreenHeight() - 120);
}

void PositionTestState::Pause() {
    std::cout << "Pausing Position Test State" << std::endl;
    isPaused = true;
}

void PositionTestState::Resume() {
    std::cout << "Resuming Position Test State" << std::endl;
    isPaused = false;
}

void PositionTestState::RenderBattlefield(int x, int y) {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get position component references
    auto& playerPos = playerEntity->GetComponent<PositionComponent>();
    auto& enemyPos = enemyEntity->GetComponent<PositionComponent>();
    
    // Draw battlefield tiles
    for (int i = 0; i < battlefieldSize; i++) {
        // Alternate tile colors for visibility
        Engine::RColor tileColor = (i % 2 == 0) ? LIGHTGRAY : WHITE;
        
        // Draw tile
        renderer.DrawRect(x + i * tileWidth, y, tileWidth, tileHeight, tileColor);
        
        // Draw tile border
        renderer.DrawRectLines(x + i * tileWidth, y, tileWidth, tileHeight, DARKGRAY);
        
        // Draw position number
        renderer.DrawText(std::to_string(i).c_str(), 
                         x + i * tileWidth + tileWidth / 2 - 5, 
                         y + tileHeight - 20, 
                         16, 
                         DARKGRAY);
    }
    
    // Draw player
    int playerX = x + playerPos.GetPosition() * tileWidth;
    renderer.DrawRect(playerX + 10, y + 10, tileWidth - 20, tileHeight - 20, BLUE);
    
    // Draw enemy
    int enemyX = x + enemyPos.GetPosition() * tileWidth;
    renderer.DrawRect(enemyX + 10, y + 10, tileWidth - 20, tileHeight - 20, RED);
    
    // In range test mode, show the range
    if (currentMode == ActionMode::RANGE_TEST) {
        // Draw connection between entities (a thin rectangle)
        int minX = std::min(playerX, enemyX) + tileWidth / 2;
        int maxX = std::max(playerX, enemyX) + tileWidth / 2;
        int lineWidth = maxX - minX;
        
        renderer.DrawRect(
            minX, 
            y + tileHeight / 2 - 2,
            lineWidth,
            4,
            MAROON
        );
        
        // Indicate if in range
        bool inRange = playerPos.IsWithinRange(enemyPos, rangeToTest);
        std::string rangeText = "Range: " + std::to_string(rangeToTest) + 
                              " - " + (inRange ? "IN RANGE" : "OUT OF RANGE");
                              
        renderer.DrawTextCentered(
            rangeText.c_str(),
            renderer.GetScreenWidth() / 2,
            y + tileHeight + 20,
            20,
            inRange ? GREEN : RED
        );
    }
}

void PositionTestState::RenderEntityInfo(int x, int y) {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get position component references
    auto& playerPos = playerEntity->GetComponent<PositionComponent>();
    auto& enemyPos = enemyEntity->GetComponent<PositionComponent>();
    
    // Player information
    renderer.DrawText("Player:", x, y, 20, BLUE);
    renderer.DrawText(("Position: " + std::to_string(playerPos.GetPosition())).c_str(), 
                     x + 20, y + 30, 16, DARKGRAY);
    
    std::string playerEdgeInfo;
    if (playerPos.IsAtLeftEdge()) playerEdgeInfo = "At left edge";
    else if (playerPos.IsAtRightEdge()) playerEdgeInfo = "At right edge";
    else playerEdgeInfo = "Not at edge";
    
    renderer.DrawText(playerEdgeInfo.c_str(), x + 20, y + 50, 16, DARKGRAY);
    
    // Enemy information
    renderer.DrawText("Enemy:", x, y + 90, 20, RED);
    renderer.DrawText(("Position: " + std::to_string(enemyPos.GetPosition())).c_str(), 
                     x + 20, y + 120, 16, DARKGRAY);
    
    std::string enemyEdgeInfo;
    if (enemyPos.IsAtLeftEdge()) enemyEdgeInfo = "At left edge";
    else if (enemyPos.IsAtRightEdge()) enemyEdgeInfo = "At right edge";
    else enemyEdgeInfo = "Not at edge";
    
    renderer.DrawText(enemyEdgeInfo.c_str(), x + 20, y + 140, 16, DARKGRAY);
    
    // Relative positioning
    renderer.DrawText("Relative:", x, y + 180, 20, PURPLE);
    
    int distance = playerPos.GetDistanceTo(enemyPos);
    renderer.DrawText(("Distance: " + std::to_string(distance) + " tiles").c_str(), 
                     x + 20, y + 210, 16, DARKGRAY);
    
    int direction = playerPos.GetDirectionTo(enemyPos);
    std::string directionText;
    
    if (direction > 0) directionText = "Enemy is ahead";
    else if (direction < 0) directionText = "Enemy is behind";
    else directionText = "Same position";
    
    renderer.DrawText(directionText.c_str(), x + 20, y + 230, 16, DARKGRAY);
}

void PositionTestState::RenderControls(int y) {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    renderer.DrawText("Controls:", 50, y, 18, DARKBLUE);
    y += 25;
    
    if (currentMode == ActionMode::MOVE) {
        renderer.DrawText("LEFT/RIGHT: Move player", 50, y, 16, DARKGRAY);
        y += 20;
        
        renderer.DrawText("UP/DOWN: Move enemy", 50, y, 16, DARKGRAY);
    } else {
        renderer.DrawText("UP/DOWN: Change range to test", 50, y, 16, DARKGRAY);
    }
    
    y += 20;
    renderer.DrawText("X: Toggle mode (Movement/Range Test)", 50, y, 16, DARKGRAY);
    y += 20;
    
    renderer.DrawText("ESC: Exit test", 50, y, 16, DARKGRAY);
}

} // namespace Game 