#include "Action.h"
#include "Battlefield.h"
#include "../entities/components/StatsComponent.h"
#include "../entities/components/PositionComponent.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

namespace Game {

//---------- ActionEffect Implementations ----------//

// DamageEffect implementation
DamageEffect::DamageEffect(int baseDamage, bool isPhysical)
    : damage(baseDamage), isPhysical(isPhysical) {
}

bool DamageEffect::Execute(Entity* user, Entity* target, Battlefield* battlefield) {
    if (!user || !target) {
        return false;
    }
    
    // Get stats for damage calculation
    if (!user->HasComponent<StatsComponent>() || !target->HasComponent<StatsComponent>()) {
        std::cout << "Missing StatsComponent for damage calculation" << std::endl;
        return false;
    }
    
    auto& userStats = user->GetComponent<StatsComponent>();
    auto& targetStats = target->GetComponent<StatsComponent>();
    
    // Calculate final damage
    int finalDamage = damage;
    
    // Add STR bonus for physical attacks or INT bonus for magical attacks
    if (isPhysical) {
        finalDamage += userStats.GetCurrentStat(StatType::STRENGTH) / 2;
    } else {
        finalDamage += userStats.GetCurrentStat(StatType::INTELLECT) / 2;
    }
    
    // Apply critical hit chance
    int critChance = userStats.CalculateCriticalChance();
    bool isCritical = (std::rand() % 100 + 1) <= critChance;
    
    if (isCritical) {
        finalDamage *= 2;  // Double damage on critical hit
        std::cout << "Critical hit!" << std::endl;
    }
    
    // Apply damage to target
    bool killed = targetStats.TakeDamage(finalDamage);
    
    std::cout << "Attack dealt " << finalDamage << " damage to " 
              << target->GetName() << std::endl;
    
    if (killed) {
        std::cout << target->GetName() << " was defeated!" << std::endl;
    }
    
    return true;
}

bool DamageEffect::CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const {
    if (!user || !target) {
        return false;
    }
    
    // Both need stats components
    if (!user->HasComponent<StatsComponent>() || !target->HasComponent<StatsComponent>()) {
        return false;
    }
    
    // Can't attack self unless specifically allowed in the action properties
    if (user == target) {
        return false;
    }
    
    return true;
}

// HealingEffect implementation
HealingEffect::HealingEffect(int healAmount)
    : amount(healAmount) {
}

bool HealingEffect::Execute(Entity* user, Entity* target, Battlefield* battlefield) {
    if (!target || !target->HasComponent<StatsComponent>()) {
        return false;
    }
    
    auto& stats = target->GetComponent<StatsComponent>();
    stats.Heal(amount);
    
    std::cout << "Healed " << target->GetName() << " for " << amount << " HP" << std::endl;
    return true;
}

bool HealingEffect::CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const {
    if (!target || !target->HasComponent<StatsComponent>()) {
        return false;
    }
    
    // Check if target is at full health
    auto& stats = const_cast<Entity*>(target)->GetComponent<StatsComponent>();
    if (stats.GetCurrentHealth() >= stats.GetMaxHealth()) {
        std::cout << target->GetName() << " is already at full health." << std::endl;
        return false;
    }
    
    return true;
}

// MovementEffect implementation
MovementEffect::MovementEffect(int positionChange)
    : positionChange(positionChange) {
}

bool MovementEffect::Execute(Entity* user, Entity* target, Battlefield* battlefield) {
    if (!target || !battlefield || !target->HasComponent<PositionComponent>()) {
        return false;
    }
    
    auto& posComp = target->GetComponent<PositionComponent>();
    int currentPos = posComp.GetPosition();
    int newPos = currentPos + positionChange;
    
    // Try to move
    if (battlefield->CanMoveTo(target, newPos)) {
        battlefield->MoveEntity(target, newPos);
        
        // Improved output message for clarity
        std::string direction;
        if (positionChange > 0) {
            direction = "forward";
        } else if (positionChange < 0) {
            direction = "backward";
        }
        
        std::cout << target->GetName() << " moved " << direction << " to position " << newPos << std::endl;
        return true;
    } else {
        // More informative error message
        std::string reason;
        if (!battlefield->IsValidPosition(newPos)) {
            reason = "position is out of bounds";
        } else if (battlefield->IsPositionOccupied(newPos)) {
            reason = "position is already occupied";
        } else {
            reason = "movement is not possible";
        }
        
        std::cout << "Cannot move to position " << newPos << ": " << reason << std::endl;
        return false;
    }
}

bool MovementEffect::CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const {
    if (!target || !battlefield || !target->HasComponent<PositionComponent>()) {
        return false;
    }
    
    // Calculate new position
    int currentPos = const_cast<Entity*>(target)->GetComponent<PositionComponent>().GetPosition();
    int newPos = currentPos + positionChange;
    
    // Check if the move is valid
    return battlefield->IsValidPosition(newPos) && 
           battlefield->CanMoveTo(const_cast<Entity*>(target), newPos);
}

// StatModifierEffect implementation
StatModifierEffect::StatModifierEffect(StatType statType, int value, int duration)
    : statType(statType), value(value), duration(duration) {
}

bool StatModifierEffect::Execute(Entity* user, Entity* target, Battlefield* battlefield) {
    if (!target || !target->HasComponent<StatsComponent>()) {
        return false;
    }
    
    auto& stats = target->GetComponent<StatsComponent>();
    stats.AddModifier(statType, value, duration);
    
    std::string effectType = (value > 0) ? "buffed" : "debuffed";
    std::string statName;
    
    switch (statType) {
        case StatType::STRENGTH: statName = "STRENGTH"; break;
        case StatType::INTELLECT: statName = "INTELLECT"; break;
        case StatType::SPEED: statName = "SPEED"; break;
        case StatType::DEXTERITY: statName = "DEXTERITY"; break;
        case StatType::CONSTITUTION: statName = "CONSTITUTION"; break;
        case StatType::DEFENSE: statName = "DEFENSE"; break;
        case StatType::LUCK: statName = "LUCK"; break;
    }
    
    std::cout << target->GetName() << "'s " << statName << " was " 
              << effectType << " by " << std::abs(value) << " for " 
              << duration << " turns" << std::endl;
    
    return true;
}

bool StatModifierEffect::CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const {
    return target && target->HasComponent<StatsComponent>();
}

//---------- Action Implementation ----------//

Action::Action(const std::string& id, const std::string& name, ActionType type)
    : id(id), name(name), type(type) {
    // Initialize random seed for accuracy checks
    static bool seedInitialized = false;
    if (!seedInitialized) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seedInitialized = true;
    }
}

