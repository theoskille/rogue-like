#include "CombatTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Game {

CombatTestState::CombatTestState()
    : player(nullptr),
      selectedActionIndex(0),
      selectedTargetIndex(0),
      uiState(CombatUIState::SELECT_ACTION),
      statusMessage("Select an action"),
      combatResult(CombatResult::NONE),
      isPaused(false) {
}

CombatTestState::~CombatTestState() {
    std::cout << "CombatTestState destroyed" << std::endl;
}

void CombatTestState::Enter() {
    std::cout << "Entering Combat Test State" << std::endl;
    
    // Create player and enemies
    CreatePlayer();
    CreateEnemies();
    
    // Load actions
    LoadActions();
    
    // Start combat
    StartCombat();
}

void CombatTestState::Exit() {
    std::cout << "Exiting Combat Test State" << std::endl;
    
    // Clear teams
    playerTeam.clear();
    enemyTeam.clear();
    player = nullptr;
    
    // Clear actions
    playerActions.clear();
}

void CombatTestState::Pause() {
    std::cout << "Pausing Combat Test State" << std::endl;
    isPaused = true;
}

void CombatTestState::Resume() {
    std::cout << "Resuming Combat Test State" << std::endl;
    isPaused = false;
}

void CombatTestState::CreatePlayer() {
    // Create player entity
    player = std::make_shared<Entity>("Player");
    
    // Add components
    auto& playerStats = player->AddComponent<StatsComponent>();
    playerStats.Initialize(12, 10, 12, 10, 15, 10, 8);
    
    player->AddComponent<PositionComponent>();
    player->AddComponent<StatusEffectsComponent>();
    
    // Add to player team
    playerTeam.push_back(player);
}

void CombatTestState::CreateEnemies() {
    // Create a variety of enemies
    
    // Fast enemy
    auto fastEnemy = std::make_shared<Entity>("Fast Enemy");
    auto& fastStats = fastEnemy->AddComponent<StatsComponent>();
    fastStats.Initialize(8, 6, 18, 14, 10, 6, 10);
    fastEnemy->AddComponent<PositionComponent>();
    fastEnemy->AddComponent<StatusEffectsComponent>();
    
    // Strong enemy
    auto strongEnemy = std::make_shared<Entity>("Strong Enemy");
    auto& strongStats = strongEnemy->AddComponent<StatsComponent>();
    strongStats.Initialize(16, 4, 8, 8, 12, 10, 6);
    strongEnemy->AddComponent<PositionComponent>();
    strongEnemy->AddComponent<StatusEffectsComponent>();
    
    // Tank enemy
    auto tankEnemy = std::make_shared<Entity>("Tank Enemy");
    auto& tankStats = tankEnemy->AddComponent<StatsComponent>();
    tankStats.Initialize(10, 4, 6, 6, 18, 16, 4);
    tankEnemy->AddComponent<PositionComponent>();
    tankEnemy->AddComponent<StatusEffectsComponent>();
    
    // Add to enemy team
    enemyTeam.push_back(fastEnemy);
    enemyTeam.push_back(strongEnemy);
    enemyTeam.push_back(tankEnemy);
}

void CombatTestState::LoadActions() {
    // Load actions from JSON file
    bool success = actionLoader.LoadActions("src/data/schemas/actions.json");
    
    if (!success) {
        std::cerr << "Failed to load actions from JSON" << std::endl;
        return;
    }
    
    // Clear previous actions
    playerActions.clear();
    
    // Assign specific actions to player
    playerActions.push_back(actionLoader.GetAction("slash"));
    playerActions.push_back(actionLoader.GetAction("fireball"));
    playerActions.push_back(actionLoader.GetAction("heal"));
    playerActions.push_back(actionLoader.GetAction("strength_buff"));
    playerActions.push_back(actionLoader.GetAction("advance"));
    playerActions.push_back(actionLoader.GetAction("retreat"));
    playerActions.push_back(actionLoader.GetAction("power_strike"));
    playerActions.push_back(actionLoader.GetAction("stun_slash"));
    
    std::cout << "Loaded " << playerActions.size() << " actions for player" << std::endl;
}

