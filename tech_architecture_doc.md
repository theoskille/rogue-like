# Rogue-Like - Technical Architecture

## Technology Stack
- **Language**: C++17 or later
- **Graphics/Audio**: Raylib (installed via Homebrew)
- **Build System**: Makefile
- **Architecture**: Event-driven, data-driven, modular components

## Project Structure

```
rogue-like/
├── Makefile                    # Build configuration and rules
├── README.md                   # Project setup and build instructions
├── src/                        # All source code
│   ├── main.cpp               # Entry point, initializes Application
│   ├── engine/                # Reusable engine components (game-agnostic)
│   │   ├── core/              # Core engine systems
│   │   │   ├── Application.cpp/.h     # Main app loop, window management
│   │   │   ├── EventSystem.cpp/.h     # Pub/sub event handling
│   │   │   ├── StateManager.cpp/.h    # Game state stack management
│   │   │   └── GameTime.cpp/.h        # Delta time, frame timing
│   │   ├── rendering/         # Graphics and visual systems
│   │   │   ├── Renderer.cpp/.h        # Raylib wrapper, drawing primitives
│   │   │   ├── Camera.cpp/.h          # 2D camera transforms
│   │   │   └── UI/            # User interface components
│   │   │       ├── UIManager.cpp/.h   # UI system coordinator
│   │   │       ├── Menu.cpp/.h        # Menu navigation logic
│   │   │       └── Widget.cpp/.h      # Base UI element class
│   │   ├── input/             # Input handling systems
│   │   │   └── InputHandler.cpp/.h    # WASD + Space input processing
│   │   └── utils/             # Utility classes and helpers
│   │       ├── Logger.cpp/.h          # Debug/error logging
│   │       ├── Random.cpp/.h          # Random number generation
│   │       └── Math.cpp/.h            # Math utilities, constants
│   ├── game/                  # Game-specific logic and rules
│   │   ├── core/              # Central game management
│   │   │   ├── GameManager.cpp/.h     # Main game controller, run coordination
│   │   │   └── GameConfig.cpp/.h      # Game settings, constants
│   │   ├── entities/          # Player, enemies, and entity components
│   │   │   ├── Entity.cpp/.h          # Base entity class with components
│   │   │   ├── Player.cpp/.h          # Player-specific logic and data
│   │   │   ├── Enemy.cpp/.h           # Enemy-specific logic and AI
│   │   │   └── components/    # Entity component implementations
│   │   │       ├── Stats.cpp/.h       # STR, INT, DEX, etc. stats component
│   │   │       ├── Equipment.cpp/.h   # Equipment slots and bonuses
│   │   │       ├── StatusEffects.cpp/.h # Poison, stun, buffs component
│   │   │       └── Position.cpp/.h    # Battlefield position component
│   │   ├── combat/            # Turn-based combat mechanics
│   │   │   ├── CombatSystem.cpp/.h    # Combat orchestration and rules
│   │   │   ├── Battlefield.cpp/.h     # 8-tile strip, positioning logic
│   │   │   ├── TurnManager.cpp/.h     # Speed-based turn order system
│   │   │   └── Move.cpp/.h            # Individual attack/ability definitions
│   │   ├── dungeon/           # Dungeon generation and exploration
│   │   │   ├── DungeonGenerator.cpp/.h # Random graph-based room generation
│   │   │   ├── Room.cpp/.h            # Individual room data and connections
│   │   │   ├── Floor.cpp/.h           # Complete dungeon floor with boss
│   │   │   └── encounters/    # Different types of room encounters
│   │   │       ├── Encounter.cpp/.h   # Base encounter interface
│   │   │       ├── CombatEncounter.cpp/.h # Enemy fight encounters
│   │   │       └── TreasureEncounter.cpp/.h # Chest/loot encounters
│   │   ├── progression/       # Character advancement systems
│   │   │   ├── LevelSystem.cpp/.h     # XP, leveling, skill points
│   │   │   ├── SkillTree.cpp/.h       # Skill point allocation system
│   │   │   └── ClassManager.cpp/.h    # Class unlocks and abilities
│   │   └── states/            # Game screen/state implementations
│   │       ├── MainMenuState.cpp/.h   # Title screen, class selection
│   │       ├── DungeonState.cpp/.h    # Room navigation, map view
│   │       ├── CombatState.cpp/.h     # Turn-based combat interface
│   │       ├── InventoryState.cpp/.h  # Equipment, items management
│   │       └── GameOverState.cpp/.h   # Death screen, restart options
│   └── data/                  # Data loading and JSON schemas
│       ├── DataLoader.cpp/.h          # JSON parsing and data loading
│       └── schemas/           # JSON definitions for game content
│           ├── items.json             # Equipment stats and properties
│           ├── enemies.json           # Enemy stats, moves, AI types
│           ├── moves.json             # Attack definitions and effects
│           ├── classes.json           # Class movesets and unlock conditions
│           └── encounters.json        # Encounter types and generation rules
├── assets/                    # Game resources (loaded at runtime)
│   ├── textures/              # Sprite images, UI graphics
│   ├── sounds/                # Audio effects and music
│   └── fonts/                 # Text rendering fonts
└── build/                     # Generated build files (gitignored)
```

## Directory Purposes

