#include "TurnManagerTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include "../entities/components/StatsComponent.h"
#include "../entities/components/PositionComponent.h"
#include <iostream>

namespace Game {

TurnManagerTestState::TurnManagerTestState()
    : currentState(TestState::WAITING_FOR_INPUT),
      gameOver(false) {
    std::cout << "TurnManagerTestState created" << std::endl;
}

TurnManagerTestState::~TurnManagerTestState() {
    std::cout << "TurnManagerTestState destroyed" << std::endl;
}

void TurnManagerTestState::Enter() {
    std::cout << "Entering TurnManagerTestState" << std::endl;
    
    // Create test entities and initialize turn manager
    CreateEntities();
    
    // Initialize the turn manager with our entities
    turnManager.Initialize(entities);
    
    currentState = TestState::WAITING_FOR_INPUT;
    gameOver = false;
    statusMessage = "Press SPACE to advance turns, ESC to exit";
}

void TurnManagerTestState::Exit() {
    std::cout << "Exiting TurnManagerTestState" << std::endl;
    
    // Clear entities
    entities.clear();
    player = nullptr;
    fastEnemy = nullptr;
    strongEnemy = nullptr;
    tankEnemy = nullptr;
    
    // Reset battlefield and turn manager
    battlefield.Clear();
    turnManager.Reset();
}

void TurnManagerTestState::Update(float deltaTime) {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Check for combat over condition
    if (turnManager.IsCombatOver(&battlefield) && !gameOver) {
        gameOver = true;
        currentState = TestState::COMBAT_OVER;
        
        // Determine winner
        auto playerSideEntities = battlefield.GetPlayerSideEntities();
        bool playerSideDefeated = true;
        
        for (const auto& entity : playerSideEntities) {
            if (entity && entity->HasComponent<StatsComponent>() && 
                !entity->GetComponent<StatsComponent>().IsDead()) {
                playerSideDefeated = false;
                break;
            }
        }
        
        if (playerSideDefeated) {
            gameOverMessage = "Game Over - Enemies Win!";
        } else {
            gameOverMessage = "Victory! Player Wins!";
        }
    }
    
    // Handle input based on current state
    switch (currentState) {
        case TestState::WAITING_FOR_INPUT:
            if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
                // Advance turn
                auto currentEntity = turnManager.GetCurrentEntity();
                
                if (currentEntity) {
                    statusMessage = currentEntity->GetName() + "'s turn completed";
                    turnManager.EndTurn();
                    currentState = TestState::EXECUTING_TURN;
                }
            }
            break;
            
        case TestState::EXECUTING_TURN:
            // Automatic transition back to waiting for input
            currentState = TestState::WAITING_FOR_INPUT;
            break;
            
        case TestState::COMBAT_OVER:
            if (input.IsActionJustPressed(Engine::InputAction::CONFIRM) || 
                input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
                Engine::StateManager::GetInstance().PopState();
            }
            break;
    }
    
    // Exit on ESC
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void TurnManagerTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("TURN MANAGER TEST", 
                              renderer.GetScreenWidth() / 2, 
                              30, 
                              30, 
                              BLACK);
    
    // Draw battlefield
    int tileSize = 60;
    int tileSpacing = 10;
    int totalWidth = Battlefield::MAX_TILES * (tileSize + tileSpacing) - tileSpacing;
    int startX = (renderer.GetScreenWidth() - totalWidth) / 2;
    int startY = renderer.GetScreenHeight() / 2 - tileSize / 2;
    
    // Draw battlefield divider
    int dividerX = startX + 4 * (tileSize + tileSpacing) - tileSpacing / 2;
    renderer.DrawRect(dividerX - 2, startY - 20, 4, tileSize + 40, DARKGRAY);
    
