#include <iostream>

#include "Action.h"
#include "Assets.h"
#include "Components.h"
#include "GameEngine.h"
#include "Physics.h"
#include "Scene_Level.h"

ScenePlatformer::ScenePlatformer(GameEngine* game, const std::string& levelPath)
    : Scene(game), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void ScenePlatformer::init(const std::string& levelPath)
{
    // TODO: register all other gameplay actions

    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle visible bounding boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    registerAction(sf::Keyboard::W, "UP");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Tech"));

    loadLevel(levelPath);
}

void ScenePlatformer::update()
{
    m_entityManager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    sAnimation();
    sRender();
}

void ScenePlatformer::loadLevel(const std::string& filename)
{
    // Overwrites previous EntityManager
    m_entityManager = EntityManager();

    // TODO: read in level file and add appropriate entities to entity manager -> will automatically be drawn to screen
        // Add correct bounding boxes to Tile entities, no bounding boxes to Dec entities
    // Use playerConfig struct m_playerConfig to store player properties
    // This struct is defined at top of ScenePlatformer.h

    // NOTE: all of code below is sample code which shows you how to set up and use entities with the new syntax -> should be removed

    spawnPlayer();

    // some sample entities
    auto brick = m_entityManager.addEntity("tile");
    // IMPORTANT: always add CAnimatio component first so gridToMidPixel can compute correctly
    brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
    brick->addComponent<CTransform>(Vec2(96, 480));
    // NOTE: final code should position entity with grid x,y position read from file:
    // brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick));

    if (brick->getComponent<CAnimation>().animation.getName() == "Brick")
    {
        std::cout << "This could be good way of identifying if a tile is a brick" << std::endl;
    }

    auto question = m_entityManager.addEntity("tile");
    question->addComponent<CAnimation>(m_game->assets().getAnimation("Question"), true);
    question->addComponent<CTransform>(Vec2(352, 480));

    // NOTE - THIS IS IMPORTANT, READ THIS SAMPLE
        // Components are now returned as references, not pointers.
        // If you do not specify a reference variable type, it will only copy.
        //The follow is an example:
            // auto transform1 = entity->getComponent<CTransform>()
            // transform one is a copy
            // auto& transform2 = entity->getComponent<CTransform>()
            // This is correct
}

void ScenePlatformer::endScene()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void ScenePlatformer::sMovement()
{
    // TODO: Implement player movemement/jumping based on its CInput component
        // Use scales of -1/1 to look left/right
    // TODO: Implement gravity's effect on player
    // TODO: Implement max player speed in both X and Y directions

    // Update player velocity according to input
    Vec2 velocity(0, m_player->getComponent<CTransform>().velocity.y);
    if (m_player->getComponent<CInput>().up)
    {
        // m_player.getComponent<CState>().state = "jumping";      // Used for changing player animation
        velocity.y = -3;
    }
    m_player->getComponent<CTransform>().velocity = velocity;

    // For each entity in scene, handle movement according to velocities
    for (auto entity : m_entityManager.getEntities())   // Why isn't this auto&? Are they already references?
    {
        /*
        auto& velocity = entity->getComponent<CTransform>().velocity;

        if (entity->hasComponent<CGravity>())
        {
            velocity.y += entity->getComponent<CGravity>().acceleration;
        }

        // Should cap velocity in all directions to ensure no moving throughfloors
        if (velocity.y > m_playerConfig.MAX_SPEED)
        {
            velocity.y = m_playerConfig.MAX_SPEED;
        }
        if (velocity.x > m_playerConfig.MAX_SPEED)
        {
            velocity.x = m_playerConfig.MAX_SPEED;
        }
        */

        // Update Entity's position according to velocity
        entity->getComponent<CTransform>().position += velocity;
    }
}

void ScenePlatformer::sLifespan()
{
    // TODO: Check lifespan of entities that have them, and destroy if they go over
}

void ScenePlatformer::sCollision()
{
    // TODO: Implement Physics::getOverlap() to use inside this function

    // TODO: Implement bullet/tile collisions
        // Destroy tile if it has Brick animation
    // TODO: Implement player/tile collisions and resolutions
        // Update the CState component of player to store whether it's currently on ground or in air; this will be used by Animation system
    // TODO: Check to see if player has fallen down hole i.e. y > height
    // TODO: Don't let player walk off left side of map
}

void ScenePlatformer::sActions(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.getName() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.getName() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
        else if (action.getName() == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }
        else if (action.getName() == "PAUSE")
        {
            setPaused(!m_paused);   // What does this line mean?
        }
        else if (action.getName() == "QUIT")
        {
            endScene();
        }
        else if (action.getName() == "UP")
        {
            m_player->getComponent<CInput>.up = true;
        }
        else if (action.getName() == "DOWN")
        {
            m_player->getComponent<CInput>.down = true;
        }
        else if (action.getName() == "LEFT")
        {
            m_player->getComponent<CInput>.left = true;
        }
        else if (action.getName() == "RIGHT")
        {
            m_player->getComponent<CInput>.right = true;
        }
    }
    else if (action.getType() == "END")
    {
        if (action.getName() == "UP")
        {
            m_player->getComponent<CInput>.up = false;
        }
        else if (action.getName() == "DOWN")
        {
            m_player->getComponent<CInput>.up = false;
        }
        else if (action.getName() == "LEFT")
        {
            m_player->getComponent<CInput>.up = false;
        }
        else if (action.getName() == "RIGHT")
        {
            m_player->getComponent<CInput>.up = false;
        }
    }
}

