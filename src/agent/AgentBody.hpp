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
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
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

// Action result error codes
enum class ActionResultCode {
    Success = 0,
    InvalidAction = 1,
    OutOfRange = 2,
    Collision = 3,
    NoEnergy = 4,
    InternalError = 5,
    Unauthorized = 6,
    InvalidState = 7,
    Timeout = 8
};

// Action result from the world\'s perspective
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    ActionResultCode errorCode;
    
    ActionResult() : reward(0.0f), success(false), message("") , errorCode(ActionResultCode::InternalError) {}
    ActionResult(float r, bool s, const std::string& m = "", ActionResultCode ec = ActionResultCode::InternalError) 
        : reward(r), success(s), message(m), errorCode(ec) {}
    
    bool isSuccess() const { return success && errorCode == ActionResultCode::Success; }
    bool isFailure() const { return !success; }
    
    static const char* getErrorDescription(ActionResultCode code) {
        switch (code) {
            case ActionResultCode::Success: return "Success";
            case ActionResultCode::InvalidAction: return "Invalid action specified";
            case ActionResultCode::OutOfRange: return "Action parameters out of range";
            case ActionResultCode::Collision: return "Collision with obstacle";
            case ActionResultCode::NoEnergy: return "Insufficient energy";
            case ActionResultCode::InternalError: return "Internal system error";
            case ActionResultCode::Unauthorized: return "Unauthorized action";
            case ActionResultCode::InvalidState: return "Invalid agent or world state";
            case ActionResultCode::Timeout: return "Action timed out";
            default: return "Unknown error";
        }
    }
    
    const char* getErrorDescription() const { return getErrorDescription(errorCode); }
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
