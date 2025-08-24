#include "Menu.h"
#include "../core/Engine.h"
#include "../core/InputManager.h"
#include "../core/ResourceManager.h"
#include "../engine/Renderer.h"
#include "../core/Constants.h"

Menu::Menu()
    : m_engine(nullptr)
    , m_selectedItem(0)
    , m_fontTexture(nullptr)
{
}

Menu::~Menu() {
    shutdown();
}

bool Menu::initialize(Engine* engine) {
    m_engine = engine;
    
    ResourceManager* resources = ResourceManager::getInstance();
    if (resources) {
        m_fontTexture = resources->getTexture("ui/font.png");
    }
    
    setupMainMenu();
    setupPauseMenu();
    setupGameOverMenu();
    setupVictoryMenu();
    setupSettingsMenu();
    
    return true;
}

void Menu::shutdown() {
}

void Menu::handleEvent(const SDL_Event& event) {
    if (!m_engine) return;
    
    InputManager* input = m_engine->getInputManager();
    if (!input) return;
    
    if (input->isActionJustPressed(InputAction::MENU_UP)) {
        m_selectedItem = std::max(0, m_selectedItem - 1);
    } else if (input->isActionJustPressed(InputAction::MENU_DOWN)) {
        m_selectedItem = std::min(static_cast<int>(m_mainMenuItems.size()) - 1, m_selectedItem + 1);
    } else if (input->isActionJustPressed(InputAction::MENU_SELECT)) {
        if (m_selectedItem >= 0 && m_selectedItem < static_cast<int>(m_mainMenuItems.size())) {
            if (m_mainMenuItems[m_selectedItem].callback) {
                m_mainMenuItems[m_selectedItem].callback();
            }
        }
    }
}

void Menu::update(float deltaTime) {
}

void Menu::render(Renderer* renderer, GameState gameState) {
    switch (gameState) {
        case GameState::MENU:
            renderMenu(renderer, m_mainMenuItems, "SUPER MARIO WASM");
            break;
        case GameState::PAUSED:
            renderMenu(renderer, m_pauseMenuItems, "PAUSED");
            break;
        case GameState::GAME_OVER:
            renderMenu(renderer, m_gameOverMenuItems, "GAME OVER");
            break;
        case GameState::VICTORY:
            renderMenu(renderer, m_victoryMenuItems, "VICTORY!");
            break;
        case GameState::SETTINGS:
            renderMenu(renderer, m_settingsMenuItems, "SETTINGS");
            break;
        default:
            break;
    }
}

void Menu::setupMainMenu() {
    m_mainMenuItems.clear();
    
    MenuItem newGame;
    newGame.text = "NEW GAME";
    newGame.callback = []() {};
    m_mainMenuItems.push_back(newGame);
    
    MenuItem loadGame;
    loadGame.text = "CONTINUE";
    loadGame.callback = []() {};
    m_mainMenuItems.push_back(loadGame);
    
    MenuItem settings;
    settings.text = "SETTINGS";
    settings.callback = []() {};
    m_mainMenuItems.push_back(settings);
    
    MenuItem exit;
    exit.text = "EXIT";
    exit.callback = []() {};
    m_mainMenuItems.push_back(exit);
}

void Menu::setupPauseMenu() {
    m_pauseMenuItems.clear();
    
    MenuItem resume;
    resume.text = "RESUME";
    resume.callback = []() {};
    m_pauseMenuItems.push_back(resume);
    
    MenuItem restart;
    restart.text = "RESTART LEVEL";
    restart.callback = []() {};
    m_pauseMenuItems.push_back(restart);
    
    MenuItem mainMenu;
    mainMenu.text = "MAIN MENU";
    mainMenu.callback = []() {};
    m_pauseMenuItems.push_back(mainMenu);
}

void Menu::setupGameOverMenu() {
    m_gameOverMenuItems.clear();
    
    MenuItem restart;
    restart.text = "TRY AGAIN";
    restart.callback = []() {};
    m_gameOverMenuItems.push_back(restart);
    
    MenuItem mainMenu;
    mainMenu.text = "MAIN MENU";
    mainMenu.callback = []() {};
    m_gameOverMenuItems.push_back(mainMenu);
}

void Menu::setupVictoryMenu() {
    m_victoryMenuItems.clear();
    
    MenuItem continue_;
    continue_.text = "CONTINUE";
    continue_.callback = []() {};
    m_victoryMenuItems.push_back(continue_);
    
    MenuItem mainMenu;
    mainMenu.text = "MAIN MENU";
    mainMenu.callback = []() {};
    m_victoryMenuItems.push_back(mainMenu);
}

void Menu::setupSettingsMenu() {
    m_settingsMenuItems.clear();
    
    MenuItem musicVolume;
    musicVolume.text = "MUSIC VOLUME";
    musicVolume.callback = []() {};
    m_settingsMenuItems.push_back(musicVolume);
    
    MenuItem sfxVolume;
    sfxVolume.text = "SFX VOLUME";
    sfxVolume.callback = []() {};
    m_settingsMenuItems.push_back(sfxVolume);
    
    MenuItem back;
    back.text = "BACK";
    back.callback = []() {};
    m_settingsMenuItems.push_back(back);
}

void Menu::renderMenu(Renderer* renderer, const std::vector<MenuItem>& items, const std::string& title) {
    SDL_Color overlayColor = {0, 0, 0, 128};
    renderer->drawRectangle(Rectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), 
                           overlayColor, true, 100);
    
    Vector2 titlePos(Constants::SCREEN_WIDTH / 2 - title.length() * 8, 100);
    renderText(renderer, title, titlePos, {255, 255, 0, 255});
    
    for (size_t i = 0; i < items.size(); ++i) {
        SDL_Color color = (static_cast<int>(i) == m_selectedItem) ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
        
        Vector2 itemPos(Constants::SCREEN_WIDTH / 2 - items[i].text.length() * 8, 
                       200 + static_cast<int>(i) * Constants::MENU_ITEM_HEIGHT);
        
        if (static_cast<int>(i) == m_selectedItem) {
            renderText(renderer, "> " + items[i].text + " <", itemPos, color);
        } else {
            renderText(renderer, items[i].text, itemPos, color);
        }
    }
}

void Menu::renderText(Renderer* renderer, const std::string& text, const Vector2& position, SDL_Color color) {
    // Simple bitmap font rendering
    if (!m_fontTexture) {
        // Fallback to rectangle rendering for each character
        for (size_t i = 0; i < text.length(); ++i) {
            Vector2 charPos(position.x + i * 16, position.y);
            renderer->drawRectangle(Rectangle(charPos.x, charPos.y, 12, 16), color, true, 101);
        }
    }
}