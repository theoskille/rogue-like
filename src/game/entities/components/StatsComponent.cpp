#include "StatsComponent.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace Game {

StatsComponent::StatsComponent() : maxHealth(0), currentHealth(0) {
    // Initialize all stats to 0
    baseStats[StatType::STRENGTH] = 0;
    baseStats[StatType::INTELLECT] = 0;
    baseStats[StatType::SPEED] = 0;
    baseStats[StatType::DEXTERITY] = 0;
    baseStats[StatType::CONSTITUTION] = 0;
    baseStats[StatType::DEFENSE] = 0;
    baseStats[StatType::LUCK] = 0;
}

void StatsComponent::Initialize(int str, int intel, int spd, int dex, int con, int def, int lck) {
    baseStats[StatType::STRENGTH] = str;
    baseStats[StatType::INTELLECT] = intel;
    baseStats[StatType::SPEED] = spd;
    baseStats[StatType::DEXTERITY] = dex;
    baseStats[StatType::CONSTITUTION] = con;
    baseStats[StatType::DEFENSE] = def;
    baseStats[StatType::LUCK] = lck;
    
    // Calculate derived stats
    RecalculateDerivedStats();
    
    // Start with full health
    currentHealth = maxHealth;
}

void StatsComponent::Start() {
    // Called when the component is activated
    RecalculateDerivedStats();
}

int StatsComponent::GetBaseStat(StatType type) const {
    auto it = baseStats.find(type);
    if (it != baseStats.end()) {
        return it->second;
    }
    return 0;
}

void StatsComponent::SetBaseStat(StatType type, int value) {
    baseStats[type] = value;
    RecalculateDerivedStats();
}

int StatsComponent::GetCurrentStat(StatType type) const {
    int base = GetBaseStat(type);
    
    // Add up all modifiers
    int totalModifier = 0;
    auto it = modifiers.find(type);
    if (it != modifiers.end()) {
        for (const auto& mod : it->second) {
            totalModifier += mod.first;
        }
    }
    
    return base + totalModifier;
}

void StatsComponent::AddModifier(StatType type, int value, int duration) {
    modifiers[type].push_back(std::make_pair(value, duration));
    RecalculateDerivedStats();
}

void StatsComponent::ClearModifiers() {
    modifiers.clear();
    RecalculateDerivedStats();
}

void StatsComponent::UpdateModifiers() {
    bool changed = false;
    
    // Update each stat's modifiers
    for (auto& [type, mods] : modifiers) {
        // Process in reverse order so we can safely erase
        for (int i = static_cast<int>(mods.size()) - 1; i >= 0; i--) {
            // Skip permanent modifiers (duration < 0)
            if (mods[i].second < 0) {
                continue;
            }
            
            // Decrease duration
            mods[i].second--;
            
            // Remove expired modifiers
            if (mods[i].second <= 0) {
                mods.erase(mods.begin() + i);
                changed = true;
            }
        }
    }
    
    // Recalculate stats if modifiers changed
    if (changed) {
        RecalculateDerivedStats();
    }
}

int StatsComponent::CalculateMaxHealth() const {
    // Base health formula: 10 + (CON * 5)
    return 10 + (GetCurrentStat(StatType::CONSTITUTION) * 5);
}

int StatsComponent::CalculateDamage(int baseDamage) const {
    // Damage formula: baseDamage + (STR * 0.5) + (DEX * 0.3)
    return static_cast<int>(baseDamage + 
                           (GetCurrentStat(StatType::STRENGTH) * 0.5f) + 
                           (GetCurrentStat(StatType::DEXTERITY) * 0.3f));
}

int StatsComponent::CalculateDodgeChance() const {
    // Dodge chance formula: DEX * 2 (capped at 40%)
    return std::min(40, GetCurrentStat(StatType::DEXTERITY) * 2);
}

int StatsComponent::CalculateBlockChance() const {
    // Block chance formula: DEF * 3 (capped at 50%)
    return std::min(50, GetCurrentStat(StatType::DEFENSE) * 3);
}

int StatsComponent::CalculateCriticalChance() const {
    // Critical hit chance formula: LCK * 2 (capped at 30%)
    return std::min(30, GetCurrentStat(StatType::LUCK) * 2);
}

bool StatsComponent::TakeDamage(int damage) {
    // Calculate actual damage after potential block
    int blockChance = CalculateBlockChance();
    
    // Check for block (completely negates damage)
    if (rand() % 100 < blockChance) {
        std::cout << "Attack blocked!" << std::endl;
        return false;  // Not dead
    }
    
    // Apply damage
    currentHealth = std::max(0, currentHealth - damage);
    
    // Return true if entity is dead
    return IsDead();
}

void StatsComponent::RecalculateDerivedStats() {
    int oldMaxHealth = maxHealth;
    maxHealth = CalculateMaxHealth();
    
    // Adjust current health proportionally if max health changed
    if (oldMaxHealth > 0 && maxHealth != oldMaxHealth) {
        float ratio = static_cast<float>(currentHealth) / oldMaxHealth;
        currentHealth = static_cast<int>(maxHealth * ratio);
    }
    
    // Ensure current health doesn't exceed max health
    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
}

std::string StatsComponent::GetStatName(StatType type) {
    switch (type) {
        case StatType::STRENGTH:     return "Strength";
        case StatType::INTELLECT:    return "Intellect";
        case StatType::SPEED:        return "Speed";
        case StatType::DEXTERITY:    return "Dexterity";
        case StatType::CONSTITUTION: return "Constitution";
        case StatType::DEFENSE:      return "Defense";
        case StatType::LUCK:         return "Luck";
        default:                     return "Unknown";
    }
}

} // namespace Game 