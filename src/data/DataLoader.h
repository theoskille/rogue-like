#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include "json.hpp"

// For convenience
using json = nlohmann::json;

namespace Game {

// Base class for data objects that can be loaded from JSON
class JsonSerializable {
public:
    virtual ~JsonSerializable() = default;
    
    // Load data from JSON
    virtual void FromJson(const json& data) = 0;
    
    // Convert to JSON
    virtual json ToJson() const = 0;
};

// Generic data loader for loading JSON files
template<typename T>
class DataLoader {
public:
    DataLoader() = default;
    
    // Load data from a JSON file
    bool LoadFromFile(const std::string& filePath) {
        try {
            // Read file
            std::ifstream file(filePath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filePath << std::endl;
                return false;
            }
            
            // Parse JSON
            json data = json::parse(file);
            
            // Load data into objects
            return LoadFromJson(data);
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading from file " << filePath << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load data from JSON
    bool LoadFromJson(const json& data) {
        try {
            items.clear();
            
            // Parse each item in the JSON
            for (auto& [key, value] : data.items()) {
                // Create object
                auto item = std::make_shared<T>();
                
                // Parse from JSON if it's a JsonSerializable
                if constexpr (std::is_base_of<JsonSerializable, T>::value) {
                    item->FromJson(value);
                }
                
                // Store item
                items[key] = item;
            }
            
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading from JSON: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Save data to a JSON file
    bool SaveToFile(const std::string& filePath) {
        try {
            // Convert to JSON
            json data = ToJson();
            
            // Write to file
            std::ofstream file(filePath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << filePath << std::endl;
                return false;
            }
            
            file << data.dump(4); // Pretty-print with 4-space indentation
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving to file " << filePath << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Convert all items to JSON
    json ToJson() const {
        json data;
        
        for (const auto& [key, item] : items) {
            // Convert to JSON if it's a JsonSerializable
            if constexpr (std::is_base_of<JsonSerializable, T>::value) {
                data[key] = item->ToJson();
            }
        }
        
        return data;
    }
    
    // Access items
    std::shared_ptr<T> GetItem(const std::string& key) const {
        auto it = items.find(key);
        if (it != items.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Check if an item exists
    bool HasItem(const std::string& key) const {
        return items.find(key) != items.end();
    }
    
    // Add or update an item
    void SetItem(const std::string& key, std::shared_ptr<T> item) {
        items[key] = item;
    }
    
    // Remove an item
    void RemoveItem(const std::string& key) {
        items.erase(key);
    }
    
    // Get all items
    const std::unordered_map<std::string, std::shared_ptr<T>>& GetItems() const {
        return items;
    }
    
    // Clear all items
    void Clear() {
        items.clear();
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<T>> items;
};

} // namespace Game 