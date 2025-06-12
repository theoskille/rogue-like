#pragma once

#include "StateManager.h"
#include "/opt/homebrew/include/raylib.h"

namespace Engine {

// A simple menu state
class MenuState : public GameState {
public:
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    std::string GetStateName() const override { return "MenuState"; }

private:
    float animationTime = 0.0f;
    bool isPaused = false;
};

// A simple game play state
class GameplayState : public GameState {
public:
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    std::string GetStateName() const override { return "GameplayState"; }

private:
    int playerX = 400;
    int playerY = 300;
    float animationTime = 0.0f;
    bool isPaused = false;
};

} // namespace Engine 