void CombatTestState::StartCombat() {
    // Initialize the combat system
    combatSystem.StartCombat(playerTeam, enemyTeam);
    
    // Reset UI state
    uiState = CombatUIState::SELECT_ACTION;
    selectedActionIndex = 0;
    selectedTargetIndex = 0;
    combatResult = CombatResult::NONE;
    statusMessage = "Select an action";
    
    std::cout << "Combat started with " << playerTeam.size() << " player characters and " 
              << enemyTeam.size() << " enemies" << std::endl;
}

void CombatTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Don't process input if combat is over
    if (uiState == CombatUIState::COMBAT_OVER) {
        Engine::InputHandler& input = Engine::InputHandler::GetInstance();
        
        // Press space to restart combat
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
            ResetCombat();
        }
        
        // Press escape to exit
        if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
            Engine::StateManager::GetInstance().PopState();
        }
        
        return;
    }
    
    // Handle different UI states
    switch (uiState) {
        case CombatUIState::SELECT_ACTION:
            HandleActionSelection();
            break;
            
        case CombatUIState::SELECT_TARGET:
            HandleTargetSelection();
            break;
            
        case CombatUIState::EXECUTING_ACTION:
            ExecuteSelectedAction();
            break;
            
        case CombatUIState::ENEMY_TURN:
            ProcessEnemyTurn();
            break;
            
        default:
            break;
    }
    
    // Check if combat is over
    CheckCombatResult();
    
    // Press escape to exit
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void CombatTestState::HandleActionSelection() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Navigate actions
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedActionIndex = (selectedActionIndex - 1 + playerActions.size()) % playerActions.size();
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedActionIndex = (selectedActionIndex + 1) % playerActions.size();
    }
    
    // Select action
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        if (selectedActionIndex >= 0 && selectedActionIndex < static_cast<int>(playerActions.size())) {
            auto action = playerActions[selectedActionIndex];
            
            // Check if it's a self-targeted action
            if (IsSelfTargetedAction(action.get())) {
                // No target selection needed
                uiState = CombatUIState::EXECUTING_ACTION;
                statusMessage = "Executing action...";
            } else {
                // Move to target selection
                uiState = CombatUIState::SELECT_TARGET;
                selectedTargetIndex = 0;
                statusMessage = "Select a target";
            }
        }
    }
}

void CombatTestState::HandleTargetSelection() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Get valid targets for the selected action
    auto action = playerActions[selectedActionIndex];
    auto validTargets = combatSystem.GetValidTargets(action);
    
    if (validTargets.empty()) {
        // No valid targets, go back to action selection
        uiState = CombatUIState::SELECT_ACTION;
        statusMessage = "No valid targets. Select another action.";
        return;
    }
    
    // Navigate targets
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT) || 
        input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedTargetIndex = (selectedTargetIndex - 1 + validTargets.size()) % validTargets.size();
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT) || 
        input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedTargetIndex = (selectedTargetIndex + 1) % validTargets.size();
    }
    
    // Select target
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        if (selectedTargetIndex >= 0 && selectedTargetIndex < static_cast<int>(validTargets.size())) {
            // Execute action with selected target
            uiState = CombatUIState::EXECUTING_ACTION;
            statusMessage = "Executing action...";
        }
    }
    
    // Go back to action selection
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        uiState = CombatUIState::SELECT_ACTION;
        statusMessage = "Select an action";
    }
}

void CombatTestState::ExecuteSelectedAction() {
    // Get the selected action and target
    auto action = playerActions[selectedActionIndex];
    
    // If it's a self-targeted action, target is the player
    std::shared_ptr<Entity> target;
    if (IsSelfTargetedAction(action.get())) {
        target = player;
    } else {
        // Get valid targets and select the chosen one
        auto validTargets = combatSystem.GetValidTargets(action);
        if (selectedTargetIndex >= 0 && selectedTargetIndex < static_cast<int>(validTargets.size())) {
            target = validTargets[selectedTargetIndex];
        } else {
            // Invalid target, go back to action selection
            uiState = CombatUIState::SELECT_ACTION;
            statusMessage = "Invalid target. Select another action.";
            return;
        }
    }
    
    // Execute the action
    bool success = combatSystem.ProcessTurn(action, target);
    
    if (success) {
        std::stringstream ss;
        ss << player->GetName() << " used " << action->GetName() << " on " << target->GetName();
        statusMessage = ss.str();
        
        // Check if combat is over
        auto result = combatSystem.CheckCombatResult();
        if (result != CombatResult::NONE) {
            uiState = CombatUIState::COMBAT_OVER;
            combatResult = result;
        } else {
            // Switch to enemy turn
            uiState = CombatUIState::ENEMY_TURN;
        }
    } else {
        // Action failed, go back to action selection
        uiState = CombatUIState::SELECT_ACTION;
        statusMessage = "Action failed. Select another action.";
    }
}

