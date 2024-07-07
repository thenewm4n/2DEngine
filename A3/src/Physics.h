#pragma once

#include "Entity.h"
#include "Vec2.h"

namespace Physics
{
    Vec2f getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);     // Positive if overlap
    Vec2f getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    void resolveCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};