    // Draw battlefield tiles
    for (int i = 0; i < Battlefield::MAX_TILES; i++) {
        int x = startX + i * (tileSize + tileSpacing);
        
        // Determine color based on side (player/enemy)
        Color tileColor = (i < 4) ? SKYBLUE : PINK;
        
        // Draw tile
        renderer.DrawRect(x, startY, tileSize, tileSize, tileColor);
        
        // Draw position number
        renderer.DrawTextCentered(std::to_string(i).c_str(), 
                                 x + tileSize / 2, 
                                 startY - 15, 
                                 16, 
                                 BLACK);
        
        // Draw entity if present
        Entity* entity = battlefield.GetEntityAtPosition(i);
        if (entity) {
            // Determine if this is player or enemy
            bool isPlayer = (entity == player.get());
            
            // Highlight current entity's turn
            bool isCurrentTurn = (entity == turnManager.GetCurrentEntity().get());
            
            // Draw entity circle
            Color entityColor = isPlayer ? BLUE : RED;
            
            // Add highlight for current turn
            if (isCurrentTurn) {
                // Draw highlight
                renderer.DrawCircle(x + tileSize / 2, startY + tileSize / 2, 
                                   tileSize / 2.5f, YELLOW);
            }
            
            renderer.DrawCircle(x + tileSize / 2, startY + tileSize / 2, 
                               tileSize / 3, entityColor);
            
            // Draw entity initial
            char label = isPlayer ? 'P' : (entity == fastEnemy.get() ? 'F' : 
                                          (entity == strongEnemy.get() ? 'S' : 'T'));
            
            renderer.DrawTextCentered(std::string(1, label).c_str(), 
                                     x + tileSize / 2, 
                                     startY + tileSize / 2, 
                                     20, 
                                     WHITE);
        }
    }
    
    // Draw entity info
    int infoWidth = 220;
    int infoHeight = 180;
    int padding = 20;
    
    if (player) {
        DrawEntityInfo(player.get(), 
                      padding, 
                      padding + 50, 
                      infoWidth, 
                      infoHeight);
    }
    
    if (fastEnemy) {
        DrawEntityInfo(fastEnemy.get(), 
                      renderer.GetScreenWidth() - infoWidth - padding, 
                      padding + 50, 
                      infoWidth, 
                      infoHeight);
    }
    
    if (strongEnemy) {
        DrawEntityInfo(strongEnemy.get(), 
                      renderer.GetScreenWidth() - infoWidth - padding, 
                      padding + 50 + infoHeight + 10, 
                      infoWidth, 
                      infoHeight);
    }
    
    if (tankEnemy) {
        DrawEntityInfo(tankEnemy.get(), 
                      renderer.GetScreenWidth() - infoWidth - padding, 
                      padding + 50 + (infoHeight + 10) * 2, 
                      infoWidth, 
                      infoHeight);
    }
    
    // Draw turn order
    DrawTurnOrder();
    
    // Draw status message
    renderer.DrawTextCentered(statusMessage.c_str(), 
                             renderer.GetScreenWidth() / 2, 
                             startY + tileSize + 50, 
                             20, 
                             DARKGRAY);
    
    // Draw controls
    DrawControls();
    
    // Draw game over screen if needed
    if (gameOver) {
        // Semi-transparent overlay
        renderer.DrawRect(0, 0, renderer.GetScreenWidth(), renderer.GetScreenHeight(), 
                         Color{0, 0, 0, 150});
        
        // Game over message
        renderer.DrawTextCentered(gameOverMessage.c_str(), 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() / 2 - 20, 
                                 40, 
                                 WHITE);
        
        // Exit message
        renderer.DrawTextCentered("Press SPACE or ESC to continue", 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() / 2 + 40, 
                                 20, 
                                 WHITE);
    }
}

void TurnManagerTestState::Pause() {
    std::cout << "TurnManagerTestState paused" << std::endl;
}

void TurnManagerTestState::Resume() {
    std::cout << "TurnManagerTestState resumed" << std::endl;
}

