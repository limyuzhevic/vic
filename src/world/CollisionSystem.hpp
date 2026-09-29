#pragma once

#include "../agent/AgentBody.hpp"
#include "SimpleWorld.hpp"
#include "WorldConstants.hpp"
#include <cmath>

namespace nlm {

class CollisionSystem {
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;
    
    void checkAgentObjectCollisions(AgentBody& agent, const std::vector<WorldObject>& objects);
    void checkBoundaryCollisions(AgentBody& agent, float width, float height);
    float calculateDistance(float x1, float y1, float x2, float y2) const;
};

} // namespace nlm