#pragma once

#include <cstdint>

namespace nlm {

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

// Action types for motor output
// These are semantic actions for the agent's body
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
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