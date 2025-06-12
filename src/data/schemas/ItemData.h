#pragma once

#include "../DataLoader.h"
#include <string>

namespace Game {

// Data class for items in the game
class ItemData : public JsonSerializable {
public:
    ItemData() = default;
    
    // Getters
    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    int GetValue() const { return value; }
    int GetWeight() const { return weight; }
    bool IsEquippable() const { return equippable; }
    const std::string& GetEquipSlot() const { return equipSlot; }
    int GetStrBonus() const { return strBonus; }
    int GetDexBonus() const { return dexBonus; }
    int GetConBonus() const { return conBonus; }
    int GetIntBonus() const { return intBonus; }
    int GetSpdBonus() const { return spdBonus; }
    
    // Setters
    void SetName(const std::string& value) { name = value; }
    void SetDescription(const std::string& value) { description = value; }
    void SetValue(int value) { this->value = value; }
    void SetWeight(int value) { weight = value; }
    void SetEquippable(bool value) { equippable = value; }
    void SetEquipSlot(const std::string& value) { equipSlot = value; }
    void SetStrBonus(int value) { strBonus = value; }
    void SetDexBonus(int value) { dexBonus = value; }
    void SetConBonus(int value) { conBonus = value; }
    void SetIntBonus(int value) { intBonus = value; }
    void SetSpdBonus(int value) { spdBonus = value; }
    
    // Load from JSON
    void FromJson(const json& data) override {
        name = data.value("name", "Unknown Item");
        description = data.value("description", "");
        value = data.value("value", 0);
        weight = data.value("weight", 0);
        equippable = data.value("equippable", false);
        equipSlot = data.value("equipSlot", "");
        
        // Stats bonuses
        if (data.contains("bonuses")) {
            const auto& bonuses = data["bonuses"];
            strBonus = bonuses.value("str", 0);
            dexBonus = bonuses.value("dex", 0);
            conBonus = bonuses.value("con", 0);
            intBonus = bonuses.value("int", 0);
            spdBonus = bonuses.value("spd", 0);
        }
    }
    
    // Convert to JSON
    json ToJson() const override {
        json data;
        
        data["name"] = name;
        data["description"] = description;
        data["value"] = value;
        data["weight"] = weight;
        data["equippable"] = equippable;
        
        if (equippable) {
            data["equipSlot"] = equipSlot;
            
            // Only include bonuses if they're non-zero
            json bonuses;
            if (strBonus != 0) bonuses["str"] = strBonus;
            if (dexBonus != 0) bonuses["dex"] = dexBonus;
            if (conBonus != 0) bonuses["con"] = conBonus;
            if (intBonus != 0) bonuses["int"] = intBonus;
            if (spdBonus != 0) bonuses["spd"] = spdBonus;
            
            if (!bonuses.empty()) {
                data["bonuses"] = bonuses;
            }
        }
        
        return data;
    }
    
private:
    std::string name;
    std::string description;
    int value = 0;
    int weight = 0;
    bool equippable = false;
    std::string equipSlot;
    
    // Stats bonuses (for equippable items)
    int strBonus = 0;
    int dexBonus = 0;
    int conBonus = 0;
    int intBonus = 0;
    int spdBonus = 0;
};

// Type alias for a loader that handles ItemData
using ItemLoader = DataLoader<ItemData>;

} // namespace Game 