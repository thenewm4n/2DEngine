#include "Physics.h"

#include "Components.h"

// Anonymous namespace -> essentially "private" helper function
namespace
{
    // Helper function to avoid code repetition
    Vec2f getOverlapHelper(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b, bool usePrevious)
    {
        if (a->hasComponent<CBody>() && b->hasComponent<CBody>() && a->hasComponent<CTransform>() && b->hasComponent<CTransform>())
        {
            const Vec2f positionA = usePrevious ? a->getComponent<CTransform>().previousPosition : a->getComponent<CTransform>().position;
            const Vec2f positionB = usePrevious ? b->getComponent<CTransform>().previousPosition : b->getComponent<CTransform>().position;
            
            const Vec2f halfSizeA = a->getComponent<CBody>().bBox.size / 2.0f;
            const Vec2f halfSizeB = b->getComponent<CBody>().bBox.size / 2.0f;

            Vec2f overlap;

            float diffX = std::abs(positionA.x - positionB.x);
            overlap.x = halfSizeA.x + halfSizeB.x - diffX;
            
            float diffY = std::abs(positionA.y - positionB.y);
            overlap.y = halfSizeA.y + halfSizeB.y - diffY;

            return overlap;
        }

        std::cerr << "Physics.cpp: one or both entities lack a CBody or CTransform." << std::endl;
        return Vec2f(0.0f, 0.0f);
    }

    // Axis-agnostic move function to avoid repeated code
    // Moves entity a out of entity b
    void moveEntity(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b, bool isXDirection)
    {
        float& positionA = isXDirection ? a->getComponent<CTransform>().position.x : a->getComponent<CTransform>().position.y;
        float& prevPositionA = isXDirection ? a->getComponent<CTransform>().previousPosition.x : a->getComponent<CTransform>().previousPosition.y;
        float positionB = isXDirection ? b->getComponent<CTransform>().position.x : b->getComponent<CTransform>().position.y;
        float overlap = isXDirection ? Physics::getOverlap(a, b).x : Physics::getOverlap(a, b).y;

        // Move player in correct direction
        int direction = positionA > positionB ? 1 : -1;
        positionA += direction * overlap;

        // Previous position for the next frame is now the position after resolution
        prevPositionA = positionA;
    }
}


// "Public" functions
namespace Physics
{
    Vec2f getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
    {
        return getOverlapHelper(a, b, false);
    }

    Vec2f getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
    {
        return getOverlapHelper(a, b, true);
    }

    bool resolveCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
    {
        // This function is only called there's overlap in both x and y directions;
        // if previous x overlap is negative, x overlap is new

        const Vec2f previousOverlap = Physics::getPreviousOverlap(a, b);
        bool isXDirection = previousOverlap.x <= 0 && previousOverlap.y > 0;
        moveEntity(a, b, isXDirection);
        return isXDirection;
    }
}