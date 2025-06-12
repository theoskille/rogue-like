#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/rendering/UI/UIManager.h"
#include "../../engine/rendering/UI/Menu.h"
#include <memory>

namespace Game {

class UITestState : public Engine::GameState {
public:
    UITestState();
    ~UITestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "UITestState"; }
    
private:
    // UI elements
    std::shared_ptr<Engine::UI::Menu> mainMenu;
    
    // Menu action callbacks
    void OnStartGame();
    void OnOptionsSelected();
    void OnCreditsSelected();
    void OnQuitSelected();
    
    // Current selection display
    std::string currentSelection;
    
    bool isPaused = false;
};

} // namespace Game 