void CombatTestState::ProcessEnemyTurn() {
    // Process enemy turn
    bool success = combatSystem.ProcessEnemyTurn();
    
    if (success) {
        auto enemy = combatSystem.GetCurrentEntity();
        if (enemy) {
            std::stringstream ss;
            ss << enemy->GetName() << " acted";
            statusMessage = ss.str();
        }
        
        // Check if it's the player's turn again
        auto currentEntity = combatSystem.GetCurrentEntity();
        if (currentEntity && currentEntity == player) {
            uiState = CombatUIState::SELECT_ACTION;
            statusMessage = "Select an action";
        }
    } else {
        // Switch back to player's turn if all enemies have acted
        uiState = CombatUIState::SELECT_ACTION;
        statusMessage = "Select an action";
    }
}

void CombatTestState::CheckCombatResult() {
    auto result = combatSystem.CheckCombatResult();
    if (result != CombatResult::NONE) {
        uiState = CombatUIState::COMBAT_OVER;
        combatResult = result;
        
        // Set appropriate message
        if (result == CombatResult::PLAYER_VICTORY) {
            statusMessage = "Victory! The player has won the battle.";
        } else if (result == CombatResult::PLAYER_DEFEAT) {
            statusMessage = "Defeat! The player has been defeated.";
        } else if (result == CombatResult::ESCAPE) {
            statusMessage = "Escaped! The player has fled from battle.";
        }
    }
}

void CombatTestState::ResetCombat() {
    // Clear existing entities
    playerTeam.clear();
    enemyTeam.clear();
    player = nullptr;
    
    // Create new entities
    CreatePlayer();
    CreateEnemies();
    
    // Start fresh combat
    StartCombat();
}

void CombatTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawText("COMBAT SYSTEM TEST", 50, 30, 24, BLACK);
    
    // Draw status message
    renderer.DrawText(statusMessage.c_str(), 50, 60, 18, DARKGRAY);
    
    // Draw battlefield
    RenderBattlefield();
    
    // Draw UI based on current state
    switch (uiState) {
        case CombatUIState::SELECT_ACTION:
            RenderActionMenu();
            break;
            
        case CombatUIState::SELECT_TARGET:
            RenderTargetSelection();
            break;
            
        case CombatUIState::COMBAT_OVER:
            RenderCombatResult();
            break;
            
        default:
            break;
    }
    
    // Draw player stats (always visible)
    if (player) {
        RenderEntityStats(player.get(), 50, 400, true);
    }
    
    // Draw help text
    renderer.DrawText("ESC: Exit", 50, 700, 16, DARKGRAY);
    
    if (uiState == CombatUIState::COMBAT_OVER) {
        renderer.DrawText("SPACE: Restart Combat", 50, 720, 16, DARKGRAY);
    }
}