Action::~Action() {
    // Clear effects
    effects.clear();
}

bool Action::Execute(Entity* user, Entity* target, Battlefield* battlefield) {
    // Check if action can be used
    if (!CanUse(user, target, battlefield)) {
        std::cout << "Action " << name << " cannot be used in this situation." << std::endl;
        
        // For movement actions, provide more detail
        if (type == ActionType::MOVEMENT || 
            (type == ActionType::COMPOUND && GetProperty("position_change") != 0)) {
            
            // Check battlefield conditions for movement
            if (battlefield) {
                int posChange = GetProperty("position_change");
                int currentPos = user->GetComponent<PositionComponent>().GetPosition();
                int newPos = currentPos + posChange;
                
                if (!battlefield->IsValidPosition(newPos)) {
                    std::cout << "  Reason: Movement would go out of bounds." << std::endl;
                } else if (battlefield->IsPositionOccupied(newPos)) {
                    Entity* blockingEntity = battlefield->GetEntityAtPosition(newPos);
                    std::cout << "  Reason: Position " << newPos << " is occupied by " 
                              << (blockingEntity ? blockingEntity->GetName() : "an entity") << "." << std::endl;
                }
            }
        }
        
        return false;
    }
    
    // Check for accuracy/hit chance
    bool hit = true;
    if (accuracy < 100) {
        int roll = std::rand() % 100 + 1; // 1-100
        hit = (roll <= accuracy);
        
        if (!hit) {
            std::cout << "Action " << name << " missed!" << std::endl;
            StartCooldown();
            return false;
        }
    }
    
    // If we don't have any effects, create them from properties
    if (effects.empty()) {
        CreateEffectsFromProperties();
    }
    
    // For compound actions, announce it's a combo
    if (type == ActionType::COMPOUND && effects.size() > 1) {
        std::cout << "-- " << name << " combo: " << effects.size() << " effects --" << std::endl;
    }
    
    // Execute all effects
    bool anyEffectExecuted = false;
    for (auto& effect : effects) {
        if (effect->CanApply(user, target, battlefield)) {
            if (effect->Execute(user, target, battlefield)) {
                anyEffectExecuted = true;
            }
        }
    }
    
    // Handle the old effectCallback if present
    if (effectCallback) {
        effectCallback(user, target, battlefield);
        anyEffectExecuted = true;
    }
    
    // Apply cooldown if any effect was executed
    if (anyEffectExecuted) {
        StartCooldown();
    }
    
    // For compound actions, end the combo announcement
    if (type == ActionType::COMPOUND && effects.size() > 1) {
        std::cout << "-- End of " << name << " combo --" << std::endl;
    }
    
    return anyEffectExecuted;
}

