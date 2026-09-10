/*
 * AgentBody.cpp - Implementation of agent physical properties and movement
 * 
 * This file implements the AgentBody class which represents the physical
 * properties of an agent in the environment. It handles position, velocity,
 * orientation, energy, health, and movement mechanics. This is part of the
 * environment interface, not the brain.
 */

#include "AgentBody.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Update agent physics based on time step
timestamp_t updatePhysics(timestamp_t currentTime) {
    timestamp_t deltaTime = currentTime - lastActionTime;
    if (deltaTime <= 0.0f) return currentTime;
    
    // Update position based on velocity
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
    
    // Update orientation based on angular velocity
    orientation += angularVelocity * deltaTime;
    
    // Normalize orientation to [-PI, PI]
    while (orientation > M_PI) orientation -= 2.0f * M_PI;
    while (orientation < -M_PI) orientation += 2.0f * M_PI;
    
    // Apply boundary constraints
    x = std::clamp(x, 0.0f, width - 0.01f);
    y = std::clamp(y, 0.0f, height - 0.01f);
    
    // Update energy (decay over time when moving)
    if (isMoving || isTurning) {
        energy -= energyDecayRate * deltaTime;
        if (energy < 0.0f) energy = 0.0f;
    } else {
        // Regenerate energy when stationary
        energy += energyRegenerationRate * deltaTime;
        if (energy > 1.0f) energy = 1.0f;
    }
    
    // Update health (slow decay over time)
    health -= healthDecayRate * deltaTime;
    if (health < 0.0f) health = 0.0f;
    
    // Update age
    age += deltaTime;
    
    // Mark as no longer moving/turning until next command
    isMoving = false;
    isTurning = false;
    
    lastActionTime = currentTime;
    return currentTime;
}

// Apply motor command to agent
// TODO PHASE 2: Add inertia and smooth movement
void applyMotorCommand(MotorCommand cmd, double currentTime) {
    // Current implementation - instant response
    isMoving = false;
    isTurning = false;
    
    switch (cmd) {
        case MotorCommand::MoveForward:
            velocityX += forwardThrustX * thrustMultiplier;
            velocityY += forwardThrustY * thrustMultiplier;
            isMoving = true;
            break;
        case MotorCommand::MoveBackward:
            velocityX -= forwardThrustX * thrustMultiplier;
            velocityY -= forwardThrustY * thrustMultiplier;
            isMoving = true;
            break;
        case MotorCommand::TurnLeft:
            angularVelocity += turnRate * turnMultiplier;
            isTurning = true;
            break;
        case MotorCommand::TurnRight:
            angularVelocity -= turnRate * turnMultiplier;
            isTurning = true;
            break;
        case MotorCommand::LookLeft:
            // Rotate sensor view left
            sensorRotation += sensorTurnSpeed;
            break;
        case MotorCommand::LookRight:
            // Rotate sensor view right
            sensorRotation -= sensorTurnSpeed;
            break;
        case MotorCommand::Interact:
            // Can be implemented for object interaction
            break;
        case MotorCommand::Wait:
            // No movement
            break;
    }
}

// Get distance to target
float distanceToTarget(float targetX, float targetY) const {
    float dx = targetX - x;
    float dy = targetY - y;
    return std::sqrt(dx * dx + dy * dy);
}

// Get angle to target
float angleToTarget(float targetX, float targetY) const {
    float dx = targetX - x;
    float dy = targetY - y;
    float angle = std::atan2(dy, dx);
    
    // Adjust for sensor rotation
    angle -= sensorRotation;
    
    // Normalize to [-PI, PI]
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
    
    return angle;
}

// Check if agent is near target position
bool isNearTarget(float targetX, float targetY, float threshold) const {
    return distanceToTarget(targetX, targetY) <= threshold;
}

// Apply damage to agent
void applyDamage(float damage) {
    health -= damage;
    if (health < 0.0f) health = 0.0f;
}

