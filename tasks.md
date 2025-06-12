# Rogue-Like Implementation Tasks

This document outlines the step-by-step implementation plan for our C++ rogue-like game, with clear, testable tasks. After completing each task, we'll test to ensure everything works before moving on to the next one.

## Phase 1: Project Setup & Core Engine Components

### Task 1.1: Project Structure & Build System
- [x] Create directory structure as per tech architecture doc
- [x] Set up Makefile with Raylib integration
- [x] Create README.md with build instructions
- [x] Verify build system works with a simple "Hello World" program
- [x] Test: Successfully compile and run the basic program

### Task 1.2: Core Engine - Application & Window Management
- [x] Implement Application class (src/engine/core/Application.cpp/.h)
- [x] Create main.cpp with initialization code
- [x] Implement basic window creation and main loop
- [x] Test: Application opens a window and responds to close events

### Task 1.3: Core Engine - Event System
- [x] Implement EventSystem class (src/engine/core/EventSystem.cpp/.h)
- [x] Create event types enumeration
- [x] Implement event publishing and subscription mechanisms
- [x] Create simple test events to verify functionality
- [x] Test: Events can be published and subscribers receive them

### Task 1.4: Core Engine - Game State Management
- [x] Implement StateManager class (src/engine/core/StateManager.cpp/.h)
- [x] Create GameState base class with lifecycle methods
- [x] Implement state stack operations (push, pop, change)
- [x] Test: Transition between different states works correctly

### Task 1.5: Core Engine - Input Handling
- [x] Implement InputHandler class (src/engine/input/InputHandler.cpp/.h)
- [x] Map WASD and Space inputs to game actions
- [x] Create input event types
- [x] Test: Input events are generated on key presses

### Task 1.6: Core Engine - Renderer
- [x] Implement Renderer class (src/engine/rendering/Renderer.cpp/.h)
- [x] Create basic drawing primitives and text rendering
- [x] Implement render loop with BeginFrame/EndFrame
- [x] Test: Basic shapes and text can be rendered to the screen

## Phase 2: Basic Game Systems

### Task 2.1: Entity Component System
- [x] Implement Entity base class (src/game/entities/Entity.cpp/.h)
- [x] Create Component base class
- [x] Implement AddComponent, GetComponent, HasComponent methods
- [x] Test: Create an entity with multiple components and verify operations

### Task 2.2: Data Loading
- [x] Implement DataLoader class (src/data/DataLoader.cpp/.h)
- [x] Create JSON parsing utilities
- [x] Set up initial schema structure
- [x] Test: Load sample JSON data into game structures

### Task 2.3: Stats Component
- [x] Implement Stats component (src/game/entities/components/StatsComponent.cpp/.h)
- [x] Define core stats (STR, INT, SPD, DEX, CON, DEF, LCK)
- [x] Create methods for stat calculations and modifications
- [x] Test: Create an entity with stats and modify them

### Task 2.4: Position Component
- [x] Implement Position component (src/game/entities/components/PositionComponent.cpp/.h)
- [x] Create position tracking for the battlefield
- [x] Test: Entity can be positioned and moved on a grid

### Task 2.5: Basic UI Framework
- [x] Implement UIManager class (src/engine/rendering/UI/UIManager.cpp/.h)
- [x] Create Widget base class (src/engine/rendering/UI/Widget.cpp/.h)
- [x] Implement Menu class (src/engine/rendering/UI/Menu.cpp/.h)
- [x] Test: Create and navigate a simple menu

## Phase 3: Combat System

### Task 3.1: Battlefield Implementation
- [ ] Implement Battlefield class (src/game/combat/Battlefield.cpp/.h)
- [ ] Create 8-tile linear strip representation
- [ ] Implement position validation and movement rules
- [ ] Test: Entities can be placed and moved on battlefield

### Task 3.2: Move Implementation
- [ ] Implement Move class (src/game/combat/Move.cpp/.h)
- [ ] Define properties (accuracy, damage, range, etc.)
- [ ] Create JSON schema for moves
- [ ] Test: Load and execute basic moves

### Task 3.3: Turn Manager
- [ ] Implement TurnManager class (src/game/combat/TurnManager.cpp/.h)
- [ ] Create speed-based turn order calculation
- [ ] Implement turn processing logic
- [ ] Test: Turn order is correctly determined by speed

