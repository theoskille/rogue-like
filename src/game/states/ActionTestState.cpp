#include "ActionTestState.h"
#include "../../engine/input/InputHandler.h"
#include "../../data/ActionDataLoader.h"
#include "../entities/components/PositionComponent.h"
#include "../entities/components/StatsComponent.h"
#include <iostream>
#include <algorithm>

namespace Game {

ActionTestState::ActionTestState()
    : selectedActionIndex(0), 
      currentState(UIState::SELECT_ACTION),
      playerTurn(true),
      gameOver(false) {
    std::cout << "ActionTestState created" << std::endl;
}

ActionTestState::~ActionTestState() {
    std::cout << "ActionTestState destroyed" << std::endl;
}

void ActionTestState::Enter() {
    std::cout << "Entering Action Test State" << std::endl;
    
    // Load actions from JSON
    LoadActions();
    
    // Create entities and set up battlefield
    CreateEntities();
    
    // Reset state
    currentState = UIState::SELECT_ACTION;
    playerTurn = true;
    gameOver = false;
    gameOverMessage = "";
    selectedActionIndex = 0;
}

void ActionTestState::Exit() {
    std::cout << "Exiting Action Test State" << std::endl;
    
    // Clear entities and battlefield
    player.reset();
    enemy.reset();
    battlefield.Clear();
    
    // Clear actions
    playerActions.clear();
    enemyActions.clear();
}

void ActionTestState::Update(float deltaTime) {
    // Check for game over condition
    if (player->GetComponent<StatsComponent>().IsDead() || 
        enemy->GetComponent<StatsComponent>().IsDead()) {
        
        if (!gameOver) {
            gameOver = true;
            currentState = UIState::GAME_OVER;
            
            if (player->GetComponent<StatsComponent>().IsDead()) {
                gameOverMessage = "You were defeated!";
            } else {
                gameOverMessage = "Victory! Enemy defeated!";
            }
        }
        
        // Handle input in game over state
        Engine::InputHandler& input = Engine::InputHandler::GetInstance();
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM) || 
            input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
            Engine::StateManager::GetInstance().PopState();
        }
        
        return;
    }
    
    // Handle input based on current state
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    if (currentState == UIState::SELECT_ACTION && playerTurn) {
        // Navigate action list with up/down
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
            selectedActionIndex = (selectedActionIndex - 1 + playerActions.size()) % playerActions.size();
        }
        else if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
            selectedActionIndex = (selectedActionIndex + 1) % playerActions.size();
        }
        
        // Select action with CONFIRM
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
            ExecuteSelectedAction();
        }
    }
    else if (currentState == UIState::WAITING && !playerTurn) {
        // AI takes its turn
        ExecuteEnemyTurn();
    }
    
    // Exit on CANCEL
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void ActionTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("ACTION SYSTEM TEST", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             30, 
                             BLACK);
    
    // Draw battle UI
    DrawBattlefield();
    
    // Draw player and enemy info
    int infoWidth = 220;
    int infoHeight = 180;
    int padding = 20;
    
    // Player info (left side)
    DrawEntityInfo(player.get(), 
                  padding, 
                  padding + 50, 
                  infoWidth, 
                  infoHeight);
    
    // Enemy info (right side)
    DrawEntityInfo(enemy.get(), 
                  renderer.GetScreenWidth() - infoWidth - padding, 
                  padding + 50, 
                  infoWidth, 
                  infoHeight);
    
    // Draw action list (bottom left)
    DrawActionList();
    
    // Draw game state (bottom center)
    DrawGameState();
    
    // Draw controls
    int controlsY = renderer.GetScreenHeight() - 100;
    renderer.DrawTextCentered("Controls: UP/DOWN to select action, SPACE to execute, ESC to exit", 
                             renderer.GetScreenWidth() / 2, 
                             controlsY, 
                             16, 
                             DARKGRAY);
    
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

void ActionTestState::Pause() {
    std::cout << "Pausing Action Test State" << std::endl;
}

void ActionTestState::Resume() {
    std::cout << "Resuming Action Test State" << std::endl;
}