void TurnManagerTestState::CreateEntities() {
    // Clear any existing entities
    entities.clear();
    battlefield.Clear();
    
    // Create player (balanced stats)
    player = std::make_shared<Entity>("Player");
    auto& playerPos = player->AddComponent<PositionComponent>();
    playerPos.SetPosition(2);
    auto& playerStats = player->AddComponent<StatsComponent>();
    playerStats.Initialize(12, 10, 12, 10, 20, 8, 5);  // STR, INT, SPD, DEX, CON, DEF, LCK
    
    // Create fast enemy (high speed)
    fastEnemy = std::make_shared<Entity>("Fast Enemy");
    auto& fastEnemyPos = fastEnemy->AddComponent<PositionComponent>();
    fastEnemyPos.SetPosition(5);
    auto& fastEnemyStats = fastEnemy->AddComponent<StatsComponent>();
    fastEnemyStats.Initialize(8, 8, 18, 14, 15, 5, 8);  // High speed and dexterity
    
    // Create strong enemy (high strength)
    strongEnemy = std::make_shared<Entity>("Strong Enemy");
    auto& strongEnemyPos = strongEnemy->AddComponent<PositionComponent>();
    strongEnemyPos.SetPosition(6);
    auto& strongEnemyStats = strongEnemy->AddComponent<StatsComponent>();
    strongEnemyStats.Initialize(16, 6, 8, 8, 18, 7, 4);  // High strength and constitution
    
    // Create tank enemy (high defense)
    tankEnemy = std::make_shared<Entity>("Tank Enemy");
    auto& tankEnemyPos = tankEnemy->AddComponent<PositionComponent>();
    tankEnemyPos.SetPosition(7);
    auto& tankEnemyStats = tankEnemy->AddComponent<StatsComponent>();
    tankEnemyStats.Initialize(10, 5, 6, 6, 20, 15, 3);  // High constitution and defense
    
    // Add to battlefield
    battlefield.PlaceEntity(player, 2);
    battlefield.PlaceEntity(fastEnemy, 5);
    battlefield.PlaceEntity(strongEnemy, 6);
    battlefield.PlaceEntity(tankEnemy, 7);
    
    // Add to entities list for turn management
    entities.push_back(player);
    entities.push_back(fastEnemy);
    entities.push_back(strongEnemy);
    entities.push_back(tankEnemy);
}

