#include "ScenePlatformer.h"

#include "Action.h"
#include "Assets.h"
#include "Components.h"
#include "GameEngine.h"
#include "Physics.h"
#include "SceneStartMenu.h"

#include <fstream>
#include <iostream>
#include <sstream>


ScenePlatformer::ScenePlatformer(GameEngine* game, const std::string& levelPath)
    : Scene(game), m_levelPath(levelPath)
{
    init();
}

void ScenePlatformer::init()
{
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURES");
    registerAction(sf::Keyboard::B, "TOGGLE_BBOXES");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Space, "SHOOT");
    registerAction(sf::Keyboard::Equal, "ZOOM_IN");
    registerAction(sf::Keyboard::Hyphen, "ZOOM_OUT");

    m_gridText.setFont(m_game->getAssets().getFont("Tech"));
    m_gridText.setCharacterSize(50);
    m_gridText.setScale(0.05f, 0.05f);

	sf::View view(sf::FloatRect(0, -m_viewSize.y, m_viewSize.x, m_viewSize.y));     // Top left corner of view is at (0, 0); view extends in negative y direction (upwards)
    m_game->getWindow().setView(view);

    loadLevel(m_levelPath);
}

void ScenePlatformer::loadLevel(const std::string& filename)
{
    // Overwrites previous EntityManager
    m_entityManager = EntityManager();

    // Can be Player, Block, or Prop
    std::ifstream levelFile(filename);
    if (!levelFile.is_open())
    {
        std::cerr << "ScenePlatformer.cpp, Line 60: Error opening level file." << std::endl;
        exit(-1);
    }

    std::string line;
    std::string token;

    while (std::getline(levelFile, line))
    {
        // Enables reading of strings separated by whitespace
        std::istringstream lineStream(line);

        if (lineStream >> token)
        {
            if (token == "Player")
            {
                // Read values into PlayerConfig struct
                lineStream >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.BB_WIDTH
                >> m_playerConfig.BB_HEIGHT >> m_playerConfig.X_SPEED >> m_playerConfig.JUMP_SPEED
                >> m_playerConfig.MAX_SPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;

                // Spawn player according to PlayerConfig values
                spawnPlayer();
            }
            else
            {
                std::string animationName;
                Vec2f position;
                std::shared_ptr<Entity> entity;

                lineStream >> animationName >> position.x >> position.y;

                if (token == "Solid")
                {
                    entity = m_entityManager.addEntity("Solid");
                    entity->addComponent<CBody>(m_game->getAssets().getAnimation(animationName).getSize());
                }
                else    // if Prop
                {
                    entity = m_entityManager.addEntity("Prop");
                }

                entity->addComponent<CAnimation>(m_game->getAssets().getAnimation(animationName), true);    // IMPORTANT: add CAnimation first so gridToMidPixel can compute correctly
                entity->addComponent<CTransform>(gridToMidPixel(position, entity));
            }
        }
    }

    levelFile.close();
}

void ScenePlatformer::update()
{
    if (!m_paused)
	{
        // Adds and removes entities while avoiding iterator invalidation
        m_entityManager.update();

        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
	}

    sRender();

    m_currentFrame++;
}

void ScenePlatformer::endScene()
{
    m_game->changeScene("MENU", std::make_shared<SceneStartMenu>(m_game));
}

