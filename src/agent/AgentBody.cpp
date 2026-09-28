#include "AgentBody.hpp"
#include "../world/SimpleWorld.hpp"
#include <cmath>

namespace nlm {

void AgentBody::update(double timestep) {
    // Update position based on velocity
    x += velocityX * timestep;
    y += velocityY * timestep;
    
    // Update orientation based on angular velocity
    orientation += angularVelocity * timestep;
    
    // Normalize orientation to [0, 2π)
    if (orientation >= 2.0f * M_PI) {
        orientation -= 2.0f * M_PI;
    } else if (orientation < 0.0f) {
        orientation += 2.0f * M_PI;
    }
    
    // Update energy (simple consumption model)
    energy = std::max(0.0f, energy - 0.001f * timestep);
    
    // Update health (slow decay)
    health = std::max(0.0f, health - 0.0001f * timestep);
    
    // Update age
    age += timestep;
    
    // Update moving state
    isMoving = (fabs(velocityX) > 0.001f || fabs(velocityY) > 0.001f);
    isTurning = (fabs(angularVelocity) > 0.001f);
    
    // Update last action time
    lastActionTime = timestep;
}

void AgentBody::applyMovementCommand(MotorCommand command, double timestep) {
    // Reset velocity for this timestep
    velocityX = 0.0f;
    velocityY = 0.0f;
    angularVelocity = 0.0f;
    
    // Apply movement based on command
    switch (command) {
        case MotorCommand::MoveForward:
            // Move forward in current orientation
            velocityX = cos(orientation);
            velocityY = sin(orientation);
            velocityX *= 2.0f;  // Speed factor
            velocityY *= 2.0f;
            break;
            
        case MotorCommand::MoveBackward:
            // Move backward in current orientation
            velocityX = -cos(orientation);
            velocityY = -sin(orientation);
            velocityX *= 1.5f;  // Slower backward
            velocityY *= 1.5f;
            break;
            
        case MotorCommand::TurnLeft:
            angularVelocity = -2.0f;  // Rotate left
            break;
            
        case MotorCommand::TurnRight:
            angularVelocity = 2.0f;   // Rotate right
            break;
            
        case MotorCommand::LookLeft:
            angularVelocity = -0.5f;  // Pan left (slower)
            break;
            
        case MotorCommand::LookRight:
            angularVelocity = 0.5f;   // Pan right (slower)
            break;
            
        case MotorCommand::Interact:
            // Interaction doesn't move the agent, just triggers world events
            break;
            
        case MotorCommand::Wait:
            // No movement
            break;
    }
    
    // Update last action time
    lastActionTime = timestep;
}

void AgentBody::applyWorldCollision(const SimpleWorld& world) {
    // Simple boundary collision with world
    // In a full implementation, this would check against world geometry
    // and apply proper collision response
    
    // For now, just clamp position to reasonable bounds
    float buffer = 1.0f;  // Safe margin from boundary
    x = std::max(buffer, std::min(x, world.getWidth() - buffer));
    y = std::max(buffer, std::min(y, world.getHeight() - buffer));
    
    // If collision occurs, apply energy loss
    if (fabs(velocityX) > 0.1f || fabs(velocityY) > 0.1f) {
        energy = std::max(0.0f, energy - 0.05f);
        health = std::max(0.0f, health - 0.02f);
    }
}

void AgentBody::takeDamage(float amount) {
    health = std::max(0.0f, health - amount);
    
    // If health is critically low, reduce energy too
    if (health < 0.3f) {
        energy = std::max(0.0f, energy - amount * 2.0f);
    }
}

void AgentBody::heal(float amount) {
    health = std::min(1.0f, health + amount);
    energy = std::min(1.0f, energy + amount * 0.5f);  // Partial energy recovery
}

bool AgentBody::isAlive() const {
    return health > 0.0f && energy > 0.0f;
}

float AgentBody::getSpeed() const {
    return sqrt(velocityX * velocityX + velocityY * velocityY);
}

} // namespace nlm