void TurnManagerTestState::DrawEntityInfo(const Entity* entity, int x, int y, int width, int height) {
    if (!entity) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get entity stats
    auto& statsComponent = const_cast<Entity*>(entity)->GetComponent<StatsComponent>();
    auto& posComponent = const_cast<Entity*>(entity)->GetComponent<PositionComponent>();
    
    // Determine if this is the current entity's turn
    bool isCurrentTurn = (entity == turnManager.GetCurrentEntity().get());
    
    // Background panel
    Color bgColor;
    if (entity == player.get()) {
        bgColor = SKYBLUE;
    } else if (entity == fastEnemy.get()) {
        bgColor = PINK;
    } else if (entity == strongEnemy.get()) {
        bgColor = RED;
    } else {
        bgColor = MAROON;
    }
    
    bgColor.a = 100; // Semi-transparent
    
    // Add highlight for current turn
    if (isCurrentTurn) {
        renderer.DrawRectLines(x - 3, y - 3, width + 6, height + 6, YELLOW);
    }
    
    renderer.DrawRect(x, y, width, height, bgColor);
    renderer.DrawRectLines(x, y, width, height, BLACK);
    
    // Entity name
    renderer.DrawText(entity->GetName().c_str(), x + 10, y + 10, 20, BLACK);
    
    // Health bar
    int currentHealth = statsComponent.GetCurrentHealth();
    int maxHealth = statsComponent.GetMaxHealth();
    int healthBarWidth = width - 20;
    int healthBarHeight = 15;
    int healthBarX = x + 10;
    int healthBarY = y + 40;
    
    // Health bar background
    renderer.DrawRect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, LIGHTGRAY);
    
    // Health bar fill
    float healthPercentage = static_cast<float>(currentHealth) / maxHealth;
    int fillWidth = static_cast<int>(healthBarWidth * healthPercentage);
    renderer.DrawRect(healthBarX, healthBarY, fillWidth, healthBarHeight, GREEN);
    
    // Health text
    std::string healthText = std::to_string(currentHealth) + "/" + std::to_string(maxHealth);
    renderer.DrawTextCentered(healthText.c_str(), 
                             healthBarX + healthBarWidth / 2, 
                             healthBarY + healthBarHeight / 2, 
                             12, 
                             BLACK);
    
    // Core stats
    int statX = x + 10;
    int statY = y + 65;
    int lineHeight = 18;
    
    renderer.DrawText(("STR: " + std::to_string(statsComponent.GetCurrentStat(StatType::STRENGTH))).c_str(), 
                     statX, statY, 14, BLACK);
    statY += lineHeight;
    
    renderer.DrawText(("INT: " + std::to_string(statsComponent.GetCurrentStat(StatType::INTELLECT))).c_str(), 
                     statX, statY, 14, BLACK);
    statY += lineHeight;
    
    renderer.DrawText(("SPD: " + std::to_string(statsComponent.GetCurrentStat(StatType::SPEED))).c_str(), 
                     statX, statY, 14, BLACK);
    statY += lineHeight;
    
    renderer.DrawText(("DEX: " + std::to_string(statsComponent.GetCurrentStat(StatType::DEXTERITY))).c_str(), 
                     statX, statY, 14, BLACK);
    statY += lineHeight;
    
    // Position
    renderer.DrawText(("Pos: " + std::to_string(posComponent.GetPosition())).c_str(), 
                     statX, statY, 14, BLACK);
    
    // Speed value (used directly for initiative)
    int speed = statsComponent.GetCurrentStat(StatType::SPEED);
    
    renderer.DrawText(("Initiative: " + std::to_string(speed)).c_str(), 
                     x + width - 120, y + height - 40, 14, DARKBLUE);
                     
    // Display next turn info
    renderer.DrawText("Next Turn", x + width - 120, y + height - 20, 14, DARKBLUE);
}

void TurnManagerTestState::DrawTurnOrder() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get turn order
    auto turnOrder = turnManager.GetTurnOrder();
    
    // Panel background
    int x = 20;
    int y = renderer.GetScreenHeight() - 200;
    int width = 250;
    int height = 180;
    
    renderer.DrawRect(x, y, width, height, LIGHTGRAY);
    renderer.DrawRectLines(x, y, width, height, BLACK);
    
    // Title
    renderer.DrawText("TURN ORDER", x + 10, y + 10, 20, BLACK);
    
    // Turn list
    int turnX = x + 20;
    int turnY = y + 40;
    int lineHeight = 30;
    
    // Current entity
    auto currentEntity = turnManager.GetCurrentEntity();
    if (currentEntity) {
        std::string currentText = "Current: " + currentEntity->GetName();
        renderer.DrawText(currentText.c_str(), turnX, turnY, 16, DARKBLUE);
        turnY += lineHeight;
    }
    
    // Next entities
    renderer.DrawText("Next turns:", turnX, turnY, 16, BLACK);
    turnY += lineHeight;
    
    int count = 0;
    for (const auto& entity : turnOrder) {
        if (entity != currentEntity) {
            std::string turnText = std::to_string(count + 1) + ". " + entity->GetName();
            renderer.DrawText(turnText.c_str(), turnX + 10, turnY, 14, DARKGRAY);
            turnY += lineHeight - 10;
            count++;
            
            // Only show first 5 upcoming turns
            if (count >= 5) break;
        }
    }
}

void TurnManagerTestState::DrawControls() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int y = renderer.GetScreenHeight() - 40;
    renderer.DrawTextCentered("SPACE: Next Turn | ESC: Exit", 
                             renderer.GetScreenWidth() / 2, 
                             y, 
                             20, 
                             DARKGRAY);
}

} // namespace Game 