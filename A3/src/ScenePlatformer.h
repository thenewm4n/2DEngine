#pragma once

#include "EntityManager.h"
#include "Scene.h"

#include <memory>


class ScenePlatformer : public Scene
{
    struct PlayerConfig
    {
        float X = 0.0f, Y = 0.0f, BB_WIDTH = 1.0f, BB_HEIGHT = 1.0f, X_SPEED = 0.0f, JUMP_SPEED = 0.0f, MAX_SPEED = 10.0f, GRAVITY = 2.0f;
        std::string WEAPON = "NONE";
    };

public:
    ScenePlatformer(GameEngine* game, const std::string& levelPath);

private:
    std::unordered_map<PlayerState, std::string> m_stateToAnimationMap =
    {
        {PlayerState::Running, "Run"},
        {PlayerState::Standing, "Stand"},
        {PlayerState::InAir, "Air"},
        {PlayerState::Shooting, "Shoot"}
    };

    std::string m_levelPath;
    std::shared_ptr<Entity> m_player;
    PlayerConfig m_playerConfig;
    EntityManager m_entityManager;
    sf::Vector2f m_viewSize{384.f, 216.f};          // Was 320.f, 180.f on Mac, 384.f, 216.f on PC
    const Vec2i m_gridCellSize{16, 16};
    sf::Text m_gridText;
    bool m_drawBoundingBoxes = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;

    void init();
    void loadLevel(const std::string& filename);
    void update() override;
    void endScene() override;

    void sDoAction(const Action& action) override;
    void sMovement();
    void sLifespan();
    void sCollision();
    void sAnimation();
    void sRender() override;

    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
    
    Vec2f gridToMidPixel(const Vec2f& gridPosition, std::shared_ptr<Entity> entity);
    void renderEntity(std::shared_ptr<Entity> e);
    void renderGrid(sf::RenderWindow& window, const sf::View& view);
    void renderBBox(std::shared_ptr<Entity> entity);

    void updatePlayerAnimation(CAnimation& animComponent);
    void updateEntityAnimation(std::shared_ptr<Entity> entity, CAnimation& animComponent);
};