void CombatTestState::RenderBattlefield() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw battlefield background
    int battlefieldX = 200;
    int battlefieldY = 150;
    int tileWidth = 75;
    int tileHeight = 100;
    
    // Draw battlefield tiles
    for (int i = 0; i < 8; i++) {
        // Alternate tile colors for visibility
        Engine::RColor tileColor = (i % 2 == 0) ? LIGHTGRAY : WHITE;
        
        // Draw tile
        renderer.DrawRect(battlefieldX + i * tileWidth, battlefieldY, 
                         tileWidth, tileHeight, tileColor);
        
        // Draw tile border
        renderer.DrawRectLines(battlefieldX + i * tileWidth, battlefieldY, 
                              tileWidth, tileHeight, DARKGRAY);
        
        // Draw position number
        renderer.DrawText(std::to_string(i).c_str(), 
                         battlefieldX + i * tileWidth + tileWidth / 2 - 5, 
                         battlefieldY + tileHeight - 20, 
                         16, 
                         DARKGRAY);
    }
    
    // Draw entities on battlefield
    for (const auto& entity : playerTeam) {
        if (entity && entity->HasComponent<PositionComponent>()) {
            int pos = entity->GetComponent<PositionComponent>().GetPosition();
            int x = battlefieldX + pos * tileWidth;
            
            // Draw entity
            renderer.DrawRect(x + 10, battlefieldY + 10, 
                             tileWidth - 20, tileHeight / 2 - 10, BLUE);
            
            // Draw name
            renderer.DrawText(entity->GetName().c_str(), 
                             x + 10, battlefieldY + tileHeight / 2 + 5, 
                             12, BLACK);
            
            // Draw health
            if (entity->HasComponent<StatsComponent>()) {
                auto& stats = entity->GetComponent<StatsComponent>();
                int hp = stats.GetCurrentHealth();
                int maxHp = stats.GetMaxHealth();
                
                // Health bar
                int barWidth = tileWidth - 20;
                int barHeight = 8;
                float healthPercent = static_cast<float>(hp) / maxHp;
                
                renderer.DrawRect(x + 10, battlefieldY + tileHeight / 2 + 25, 
                                 barWidth, barHeight, LIGHTGRAY);
                renderer.DrawRect(x + 10, battlefieldY + tileHeight / 2 + 25, 
                                 static_cast<int>(barWidth * healthPercent), barHeight, 
                                 GREEN);
                
                // Health text
                std::stringstream ss;
                ss << hp << "/" << maxHp;
                renderer.DrawText(ss.str().c_str(), 
                                 x + 10, battlefieldY + tileHeight / 2 + 35, 
                                 10, BLACK);
            }
        }
    }
    
    for (const auto& entity : enemyTeam) {
        if (entity && entity->HasComponent<PositionComponent>()) {
            int pos = entity->GetComponent<PositionComponent>().GetPosition();
            int x = battlefieldX + pos * tileWidth;
            
            // Draw entity
            renderer.DrawRect(x + 10, battlefieldY + 10, 
                             tileWidth - 20, tileHeight / 2 - 10, RED);
            
            // Draw name
            renderer.DrawText(entity->GetName().c_str(), 
                             x + 10, battlefieldY + tileHeight / 2 + 5, 
                             12, BLACK);
            
            // Draw health
            if (entity->HasComponent<StatsComponent>()) {
                auto& stats = entity->GetComponent<StatsComponent>();
                int hp = stats.GetCurrentHealth();
                int maxHp = stats.GetMaxHealth();
                
                // Health bar
                int barWidth = tileWidth - 20;
                int barHeight = 8;
                float healthPercent = static_cast<float>(hp) / maxHp;
                
                renderer.DrawRect(x + 10, battlefieldY + tileHeight / 2 + 25, 
                                 barWidth, barHeight, LIGHTGRAY);
                renderer.DrawRect(x + 10, battlefieldY + tileHeight / 2 + 25, 
                                 static_cast<int>(barWidth * healthPercent), barHeight, 
                                 RED);
                
                // Health text
                std::stringstream ss;
                ss << hp << "/" << maxHp;
                renderer.DrawText(ss.str().c_str(), 
                                 x + 10, battlefieldY + tileHeight / 2 + 35, 
                                 10, BLACK);
            }
        }
    }
    
    // Highlight current entity
    auto currentEntity = combatSystem.GetCurrentEntity();
    if (currentEntity && currentEntity->HasComponent<PositionComponent>()) {
        int pos = currentEntity->GetComponent<PositionComponent>().GetPosition();
        int x = battlefieldX + pos * tileWidth;
        
        // Draw highlight
        renderer.DrawRectLines(x + 5, battlefieldY + 5, 
                              tileWidth - 10, tileHeight - 10, YELLOW);
    }
}

