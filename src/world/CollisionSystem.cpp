#include "CollisionSystem.hpp"
#include "../agent/AgentBody.hpp"
#include "../world/WorldObject.hpp"
#include "WorldConstants.hpp"
#include <cmath>

namespace nlm {

void CollisionSystem::checkAgentObjectCollisions(AgentBody& agent, const std::vector<WorldObject>& objects) {
    for (const auto& obj : objects) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent.x;
        float dy = obj.y - agent.y;
        float dist = calculateDistance(0, 0, dx, dy);
        
        if (dist < obj.radius) {
            if (obj.type == WorldObjectType::Hazard) {
                agent.health -= WorldConstants::PASSIVE_HAZARD_DAMAGE;
            }
        }
    }
}

void CollisionSystem::checkBoundaryCollisions(AgentBody& agent, float width, float height) {
    // Boundary collisions are handled in PhysicsSystem
}

float CollisionSystem::calculateDistance(float x1, float y1, float x2, float y2) const {
    return std::sqrt(x1 * x1 + y1 * y1);
}

} // namespace nlm