void ScenePlatformer::sDoAction(const Action& action)
{
    const std::string& actionName = action.getName();

    if (action.getType() == "START")
    {
        // Check for pausing or quitting
        if (actionName == "PAUSE")
        {
            m_paused = !m_paused;
        }
        else if (actionName == "QUIT")
        {
            endScene();
        }

        // If paused, don't allow player input or toggling
        if (m_paused)
        {
            return;
        }

        if (m_player->hasComponent<CInput>())
        {
            // Handle player input and return early to decrease latency
            if (actionName == "UP")
            {
                m_player->getComponent<CInput>().up = true;
                return;
            }
            else if (actionName == "DOWN")
            {
                m_player->getComponent<CInput>().down = true;
                return;
            }
            else if (actionName == "LEFT")
            {
                m_player->getComponent<CInput>().left = true;
                return;
            }
            else if (actionName == "RIGHT")
            {
                m_player->getComponent<CInput>().right = true;
                return;
            }
            else if (actionName == "SHOOT")
            {
                if (m_player->getComponent<CInput>().canShoot)
                {
                    m_player->getComponent<CState>().currentState = PlayerState::Shooting;      // Change state, and hence animation
                    spawnBullet(m_player);
                    m_player->getComponent<CInput>().canShoot = false;                          // Can't shoot again until SHOOT released
                }
                
                return;
            }
        }

        // Toggle actions
        if (actionName == "TOGGLE_TEXTURES")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (actionName == "TOGGLE_BBOXES")
        {
            m_drawBoundingBoxes = !m_drawBoundingBoxes;
        }
        else if (actionName == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }
        // Handle view manipulation actions
        else if (actionName == "ZOOM_IN")
        {
            m_viewSize.y -= 20;
            m_viewSize.x = m_viewSize.y * m_game->getAspectRatio();
        }
        else if (actionName == "ZOOM_OUT")
        {
            m_viewSize.y += 20;
            m_viewSize.x = m_viewSize.y * m_game->getAspectRatio();
        }
    }
    else if (action.getType() == "END")
    {
        if (m_player->hasComponent<CInput>())
        {
            if (actionName == "UP")
            {
                m_player->getComponent<CInput>().up = false;
            }
            else if (actionName == "DOWN")
            {
                m_player->getComponent<CInput>().down = false;
            }
            else if (actionName == "LEFT")
            {
                m_player->getComponent<CInput>().left = false;
            }
            else if (actionName == "RIGHT")
            {
                m_player->getComponent<CInput>().right = false;
            }
            else if (actionName == "SHOOT")
            {
                m_player->getComponent<CInput>().canShoot = true;
            }
        }
    }
}

void ScenePlatformer::sMovement()
{
    // For each entity in scene, handle movement according to entity's velocity
    for (auto entity : m_entityManager.getEntities())
    {
        auto& transform = entity->getComponent<CTransform>();

        if (entity == m_player)
        {
            auto& input = m_player->getComponent<CInput>();
            auto& state = m_player->getComponent<CState>();

            // Change velocity and animation direction according to input
            if (input.right && !input.left)
            {
                transform.velocity.x = m_playerConfig.X_SPEED;
            }
            else if (input.left && !input.right)
            {
                transform.velocity.x = -m_playerConfig.X_SPEED;
            }
            else
            {
                transform.velocity.x = 0;
            }

            // Jump if on ground and W pressed
            if (input.up && input.canJump)
			{
				transform.velocity.y = m_playerConfig.JUMP_SPEED;
				input.canJump = false;
			}

            // Set CState according to horizontal velocity -> changes animation set in sAnimation()
            if (transform.velocity.x != 0)
            {
                state.currentState = PlayerState::Running;

                 // Flip animation in X direction if mismatch between velocity and orientation of animation in X
                if ((transform.velocity.x < 0 && transform.scale.x < 0) ||
                    (transform.velocity.x > 0 && transform.scale.x > 0))
                {
                    transform.scale.x *= -1;
                }
            }
            else if (state.currentState != PlayerState::Shooting)
            {
                state.currentState = PlayerState::Standing;     // No change to direction when velocity is 0
            }
            
            // Falling animation overrides running or standing animations
            if (transform.velocity.y != 0)
            {
                state.currentState = PlayerState::InAir;
            }
        }

		// Apply gravity if Entity has CGravity
        if (entity->hasComponent<CGravity>())
		{
	        transform.velocity.y -= entity->getComponent<CGravity>().acceleration;
		}

        // Cap velocity in both directions; necessary in y direction to ensure no moving through floor
        transform.velocity.x = std::min(transform.velocity.x, m_playerConfig.MAX_SPEED);
        transform.velocity.y = std::min(transform.velocity.y, m_playerConfig.MAX_SPEED);

        // Update Entity's position according to velocity
        transform.previousPosition = transform.position;
        transform.position += transform.velocity;
    }
}

