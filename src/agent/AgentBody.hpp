#pragma once

#include <cstdint>
#include <string>

namespace nlm {

// Agent body state - contains physical properties and internal state
// This is part of the environment, not the brain
// The AgentBody represents the physical agent's properties and state in the world
// It tracks position, movement, energy, health, and developmental age
struct AgentBody {
    // Spatial position in 2D world coordinates (meters)
    float x;                    // X coordinate (east-west)
    float y;                    // Y coordinate (north-south)
    
    // Orientation in radians, where 0 = positive X axis, PI/2 = positive Y axis
    float orientation;           // Facing direction relative to world (radians)
    
    // Linear velocity components in world coordinates
    float velocityX;             // Velocity in X direction (m/s)
    float velocityY;             // Velocity in Y direction (m/s)
    float angularVelocity;       // Angular velocity (rad/s)
    
    // Biological/metabolic state variables
    float energy;               // Energy level (0.0 = depleted, 1.0 = full)
    float health;               // Health status (0.0 = dead, 1.0 = perfect)
    float age;                  // Developmental age in simulation steps
    
    // Behavioral state flags
    bool isMoving;              // Whether agent is currently moving
    bool isTurning;             // Whether agent is currently turning
    float lastActionTime;       // Timestamp of last action taken
    
    // Default constructor initializes agent at origin with full health
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    // Reset agent to initial state (full health, at origin)
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
