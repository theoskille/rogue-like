# Turn-Based Dungeon Crawler - Game Design Reference

## Core Concept
Turn-based combat roguelike with tactical positioning on linear battlefield. Navigate procedurally generated dungeons, defeat bosses, progress through floors until death.

## Dungeon Generation

### Structure
- **Graph-based nodes**: Each room is a connected node
- **Random generation**: Procedural layout each run
- **Boss placement**: One boss room per floor
- **Navigation**: Select connected rooms from map view
- **Encounter trigger**: Entering room immediately starts encounter

### Room Types
- **Combat**: Fight enemies
- **Treasure**: Loot chests
- **Boss**: Floor-ending battle

## Combat System

### Battlefield
- **8-tile linear strip**
- **Starting positions**: Player and enemy 2 tiles apart
- **Movement restriction**: Cannot move past each other

### Turn Actions
1. **Fight**: Use class-specific actions
2. **Move**: Advance (toward enemy) or Retreat (away)
3. **Item**: Use consumables
4. **Run**: Attempt escape

### Action Properties
- **Accuracy**: Hit chance percentage
- **Damage**: Base damage value
- **Range**: Required positioning to use
- **Position Changes**: Can move self or enemy
- **Status Effects**: Apply poison, stun, etc.
- **Stat Buffs**: Temporary stat modifications
- **Cooldown**: Turns before reuse

### Turn Order
- **Speed-based**: Higher speed goes first
- **Multiple turns**: High speed can grant extra turns
- **Excess speed system**: Accumulate speed overflow for bonus turns

## Entity Stats

### Core Stats
- **STR**: Physical damage
- **INT**: Casting power  
- **SPD**: Turn order, multiple turn potential
- **DEX**: Weapon damage + dodge chance
- **CON**: Health points
- **DEF**: Block chance (ignore damage)
- **LCK**: Critical hit chance

### Equipment Slots
- Chest, Helmet, Right Arm, Left Arm, Accessories
- All slots modify stats

## Character Progression

### Classes
- **Unique action sets** per class
- **Unlock through gameplay**

### Leveling
- **Gain levels**: Through combat/progression
- **Skill points**: Earned on level up
- **Skill trees**: Upgrade actions and abilities

## Controls
- **WASD**: Menu navigation
- **Space**: Select/confirm

## Roguelike Elements

### Permadeath
- Death ends run, restart from beginning

### Persistence
- Class unlocks carry between runs

## Technical Notes
- C++ with Raylib
- Basic Makefiles
- Focus on clean, modular architecture