void ScenePlatformer::sLifespan()
{
    for (auto& entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CLifespan>())
        {
            const auto& lifespanComponent = entity->getComponent<CLifespan>();
            
            if (m_currentFrame - lifespanComponent.frameCreated > lifespanComponent.framesDuration)
            {
                entity->destroy();
            }
        }
    }
}

void ScenePlatformer::sCollision()
{
    // TODO: Implement player/tile collisions and resolutions
    // TODO: Implement bullet/tile collisions
        // Destroy tile if it has Brick animation

    EntityVector& entities = m_entityManager.getEntities();

    for (std::shared_ptr<Entity> a : entities)
    {
        if (!a->hasComponent<CBody>())
        {
            continue;
        }

        for (std::shared_ptr<Entity> b : entities)
        {
            if (!b->hasComponent<CBody>() || a == b)
            {
                continue;
            }

            // If overlap in both x and y directions, resolve collision
            if (a == m_player)
            {
                Vec2f overlap = Physics::getOverlap(a, b);
                if (overlap.x > 0 && overlap.y > 0)
                {
                    Physics::resolveCollision(a, b);
                }
            }
        }
    }

    // Restrict player from moving off left of map
    Vec2f& posPlayer = m_player->getComponent<CTransform>().position;
    posPlayer.x = std::max(m_player->getComponent<CBody>().bBox.size.x / 2.f, posPlayer.x);

    // Restart level if player has fallen down hole
    if (posPlayer.y < 0)
    {
        loadLevel(m_levelPath);
    }
}

void ScenePlatformer::sAnimation()
{
    for (auto& entity : m_entityManager.getEntities())
    {
        // If entity has Animation component, either update it, or remove if has ended
        if (entity->hasComponent<CAnimation>())
        {
            CAnimation& animComponent = entity->getComponent<CAnimation>();

            animComponent.update();

            if (entity == m_player)
            {
                updatePlayerAnimation(animComponent);
            }
            else      // if entity is non-player
            {
                updateEntityAnimation(entity, animComponent);
            }
        }
    }
}

void ScenePlatformer::sRender()
{
    sf::RenderWindow& window = m_game->getWindow();

    // Different colour background to indicate game paused
    if (m_paused)
    {
        window.clear(sf::Color(50, 50, 150));          // Dark blue
    }
    else
    {
        window.clear(sf::Color(100, 100, 255));        // Indigo 
    }

    // Establishes variables for centring view
    sf::View view = window.getView();
    view.setSize(m_viewSize);
    const Vec2f& playerPosition = m_player->getComponent<CTransform>().position;

    // Centres view on player if further to right than middle of screen
	float newViewCentreX = std::max(m_viewSize.x / 2.f, playerPosition.x);        // Ensures view doesn't exceed left side of level; to stop going off right side, use std::min of this and level width
	view.setCenter(newViewCentreX, view.getCenter().y);
    window.setView(view);

    // Drawing of textures/animations and bounding boxes
    EntityVector& entities = m_entityManager.getEntities();
    for (auto entity : entities)
    {
        renderEntity(entity);
    }

	// Check whether player exists
    auto playerIterator = std::find_if(entities.begin(), entities.end(), [](std::shared_ptr<Entity> e)
    {
        return e->getTag() == "Player"; 
    });

	// If player exists, render it last
    if (playerIterator != entities.end())
    {
        renderEntity(*playerIterator);
    } 

	// Render bounding boxes if enabled; in a separate loop to ensure they are drawn on top of textures
    for (auto entity : entities)
    {
        if (m_drawBoundingBoxes && entity->hasComponent<CBody>())
        {
            renderBBox(entity);
        }
    }

    // Draw grid for debugging
    if (m_drawGrid)
    {
		renderGrid(window, view);
    }

    window.display();
}

