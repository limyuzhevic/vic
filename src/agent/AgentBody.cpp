#include "AgentBody.hpp"
#include <algorithm>

namespace nlm {

// AgentBody.cpp - Implementation of Agent body state and physics

void AgentBody::update(double timestep) {
    // Update position based on velocity
    x += velocityX * static_cast<float>(timestep);
    y += velocityY * static_cast<float>(timestep);
    
    // Update orientation based on angular velocity
    orientation += angularVelocity * static_cast<float>(timestep);
    
    // Normalize angle to [0, 2π]
    if (orientation < 0.0f) orientation += 2.0f * 3.141592653589793f;
    else if (orientation >= 2.0f * 3.141592653589793f) orientation -= 2.0f * 3.141592653589793f;
    
    // Update energy based on movement and time
    // Movement costs energy
    float movementCost = (std::abs(velocityX) + std::abs(velocityY) + std::abs(angularVelocity)) * 0.1f;
    energy -= movementCost * static_cast<float>(timestep);
    
    // Natural energy decay
    energy -= 0.001f * static_cast<float>(timestep);
    
    // Clamp energy to [0, 1]
    if (energy < 0.0f) energy = 0.0f;
    if (energy > 1.0f) energy = 1.0f;
    
    // Health degrades over time
    health -= 0.0001f * static_cast<float>(timestep);
    if (health < 0.0f) health = 0.0f;
    
    // Age increases
    age += timestep;
    
    // Movement flags
    isMoving = (std::abs(velocityX) > 0.01f || std::abs(velocityY) > 0.01f || 
                std::abs(angularVelocity) > 0.01f);
    
    isTurning = (std::abs(angularVelocity) > 0.01f);
    
    // Update last action time
    lastActionTime += timestep;
}

void AgentBody::applyForce(float forceX, float forceY, float forceAngle) {
    // Apply force to velocity (simple Euler integration)
    // Force angle affects rotational component
    float forceVectorX = forceX * std::cos(forceAngle);
    float forceVectorY = forceX * std::sin(forceAngle);
    
    // Linear acceleration
    velocityX += forceVectorX * 0.01f;
    velocityY += forceVectorY * 0.01f;
    
    // Rotational acceleration
    angularVelocity += forceAngle * 0.02f;
    
    // Limit max velocities
    float maxLinearVel = 5.0f;
    float maxAngularVel = 1.0f;
    
    float linearSpeed = std::sqrt(velocityX * velocityX + velocityY * velocityY);
    if (linearSpeed > maxLinearVel) {
        float scale = maxLinearVel / linearSpeed;
        velocityX *= scale;
        velocityY *= scale;
    }
    
    if (std::abs(angularVelocity) > maxAngularVel) {
        angularVelocity = std::copysign(maxAngularVel, angularVelocity);
    }
}

void AgentBody::teleport(float newX, float newY, float newOrientation) {
    x = newX;
    y = newY;
    orientation = newOrientation;
    
    // Reset velocity
    velocityX = 0.0f;
    velocityY = 0.0f;
    angularVelocity = 0.0f;
    
    // Normalize orientation
    if (orientation < 0.0f) orientation += 2.0f * 3.141592653589793f;
    else if (orientation >= 2.0f * 3.141592653589793f) orientation -= 2.0f * 3.141592653589793f;
}

float AgentBody::getSpeed() const {
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
}

float AgentBody::getDirection() const {
    return std::atan2(velocityY, velocityX);
}

bool AgentBody::isHealthy() const {
    return health > 0.1f;  // Healthy if health > 10%
}

bool AgentBody::isAlive() const {
    return health > 0.0f && energy > 0.0f;
}

void AgentBody::giveEnergy(float amount) {
    energy += amount;
    if (energy > 1.0f) energy = 1.0f;
}

void AgentBody::reduceHealth(float amount) {
    health -= amount;
    if (health < 0.0f) health = 0.0f;
}

} // namespace nlm