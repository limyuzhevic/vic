#pragma once

#include "../agent/AgentBody.hpp"
#include "PhysicsSystem.hpp"
#include "WorldConstants.hpp"
#include <cmath>

namespace nlm {

class PhysicsSystem {
public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;
    
    void updateAgentPhysics(AgentBody& agent, float width, float height, double timestep);
    void applyBoundaryConditions(AgentBody& agent, float width, float height);
    void applyMotorCommand(AgentBody& agent, MotorCommand cmd, float moveSpeed, float turnSpeed);
    bool isValidPosition(float x, float y, float width, float height);
};

} // namespace nlm