// Regenerate energy (called periodically)
void regenerateEnergy() {
    energy += energyRegenerationRate;
    if (energy > 1.0f) energy = 1.0f;
}

// Apply energy boost
void applyEnergyBoost(float boostAmount) {
    energy += boostAmount;
    if (energy > 1.0f) energy = 1.0f;
}

// Check if agent is dead
bool isDead() const {
    return health <= 0.0f;
}

// Check if agent is exhausted
bool isExhausted() const {
    return energy <= 0.0f;
}

// Get normalized velocity magnitude
float getSpeed() const {
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
}

// Get velocity in direction of orientation
float getForwardSpeed() const {
    return velocityX * std::cos(orientation) + velocityY * std::sin(orientation);
}

// Check if agent can move (has energy and health)
bool canMove() const {
    return !isDead() && !isExhausted() && health > 0.1f;
}

// Reset agent to starting position
void resetToStart(float startX, float startY) {
    x = startX;
    y = startY;
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
    sensorRotation = 0.0f;
}

// Smooth movement between current and target positions
void smoothMoveTo(float targetX, float targetY, float speed, timestamp_t currentTime) {
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance > 0.0f) {
        // Calculate desired velocity
        float desiredVx = dx / distance * speed;
        float desiredVy = dy / distance * speed;
        
        // Smooth transition (acceleration/deceleration)
        velocityX += (desiredVx - velocityX) * accelerationRate * timestep;
        velocityY += (desiredVy - velocityY) * accelerationRate * timestep;
        
        // Clamp to max speed
        float currentSpeed = getSpeed();
        if (currentSpeed > maxSpeed) {
            velocityX = velocityX / currentSpeed * maxSpeed;
            velocityY = velocityY / currentSpeed * maxSpeed;
        }
    }
}

// Implement AgentBody methods
AgentBody::AgentBody()
    : x(0.0f), y(0.0f), orientation(0.0f)
    , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
    , energy(1.0f), health(1.0f), age(0.0f)
    , isMoving(false), isTurning(false), lastActionTime(0.0f)
    , sensorRotation(0.0f) {
}

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
    sensorRotation = 0.0f;
}

void AgentBody::updatePhysics(timestamp_t currentTime) {
    // Delegate to free functions
    ::updatePhysics(currentTime);
}

void AgentBody::applyMotorCommand(MotorCommand cmd, double currentTime) {
    // Delegate to free functions
    ::applyMotorCommand(cmd, currentTime);
}

float AgentBody::distanceToTarget(float targetX, float targetY) const {
    return ::distanceToTarget(targetX, targetY);
}

float AgentBody::angleToTarget(float targetX, float targetY) const {
    return ::angleToTarget(targetX, targetY);
}

bool AgentBody::isNearTarget(float targetX, float targetY, float threshold) const {
    return ::isNearTarget(targetX, targetY, threshold);
}

void AgentBody::applyDamage(float damage) {
    ::applyDamage(damage);
}

void AgentBody::regenerateEnergy() {
    ::regenerateEnergy();
}

void AgentBody::applyEnergyBoost(float boostAmount) {
    ::applyEnergyBoost(boostAmount);
}

bool AgentBody::isDead() const {
    return ::isDead();
}

bool AgentBody::isExhausted() const {
    return ::isExhausted();
}

float AgentBody::getSpeed() const {
    return ::getSpeed();
}

float AgentBody::getForwardSpeed() const {
    return ::getForwardSpeed();
}

bool AgentBody::canMove() const {
    return ::canMove();
}

void AgentBody::resetToStart(float startX, float startY) {
    ::resetToStart(startX, startY);
}

void AgentBody::smoothMoveTo(float targetX, float targetY, float speed, timestamp_t currentTime) {
    ::smoothMoveTo(targetX, targetY, speed, currentTime);
}

} // namespace nlm