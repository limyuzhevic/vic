#include "AgentBody.hpp"
#include <cmath>

namespace nlm {

void AgentBody::move(const MotorCommand& command, float dt) {
    // Physics-based movement with constraints
    applyMovement(command, dt);
}

void AgentBody::applyMovement(const MotorCommand& command, float dt) {
    // Reset velocity if not moving
    if (command == MotorCommand::Wait) {
        velocityX = 0.0f;
        velocityY = 0.0f;
        angularVelocity = 0.0f;
        isMoving = false;
        isTurning = false;
        return;
    }
    
    // Calculate acceleration based on command
    float acc = 0.0f;
    float turnRate = 0.0f;
    
    switch (command) {
        case MotorCommand::MoveForward:
            acc = 1.0f;  // Forward acceleration
            isMoving = true;
            break;
        case MotorCommand::MoveBackward:
            acc = -0.5f;  // Slower backward acceleration
            isMoving = true;
            break;
        case MotorCommand::TurnLeft:
            turnRate = -0.5f;  // Counter-clockwise rotation
            isTurning = true;
            break;
        case MotorCommand::TurnRight:
            turnRate = 0.5f;   // Clockwise rotation
            isTurning = true;
            break;
        case MotorCommand::LookLeft:
            // Just sensor panning, no physics
            isTurning = true;
            break;
        case MotorCommand::LookRight:
            // Just sensor panning, no physics
            isTurning = true;
            break;
        case MotorCommand::Interact:
            // Special interaction behavior
            isMoving = false;
            isTurning = false;
            break;
        default:
            break;
    }
    
    // Update linear velocity with mass and friction
    if (command != MotorCommand::Wait) {
        velocityX += acc * dt / mass;
        velocityY += acc * dt / mass;
        
        // Apply friction
        velocityX *= (1.0f - friction * dt);
        velocityY *= (1.0f - friction * dt);
        
        // Clamp to maximum speed
        float speed = std::sqrt(velocityX * velocityX + velocityY * velocityY);
        if (speed > maxSpeed) {
            velocityX = velocityX / speed * maxSpeed;
            velocityY = velocityY / speed * maxSpeed;
        }
    }
    
    // Update angular velocity
    if (command == MotorCommand::TurnLeft || command == MotorCommand::TurnRight) {
        angularVelocity += turnRate * dt;
        angularVelocity *= (1.0f - friction * dt);  // Friction
        
        // Clamp angular velocity
        if (std::abs(angularVelocity) > maxAngularVel) {
            angularVelocity = (angularVelocity > 0) ? maxAngularVel : -maxAngularVel;
        }
    }
    
    // Update position with damping
    x += velocityX * dt;
    y += velocityY * dt;
    orientation += angularVelocity * dt;
    
    // Apply velocity damping over time
    velocityX *= damping;
    velocityY *= damping;
    angularVelocity *= damping;
    
    // Update energy consumption
    if (isMoving || isTurning) {
        energy = std::max(0.0f, energy - 0.001f * dt);
    }
    
    // Update health based on movement
    if (health < 1.0f && isMoving) {
        health = std::min(1.0f, health + 0.0001f * dt);
    }
    
    // Update age
    age += dt;
    
    // Update last action time
    lastActionTime += dt;
}

} // namespace nlm