bool Action::CanUse(const Entity* user, const Entity* target, const Battlefield* battlefield) const {
    // Check if action is on cooldown
    if (IsOnCooldown()) {
        std::cout << "Action " << name << " is on cooldown: " 
                  << currentCooldown << " turns remaining." << std::endl;
        return false;
    }
    
    // Check if both entities exist
    if (!user || !target) {
        return false;
    }
    
    // Check for required components
    if (!user->HasComponent<StatsComponent>() || 
        !target->HasComponent<StatsComponent>()) {
        return false;
    }
    
    // Check for positioning if battlefield is provided
    if (battlefield && range > 0) {
        // Both entities need position components
        if (!user->HasComponent<PositionComponent>() || 
            !target->HasComponent<PositionComponent>()) {
            return false;
        }
        
        // Get positions
        int userPos = const_cast<Entity*>(user)->GetComponent<PositionComponent>().GetPosition();
        int targetPos = const_cast<Entity*>(target)->GetComponent<PositionComponent>().GetPosition();
        
        // Calculate distance
        int distance = std::abs(userPos - targetPos);
        
        // Check if target is within range
        if (distance > range) {
            std::cout << "Target is out of range. Required: " << range 
                      << ", Actual: " << distance << std::endl;
            return false;
        }
    }
    
    // Check target validity based on action type
    bool isSelfTargeted = user == target;
    
    // Self-targeted actions like buffs
    if ((type == ActionType::BUFF || type == ActionType::HEAL) && !isSelfTargeted) {
        // Some buffs and heals require self-targeting
        if (GetProperty("self_only") > 0) {
            std::cout << "This action can only target the user." << std::endl;
            return false;
        }
    }
    
    // Enemy-targeted actions like attacks
    if ((type == ActionType::ATTACK || type == ActionType::DEBUFF) && isSelfTargeted) {
        // Can't attack or debuff self unless specifically allowed
        if (GetProperty("can_target_self") <= 0) {
            std::cout << "Cannot use this action on yourself." << std::endl;
            return false;
        }
    }
    
    // If we have effects, check if at least one can be applied
    if (!effects.empty()) {
        bool anyEffectApplicable = false;
        for (const auto& effect : effects) {
            if (effect->CanApply(user, target, battlefield)) {
                anyEffectApplicable = true;
                break;
            }
        }
        
        if (!anyEffectApplicable) {
            return false;
        }
    }
    
    return true;
}

int Action::GetDamage() const {
    return GetProperty("damage");
}

void Action::SetDamage(int value) {
    SetProperty("damage", value);
}

