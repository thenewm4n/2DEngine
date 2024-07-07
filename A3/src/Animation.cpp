#include "Animation.h"

#include <cmath>

Animation::Animation() {}

Animation::Animation(const std::string& animationName, const sf::Texture& tex, int frameCount, int frameDuration)
    : m_name(animationName),
    m_sprite(tex),
    m_frameCount(frameCount),
    m_frameDuration(frameDuration),
    m_currentAnimationFrame(0),
    m_gameLoopsSinceStart(0)
{
    m_size = Vec2f(static_cast<float>(tex.getSize().x) / frameCount, static_cast<float>(tex.getSize().y));
    m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);                                                                 // Origin set to centre of sprite
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentAnimationFrame * m_size.x), 0, m_size.x, m_size.y));        // left, top, width, height
}

// Updates animation to show next frame, depending on its speed, then loops when reaches end
void Animation::update()
{
    // Calculate correct frame to display
    m_currentAnimationFrame = (m_gameLoopsSinceStart - (m_gameLoopsSinceStart % m_frameDuration)) / m_frameDuration;        // for standing animation, this results in 0 / 1

    // Reset animation frame to start (this should only happen if toRepeat == true, as checked in ScenePlatformer::sAnimation())
    if (m_currentAnimationFrame > m_frameCount - 1)
    {
        m_currentAnimationFrame = 0;
        m_gameLoopsSinceStart = 0;
    }
    else
    {
        // Add the speed variable to the current frame (?)
        m_gameLoopsSinceStart++;
    }

    // Set texture rectangle properly (see constructor for sample)
    sf::IntRect frameRect(std::floor(m_currentAnimationFrame * m_size.x), 0, m_size.x, m_size.y);

    m_sprite.setTextureRect(frameRect);
     
}

bool Animation::hasEnded() const
{
    if (m_currentAnimationFrame > m_frameCount - 1)
    {
        return true;
    }

    return false;
}

bool Animation::isFlipped() const
{
    return m_flippedX;
}

const std::string& Animation::getName() const { return m_name; }

const Vec2f& Animation::getSize() const { return m_size; }

sf::Sprite& Animation::getSprite() { return m_sprite; }