void ActionTestState::CreateEntities() {
    // Clear any existing entities
    battlefield.Clear();
    
    // Create player entity
    player = std::make_shared<Entity>("Player");
    
    // Add position component at position 3 (closer to enemy for testing)
    auto& playerPos = player->AddComponent<PositionComponent>();
    playerPos.SetPosition(3);
    
    // Add stats component
    auto& playerStats = player->AddComponent<StatsComponent>();
    playerStats.Initialize(15, 10, 12, 10, 20, 8, 5);
    
    // Add to battlefield
    battlefield.PlaceEntity(player, 3);
    
    // Create enemy entity
    enemy = std::make_shared<Entity>("Enemy");
    
    // Add position component at position 5
    auto& enemyPos = enemy->AddComponent<PositionComponent>();
    enemyPos.SetPosition(5);
    
    // Add stats component
    auto& enemyStats = enemy->AddComponent<StatsComponent>();
    enemyStats.Initialize(12, 8, 10, 8, 15, 5, 3);
    
    // Add to battlefield
    battlefield.PlaceEntity(enemy, 5);
}

void ActionTestState::LoadActions() {
    // Load actions from JSON file
    ActionDataLoader& loader = ActionDataLoader::GetInstance();
    bool success = loader.LoadActions("src/data/schemas/actions.json");
    
    if (!success) {
        std::cerr << "Failed to load actions from JSON" << std::endl;
        return;
    }
    
    // Get all actions
    const auto& allActions = loader.GetAllActions();
    
    // Clear previous actions
    playerActions.clear();
    enemyActions.clear();
    
    // Assign specific actions to player
    playerActions.push_back(loader.GetAction("slash"));
    playerActions.push_back(loader.GetAction("fireball"));
    playerActions.push_back(loader.GetAction("heal"));
    playerActions.push_back(loader.GetAction("strength_buff"));
    playerActions.push_back(loader.GetAction("advance"));
    playerActions.push_back(loader.GetAction("retreat"));
    
    // Add compound actions
    playerActions.push_back(loader.GetAction("charge"));
    playerActions.push_back(loader.GetAction("power_strike"));
    playerActions.push_back(loader.GetAction("life_drain"));
    
    // Assign specific actions to enemy
    enemyActions.push_back(loader.GetAction("slash"));
    enemyActions.push_back(loader.GetAction("quick_stab"));
    enemyActions.push_back(loader.GetAction("weaken"));
    enemyActions.push_back(loader.GetAction("advance"));
    enemyActions.push_back(loader.GetAction("stun_slash"));
}

void ActionTestState::ExecuteSelectedAction() {
    // Make sure there are actions to select
    if (playerActions.empty() || selectedActionIndex >= playerActions.size()) {
        return;
    }
    
    // Get the selected action
    std::shared_ptr<Action> action = playerActions[selectedActionIndex];
    
    // Determine the correct target based on action type
    Entity* target = enemy.get(); // Default target is enemy
    
    // For movement and self-targeted actions, target the player instead
    if (action->GetType() == ActionType::MOVEMENT || 
        action->GetProperty("self_only") > 0) {
        target = player.get();
    }
    
    // Check if action can be used
    if (!action->CanUse(player.get(), target, &battlefield)) {
        // Cannot use action - show message but stay in SELECT_ACTION state
        std::cout << "Cannot use action: " << action->GetName() << std::endl;
        return;
    }
    
    // Execute the action
    bool success = action->Execute(player.get(), target, &battlefield);
    
    // Update cooldowns on all actions
    for (auto& act : playerActions) {
        if (act != action && act->IsOnCooldown()) {
            act->DecreaseCooldown();
        }
    }
    
    // Switch to enemy turn
    currentState = UIState::WAITING;
    playerTurn = false;
}

void ActionTestState::ExecuteEnemyTurn() {
    // Implement simple AI for enemy
    // For now, just select a random valid action
    
    // Filter available actions (not on cooldown and can be used)
    std::vector<std::shared_ptr<Action>> validActions;
    
    for (auto& action : enemyActions) {
        // Determine the correct target based on action type
        Entity* target = player.get(); // Default target is player
        
        // For movement and self-targeted actions, target the enemy itself
        if (action->GetType() == ActionType::MOVEMENT || 
            action->GetProperty("self_only") > 0) {
            target = enemy.get();
        }
        
        if (!action->IsOnCooldown() && action->CanUse(enemy.get(), target, &battlefield)) {
            // Store the action along with its target
            validActions.push_back(action);
        }
    }
    
    if (!validActions.empty()) {
        // Select random action
        int index = std::rand() % validActions.size();
        std::shared_ptr<Action> selectedAction = validActions[index];
        
        // Determine the correct target for the selected action
        Entity* target = player.get(); // Default target is player
        
        // For movement and self-targeted actions, target the enemy itself
        if (selectedAction->GetType() == ActionType::MOVEMENT || 
            selectedAction->GetProperty("self_only") > 0) {
            target = enemy.get();
        }
        
        // Execute action
        std::cout << "Enemy uses " << selectedAction->GetName() << std::endl;
        selectedAction->Execute(enemy.get(), target, &battlefield);
        
        // Update cooldowns on all actions
        for (auto& act : enemyActions) {
            if (act != selectedAction && act->IsOnCooldown()) {
                act->DecreaseCooldown();
            }
        }
    }
    else {
        std::cout << "Enemy has no valid actions" << std::endl;
    }
    
    // Switch back to player turn
    currentState = UIState::SELECT_ACTION;
    playerTurn = true;
}

