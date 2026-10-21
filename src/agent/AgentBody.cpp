// NLM (熙然) - Neural Learning Machine
// Agent body implementation
// Part of the environment system, not the brain

#include "AgentBody.hpp"
#include <cmath>

namespace nlm {

// Update agent position based on velocity and time
void AgentBody::update(double timestep) {
    x += velocityX * timestep;
    y += velocityY * timestep;
    orientation += angularVelocity * timestep;
    
    // Normalize orientation to [0, 2π)
    orientation = fmod(orientation, 2.0f * M_PI);
    if (orientation < 0.0f) {
        orientation += 2.0f * M_PI;
    }
    
    // Age increases
    age += timestep;
    
    // Update movement flags
    isMoving = (std::abs(velocityX) > 0.001f || std::abs(velocityY) > 0.001f);
    isTurning = (std::abs(angularVelocity) > 0.001f);
    
    // Update last action time
    lastActionTime += timestep;
}

// Calculate distance to another position
float AgentBody::distanceTo(float otherX, float otherY) const {
    float dx = x - otherX;
    float dy = y - otherY;
    return std::sqrt(dx * dx + dy * dy);
}

// Check if agent is within range of another position
bool AgentBody::isInRange(float otherX, float otherY, float range) const {
    return distanceTo(otherX, otherY) <= range;
}

// Get position as vector
std::pair<float, float> AgentBody::getPosition() const {
    return std::make_pair(x, y);
}

// Set position
void AgentBody::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

// Get velocity magnitude
float AgentBody::getSpeed() const {
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
}

// Set velocity
void AgentBody::setVelocity(float vx, float vy) {
    velocityX = vx;
    velocityY = vy;
}

// Get angular velocity
float AgentBody::getAngularVelocity() const {
    return angularVelocity;
}

// Set angular velocity
void AgentBody::setAngularVelocity(float av) {
    angularVelocity = av;
}

// Get energy level
float AgentBody::getEnergy() const {
    return energy;
}

// Set energy level
void AgentBody::setEnergy(float e) {
    energy = std::max(0.0f, std::min(1.0f, e));  // Clamp to [0, 1]
}

// Get health level
float AgentBody::getHealth() const {
    return health;
}

// Set health level
void AgentBody::setHealth(float h) {
    health = std::max(0.0f, std::min(1.0f, h));  // Clamp to [0, 1]
}

// Get age
float AgentBody::getAge() const {
    return age;
}

// Check if agent is alive
bool AgentBody::isAlive() const {
    return health > 0.0f;
}

// Check if agent has energy
bool AgentBody::hasEnergy() const {
    return energy > 0.0f;
}

// Consume energy
bool AgentBody::consumeEnergy(float amount) {
    if (energy >= amount) {
        energy -= amount;
        return true;
    }
    return false;
}

// Recover energy
void AgentBody::recoverEnergy(float amount) {
    energy = std::min(1.0f, energy + amount);
}

// Apply damage
void AgentBody::applyDamage(float damage) {
    health = std::max(0.0f, health - damage);
}

// Heal
void AgentBody::heal(float amount) {
    health = std::min(1.0f, health + amount);
}

// Check if moving
bool AgentBody::isMoving() const {
    return isMoving;
}

// Check if turning
bool AgentBody::isTurning() const {
    return isTurning;
}

// Get last action time
float AgentBody::getLastActionTime() const {
    return lastActionTime;
}

// Reset body to initial state
void AgentBody::reset() {
    x = 0.0f;
    y = 0.0f;
    orientation = 0.0f;
    velocityX = 0.0f;
    velocityY = 0.0f;
    angularVelocity = 0.0f;
    energy = 1.0f;
    health = 1.0f;
    age = 0.0f;
    isMoving = false;
    isTurning = false;
    lastActionTime = 0.0f;
}

// Clone agent body
AgentBody AgentBody::clone() const {
    AgentBody cloned;
    cloned.x = x;
    cloned.y = y;
    cloned.orientation = orientation;
    cloned.velocityX = velocityX;
    cloned.velocityY = velocityY;
    cloned.angularVelocity = angularVelocity;
    cloned.energy = energy;
    cloned.health = health;
    cloned.age = age;
    cloned.isMoving = isMoving;
    cloned.isTurning = isTurning;
    cloned.lastActionTime = lastActionTime;
    return cloned;
}

} // namespace nlm