# Rogue-Like Game

A turn-based combat roguelike with tactical positioning, procedurally generated dungeons, and permadeath.

## Prerequisites

- C++17 compatible compiler
- Raylib (installed via Homebrew on macOS)

## Installation

### macOS

1. Install Homebrew if not already installed:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. Install Raylib:
   ```bash
   brew install raylib
   ```

## Building

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd rogue-like
   ```

2. Build the project:
   ```bash
   make clean && make
   ```

3. Run the game:
   ```bash
   ./rogue-like
   ```

## Game Controls

- **WASD**: Menu navigation
- **Space**: Select/confirm

## Development

The project follows a modular architecture with:
- Event-driven communication
- Data-driven design using JSON
- Component-based entity system

See `tech_architecture_doc.md` and `game_design_doc.md` for detailed technical and design information. 