void ActionTestState::DrawBattlefield() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int tileSize = 60;
    int tileSpacing = 10;
    int totalWidth = Battlefield::MAX_TILES * (tileSize + tileSpacing) - tileSpacing;
    int startX = (renderer.GetScreenWidth() - totalWidth) / 2;
    int startY = renderer.GetScreenHeight() / 2 - tileSize / 2;
    
    // Draw battlefield divider (middle of the 8 tiles)
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
            
            // Draw entity circle
            Color entityColor = isPlayer ? BLUE : RED;
            renderer.DrawCircle(x + tileSize / 2, startY + tileSize / 2, tileSize / 3, entityColor);
            
            // Draw entity initial (P for player, E for enemy)
            std::string label = isPlayer ? "P" : "E";
            renderer.DrawTextCentered(label.c_str(), 
                                     x + tileSize / 2, 
                                     startY + tileSize / 2, 
                                     20, 
                                     WHITE);
        }
    }
    
    // Draw side labels
    renderer.DrawText("PLAYER SIDE", startX, startY + tileSize + 10, 20, DARKBLUE);
    renderer.DrawText("ENEMY SIDE", startX + 4 * (tileSize + tileSpacing), startY + tileSize + 10, 20, MAROON);
}

void ActionTestState::DrawEntityInfo(const Entity* entity, int x, int y, int width, int height) {
    if (!entity) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get entity stats
    auto& statsComponent = const_cast<Entity*>(entity)->GetComponent<StatsComponent>();
    auto& posComponent = const_cast<Entity*>(entity)->GetComponent<PositionComponent>();
    
    // Background panel
    Color bgColor = (entity == player.get()) ? SKYBLUE : PINK;
    bgColor.a = 100; // Semi-transparent
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
    
    renderer.DrawText(("Position: " + std::to_string(posComponent.GetPosition())).c_str(), 
                     statX, statY, 14, BLACK);
}

void ActionTestState::DrawActionList() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Panel background
    int x = 20;
    int y = renderer.GetScreenHeight() - 250;
    int width = 250;
    int height = 230;
    
    renderer.DrawRect(x, y, width, height, LIGHTGRAY);
    renderer.DrawRectLines(x, y, width, height, BLACK);
    
    // Title
    renderer.DrawText("ACTIONS", x + 10, y + 10, 20, BLACK);
    
    // Actions list
    int actionX = x + 20;
    int actionY = y + 40;
    int lineHeight = 30;
    
    for (size_t i = 0; i < playerActions.size(); i++) {
        auto& action = playerActions[i];
        
        // Background for selected action
        if (static_cast<int>(i) == selectedActionIndex && currentState == UIState::SELECT_ACTION && playerTurn) {
            renderer.DrawRect(x + 5, actionY - 5, width - 10, lineHeight + 5, BLUE);
        }
        
        // Action name
        Color textColor = WHITE;
        if (static_cast<int>(i) != selectedActionIndex || currentState != UIState::SELECT_ACTION || !playerTurn) {
            textColor = BLACK;
        }
        
        std::string actionText = action->GetName();
        
        // Show cooldown if on cooldown
        if (action->IsOnCooldown()) {
            actionText += " (CD: " + std::to_string(action->GetCurrentCooldown()) + ")";
            textColor = DARKGRAY; // Dimmed text for actions on cooldown
        }
        
        renderer.DrawText(actionText.c_str(), actionX, actionY, 16, textColor);
        
        actionY += lineHeight;
    }
}

void ActionTestState::DrawGameState() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Panel background
    int x = renderer.GetScreenWidth() / 2 - 100;
    int y = renderer.GetScreenHeight() - 120;
    int width = 200;
    int height = 50;
    
    renderer.DrawRect(x, y, width, height, LIGHTGRAY);
    renderer.DrawRectLines(x, y, width, height, BLACK);
    
    // Current turn
    std::string turnText = playerTurn ? "Player's Turn" : "Enemy's Turn";
    Color turnColor = playerTurn ? BLUE : RED;
    
    renderer.DrawTextCentered(turnText.c_str(), 
                             x + width / 2, 
                             y + height / 2, 
                             20, 
                             turnColor);
}

} // namespace Game 