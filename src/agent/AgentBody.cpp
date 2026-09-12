// Agent body implementation - manages physical properties and movement
// This is part of the environment, not the brain

#include "AgentBody.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Default constructor already provides basic initialization

// Update body based on motor command
void AgentBody::update(MotorCommand command, float timestep) {
    // Reset movement flags
    isMoving = false;
    isTurning = false;
    
    // Apply movement based on command
    switch (command) {
        case MotorCommand::MoveForward:
            velocityX += std::cos(orientation) * 2.0f * timestep;
            velocityY += std::sin(orientation) * 2.0f * timestep;
            isMoving = true;
            break;
            
        case MotorCommand::MoveBackward:
            velocityX -= std::cos(orientation) * 2.0f * timestep;
            velocityY -= std::sin(orientation) * 2.0f * timestep;
            isMoving = true;
            break;
            
        case MotorCommand::TurnLeft:
            angularVelocity -= 0.5f * timestep;
            isTurning = true;
            break;
            
        case MotorCommand::TurnRight:
            angularVelocity += 0.5f * timestep;
            isTurning = true;
            break;
            
        case MotorCommand::LookLeft:
            // Pan sensors left (optional implementation)
            break;
            
        case MotorCommand::LookRight:
            // Pan sensors right (optional implementation)
            break;
            
        case MotorCommand::Interact:
            // Perform interaction (optional implementation)
            break;
            
        case MotorCommand::Wait:
            // No movement
            break;
    }
    
    // Update position based on velocity
    x += velocityX * timestep;
    y += velocityY * timestep;
    
    // Update orientation based on angular velocity
    orientation += angularVelocity * timestep;
    
    // Clamp orientation to [-π, π]
    if (orientation > 3.14159f) {
        orientation -= 6.28318f;
    } else if (orientation < -3.14159f) {
        orientation += 6.28318f;
    }
    
    // Apply friction to velocity
    float friction = 0.95f;
    velocityX *= friction;
    velocityY *= friction;
    
    // Apply angular friction
    angularVelocity *= 0.98f;
    
    // Clamp velocities
    float maxVel = 10.0f;
    float velMag = std::sqrt(velocityX * velocityX + velocityY * velocityY);
    if (velMag > maxVel) {
        velocityX = velocityX / velMag * maxVel;
        velocityY = velocityY / velMag * maxVel;
    }
    
    // Update age
    age += timestep;
    
    // Decay energy over time
    energy *= 0.999f;
    health = std::max(0.0f, health - 0.0001f * timestep);
    
    // Check bounds and wrap around
    if (x < -100.0f) x = 100.0f;
    if (x > 100.0f) x = -100.0f;
    if (y < -100.0f) y = 100.0f;
    if (y > 100.0f) y = -100.0f;
}

// Get position vector
Vector2 AgentBody::getPosition() const {
    return Vector2{x, y};
}

// Get velocity vector
Vector2 AgentBody::getVelocity() const {
    return Vector2{velocityX, velocityY};
}

// Get distance to another position
float AgentBody::distanceTo(float x2, float y2) const {
    float dx = x2 - x;
    float dy = y2 - y;
    return std::sqrt(dx * dx + dy * dy);
}

// Get angle to another position
float AgentBody::angleTo(float x2, float y2) const {
    float dx = x2 - x;
    float dy = y2 - y;
    return std::atan2(dy, dx);
}

// Get angle difference (shortest path)
float AgentBody::angleDifference(float angle) const {
    float diff = angle - orientation;
    // Normalize to [-π, π]
    while (diff > 3.14159f) diff -= 6.28318f;
    while (diff < -3.14159f) diff += 6.28318f;
    return diff;
}

// Check if body is within radius of position
bool AgentBody::isNear(float x2, float y2, float radius) const {
    return distanceTo(x2, y2) < radius;
}

// Get state as string for debugging
std::string AgentBody::getStateString() const {
    std::stringstream ss;
    ss << "Pos: (" << x << ", " << y << ") "
       << "Vel: (" << velocityX << ", " << velocityY << ") "
       << "Ang: " << orientation << " "
       << "Age: " << age << " "
       << "Energy: " << energy << " "
       << "Health: " << health;
    return ss.str();
}

} // namespace nlm
