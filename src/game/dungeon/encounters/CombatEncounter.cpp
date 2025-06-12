#include "CombatEncounter.h"
#include "../../entities/components/StatsComponent.h"
#include "../../entities/components/PositionComponent.h"
#include "../../entities/components/StatusEffectsComponent.h"
#include <iostream>
#include <random>
#include <sstream>

namespace Game {

CombatEncounter::CombatEncounter(const std::string& name, int difficulty)
    : Encounter(EncounterType::COMBAT, name),
      difficulty(std::max(1, difficulty)),
      isActive(false),
      timeElapsed(0.0f) {
    
    // Set more specific description based on difficulty
    std::stringstream ss;
    if (difficulty <= 1) {
        ss << "A small group of weak enemies blocks your path.";
    } else if (difficulty <= 3) {
        ss << "Several enemies stand in your way. They look dangerous.";
    } else if (difficulty <= 5) {
        ss << "A large group of strong enemies prepares to attack!";
    } else {
        ss << "An extremely powerful enemy force threatens your very existence!";
    }
    description = ss.str();
}

void CombatEncounter::Start() {
    if (!completed && !isActive) {
        std::cout << "Starting combat encounter: " << name << std::endl;
        
        // Ensure we have some enemies
        if (enemyTeam.empty()) {
            // Generate enemies based on difficulty
            GenerateEnemies(1 + difficulty / 2);
        }
        
        // Start combat
        combatSystem.StartCombat(playerTeam, enemyTeam);
        isActive = true;
        timeElapsed = 0.0f;
    }
}

void CombatEncounter::Update(float deltaTime) {
    if (!completed && isActive) {
        timeElapsed += deltaTime;
        
        // Check combat result
        CombatResult result = combatSystem.CheckCombatResult();
        if (result != CombatResult::NONE) {
            // Map combat result to encounter result
            EncounterResult encounterResult;
            switch (result) {
                case CombatResult::PLAYER_VICTORY:
                    encounterResult = EncounterResult::VICTORY;
                    break;
                case CombatResult::PLAYER_DEFEAT:
                    encounterResult = EncounterResult::DEFEAT;
                    break;
                case CombatResult::ESCAPE:
                    encounterResult = EncounterResult::SKIPPED;
                    break;
                default:
                    encounterResult = EncounterResult::NONE;
                    break;
            }
            
            // Complete the encounter if we have a result
            if (encounterResult != EncounterResult::NONE) {
                Complete(encounterResult);
            }
        }
    }
}

bool CombatEncounter::IsActive() const {
    return isActive && !completed;
}

void CombatEncounter::Complete(EncounterResult result) {
    // Call base class implementation
    Encounter::Complete(result);
    
    // Clean up combat
    isActive = false;
}

void CombatEncounter::SetPlayerTeam(const std::vector<std::shared_ptr<Entity>>& team) {
    playerTeam = team;
}

void CombatEncounter::AddEnemy(std::shared_ptr<Entity> enemy) {
    if (enemy) {
        enemyTeam.push_back(enemy);
    }
}

void CombatEncounter::GenerateEnemies(int count) {
    // Clear existing enemies
    enemyTeam.clear();
    
    // Generate new enemies
    for (int i = 0; i < count; ++i) {
        // Create enemy with level based on difficulty
        auto enemy = CreateRandomEnemy(difficulty);
        enemyTeam.push_back(enemy);
    }
    
    std::cout << "Generated " << enemyTeam.size() << " enemies for encounter: " << name << std::endl;
}

const std::vector<std::shared_ptr<Entity>>& CombatEncounter::GetEnemies() const {
    return enemyTeam;
}

std::shared_ptr<Entity> CombatEncounter::CreateRandomEnemy(int level) {
    // Random engine
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // Generate random enemy type (0-2)
    std::uniform_int_distribution<> typeDist(0, 2);
    int enemyType = typeDist(gen);
    
    // Create name based on type and randomness
    std::uniform_int_distribution<> idDist(1, 1000);
    int enemyId = idDist(gen);
    
    std::stringstream nameSs;
    std::shared_ptr<Entity> enemy;
    
    switch (enemyType) {
        case 0: // Fast enemy
            nameSs << "Quick Scout #" << enemyId;
            enemy = std::make_shared<Entity>(nameSs.str());
            
            // Add stats component with focus on speed
            {
                auto& stats = enemy->AddComponent<StatsComponent>();
                int baseStats = 5 + level;
                stats.Initialize(
                    baseStats,               // STR
                    baseStats - 2,           // INT
                    baseStats + 5,           // SPD
                    baseStats + 3,           // DEX
                    baseStats - 1,           // CON
                    baseStats - 2,           // DEF
                    baseStats + 2            // LCK
                );
                
                // Health scales with level through Constitution
                int desiredHealth = 18 + level * 4;
                // Adjust CON to achieve target health (base health formula is 10 + (CON * 5))
                int neededCon = (desiredHealth - 10) / 5;
                stats.SetBaseStat(StatType::CONSTITUTION, neededCon);
                stats.SetCurrentHealth(stats.GetMaxHealth());
            }
            break;
            
        case 1: // Strong enemy
            nameSs << "Brute Warrior #" << enemyId;
            enemy = std::make_shared<Entity>(nameSs.str());
            
            // Add stats component with focus on strength
            {
                auto& stats = enemy->AddComponent<StatsComponent>();
                int baseStats = 5 + level;
                stats.Initialize(
                    baseStats + 5,           // STR
                    baseStats - 3,           // INT
                    baseStats - 1,           // SPD
                    baseStats,               // DEX
                    baseStats + 3,           // CON
                    baseStats + 1,           // DEF
                    baseStats - 2            // LCK
                );
                
                // Health scales with level through Constitution
                int desiredHealth = 20 + level * 5;
                // Adjust CON to achieve target health (base health formula is 10 + (CON * 5))
                int neededCon = (desiredHealth - 10) / 5;
                stats.SetBaseStat(StatType::CONSTITUTION, neededCon);
                stats.SetCurrentHealth(stats.GetMaxHealth());
            }
            break;
            
        case 2: // Magic enemy
            nameSs << "Dark Mage #" << enemyId;
            enemy = std::make_shared<Entity>(nameSs.str());
            
            // Add stats component with focus on intelligence
            {
                auto& stats = enemy->AddComponent<StatsComponent>();
                int baseStats = 5 + level;
                stats.Initialize(
                    baseStats - 2,           // STR
                    baseStats + 5,           // INT
                    baseStats + 1,           // SPD
                    baseStats - 1,           // DEX
                    baseStats,               // CON
                    baseStats - 2,           // DEF
                    baseStats + 3            // LCK
                );
                
                // Health scales with level through Constitution
                int desiredHealth = 18 + level * 4;
                // Adjust CON to achieve target health (base health formula is 10 + (CON * 5))
                int neededCon = (desiredHealth - 10) / 5;
                stats.SetBaseStat(StatType::CONSTITUTION, neededCon);
                stats.SetCurrentHealth(stats.GetMaxHealth());
            }
            break;
            
        default:
            // Fallback generic enemy
            nameSs << "Enemy #" << enemyId;
            enemy = std::make_shared<Entity>(nameSs.str());
            
            // Add stats component
            {
                auto& stats = enemy->AddComponent<StatsComponent>();
                int baseStats = 5 + level;
                stats.Initialize(
                    baseStats,               // STR
                    baseStats,               // INT
                    baseStats,               // SPD
                    baseStats,               // DEX
                    baseStats,               // CON
                    baseStats,               // DEF
                    baseStats                // LCK
                );
                
                // Health scales with level through Constitution
                int desiredHealth = 20 + level * 5;
                // Adjust CON to achieve target health (base health formula is 10 + (CON * 5))
                int neededCon = (desiredHealth - 10) / 5;
                stats.SetBaseStat(StatType::CONSTITUTION, neededCon);
                stats.SetCurrentHealth(stats.GetMaxHealth());
            }
            break;
    }
    
    // Add position component
    enemy->AddComponent<PositionComponent>().SetPosition(7); // Far right of battlefield
    
    // Add status effects component
    enemy->AddComponent<StatusEffectsComponent>();
    
    return enemy;
}

} // namespace Game 