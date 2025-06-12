#pragma once

#include "Encounter.h"
#include "../../combat/CombatSystem.h"
#include "../../entities/Entity.h"
#include <vector>

namespace Game {

/**
 * Combat encounter against enemies
 */
class CombatEncounter : public Encounter {
public:
    CombatEncounter(const std::string& name, int difficulty);
    ~CombatEncounter() override = default;
    
    // Difficulty level (affects enemy count/strength)
    int GetDifficulty() const { return difficulty; }
    
    // Encounter interface implementation
    void Start() override;
    void Update(float deltaTime) override;
    bool IsActive() const override;
    void Complete(EncounterResult result) override;
    
    // Combat management
    void SetPlayerTeam(const std::vector<std::shared_ptr<Entity>>& team);
    void AddEnemy(std::shared_ptr<Entity> enemy);
    void GenerateEnemies(int count);
    
    // Getters
    const std::vector<std::shared_ptr<Entity>>& GetEnemies() const;
    
private:
    int difficulty;
    CombatSystem combatSystem;
    std::vector<std::shared_ptr<Entity>> playerTeam;
    std::vector<std::shared_ptr<Entity>> enemyTeam;
    bool isActive;
    float timeElapsed;
    
    // Enemy generation helper
    std::shared_ptr<Entity> CreateRandomEnemy(int level);
};

} // namespace Game 