void CombatTestState::RenderActionMenu() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw action menu
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 300;
    int menuHeight = 250;
    
    // Draw menu background
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw title
    renderer.DrawText("Actions", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw actions
    int y = menuY + 40;
    for (size_t i = 0; i < playerActions.size(); i++) {
        auto action = playerActions[i];
        
        // Highlight selected action
        Engine::RColor color = (i == static_cast<size_t>(selectedActionIndex)) ? RED : BLACK;
        
        // Draw action name
        renderer.DrawText(action->GetName().c_str(), menuX + 20, y, 16, color);
        
        // Draw action type
        std::string typeStr;
        switch (action->GetType()) {
            case ActionType::ATTACK: typeStr = "Attack"; break;
            case ActionType::BUFF: typeStr = "Buff"; break;
            case ActionType::DEBUFF: typeStr = "Debuff"; break;
            case ActionType::HEAL: typeStr = "Heal"; break;
            case ActionType::MOVEMENT: typeStr = "Move"; break;
            case ActionType::SPECIAL: typeStr = "Special"; break;
            case ActionType::COMPOUND: typeStr = "Combo"; break;
            default: typeStr = "Unknown"; break;
        }
        
        renderer.DrawText(typeStr.c_str(), menuX + 150, y, 14, color);
        
        // Draw action info
        std::stringstream ss;
        
        if (action->GetType() == ActionType::ATTACK) {
            ss << "DMG: " << action->GetDamage();
        } else if (action->GetType() == ActionType::HEAL) {
            ss << "HEAL: " << action->GetProperty("heal_amount");
        } else if (action->GetType() == ActionType::MOVEMENT) {
            int moveAmount = action->GetProperty("position_change");
            ss << (moveAmount > 0 ? "FWD: " : "BWD: ") << std::abs(moveAmount);
        }
        
        renderer.DrawText(ss.str().c_str(), menuX + 220, y, 14, color);
        
        y += 25;
    }
}

void CombatTestState::RenderTargetSelection() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get the selected action
    auto action = playerActions[selectedActionIndex];
    
    // Get valid targets
    auto validTargets = combatSystem.GetValidTargets(action);
    
    if (validTargets.empty()) {
        return;
    }
    
    // Draw target selection
    int menuX = 400;
    int menuY = 120;
    int menuWidth = 300;
    int menuHeight = 200;
    
    // Draw menu background
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw title
    renderer.DrawText("Select Target", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw targets
    int y = menuY + 40;
    for (size_t i = 0; i < validTargets.size(); i++) {
        auto target = validTargets[i];
        
        // Highlight selected target
        Engine::RColor color = (i == static_cast<size_t>(selectedTargetIndex)) ? RED : BLACK;
        
        // Draw target name
        renderer.DrawText(target->GetName().c_str(), menuX + 20, y, 16, color);
        
        // Draw target health if it has stats
        if (target->HasComponent<StatsComponent>()) {
            auto& stats = target->GetComponent<StatsComponent>();
            int hp = stats.GetCurrentHealth();
            int maxHp = stats.GetMaxHealth();
            
            std::stringstream ss;
            ss << "HP: " << hp << "/" << maxHp;
            
            renderer.DrawText(ss.str().c_str(), menuX + 150, y, 14, color);
        }
        
        // Draw target position if it has position
        if (target->HasComponent<PositionComponent>()) {
            auto& pos = target->GetComponent<PositionComponent>();
            
            std::stringstream ss;
            ss << "Pos: " << pos.GetPosition();
            
            renderer.DrawText(ss.str().c_str(), menuX + 220, y, 14, color);
        }
        
        y += 25;
    }
}

