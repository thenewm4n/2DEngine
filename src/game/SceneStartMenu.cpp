#include "SceneStartMenu.h"
#include "SceneLevelEditor.h"
#include "ScenePlatformer.h"

#include <fstream>
#include <sstream>


SceneStartMenu::SceneStartMenu(GameEngine* gameEngine) :
    Scene(gameEngine),
    m_title(m_game->getTitle()),
    m_menuText(sf::Text(m_game->getAssets().getFont("Tech"), "", 150))
{
    init();
}

void SceneStartMenu::init()
{
    std::ifstream levelsTextFile(m_game->getExecutableDir() / "assets/levels/levels.txt");
    std::string line;
    std::string token;

    while (std::getline(levelsTextFile, line))
    {
        std::istringstream lineStream(line);
        
        LevelData level;
        if (lineStream >> level.name >> level.path)
        {
            m_levels.emplace_back(level);
        }
    }

    Menu mainMenu{ m_title, {"Level Select", "Exit"} };
    m_menuStack.push(mainMenu);

    registerAction(sf::Keyboard::Key::W, "UP");
    registerAction(sf::Keyboard::Key::S, "DOWN");
    registerAction(sf::Keyboard::Key::Enter, "SELECT");
    registerAction(sf::Keyboard::Key::Grave, "EDIT_LEVEL");
    registerAction(sf::Keyboard::Key::Escape, "ESCAPE");

    m_menuText.setOutlineColor(sf::Color::Black);
    m_menuText.setOutlineThickness(1.5f);

    sf::View view(sf::FloatRect({ 0.0f, 0.0f }, {1920.0f, 1080.0f}));     // View size is hardcoded, as to be consistent after window resizing
    m_game->getWindow().setView(view);
}

void SceneStartMenu::update()
{
    sRender();
}

void SceneStartMenu::sPerformAction(const Action& action)
{
    Menu& currentMenu = m_menuStack.top();
    const std::string& actionName = action.getName();

    if (action.getType() == "START")
    {
        if (actionName == "UP")
        {
            if (currentMenu.selectedIndex > 0)
            {
                currentMenu.selectedIndex--;
            }
        }
        else if (actionName == "DOWN")
        {
            if (currentMenu.selectedIndex < currentMenu.options.size() - 1)
            {
                currentMenu.selectedIndex++;
            }
        }
        else if (actionName == "SELECT" || actionName == "EDIT_LEVEL")
        {
            const std::string& selectedString = currentMenu.options[currentMenu.selectedIndex];

            // Menu-agnostic options
            if (selectedString == "Back")
            {
                m_menuStack.pop();
                return;
            }
            else if (selectedString == "Exit")
            {
                m_game->quit();
                return;
            }

            // Menu-specific options
            if (currentMenu.title == m_title)
            {
                if (selectedString == "Level Select")
                {
                    std::vector<std::string> levelStrings;
                    for (size_t i = 0; i < m_levels.size(); i++)
                    {
                        levelStrings.push_back(m_levels[i].name);
                    }
                    levelStrings.push_back("Back");

                    Menu levelSelectMenu("Level Select", levelStrings);
                    m_menuStack.push(levelSelectMenu);
                }
            }
            else if (currentMenu.title == "Level Select")
            {
                const std::filesystem::path levelPath =
                    m_game->getExecutableDir() /
                    "assets" /
                    "levels" /
                    m_levels[currentMenu.selectedIndex].path;

                std::shared_ptr<Scene> levelScene;

                if (actionName == "SELECT")
                    levelScene = std::make_shared<ScenePlatformer>(m_game, levelPath);
                else                // If editing level
                    levelScene = std::make_shared<SceneLevelEditor>(m_game, levelPath);

                m_game->changeScene(m_levels[currentMenu.selectedIndex].name, levelScene);
            }
        }
        else if (actionName == "ESCAPE")
        {
            if (currentMenu.title == m_title)
            {
                m_game->quit();
                return;
            }
            else
            {
                m_menuStack.pop();
                return;
            }
        }
    }
}

void SceneStartMenu::sRender()
{
    sf::RenderWindow& window = m_game->getWindow();
    float midScreenX = window.getView().getSize().x / 2.0f;
    const Menu& currentMenu = m_menuStack.top();
    float distanceBetweenStrings = window.getView().getSize().y / static_cast<float>(currentMenu.options.size() + 2);

    // Clear screen with the background colour
    window.clear(sf::Color(235, 81, 96));

    // Set title text properties
    m_menuText.setString(currentMenu.title);
    m_menuText.setCharacterSize(100);
    m_menuText.setFillColor(sf::Color(7, 16, 19));

    // Set origin to center of text after changing string and character size, then set position with new origin
    sf::FloatRect textRect = m_menuText.getLocalBounds();
    m_menuText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
    m_menuText.setPosition({midScreenX, distanceBetweenStrings});

    // Draw title text
    window.draw(m_menuText);

    // Set character size for selectable menu text
    m_menuText.setCharacterSize(80);

    // Draw selectable menu text
    for (size_t i = 0; i < currentMenu.options.size(); i++)
    {
        m_menuText.setString(currentMenu.options[i]);

        // Set origin according to new string, then set position with new origin
        textRect = m_menuText.getLocalBounds();
        m_menuText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_menuText.setPosition({midScreenX, (i + 2) * distanceBetweenStrings});

        if (i == currentMenu.selectedIndex)
        {
            m_menuText.setFillColor(sf::Color(183, 153, 156));
        }
        else
        {
            m_menuText.setFillColor(sf::Color(223, 224, 226));
        }

        window.draw(m_menuText);
    }

    window.display();
}

void SceneStartMenu::endScene()
{
    m_game->quit();
}