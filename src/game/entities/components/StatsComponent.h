#pragma once

#include "Component.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace Game {

// Enum for the different stat types
enum class StatType {
    STRENGTH,  // Physical damage
    INTELLECT, // Casting power
    SPEED,     // Turn order, multiple turn potential
    DEXTERITY, // Weapon damage + dodge chance
    CONSTITUTION, // Health points
    DEFENSE,   // Block chance (ignore damage)
    LUCK       // Critical hit chance
};

// Component that handles entity stats
class StatsComponent : public Component {
public:
    StatsComponent();
    ~StatsComponent() override = default;
    
    // Initialize with base stats
    void Initialize(int str, int intel, int spd, int dex, int con, int def, int lck);
    
    // Lifecycle methods
    void Start() override;
    
    // Get base stat value
    int GetBaseStat(StatType type) const;
    
    // Set base stat value
    void SetBaseStat(StatType type, int value);
    
    // Get current stat value (base + modifiers)
    int GetCurrentStat(StatType type) const;
    
    // Add a temporary modifier to a stat
    void AddModifier(StatType type, int value, int duration = -1);
    
    // Remove all temporary modifiers
    void ClearModifiers();
    
    // Update modifiers (reduces duration)
    void UpdateModifiers();
    
    // Calculate derived stats
    int CalculateMaxHealth() const;
    int CalculateDamage(int baseDamage) const;
    int CalculateDodgeChance() const;
    int CalculateBlockChance() const;
    int CalculateCriticalChance() const;
    
    // Current health management
    int GetCurrentHealth() const { return currentHealth; }
    int GetMaxHealth() const { return maxHealth; }
    void SetCurrentHealth(int health) { currentHealth = std::min(health, maxHealth); }
    void Heal(int amount) { currentHealth = std::min(currentHealth + amount, maxHealth); }
    bool TakeDamage(int damage);  // Returns true if entity dies
    bool IsDead() const { return currentHealth <= 0; }
    
    // Return stat name as string
    static std::string GetStatName(StatType type);
    
private:
    // Base stat values
    std::unordered_map<StatType, int> baseStats;
    
    // Temporary stat modifiers (value, duration in turns)
    std::unordered_map<StatType, std::vector<std::pair<int, int>>> modifiers;
    
    // Derived stats
    int maxHealth;
    int currentHealth;
    
    // Recalculate derived stats based on current stats
    void RecalculateDerivedStats();
};

} // namespace Game 