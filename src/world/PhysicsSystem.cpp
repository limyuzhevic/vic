#include "PhysicsSystem.hpp"
#include <cmath>

namespace nlm {

void PhysicsSystem::updateAgentPhysics(AgentBody& agent, float width, float height, double timestep) {
    // Apply velocity to position
    agent.x += agent.velocityX * timestep;
    agent.y += agent.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent.orientation += agent.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent.orientation > M_PI) agent.orientation -= 2.0f * M_PI;
    while (agent.orientation < -M_PI) agent.orientation += 2.0f * M_PI;
    
    // Apply boundary conditions
    applyBoundaryConditions(agent, width, height);
    
    // Decay velocities if not moving/turning
    if (!agent.isMoving) {
        agent.velocityX *= WorldConstants::WALL_FRICTION;
        agent.velocityY *= WorldConstants::WALL_FRICTION;
    }
    if (!agent.isTurning) {
        agent.angularVelocity *= WorldConstants::WALL_FRICTION;
    }
}

void PhysicsSystem::applyBoundaryConditions(AgentBody& agent, float width, float height) {
    if (agent.x < 0.0f) {
        agent.x = 0.0f;
        agent.velocityX = 0.0f;
    }
    if (agent.x >= width) {
        agent.x = width - WorldConstants::WALL_BUFFER;
        agent.velocityX = 0.0f;
    }
    if (agent.y < 0.0f) {
        agent.y = 0.0f;
        agent.velocityY = 0.0f;
    }
    if (agent.y >= height) {
        agent.y = height - WorldConstants::WALL_BUFFER;
        agent.velocityY = 0.0f;
    }
}

void PhysicsSystem::applyMotorCommand(AgentBody& agent, MotorCommand cmd, float moveSpeed, float turnSpeed) {
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent.orientation) * moveSpeed;
            float dy = std::sin(agent.orientation) * moveSpeed;
            agent.velocityX = dx;
            agent.velocityY = dy;
            agent.isMoving = true;
            break;
        }
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(agent.orientation) * moveSpeed * 0.5f;
            float dy = -std::sin(agent.orientation) * moveSpeed * 0.5f;
            agent.velocityX = dx;
            agent.velocityY = dy;
            agent.isMoving = true;
            break;
        }
        case MotorCommand::TurnLeft:
            agent.angularVelocity = -turnSpeed;
            agent.isTurning = true;
            break;
        case MotorCommand::TurnRight:
            agent.angularVelocity = turnSpeed;
            agent.isTurning = true;
            break;
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
        case MotorCommand::Wait:
        case MotorCommand::Interact:
        default:
            agent.velocityX = 0.0f;
            agent.velocityY = 0.0f;
            agent.angularVelocity = 0.0f;
            agent.isMoving = false;
            agent.isTurning = false;
            break;
    }
}

bool PhysicsSystem::isValidPosition(float x, float y, float width, float height) {
    return (x >= 0.0f && x < width && y >= 0.0f && y < height);
}

} // namespace nlm