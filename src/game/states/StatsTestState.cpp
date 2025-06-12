#include "StatsTestState.h"
#include "../../engine/input/InputHandler.h"
#include "../entities/components/StatsComponent.h"
#include <iostream>

namespace Game {

StatsTestState::StatsTestState() {
    std::cout << "StatsTestState created" << std::endl;
}

StatsTestState::~StatsTestState() {
    std::cout << "StatsTestState destroyed" << std::endl;
}

void StatsTestState::Enter() {
    std::cout << "Entering Stats Test State" << std::endl;
    isPaused = false;
    
    // Create test entity with stats
    testEntity = std::make_unique<Entity>("TestHero");
    
    // Add stats component with initial values
    auto& stats = testEntity->AddComponent<StatsComponent>();
    stats.Initialize(5, 3, 4, 4, 6, 2, 3);  // STR, INT, SPD, DEX, CON, DEF, LCK
    
    // Start the entity
    testEntity->Start();
    
    std::cout << "Test entity created with stats" << std::endl;
}

void StatsTestState::Exit() {
    std::cout << "Exiting Stats Test State" << std::endl;
    testEntity.reset();
}

void StatsTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Get stats component reference
    auto& stats = testEntity->GetComponent<StatsComponent>();
    
    // Handle stat selection
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedStat = (selectedStat > 0) ? selectedStat - 1 : 6;  // 7 stats total
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedStat = (selectedStat < 6) ? selectedStat + 1 : 0;
    }
    
    // Handle stat modification
    if (!damageTestMode) {
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
            // Increase selected stat
            StatType type = static_cast<StatType>(selectedStat);
            int currentValue = stats.GetBaseStat(type);
            stats.SetBaseStat(type, currentValue + 1);
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
            // Decrease selected stat
            StatType type = static_cast<StatType>(selectedStat);
            int currentValue = stats.GetBaseStat(type);
            if (currentValue > 1) {  // Don't allow below 1
                stats.SetBaseStat(type, currentValue - 1);
            }
        }
    } else {
        // Damage test mode
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
            testDamage++;
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
            if (testDamage > 1) testDamage--;
        }
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
            // Apply damage
            bool isDead = stats.TakeDamage(testDamage);
            if (isDead) {
                std::cout << "Entity died from damage!" << std::endl;
                // Resurrect for testing purposes
                stats.SetCurrentHealth(stats.GetMaxHealth());
            }
        }
    }
    
    // Toggle damage test mode
    if (input.IsActionJustPressed(Engine::InputAction::ATTACK)) {
        damageTestMode = !damageTestMode;
    }
    
    // Exit on cancel
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void StatsTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("STATS COMPONENT TEST", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             30, 
                             BLACK);
    
    // Draw entity name
    renderer.DrawText(("Entity: " + testEntity->GetName()).c_str(), 
                     50, 80, 20, DARKGRAY);
    
    // Draw stats
    RenderStats(50, 120);
    
    // Draw controls help
    RenderControls(renderer.GetScreenHeight() - 140);
    
    // Draw damage test mode indicator
    if (damageTestMode) {
        renderer.DrawTextCentered("DAMAGE TEST MODE", 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() - 150, 
                                 20, 
                                 RED);
        
        // Draw damage amount
        renderer.DrawText(("Damage Amount: " + std::to_string(testDamage)).c_str(), 
                         renderer.GetScreenWidth() / 2, 
                         renderer.GetScreenHeight() - 120, 
                         16, 
                         MAROON);
        
        // Draw apply instruction
        renderer.DrawText("Press SPACE to apply damage", 
                         renderer.GetScreenWidth() / 2, 
                         renderer.GetScreenHeight() - 100, 
                         16, 
                         DARKGRAY);
    }
}

void StatsTestState::Pause() {
    std::cout << "Pausing Stats Test State" << std::endl;
    isPaused = true;
}

void StatsTestState::Resume() {
    std::cout << "Resuming Stats Test State" << std::endl;
    isPaused = false;
}

void StatsTestState::RenderStats(int x, int y) {
    if (!testEntity) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    auto& stats = testEntity->GetComponent<StatsComponent>();
    
    // Get all stat values
    StatType statTypes[] = {
        StatType::STRENGTH,
        StatType::INTELLECT,
        StatType::SPEED,
        StatType::DEXTERITY,
        StatType::CONSTITUTION,
        StatType::DEFENSE,
        StatType::LUCK
    };
    
    // Draw health bar
    int currentHealth = stats.GetCurrentHealth();
    int maxHealth = stats.GetMaxHealth();
    float healthPercent = static_cast<float>(currentHealth) / maxHealth;
    
    renderer.DrawText(("Health: " + std::to_string(currentHealth) + 
                      " / " + std::to_string(maxHealth)).c_str(), 
                     x, y, 20, MAROON);
    
    // Health bar background
    renderer.DrawRect(x, y + 25, 200, 20, LIGHTGRAY);
    
    // Health bar fill
    renderer.DrawRect(x, y + 25, 
                     static_cast<int>(200 * healthPercent), 20, RED);
    
    y += 60;  // Move down for stats
    
    // Draw each stat
    for (int i = 0; i < 7; i++) {
        StatType type = statTypes[i];
        std::string name = StatsComponent::GetStatName(type);
        int baseValue = stats.GetBaseStat(type);
        int currentValue = stats.GetCurrentStat(type);
        
        // Highlight selected stat
        Engine::RColor color = (i == selectedStat) ? RED : BLACK;
        
        // Draw stat name and value
        renderer.DrawText((name + ":").c_str(), x, y, 16, color);
        
        // Show both base and current value if they differ
        if (baseValue != currentValue) {
            renderer.DrawText((std::to_string(baseValue) + " (" + 
                              std::to_string(currentValue) + ")").c_str(), 
                             x + 150, y, 16, color);
        } else {
            renderer.DrawText(std::to_string(baseValue).c_str(), 
                             x + 150, y, 16, color);
        }
        
        y += 25;
    }
    
    // Draw derived stats
    y += 20;
    renderer.DrawText("Derived Stats:", x, y, 18, DARKGRAY);
    y += 25;
    
    renderer.DrawText(("Dodge Chance: " + 
                      std::to_string(stats.CalculateDodgeChance()) + "%").c_str(), 
                     x, y, 16, DARKGRAY);
    y += 25;
    
    renderer.DrawText(("Block Chance: " + 
                      std::to_string(stats.CalculateBlockChance()) + "%").c_str(), 
                     x, y, 16, DARKGRAY);
    y += 25;
    
    renderer.DrawText(("Critical Chance: " + 
                      std::to_string(stats.CalculateCriticalChance()) + "%").c_str(), 
                     x, y, 16, DARKGRAY);
    y += 25;
    
    // Calculate damage with a base value of 10
    int damage = stats.CalculateDamage(10);
    renderer.DrawText(("Base Damage (10): " + std::to_string(damage)).c_str(), 
                     x, y, 16, DARKGRAY);
}

void StatsTestState::RenderControls(int y) {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    renderer.DrawText("Controls:", 50, y, 18, DARKBLUE);
    y += 25;
    
    renderer.DrawText("UP/DOWN: Select stat", 50, y, 16, DARKGRAY);
    y += 20;
    
    renderer.DrawText("LEFT/RIGHT: Modify value", 50, y, 16, DARKGRAY);
    y += 20;
    
    renderer.DrawText("X: Toggle damage test mode", 50, y, 16, DARKGRAY);
    y += 20;
    
    renderer.DrawText("ESC: Exit test", 50, y, 16, DARKGRAY);
}

} // namespace Game 