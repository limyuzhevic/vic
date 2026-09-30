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
    
    // Gaze/observation state
    float headPitch;       // Pitch for LookUp/LookDown (0 = center, +/-1 = limits)
    float headYaw;         // Yaw for LookLeft/LookRight (0 = center)
    
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
    bool onGround;
    bool climbing;
    bool falling;
    bool grabbing;
    float lastActionTime;
    float climbProgress;
    float fallTimer;
    
    // Object manipulation state
    WorldObjectType* grabbedObject;  // Pointer to object being grabbed
    float grabbedValue;              // Value/energy of grabbed object
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , headPitch(0.0f), headYaw(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), onGround(true)
        , climbing(false), falling(false), grabbing(false), lastActionTime(0.0f)
        , climbProgress(0.0f), fallTimer(0.0f)
        , grabbedObject(nullptr), grabbedValue(0.0f) {}
    
    void reset() {
        x = 0.0f;
        y = 0.0f;
        orientation = 0.0f;
        headPitch = 0.0f;
        headYaw = 0.0f;
        velocityX = 0.0f;
        velocityY = 0.0f;
        angularVelocity = 0.0f;
        energy = 1.0f;
        health = 1.0f;
        age = 0.0f;
        isMoving = false;
        isTurning = false;
        onGround = true;
        climbing = false;
        falling = false;
        grabbing = false;
        lastActionTime = 0.0f;
        climbProgress = 0.0f;
        fallTimer = 0.0f;
        grabbedObject = nullptr;
        grabbedValue = 0.0f;
    }
};

// Low-level motor commands that the brain can produce
// These are innate muscle-like signals, not semantic actions
enum class MotorCommand {
    // Movement commands
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    MoveLeft = 2,      // Strafe left
    MoveRight = 3,     // Strafe right
    TurnLeft = 4,      // Rotate left
    TurnRight = 5,     // Rotate right
    
    // Gaze/observation commands
    LookLeft = 6,      // Pan sensor left
    LookRight = 7,     // Pan sensor right
    LookUp = 8,        // Tilt sensor up
    LookDown = 9,      // Tilt sensor down
    
    // Dynamic movement commands
    Jump = 10,         // Jump (temporary boost)
    Climb = 11,        // Climb up/forward
    Fall = 12,         // Fall/gravity
    Roll = 13,         // Roll/slide
    
    // Object manipulation commands
    Grab = 14,         // Grab/hold object
    Release = 15,      // Release object
    Push = 16,         // Push object
    Pull = 17,         // Pull object
    Lift = 18,         // Lift object
    Drop = 19,         // Drop object
    
    // Basic interaction commands
    Interact = 20,     // Interact with nearby object
    Wait = 21          // No movement
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
