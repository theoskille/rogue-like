#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <typeindex>

namespace Engine {

// Event types enumeration
enum class EventType {
    // Engine events
    WINDOW_RESIZE,
    WINDOW_CLOSE,
    
    // Input events
    KEY_PRESSED,
    KEY_RELEASED,
    
    // Game state events
    STATE_CHANGE,
    
    // Game events
    PLAYER_MOVE,
    COMBAT_START,
    HEALTH_CHANGED,
    LEVEL_UP,
    ROOM_ENTERED
};

// Event data structure
class Event {
public:
    Event(EventType type) : type(type) {}
    
    // Add string data to event
    void SetData(const std::string& key, const std::string& value) {
        stringData[key] = value;
    }
    
    // Add int data to event
    void SetData(const std::string& key, int value) {
        intData[key] = value;
    }
    
    // Add float data to event
    void SetData(const std::string& key, float value) {
        floatData[key] = value;
    }
    
    // Add bool data to event
    void SetData(const std::string& key, bool value) {
        boolData[key] = value;
    }
    
    // Get string data from event
    std::string GetStringData(const std::string& key, const std::string& defaultValue = "") const {
        auto it = stringData.find(key);
        return (it != stringData.end()) ? it->second : defaultValue;
    }
    
    // Get int data from event
    int GetIntData(const std::string& key, int defaultValue = 0) const {
        auto it = intData.find(key);
        return (it != intData.end()) ? it->second : defaultValue;
    }
    
    // Get float data from event
    float GetFloatData(const std::string& key, float defaultValue = 0.0f) const {
        auto it = floatData.find(key);
        return (it != floatData.end()) ? it->second : defaultValue;
    }
    
    // Get bool data from event
    bool GetBoolData(const std::string& key, bool defaultValue = false) const {
        auto it = boolData.find(key);
        return (it != boolData.end()) ? it->second : defaultValue;
    }
    
    // Check if event has specific data key
    bool HasStringData(const std::string& key) const {
        return stringData.find(key) != stringData.end();
    }
    
    bool HasIntData(const std::string& key) const {
        return intData.find(key) != intData.end();
    }
    
    bool HasFloatData(const std::string& key) const {
        return floatData.find(key) != floatData.end();
    }
    
    bool HasBoolData(const std::string& key) const {
        return boolData.find(key) != boolData.end();
    }
    
    EventType type;
    
private:
    std::unordered_map<std::string, std::string> stringData;
    std::unordered_map<std::string, int> intData;
    std::unordered_map<std::string, float> floatData;
    std::unordered_map<std::string, bool> boolData;
};

// Event callback type
using EventCallback = std::function<void(const Event&)>;

// Event System
class EventSystem {
public:
    EventSystem();
    ~EventSystem();
    
    // Subscribe to an event type
    void Subscribe(EventType type, EventCallback callback);
    
    // Unsubscribe from an event type (not implemented yet)
    void Unsubscribe(EventType type, EventCallback callback);
    
    // Publish an event
    void Publish(const Event& event);
    
    // Singleton access
    static EventSystem& GetInstance();
    
private:
    // Map of event types to list of callbacks
    std::unordered_map<EventType, std::vector<EventCallback>> subscribers;
    
    // Singleton instance
    static EventSystem* instance;
};

} // namespace Engine 