void ScenePlatformer::sAnimation()
{
    // TODO: Complete Animation class code first

    // Set player animation based on state
    if (m_player->getComponent<CState>().state == "air")
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"));   // Look at constructor params
    }

    // TODO: set animation of the player based on its CState component
    // TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
        // If the animation is not repeated, and it has ended, destroy the entity
}

void ScenePlatformer::sRender()
{
    // Different colour background to indicate game paused
    if (!m_paused)
    {
        m_game->getWindow().clear(sf::Color(100, 100, 255));        // Indigo 
    }
    else
    {
        m_game->getWindow().clear(sf::Color(50, 50, 150));          // Dark blue
    }

    // Centres view on player if further to right than middle of screen
    auto& playerPosition = m_player->getComponent<CTransform>.position;
    float windowCentreX = std::max(m_game->getWindow().getSize().x / 2.f, playerPosition.x);
    sf::View view = m_game->getWindow().getView();
    view.setCenter(windowCentreX, view.getCenter().y);    // This was m_game->getWindow().getSize().y - view.getCenter().y -> used to mirror the view in the vertical midline
    m_game->getWindow().setView(view);

    // Drawing of textures/animations and bounding boxes
    for (auto entity : m_entityManager.getEntities())
    {
        // Draw Entity textures/animations
        if (m_drawTextures && entity->hasComponent<CAnimation>())
        {
            auto& transform = entity->getComponent<CTransform>();
            auto& animation = entity->getComponent<CAnimation>().animation;
            animation.getSprite().setPosition(transform.position.x, transform.position.y);
            animation.getSprite().setScale(transform.scale.x, transform.scale.y);
            animation.getSprite().setRotation(transform.angle);
            m_game->getWindow().draw(animation.getSprite());
        }

        // Draw Entity bounding boxes
        if (m_drawCollisions && entity->hasComponent<CBoundingBox>())
        {
            auto& bBox = entity->getComponent<CBoundingBox>();
            auto& transform = entity->getComponent<CTransform>();
            
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(bBox.size.x - 1, bBox.size.y - 1));  // Only takes sf::Vector2f
            rectangle.setOrigin(bBox.halfSize.x, bBox.halfSize.y);
            rectangle.setPosition(transform.position.x, transform.position.y);      
            rectangle.setFillColor(sf::Color(0, 0, 0, 0));                  // Sets alpha of fill colour to 0 i.e. transparent
            rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));       // Sets alpha of outline to 255 i.e. opaque
            rectangle.setOutlineThickness(1);
            m_game->getWindow().draw(rectangle);
        }
    }

    // Draw grid for debugging
    if (m_drawGrid)
    {
        float leftEdgeX = m_game->getWindow().getView().getCenter().x - getWidth() / 2;     // Left edge of viewable area
        float rightEdgeX = leftEdgeX + getWidth() + m_gridCellSize.x;                        // Right edge of viewable area
        float nextGridX = leftEdgeX - (static_cast<int>(leftEdgeX) % static_cast<int>(m_gridCellSize.x));

        for (float x = nextGridX; x < rightEdgeX; x += gridCellSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, getHeight()));
        }

        for (float y = 0; y < getHeight(); y += m_gridCellSize.y)
        {
            drawLine(Vec2(leftEdgeX, getHeight() - y), Vec2(rightEdgeX, getHeight() - y));

            for (float x = nextGridX; x < rightEdgeX; x += m_gridCellSize.x)
            {
                std::string xCell = std::to_string(static_cast<int>(x) / static_cast<int>(m_gridCellSize.x));
                std::string yCell = std::to_string(static_cast<int>(y) / static_cast<int>(m_gridCellSize.y));
                m_gridText.setString("(" + xCell + ", " + yCell + ")");
                m_gridText.setPosition(x + 3, getHeight() - y - m_gridSize.y + 2);
                m_game->getWindow().draw(m_gridText);
            }
        }
    }

    window.display();
}

void ScenePlatformer::spawnPlayer()
{
    // Read player config from level file and spawn player (this is where player should restart after death)

    // sample player entity which you can use to construct other entities
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(Vec2(224, 352));
    m_player->addComponenet<CBoundingBox>(Vec2(48, 48));
    m_player->addComponnent<CGravity>(0.1);

    // TODO: add remaining components to player
}

void ScenePlatformer::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: this should spawn a bullet at a given entity, going in direction entity is facing
}

Vec2 ScenePlatformer::gridToMidPixel(float gridPositionX, float gridPositionY, std::shared_ptr<Entity> entity)
{
    // TODO: Takes in a grid position where the bottom left of the entity is, and an Entity object
    // Returns Vec2 indicating the actual centre position of the entity (not a grid position)
    // Must use Entity's animation size
    // Grid width and height is stored in m_gridCellSize
    // Bottom left corner of animation should align with the bottom left of the grid cell

    return Vec2(0, 0);
}