void CombatTestState::RenderEntityStats(const Entity* entity, int x, int y, bool detailed) {
    if (!entity) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw entity name
    renderer.DrawText(entity->GetName().c_str(), x, y, 18, BLACK);
    y += 25;
    
    // Draw health if entity has stats
    if (entity->HasComponent<StatsComponent>()) {
        // Cast away const for the component access
        // This is safe because we're only reading the stats
        Entity* nonConstEntity = const_cast<Entity*>(entity);
        auto& stats = nonConstEntity->GetComponent<StatsComponent>();
        
        std::stringstream healthSs;
        healthSs << "HP: " << stats.GetCurrentHealth() << "/" << stats.GetMaxHealth();
        renderer.DrawText(healthSs.str().c_str(), x, y, 16, BLACK);
        y += 20;
        
        if (detailed) {
            // Draw stats
            for (int i = 0; i < 7; i++) {
                StatType type = static_cast<StatType>(i);
                std::string name = StatsComponent::GetStatName(type);
                int baseValue = stats.GetBaseStat(type);
                int currentValue = stats.GetCurrentStat(type);
                
                // Format name for display
                std::string shortName = name.substr(0, 3);
                std::transform(shortName.begin(), shortName.end(), shortName.begin(), ::toupper);
                
                // Draw stat name and value
                std::stringstream ss;
                ss << shortName << ": ";
                renderer.DrawText(ss.str().c_str(), x, y, 14, DARKGRAY);
                
                // Show both base and current value if they differ
                if (baseValue != currentValue) {
                    ss.str("");
                    ss << baseValue << " (" << currentValue << ")";
                    renderer.DrawText(ss.str().c_str(), x + 50, y, 14, 
                                    (currentValue > baseValue) ? GREEN : RED);
                } else {
                    ss.str("");
                    ss << baseValue;
                    renderer.DrawText(ss.str().c_str(), x + 50, y, 14, BLACK);
                }
                
                y += 18;
            }
        }
    }
    
    // Draw status effects
    if (detailed && entity->HasComponent<StatusEffectsComponent>()) {
        y += 10;
        RenderStatusEffects(entity, x, y);
    }
}

void CombatTestState::RenderStatusEffects(const Entity* entity, int x, int y) {
    if (!entity || !entity->HasComponent<StatusEffectsComponent>()) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    auto& statusEffects = const_cast<Entity*>(entity)->GetComponent<StatusEffectsComponent>();
    const auto& effects = statusEffects.GetEffects();
    
    if (effects.empty()) {
        return;
    }
    
    renderer.DrawText("Status Effects:", x, y, 14, DARKGRAY);
    y += 20;
    
    for (const auto& effect : effects) {
        // Determine color based on effect type
        Engine::RColor color;
        switch (effect->GetType()) {
            case StatusEffectType::BUFF: color = GREEN; break;
            case StatusEffectType::DEBUFF: color = RED; break;
            case StatusEffectType::POISON: color = PURPLE; break;
            case StatusEffectType::STUN: color = ORANGE; break;
            default: color = DARKGRAY; break;
        }
        
        // Draw effect name and duration
        std::stringstream ss;
        ss << effect->GetName() << " (" << effect->GetDuration() << ")";
        renderer.DrawText(ss.str().c_str(), x, y, 12, color);
        y += 16;
    }
}

void CombatTestState::RenderCombatResult() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw large message in center of screen
    int screenWidth = renderer.GetScreenWidth();
    int screenHeight = renderer.GetScreenHeight();
    
    std::string resultText;
    Engine::RColor resultColor;
    
    switch (combatResult) {
        case CombatResult::PLAYER_VICTORY:
            resultText = "VICTORY!";
            resultColor = GREEN;
            break;
            
        case CombatResult::PLAYER_DEFEAT:
            resultText = "DEFEAT!";
            resultColor = RED;
            break;
            
        case CombatResult::ESCAPE:
            resultText = "ESCAPED!";
            resultColor = BLUE;
            break;
            
        default:
            resultText = "COMBAT OVER";
            resultColor = BLACK;
            break;
    }
    
    // Draw transparent overlay
    renderer.DrawRect(0, 0, screenWidth, screenHeight, 
                     {200, 200, 200, 150});
    
    // Draw result text
    int textWidth = resultText.length() * 30;
    renderer.DrawText(resultText.c_str(), 
                     (screenWidth - textWidth) / 2, 
                     screenHeight / 2 - 30, 
                     60, resultColor);
    
    // Draw instruction
    renderer.DrawText("Press SPACE to restart or ESC to exit", 
                     screenWidth / 2 - 150, 
                     screenHeight / 2 + 50, 
                     20, BLACK);
}

bool CombatTestState::IsSelfTargetedAction(const Action* action) const {
    if (!action) return false;
    
    // Self-targeted actions are typically heals or buffs
    return (action->GetType() == ActionType::HEAL || 
            action->GetType() == ActionType::BUFF || 
            action->GetProperty("self_only") > 0);
}

} // namespace Game 