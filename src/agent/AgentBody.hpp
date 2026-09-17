#pragma once

#include <cstdint>
#include <string>

namespace nlm {

// Agent body state - contains physical properties and internal state
// This is part of the environment, not the brain
struct AgentBody {
    // Position in world
    float x;
    float y;
    
    // Orientation (radians)
    float orientation;
    
    // Velocity
    float velocityX;
    float velocityY;
    float angularVelocity;
    
    // Internal state
    float energy;          // Energy level (0-1)
    float health;          // Health (0-1)
    float age;             // Developmental age in simulation steps
    
    // Movement state
    bool isMoving;
    bool isTurning;
    float lastActionTime;
    
    // Performance tracking
    size_t totalStepsMoved;
    float totalDistanceTraveled;
    float averageSpeed;
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f)
        , totalStepsMoved(0), totalDistanceTraveled(0.0f), averageSpeed(0.0f) {}
    
    void reset() {
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
        totalStepsMoved = 0;
        totalDistanceTraveled = 0.0f;
        averageSpeed = 0.0f;
    }
    
    // Update velocity based on movement
    void updateVelocity(float newX, float newY, float dt) {
        if (dt > 0.0f) {
            float dx = newX - x;
            float dy = newY - y;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            // Update velocity
            velocityX = dx / dt;
            velocityY = dy / dt;
            
            // Track movement statistics
            totalStepsMoved++;
            totalDistanceTraveled += distance;
            averageSpeed = totalDistanceTraveled / (age + dt);
            
            // Update position
            x = newX;
            y = newY;
            
            // Set moving state
            isMoving = (distance > 0.001f);
            
            // Update orientation based on movement direction
            if (distance > 0.001f) {
                orientation = std::atan2(dy, dx);
            }
        }
    }
    
    // Apply movement with friction
    void applyMovement(float dt, float friction = 0.95f, float maxSpeed = 5.0f) {
        if (dt <= 0.0f) return;
        
        // Apply friction to velocity
        velocityX *= friction;
        velocityY *= friction;
        
        // Clamp to maximum speed
        float speed = std::sqrt(velocityX*velocityX + velocityY*velocityY);
        if (speed > maxSpeed) {
            velocityX = velocityX * maxSpeed / speed;
            velocityY = velocityY * maxSpeed / speed;
        }
        
        // Update position based on velocity
        x += velocityX * dt;
        y += velocityY * dt;
        orientation = std::atan2(velocityY, velocityX);
        
        age += dt;
    }
    
    // Check for collisions
    bool checkCollision(float x, float y, float radius) const {
        float dx = x - this->x;
        float dy = y - this->y;
        float distance = std::sqrt(dx*dx + dy*dy);
        return distance < radius;
    }
    
    // Get distance to target
    float getDistanceTo(float targetX, float targetY) const {
        float dx = targetX - x;
        float dy = targetY - y;
        return std::sqrt(dx*dx + dy*dy);
    }
    
    // Get angle to target
    float getAngleTo(float targetX, float targetY) const {
        float dx = targetX - x;
        float dy = targetY - y;
        return std::atan2(dy, dx);
    }
    
    // Decay energy over time
    void decayEnergy(float rate, float dt) {
        if (dt > 0.0f) {
            energy = std::max(0.0f, energy - rate * dt);
        }
    }
    
    // Heal damage
    void heal(float amount) {
        health = std::min(1.0f, health + amount);
    }
    
    // Take damage
    void damage(float amount) {
        health = std::max(0.0f, health - amount);
        if (health <= 0.0f) {
            // Dead
            energy = 0.0f;
        }
    }
    
    // Update aging effects
    void updateAging(float agingRate) {
        age += agingRate;
        if (age > 100.0f) {
            // Aged significantly
            velocityX *= 0.8f;
            velocityY *= 0.8f;
        }
    }
};

// Low-level motor commands that the brain can produce
// These are innate muscle-like signals, not semantic actions
enum class MotorCommand {
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};

// Action result from the world's perspective
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m) {}
};

// Object types in the world (for rendering/debug only, NOT given to brain)
enum class WorldObjectType {
    Empty = 0,
    Resource = 1,      // Positive reward
    Hazard = 2,        // Negative reward
    Wall = 3,         // Boundary
    Marker = 4        // Navigation aid
};

} // namespace nlm