void ScenePlatformer::spawnPlayer()
{
    // Add Player entity and add components according to playerConfig struct
    m_player = m_entityManager.addEntity("Player");
    
    m_player->addComponent<CInput>();
    m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(Vec2f(m_playerConfig.X, m_playerConfig.Y), m_player));       // Adding CTransform must follow adding CAnimation because gridToMidPixel uses CAnimation
    m_player->addComponent<CBody>(Vec2f(m_playerConfig.BB_WIDTH, m_playerConfig.BB_HEIGHT), 1.f);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CState>();
}

void ScenePlatformer::spawnBullet(std::shared_ptr<Entity> entity)
{
    const float bulletSpeed = 4.f;
    const int framesAlive = 30;

    // Create entity
    std::shared_ptr<Entity> bullet = m_entityManager.addEntity("Bullet");

    // For CAnimation, CBody
    const auto& animationBullet = m_game->getAssets().getAnimation("Bullet");
    
    // For CTransform
    Vec2f positionEntity = entity->getComponent<CTransform>().position;
    bool isFacingLeft = entity->getComponent<CTransform>().scale.x > 0;
    float widthEntity = entity->getComponent<CAnimation>().animation.getSize().x;
    float widthBullet = animationBullet.getSize().x;
    Vec2f velocity = isFacingLeft ? Vec2f(-bulletSpeed, 0.f) : Vec2f(bulletSpeed, 0.f);
    float offsetX = isFacingLeft ? (-widthEntity / 2.f) - (widthBullet / 2.f) : (widthEntity / 2.f) + (widthBullet / 2.f);

    // Add components with pre-calculated parameters
    bullet->addComponent<CAnimation>(animationBullet, true);
    bullet->addComponent<CBody>(animationBullet.getSize());
    bullet->addComponent<CTransform>(positionEntity + Vec2f(offsetX, 0.f), velocity, 0.f);
    bullet->getComponent<CTransform>().scale = isFacingLeft ? Vec2f(-1.f, 1.f) : Vec2f(1.f, 1.f);
    bullet->addComponent<CLifespan>(framesAlive, m_currentFrame);
}

// Used to position of entity so bottom left of sprite is at bottom left of the cell
Vec2f ScenePlatformer::gridToMidPixel(const Vec2f& gridPosition, std::shared_ptr<Entity> entity)
{
    if (!entity->hasComponent<CAnimation>())
    {
        std::cerr << "ScenePlatformer.cpp: entity has no CAnimation." << std::endl;
        return Vec2f(0.f, 0.f);
    }

    const Vec2f& spriteSize = entity->getComponent<CAnimation>().animation.getSize();
    Vec2f offset = (gridPosition * Vec2f(m_gridCellSize)) + (spriteSize / 2.f);

    return offset;
}

void ScenePlatformer::renderEntity(std::shared_ptr<Entity> e)
{
    // Draw Entity textures/animations
    if (m_drawTextures && e->hasComponent<CAnimation>())
    {
        auto& transform = e->getComponent<CTransform>();
        auto& animationSprite = e->getComponent<CAnimation>().animation.getSprite();
        animationSprite.setPosition(transform.position.x, -transform.position.y);
		animationSprite.setScale(transform.scale.x, transform.scale.y);
        animationSprite.setRotation(transform.angle);
        m_game->getWindow().draw(animationSprite);
    }
}

