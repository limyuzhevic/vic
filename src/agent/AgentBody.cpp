// NLM Agent Body Implementation
// Provides physical simulation and movement for agents

#include "AgentBody.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Add additional implementation if needed

// Additional agent body methods could be added here

// Physics simulation methods
void AgentBody::updatePhysics(float timestep) {
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
    
    // Simple damping for stability
    velocityX *= 0.95f;
    velocityY *= 0.95f;
    angularVelocity *= 0.95f;
}

// Calculate distance to another agent
float AgentBody::distanceTo(const AgentBody& other) const {
    float dx = other.x - x;
    float dy = other.y - y;
    return std::sqrt(dx * dx + dy * dy);
}

// Calculate angle to another agent (radians)
float AgentBody::angleTo(const AgentBody& other) const {
    float dx = other.x - x;
    float dy = other.y - y;
    float angle = std::atan2(dy, dx);
    
    // Normalize to [0, 2π)
    if (angle < 0.0f) {
        angle += 2.0f * M_PI;
    }
    return angle;
}

// Check if agent is facing another agent within tolerance
bool AgentBody::isFacing(const AgentBody& other, float toleranceRadians) const {
    float angleToTarget = angleTo(other);
    float angleDiff = std::abs(angleToTarget - orientation);
    
    // Handle wrap-around
    if (angleDiff > M_PI) {
        angleDiff = 2.0f * M_PI - angleDiff;
    }
    
    return angleDiff <= toleranceRadians;
}

// Check if agent is close enough to interact
bool AgentBody::canInteract(const AgentBody& other, float interactionRadius) const {
    return distanceTo(other) <= interactionRadius;
}

// Apply force/movement
void AgentBody::applyForce(float forceX, float forceY, float forceTorque) {
    velocityX += forceX * 0.1f;  // Mass = 10 for simplicity
    velocityY += forceY * 0.1f;
    angularVelocity += forceTorque * 0.05f;
}

// Stop all movement
void AgentBody::stop() {
    velocityX = 0.0f;
    velocityY = 0.0f;
    angularVelocity = 0.0f;
    isMoving = false;
    isTurning = false;
}

// Check if agent is alive (health > 0)
bool AgentBody::isAlive() const {
    return health > 0.0f;
}

// Check if agent has energy
bool AgentBody::hasEnergy(float requiredEnergy) const {
    return energy >= requiredEnergy;
}

// Consume energy
bool AgentBody::consumeEnergy(float amount) {
    if (hasEnergy(amount)) {
        energy -= amount;
        return true;
    }
    return false;
}

// Regenerate energy
void AgentBody::regenerateEnergy(float amount) {
    energy = std::min(energy + amount, 1.0f);
}

// Take damage
def AgentBody::takeDamage(float amount) {
    health = std::max(health - amount, 0.0f);
    if (health <= 0.0f) {
        energy = 0.0f;
        isMoving = false;
        isTurning = false;
    }
}

// Heal
def AgentBody::heal(float amount) {
    health = std::min(health + amount, 1.0f);
}

// Age the agent (for developmental simulation)
def AgentBody::age(float amount) {
    age += amount;
}

// Get the MotorCommand from MotorCommand enum
MotorCommand AgentBody::getRandomMotorCommand() {
    int choice = rand() % static_cast<int>(MotorCommand::Wait) + 1;  // Skip Wait (0)
    return static_cast<MotorCommand>(choice);
}

// Get action string representation
std::string AgentBody::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

// Check if action results in movement
bool AgentBody::isMovementAction(MotorCommand cmd) {
    return cmd == MotorCommand::MoveForward || 
           cmd == MotorCommand::MoveBackward ||
           cmd == MotorCommand::TurnLeft ||
           cmd == MotorCommand::TurnRight;
}

} // namespace nlm