void Action::CreateEffectsFromProperties() {
    // Clear existing effects
    effects.clear();
    
    // Based on the action type and properties, create appropriate effects
    switch (type) {
        case ActionType::ATTACK: {
            int damage = GetProperty("damage");
            bool isPhysical = GetProperty("is_physical") > 0;
            if (damage > 0) {
                effects.push_back(std::make_unique<DamageEffect>(damage, isPhysical));
            }
            break;
        }
        
        case ActionType::HEAL: {
            int healAmount = GetProperty("damage"); // Reuse damage field for heal amount
            if (healAmount > 0) {
                effects.push_back(std::make_unique<HealingEffect>(healAmount));
            }
            break;
        }
        
        case ActionType::MOVEMENT: {
            int positionChange = GetProperty("position_change");
            if (positionChange != 0) {
                effects.push_back(std::make_unique<MovementEffect>(positionChange));
            }
            break;
        }
        
        case ActionType::BUFF:
        case ActionType::DEBUFF: {
            // Check for stat modifiers (format: "mod_STATNAME")
            for (const auto& [key, value] : properties) {
                if (key.substr(0, 4) == "mod_") {
                    std::string statName = key.substr(4);
                    
                    // Convert string to StatType
                    StatType statType;
                    if (statName == "STRENGTH") statType = StatType::STRENGTH;
                    else if (statName == "INTELLECT") statType = StatType::INTELLECT;
                    else if (statName == "SPEED") statType = StatType::SPEED;
                    else if (statName == "DEXTERITY") statType = StatType::DEXTERITY;
                    else if (statName == "CONSTITUTION") statType = StatType::CONSTITUTION;
                    else if (statName == "DEFENSE") statType = StatType::DEFENSE;
                    else if (statName == "LUCK") statType = StatType::LUCK;
                    else continue; // Not a valid stat
                    
                    // Duration for effect
                    int duration = GetProperty("duration");
                    if (duration <= 0) duration = 3; // Default 3 turns
                    
                    effects.push_back(std::make_unique<StatModifierEffect>(statType, value, duration));
                }
            }
            break;
        }
        
        case ActionType::COMPOUND: {
            // For compound actions, check for all possible effect types
            
            // Check for damage
            int damage = GetProperty("damage");
            bool isPhysical = GetProperty("is_physical") > 0;
            if (damage > 0) {
                effects.push_back(std::make_unique<DamageEffect>(damage, isPhysical));
            }
            
            // Check for healing
            int healAmount = GetProperty("heal_amount");
            if (healAmount > 0) {
                // For life drain, we want to heal the user, not the target
                class SelfHealingEffect : public HealingEffect {
                public:
                    explicit SelfHealingEffect(int amount) : HealingEffect(amount) {}
                    
                    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override {
                        // Override to heal the user instead of the target
                        return HealingEffect::Execute(user, user, battlefield);
                    }
                    
                    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override {
                        // Check if user has health to heal
                        if (!user || !user->HasComponent<StatsComponent>()) {
                            return false;
                        }
                        
                        auto& stats = const_cast<Entity*>(user)->GetComponent<StatsComponent>();
                        if (stats.GetCurrentHealth() >= stats.GetMaxHealth()) {
                            std::cout << user->GetName() << " is already at full health." << std::endl;
                            return false;
                        }
                        
                        return true;
                    }
                };
                
                effects.push_back(std::make_unique<SelfHealingEffect>(healAmount));
            }
            
            // Check for self movement (like in Charge)
            int positionChange = GetProperty("position_change");
            if (positionChange != 0) {
                // For charge, we want to move the user, not the target
                class SelfMovementEffect : public MovementEffect {
                public:
                    explicit SelfMovementEffect(int change) : MovementEffect(change) {}
                    
                    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override {
                        // Override to move the user instead of the target
                        return MovementEffect::Execute(user, user, battlefield);
                    }
                    
                    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override {
                        // Check if user can move
                        return MovementEffect::CanApply(user, user, battlefield);
                    }
                };
                
                effects.push_back(std::make_unique<SelfMovementEffect>(positionChange));
            }
            
            // Check for stat modifiers
            for (const auto& [key, value] : properties) {
                if (key.substr(0, 4) == "mod_") {
                    std::string statName = key.substr(4);
                    
                    // Convert string to StatType
                    StatType statType;
                    if (statName == "STRENGTH") statType = StatType::STRENGTH;
                    else if (statName == "INTELLECT") statType = StatType::INTELLECT;
                    else if (statName == "SPEED") statType = StatType::SPEED;
                    else if (statName == "DEXTERITY") statType = StatType::DEXTERITY;
                    else if (statName == "CONSTITUTION") statType = StatType::CONSTITUTION;
                    else if (statName == "DEFENSE") statType = StatType::DEFENSE;
                    else if (statName == "LUCK") statType = StatType::LUCK;
                    else continue; // Not a valid stat
                    
                    // Duration for effect
                    int duration = GetProperty("duration");
                    if (duration <= 0) duration = 3; // Default 3 turns
                    
                    // For power_strike, we want to buff ourselves and attack the enemy
                    if (value > 0) {  // Buff effect (usually for self)
                        class SelfStatModifierEffect : public StatModifierEffect {
                        public:
                            SelfStatModifierEffect(StatType type, int val, int dur) 
                                : StatModifierEffect(type, val, dur) {}
                            
                            bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override {
                                // Override to buff the user instead of the target
                                return StatModifierEffect::Execute(user, user, battlefield);
                            }
                            
                            bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override {
                                return user && user->HasComponent<StatsComponent>();
                            }
                        };
                        
                        effects.push_back(std::make_unique<SelfStatModifierEffect>(statType, value, duration));
                    } else {  // Debuff effect (usually for enemy)
                        effects.push_back(std::make_unique<StatModifierEffect>(statType, value, duration));
                    }
                }
            }
            break;
        }
        
        case ActionType::SPECIAL:
        default:
            // For special actions, we rely on the effectCallback
            break;
    }
}

} // namespace Game 