void ScenePlatformer::renderGrid(sf::RenderWindow& window, const sf::View& view)
{
    float leftEdgeX = view.getCenter().x - (m_viewSize.x / 2);                                      // Left edge of viewable area
    float rightEdgeX = leftEdgeX + m_viewSize.x + m_gridCellSize.x;                                 // Right edge of viewable area - width of a cell is added to ensure full coverage
    float firstVertLineX = leftEdgeX - (static_cast<int>(leftEdgeX) % m_gridCellSize.x);            // X position of leftmost viewable cell starting just outside of window
    
    sf::Vector2f viewSize = view.getSize();

    float topEdgeY = view.getCenter().y - (m_viewSize.y / 2.f);                                     // Top of viewable area
    float bottomEdgeY = view.getCenter().y + (m_viewSize.y / 2.f) + m_gridCellSize.y;               // Bottom of viewable area; height of a cell added to ensure full coverage
	float firstHorizontalLineY = topEdgeY + (-static_cast<int>(topEdgeY) % m_gridCellSize.y);        // Y position of topmost cell starting just outside of window

    sf::VertexArray lines(sf::Lines);

    // Add verticle lines to vertex array
    for (float x = firstVertLineX; x < rightEdgeX; x += m_gridCellSize.x)
    {
        lines.append(sf::Vertex(sf::Vector2f(x, bottomEdgeY)));
        lines.append(sf::Vertex(sf::Vector2f(x, topEdgeY)));
    }

    // Draw horizontal lines
    for (float y = firstHorizontalLineY; y < bottomEdgeY; y += m_gridCellSize.y)
    {
        lines.append(sf::Vertex(sf::Vector2f(leftEdgeX, y)));
        lines.append(sf::Vertex(sf::Vector2f(rightEdgeX, y)));

        // Draw coordinate text for each cell in row
        for (float x = firstVertLineX; x < rightEdgeX; x += m_gridCellSize.x)
        {
            int gridX = static_cast<int>(x) / m_gridCellSize.x;
            int gridY = -static_cast<int>(y) / m_gridCellSize.y;

            m_gridText.setString("(" + std::to_string(gridX) + ", " + std::to_string(gridY) + ")");
            m_gridText.setPosition(x, y - m_gridCellSize.y);
            window.draw(m_gridText);
        }
    }

    window.draw(lines);
}

void ScenePlatformer::renderBBox(std::shared_ptr<Entity> entity)
{
    auto& bBox = entity->getComponent<CBody>().bBox;
    auto& transform = entity->getComponent<CTransform>();

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(bBox.size.x - 1, bBox.size.y - 1));  // Only takes sf::Vector2f
    rectangle.setOrigin(bBox.size.x / 2.f, bBox.size.y / 2.f);
    rectangle.setPosition(transform.position.x, -transform.position.y);
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));                  // Sets alpha of fill colour to 0 i.e. transparent
    rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));       // Sets alpha of outline to 255 i.e. opaque
    rectangle.setOutlineThickness(1);
    m_game->getWindow().draw(rectangle);
}

void ScenePlatformer::updatePlayerAnimation(CAnimation& animComponent)
{
    CState& stateComponent = m_player->getComponent<CState>();

    // Default to standing animation if previous animation ended and wasn't to repeat
    if (animComponent.animation.hasEnded() && !animComponent.toRepeat)
    {
        stateComponent.currentState = PlayerState::Standing;
    }

    // Update CAnimation if state has changed
    if (stateComponent.currentState != stateComponent.previousState)
    {
        bool toRepeat = stateComponent.currentState != PlayerState::Shooting;

        const std::string& animationName = m_stateToAnimationMap[stateComponent.currentState];
        animComponent = m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation(animationName), toRepeat);

        stateComponent.previousState = stateComponent.currentState;
    }
}

void ScenePlatformer::updateEntityAnimation(std::shared_ptr<Entity> entity, CAnimation& animComponent)
{
    if (animComponent.animation.hasEnded() && !animComponent.toRepeat)
    {
        entity->removeComponent<CAnimation>();
    }
}