### Task 3.4: Combat System
- [ ] Implement CombatSystem class (src/game/combat/CombatSystem.cpp/.h)
- [ ] Create combat flow (start, process turns, end)
- [ ] Integrate with Battlefield and TurnManager
- [ ] Test: Complete a simple combat encounter

### Task 3.5: Status Effects
- [ ] Implement StatusEffects component (src/game/entities/components/StatusEffects.cpp/.h)
- [ ] Create various effect types (poison, stun, etc.)
- [ ] Implement duration and tick mechanics
- [ ] Test: Apply and resolve different status effects

## Phase 4: Dungeon & Room Generation

### Task 4.1: Room Implementation
- [ ] Implement Room class (src/game/dungeon/Room.cpp/.h)
- [ ] Define room connections and properties
- [ ] Test: Create rooms and establish connections between them

### Task 4.2: Encounter System
- [ ] Implement Encounter base class (src/game/dungeon/encounters/Encounter.cpp/.h)
- [ ] Create CombatEncounter class (src/game/dungeon/encounters/CombatEncounter.cpp/.h)
- [ ] Implement TreasureEncounter class (src/game/dungeon/encounters/TreasureEncounter.cpp/.h)
- [ ] Test: Rooms can trigger different encounter types

### Task 4.3: Dungeon Generator
- [ ] Implement DungeonGenerator class (src/game/dungeon/DungeonGenerator.cpp/.h)
- [ ] Create graph-based random room generation
- [ ] Implement floor layouts with boss rooms
- [ ] Test: Generate and validate a complete dungeon floor

## Phase 5: Game States & Screens

### Task 5.1: Main Menu State
- [ ] Implement MainMenuState class (src/game/states/MainMenuState.cpp/.h)
- [ ] Create title screen and class selection UI
- [ ] Test: Navigate through menu options

### Task 5.2: Dungeon State
- [ ] Implement DungeonState class (src/game/states/DungeonState.cpp/.h)
- [ ] Create room navigation and map view
- [ ] Test: Navigate between rooms in the dungeon

### Task 5.3: Combat State
- [ ] Implement CombatState class (src/game/states/CombatState.cpp/.h)
- [ ] Create combat UI with turn actions
- [ ] Test: Complete a combat encounter through the UI

### Task 5.4: Game Over State
- [ ] Implement GameOverState class (src/game/states/GameOverState.cpp/.h)
- [ ] Create death screen with stats and restart option
- [ ] Test: Properly transition to game over when player dies

## Phase 6: Player Progression

### Task 6.1: Equipment System
- [ ] Implement Equipment component (src/game/entities/components/Equipment.cpp/.h)
- [ ] Create equipment slots and stat modifications
- [ ] Test: Equip items and verify stat changes

### Task 6.2: Level System
- [ ] Implement LevelSystem class (src/game/progression/LevelSystem.cpp/.h)
- [ ] Create XP gain and level up mechanics
- [ ] Test: Gain XP and level up

### Task 6.3: Class System
- [ ] Implement ClassManager class (src/game/progression/ClassManager.cpp/.h)
- [ ] Create class-specific moves and abilities
- [ ] Implement class unlocking conditions
- [ ] Test: Unlock and switch between classes

## Phase 7: Content & Polishing

### Task 7.1: Content Creation
- [ ] Create enemy definitions in JSON
- [ ] Define move sets for different classes
- [ ] Create equipment and item definitions
- [ ] Test: Load and use all game content

### Task 7.2: Balance & Tuning
- [ ] Adjust difficulty curve
- [ ] Balance stats and combat mechanics
- [ ] Test with different playstyles
- [ ] Test: Game provides appropriate challenge at all stages

### Task 7.3: UI Polish
- [ ] Add visual feedback for actions
- [ ] Improve menu aesthetics
- [ ] Create consistent UI theme
- [ ] Test: UI is intuitive and responsive

### Task 7.4: Final Integration
- [ ] Connect all systems into cohesive game loop
- [ ] Implement save/load for persistent unlocks
- [ ] Fix any remaining bugs or issues
- [ ] Test: Complete playthrough from start to finish 