### `/src/engine/` - Reusable Game Engine
Contains systems that could be used in any game. No game-specific logic.
- **core/**: Fundamental engine systems (app lifecycle, events, states, timing)
- **rendering/**: Graphics abstraction and UI framework
- **input/**: Input handling and mapping
- **utils/**: Generic utility classes and helpers

### `/src/game/` - Rogue-Like Specific Logic
Contains all game rules, mechanics, and domain logic.
- **core/**: Central game management and configuration
- **entities/**: Player, enemies, and their component data
- **combat/**: Turn-based battle system and rules
- **dungeon/**: Procedural generation and exploration
- **progression/**: Character advancement mechanics
- **states/**: Game screens and user interfaces

### `/src/data/` - Data Management
Handles loading and parsing of JSON configuration files.
- **DataLoader**: Generic JSON loading utilities
- **schemas/**: JSON files defining game content (items, enemies, moves, etc.)

### `/assets/` - Runtime Resources
External files loaded by the game during execution.

### `/build/` - Generated Files
Compiler output, object files, final executable. Should be gitignored.

## Core Architecture Principles

### 1. Event-Driven Communication
```cpp
// Events decouple engine from game logic
enum class EventType {
    PLAYER_MOVE,
    COMBAT_START,
    HEALTH_CHANGED,
    LEVEL_UP,
    ROOM_ENTERED
};

class Event {
public:
    EventType type;
    std::unordered_map<std::string, std::any> data;
};

class EventSystem {
public:
    void Subscribe(EventType type, std::function<void(const Event&)> callback);
    void Publish(const Event& event);
};
```

### 2. Data-Driven Design
```cpp
// JSON-driven entity configuration
{
  "goblin": {
    "stats": { "str": 8, "dex": 12, "con": 10 },
    "moves": ["slash", "dodge"],
    "equipment": ["rusty_sword", "leather_armor"]
  }
}

// Generic data loader
template<typename T>
class DataLoader {
public:
    std::unordered_map<std::string, T> LoadFromJSON(const std::string& filepath);
};
```

### 3. Component-Based Entities
```cpp
class Entity {
private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    
public:
    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);
    
    template<typename T>
    T& GetComponent();
    
    template<typename T>
    bool HasComponent() const;
};
```

## Key Systems

### State Management
```cpp
class GameState {
public:
    virtual ~GameState() = default;
    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Exit() = 0;
};

class StateManager {
private:
    std::stack<std::unique_ptr<GameState>> states;
    
public:
    void PushState(std::unique_ptr<GameState> state);
    void PopState();
    void ChangeState(std::unique_ptr<GameState> state);
    void Update(float deltaTime);
    void Render();
};
```

### Combat System
```cpp
class CombatSystem {
private:
    TurnManager turnManager;
    Battlefield battlefield;
    EventSystem* eventSystem;
    
public:
    void StartCombat(Entity* player, Entity* enemy);
    void ProcessTurn(const Move& move);
    bool IsCombatOver() const;
};
```

## Raylib Integration

### Makefile Configuration
```makefile
# Detect Homebrew installation
BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/usr/local")
RAYLIB_PATH := $(BREW_PREFIX)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I$(RAYLIB_PATH)/include -Isrc/
LIBS = -L$(RAYLIB_PATH)/lib -lraylib

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Source files
SRCDIR = src
OBJDIR = build
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Main target
TARGET = rogue-like

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
```

### Renderer Wrapper
```cpp
class Renderer {
private:
    int screenWidth = 1024;
    int screenHeight = 768;
    
public:
    bool Initialize();
    void BeginFrame();
    void EndFrame();
    void Shutdown();
    
    // High-level drawing functions
    void DrawText(const std::string& text, int x, int y, int fontSize, Color color);
    void DrawRect(int x, int y, int width, int height, Color color);
    void DrawBattlefield(const Battlefield& battlefield);
};
```

## Common Patterns & Guidelines

### 1. RAII for Resource Management
```cpp
class TextureManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    
public:
    ~TextureManager() {
        for (auto& [name, texture] : textures) {
            UnloadTexture(texture);
        }
    }
    
    Texture2D& LoadTexture(const std::string& name, const std::string& path);
};
```

### 2. Factory Pattern for Data-Driven Creation
```cpp
class EntityFactory {
private:
    DataLoader<EntityData> dataLoader;
    
public:
    std::unique_ptr<Entity> CreateEntity(const std::string& type);
    std::unique_ptr<Move> CreateMove(const std::string& moveId);
};
```

### 3. Observer Pattern via Events
```cpp
class HealthSystem {
public:
    void Initialize(EventSystem* eventSystem) {
        eventSystem->Subscribe(EventType::HEALTH_CHANGED, 
            [this](const Event& e) { this->OnHealthChanged(e); });
    }
    
private:
    void OnHealthChanged(const Event& e);
};
```

### 4. Strategy Pattern for AI
```cpp
class AIStrategy {
public:
    virtual ~AIStrategy() = default;
    virtual Move SelectMove(const Entity& self, const Entity& target, 
                           const Battlefield& battlefield) = 0;
};

class AggressiveAI : public AIStrategy {
public:
    Move SelectMove(const Entity& self, const Entity& target, 
                   const Battlefield& battlefield) override;
};
```

## Build Instructions

1. **Prerequisites**: Ensure Raylib is installed via Homebrew
   ```bash
   brew install raylib
   ```

2. **Build**:
   ```bash
   make clean && make
   ```

3. **Run**:
   ```bash
   ./rogue-like
   ```

## Coding Standards

- **Naming**: PascalCase for classes, camelCase for functions/variables
- **Headers**: Use include guards or #pragma once
- **Memory**: Prefer smart pointers, RAII principles
- **Error Handling**: Use exceptions sparingly, prefer return codes/optionals
- **Documentation**: Brief comments for public interfaces
- **Testing**: Keep systems testable by avoiding tight coupling

## Extension Points

- **New Moves**: Add JSON entry, no code changes needed
- **New Enemies**: Define in JSON, automatic loading
- **New States**: Inherit from GameState, register with StateManager
- **New Events**: Add to EventType enum, publish/subscribe as needed
- **New Components**: Inherit from Component, attach to entities