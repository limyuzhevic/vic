#include "PhysicsSystem.hpp"

namespace nlm {

// Physics system implementation
void PhysicsSystem::update(float dt) {
    // Detect collisions
    detectCollisions();
    
    // Resolve collisions first to prevent tunneling
    resolveCollisions();
    
    // Update positions based on current velocities
    for (auto& body : bodies_) {
        body->updatePosition(dt);
    }
    
    // Apply gravity to non-static bodies
    applyGravity(dt);
    
    // Apply other environmental forces
    applyEnvironmentalForces(dt);
}

} // namespace nlm