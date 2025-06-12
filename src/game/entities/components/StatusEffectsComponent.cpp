#include "StatusEffectsComponent.h"
#include <iostream>
#include <algorithm>

namespace Game {

//--------- StatusEffect Base Implementation ---------//

StatusEffect::StatusEffect(StatusEffectType type, int duration, const std::string& name)
    : type(type), duration(duration), name(name) {
}

bool StatusEffect::OnNewTurn(Entity* entity) {
    // Default implementation - allow the entity to take a turn
    return true;
}

//--------- StatusEffectsComponent Implementation ---------//

StatusEffectsComponent::StatusEffectsComponent()
    : owner(nullptr) {
}

void StatusEffectsComponent::AddEffect(std::unique_ptr<StatusEffect> effect) {
    if (!effect) return;
    
    // Check if an effect of the same name already exists
    auto it = std::find_if(activeEffects.begin(), activeEffects.end(),
        [&](const std::unique_ptr<StatusEffect>& existingEffect) {
            return existingEffect->GetName() == effect->GetName();
        });
    
    if (it != activeEffects.end()) {
        // Replace the existing effect
        *it = std::move(effect);
        std::cout << "Status effect " << (*it)->GetName() << " refreshed." << std::endl;
    } else {
        // Add the new effect
        std::cout << "Status effect " << effect->GetName() << " applied." << std::endl;
        activeEffects.push_back(std::move(effect));
    }
}

void StatusEffectsComponent::RemoveEffect(const std::string& effectName) {
    auto it = std::remove_if(activeEffects.begin(), activeEffects.end(),
        [&](const std::unique_ptr<StatusEffect>& effect) {
            return effect->GetName() == effectName;
        });
    
    if (it != activeEffects.end()) {
        std::cout << "Status effect " << effectName << " removed." << std::endl;
        activeEffects.erase(it, activeEffects.end());
    }
}

void StatusEffectsComponent::ClearEffects() {
    std::cout << "All status effects cleared." << std::endl;
    activeEffects.clear();
}

const std::vector<std::unique_ptr<StatusEffect>>& StatusEffectsComponent::GetEffects() const {
    return activeEffects;
}

bool StatusEffectsComponent::HasEffect(StatusEffectType type) const {
    return std::any_of(activeEffects.begin(), activeEffects.end(),
        [type](const std::unique_ptr<StatusEffect>& effect) {
            return effect->GetType() == type;
        });
}

bool StatusEffectsComponent::HasEffectByName(const std::string& effectName) const {
    return std::any_of(activeEffects.begin(), activeEffects.end(),
        [&](const std::unique_ptr<StatusEffect>& effect) {
            return effect->GetName() == effectName;
        });
}

void StatusEffectsComponent::ProcessTurnStart() {
    if (activeEffects.empty()) return;
    
    std::cout << "Processing " << activeEffects.size() << " status effects at turn start..." << std::endl;
    
    // Apply start-of-turn effects
    for (auto& effect : activeEffects) {
        effect->OnTurnStart(owner);
    }
    
    // Remove expired effects
    RemoveExpiredEffects();
}

void StatusEffectsComponent::ProcessTurnEnd() {
    if (activeEffects.empty()) return;
    
    std::cout << "Processing " << activeEffects.size() << " status effects at turn end..." << std::endl;
    
    // Apply end-of-turn effects
    for (auto& effect : activeEffects) {
        effect->OnTurnEnd(owner);
    }
    
    // Remove expired effects
    RemoveExpiredEffects();
}

bool StatusEffectsComponent::ProcessNewTurn() {
    if (activeEffects.empty()) return true;
    
    // Check if any effect prevents taking a turn
    for (auto& effect : activeEffects) {
        if (!effect->OnNewTurn(owner)) {
            std::cout << owner->GetName() << " cannot take a turn due to " 
                      << effect->GetName() << "!" << std::endl;
            return false;
        }
    }
    
    return true;
}

void StatusEffectsComponent::Update(float deltaTime) {
    // Not needed for this component
    (void)deltaTime;
}

void StatusEffectsComponent::OnAttach(Entity* entity) {
    owner = entity;
}

void StatusEffectsComponent::RemoveExpiredEffects() {
    auto it = std::remove_if(activeEffects.begin(), activeEffects.end(),
        [](const std::unique_ptr<StatusEffect>& effect) {
            if (effect->HasExpired()) {
                std::cout << "Status effect " << effect->GetName() << " expired." << std::endl;
                return true;
            }
            return false;
        });
    
    activeEffects.erase(it, activeEffects.end());
}

//--------- PoisonEffect Implementation ---------//

PoisonEffect::PoisonEffect(int duration, int damagePerTurn)
    : StatusEffect(StatusEffectType::POISON, duration, "Poison"), 
      damagePerTurn(damagePerTurn) {
    description = "Deals " + std::to_string(damagePerTurn) + " damage per turn.";
}

void PoisonEffect::OnTurnStart(Entity* entity) {
    if (!entity || !entity->HasComponent<StatsComponent>()) return;
    
    // Apply poison damage at the start of turn
    auto& stats = entity->GetComponent<StatsComponent>();
    
    std::cout << entity->GetName() << " takes " << damagePerTurn 
              << " poison damage!" << std::endl;
    
    // Apply damage without killing the entity (minimum 1 HP left)
    int currentHealth = stats.GetCurrentHealth();
    int damage = std::min(currentHealth - 1, damagePerTurn);
    
    if (damage > 0) {
        stats.TakeDamage(damage);
    } else {
        std::cout << "Poison damage prevented to avoid death." << std::endl;
    }
}

void PoisonEffect::OnTurnEnd(Entity* entity) {
    // Decrease duration at the end of turn
    DecreaseDuration();
}

//--------- StunEffect Implementation ---------//

StunEffect::StunEffect(int duration)
    : StatusEffect(StatusEffectType::STUN, duration, "Stun") {
    description = "Cannot take actions for " + std::to_string(duration) + " turns.";
}

void StunEffect::OnTurnStart(Entity* entity) {
    if (!entity) return;
    
    std::cout << entity->GetName() << " is stunned!" << std::endl;
}

void StunEffect::OnTurnEnd(Entity* entity) {
    // Decrease duration at the end of turn
    DecreaseDuration();
}

bool StunEffect::OnNewTurn(Entity* entity) {
    // If stunned, entity cannot take a turn
    return false;
}

//--------- StatModifierEffect Implementation ---------//

StatBuffEffect::StatBuffEffect(int duration, StatType statType, int modifierValue)
    : StatusEffect(modifierValue > 0 ? StatusEffectType::BUFF : StatusEffectType::DEBUFF, 
                  duration, 
                  modifierValue > 0 ? "Buff" : "Debuff"),
      statType(statType), 
      modifierValue(modifierValue),
      applied(false) {
    
    // Create descriptive name based on the stat and effect
    std::string statName;
    switch (statType) {
        case StatType::STRENGTH: statName = "Strength"; break;
        case StatType::INTELLECT: statName = "Intellect"; break;
        case StatType::SPEED: statName = "Speed"; break;
        case StatType::DEXTERITY: statName = "Dexterity"; break;
        case StatType::CONSTITUTION: statName = "Constitution"; break;
        case StatType::DEFENSE: statName = "Defense"; break;
        case StatType::LUCK: statName = "Luck"; break;
        default: statName = "Unknown"; break;
    }
    
    name = (modifierValue > 0 ? "+" : "") + std::to_string(modifierValue) + " " + statName;
    description = "Modifies " + statName + " by " + std::to_string(modifierValue) + 
                  " for " + std::to_string(duration) + " turns.";
}

void StatBuffEffect::OnTurnStart(Entity* entity) {
    if (!entity || !entity->HasComponent<StatsComponent>()) return;
    
    // Apply the stat modification if not already applied
    if (!applied) {
        auto& stats = entity->GetComponent<StatsComponent>();
        stats.AddModifier(statType, modifierValue, duration);
        applied = true;
        
        std::cout << entity->GetName() << "'s " << description << std::endl;
    }
}

void StatBuffEffect::OnTurnEnd(Entity* entity) {
    // Decrease duration at the end of turn
    DecreaseDuration();
    
    // We don't need to handle stat modifier removal here since it's managed by StatsComponent
}

//--------- Status Effect Factory ---------//

std::unique_ptr<StatusEffect> CreateStatusEffect(
    StatusEffectType type, 
    int duration, 
    int magnitude) {
    
    switch (type) {
        case StatusEffectType::POISON:
            return std::make_unique<PoisonEffect>(duration, magnitude);
            
        case StatusEffectType::STUN:
            return std::make_unique<StunEffect>(duration);
            
        case StatusEffectType::BUFF:
            // This requires more specific information
            // For now, create a generic strength buff
            return std::make_unique<StatBuffEffect>(duration, StatType::STRENGTH, std::abs(magnitude));
            
        case StatusEffectType::DEBUFF:
            // This requires more specific information
            // For now, create a generic strength debuff
            return std::make_unique<StatBuffEffect>(duration, StatType::STRENGTH, -std::abs(magnitude));
            
        // Add other effect types as needed
            
        default:
            std::cerr << "Unknown status effect type requested." << std::endl;
            return nullptr;
    }
}

} // namespace Game 