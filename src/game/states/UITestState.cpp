#include "UITestState.h"
#include "../../engine/input/InputHandler.h"
#include <iostream>

namespace Game {

UITestState::UITestState() {
    std::cout << "UITestState created" << std::endl;
}

UITestState::~UITestState() {
    std::cout << "UITestState destroyed" << std::endl;
}

void UITestState::Enter() {
    std::cout << "Entering UI Test State" << std::endl;
    isPaused = false;
    
    // Get screen dimensions for positioning
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    int screenWidth = renderer.GetScreenWidth();
    int screenHeight = renderer.GetScreenHeight();
    
    // Clear UI manager to start fresh
    Engine::UI::UIManager::GetInstance().Clear();
    
    // Create main menu
    mainMenu = std::make_shared<Engine::UI::Menu>(
        screenWidth / 2 - 150,  // x centered
        screenHeight / 2 - 100, // y centered
        300,                    // width
        250                     // height
    );
    
    // Set menu appearance
    mainMenu->SetName("MainMenu");
    mainMenu->SetBackgroundColor(DARKBLUE);
    mainMenu->SetSelectedBackgroundColor(BLUE);
    mainMenu->SetTextColor(LIGHTGRAY);
    mainMenu->SetSelectedTextColor(WHITE);
    
    // Add menu items with callbacks
    mainMenu->AddItem("Start Game", [this]() { OnStartGame(); });
    mainMenu->AddItem("Options", [this]() { OnOptionsSelected(); });
    mainMenu->AddItem("Credits", [this]() { OnCreditsSelected(); });
    mainMenu->AddItem("Quit", [this]() { OnQuitSelected(); });
    
    // Set initial selection
    mainMenu->SetSelectedIndex(0);
    currentSelection = "No selection yet";
    
    // Add menu to UI manager
    Engine::UI::UIManager::GetInstance().AddWidget(mainMenu);
    
    // Set focus to main menu
    Engine::UI::UIManager::GetInstance().SetFocusedWidget(mainMenu.get());
}

void UITestState::Exit() {
    std::cout << "Exiting UI Test State" << std::endl;
    
    // Clear UI manager
    Engine::UI::UIManager::GetInstance().Clear();
    
    // Reset pointers
    mainMenu.reset();
}

void UITestState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update UI manager
    Engine::UI::UIManager::GetInstance().Update(deltaTime);
    
    // Handle input
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Process input through UI manager
    Engine::UI::UIManager::GetInstance().OnKeyPressed(0); // Dummy key to trigger checks
    
    // Exit on cancel
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void UITestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("UI FRAMEWORK TEST", 
                             renderer.GetScreenWidth() / 2, 
                             50, 
                             30, 
                             BLACK);
    
    // Draw current selection
    renderer.DrawTextCentered(("Last selection: " + currentSelection).c_str(), 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 100, 
                             20, 
                             DARKGRAY);
    
    // Draw controls help
    renderer.DrawTextCentered("Use UP/DOWN to navigate, ENTER to select, ESC to exit", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 50, 
                             16, 
                             DARKGRAY);
    
    // Render UI elements
    Engine::UI::UIManager::GetInstance().Render();
}

void UITestState::Pause() {
    std::cout << "Pausing UI Test State" << std::endl;
    isPaused = true;
}

void UITestState::Resume() {
    std::cout << "Resuming UI Test State" << std::endl;
    isPaused = false;
}

// Menu callbacks
void UITestState::OnStartGame() {
    currentSelection = "Start Game";
    std::cout << "Start Game selected" << std::endl;
}

void UITestState::OnOptionsSelected() {
    currentSelection = "Options";
    std::cout << "Options selected" << std::endl;
}

void UITestState::OnCreditsSelected() {
    currentSelection = "Credits";
    std::cout << "Credits selected" << std::endl;
}

void UITestState::OnQuitSelected() {
    currentSelection = "Quit";
    std::cout << "Quit selected" << std::endl;
    
    // Exit the state
    Engine::StateManager::GetInstance().PopState();
}

} // namespace Game 