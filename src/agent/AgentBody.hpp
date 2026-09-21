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
    
    // Physics constraints
    float maxSpeed;        // Maximum speed for movement
    float maxAngularVel;   // Maximum rotation speed
    float mass;            // Mass for physics calculations
    float friction;        // Friction coefficient
    float damping;         // Velocity damping factor
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f)
        , maxSpeed(1.0f), maxAngularVel(0.5f), mass(1.0f), friction(0.1f), damping(0.95f) {}
    
    // Physics-based movement with constraints
    void applyMovement(const MotorCommand& command, float dt) {
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
    
    // Check for collisions with world boundaries
    void enforceBoundaries(float worldWidth, float worldHeight) {
        // Prevent going out of bounds
        if (x < 0.0f) { x = 0.0f; velocityX = 0.0f; }
        if (x > worldWidth) { x = worldWidth; velocityX = 0.0f; }
        if (y < 0.0f) { y = 0.0f; velocityY = 0.0f; }
        if (y > worldHeight) { y = worldHeight; velocityY = 0.0f; }
        
        // Keep orientation within reasonable bounds
        while (orientation > 2.0f * 3.14159f) orientation -= 2.0f * 3.14159f;
        while (orientation < 0.0f) orientation += 2.0f * 3.14159f;
    }
    
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
        
        // Reset physics parameters to defaults
        maxSpeed = 1.0f;
        maxAngularVel = 0.5f;
        mass = 1.0f;
        friction = 0.1f;
        damping = 0.95f;
    }
};

// Enhanced MotorCommand system with support for advanced users
// These commands provide granular control over agent behavior
enum class MotorCommand {
    // Basic movement
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    MoveLeft = 8,      // Strafe left (for advanced navigation)
    MoveRight = 9,     // Strafe right (for advanced navigation)
    
    // Rotation
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left (camera/seekers)
    LookRight = 5,     // Pan sensor right (camera/seekers)
    
    // Advanced actions for expert users
    Sprint = 10,       // Move forward at increased speed
    Crouch = 11,       // Reduce profile for crawling under obstacles
    Jump = 12,         // Jump to navigate elevation changes
    Swim = 13,         // Move in fluid environments
    Fly = 14,          // Elevate and fly (requires specific world support)
    Climb = 15,        // Climb surfaces vertically
    Dig = 16,          // Dig into ground (creates tunnels)
    Build = 17,        // Construct structures
    Push = 18,         // Push objects
    Pull = 19,         // Pull objects
    Grab = 20,         // Grab/hold objects
    Release = 21,      // Release held objects
    Use = 22,          // Use/interact with items
    Examine = 23,      // Examine nearby objects closely
    Attack = 24,       // Attack entities
    Defend = 25,       // Defensive posture
    Patrol = 26,       // Enter patrol mode
    Explore = 27,      // Begin exploration pattern
    Return = 28,       // Return to home/starting position
    Sleep = 29,        // Rest and recover energy
    Wait = 7,          // No movement (default)
    
    // Utility actions
    Save = 30,         // Save current state
    Load = 31,         // Load saved state
    Reset = 32,        // Reset agent body
    Repair = 33,       // Repair damage
    Feed = 34,         // Feed (if energy/health system supports)
    Drink = 35,        // Drink (if hydration system supports)
    SleepCycle = 36,   // Complete sleep cycle
    Debug = 37,        // Debug mode (prints internal state)
};

// Enhanced ActionResult with additional fields for expert users
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    // Expert-level fields
    float energyCost;        // Energy consumed to perform this action
    float healthChange;      // Health modification (positive/negative)
    float learningValue;     // How valuable this action was for learning
    std::string actionType;  // Descriptive action type for logging
    std::vector<std::string> effects;  // List of side effects
    bool continuousAction;   // Whether this action should persist until canceled
    float cooldownTime;      // Time before this action can be used again
    
    ActionResult() 
        : reward(0.0f), success(false), message("")
        , energyCost(0.0f), healthChange(0.0f), learningValue(0.0f)
        , actionType("None"), continuousAction(false), cooldownTime(0.0f) {}
    
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m)
        , energyCost(0.0f), healthChange(0.0f), learningValue(0.0f)
        , actionType("BasicAction"), continuousAction(false), cooldownTime(0.0f) {}
    
    // Enhanced constructor for expert use
    ActionResult(float r, bool s, const std::string& m, 
                 float eCost, float hChange, float lValue,
                 const std::string& aType = "AdvancedAction")
        : reward(r), success(s), message(m)
        , energyCost(eCost), healthChange(hChange), learningValue(lValue)
        , actionType(aType), continuousAction(false), cooldownTime(0.0f) {}
};

} // namespace nlm
