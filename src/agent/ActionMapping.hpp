#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Motor command types for agent-brain interface
// These are low-level motor commands that the brain decodes into actions
enum class MotorCommand {
    MoveForward,     // Propel forward
    MoveBackward,    // Propel backward  
    TurnLeft,        // Rotate left
    TurnRight,       // Rotate right
    LookLeft,        // Pan sensor left
    LookRight,       // Pan sensor right
    Interact,        // Interact with nearby object
    Wait             // No movement
};

// Action types for semantic actions
// These are the actual actions the agent can take in the environment
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

// Convert between ActionType and MotorCommand
inline MotorCommand actionTypeToMotorCommand(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::MoveLeft: return MotorCommand::TurnLeft;
        case ActionType::MoveRight: return MotorCommand::TurnRight;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::Look: return MotorCommand::LookLeft;
        case ActionType::LookUp: return MotorCommand::LookLeft;  // Simplified
        case ActionType::LookDown: return MotorCommand::LookRight;  // Simplified
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Eat: return MotorCommand::Interact;  // Simplified
        case ActionType::Drink: return MotorCommand::Interact;  // Simplified
        case ActionType::Rest: return MotorCommand::Wait;
        case ActionType::Wait: return MotorCommand::Wait;
        case ActionType::Custom: return MotorCommand::Wait;
    }
    return MotorCommand::Wait;
}

inline ActionType motorCommandToActionType(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return ActionType::MoveForward;
        case MotorCommand::MoveBackward: return ActionType::MoveBackward;
        case MotorCommand::TurnLeft: return ActionType::TurnLeft;
        case MotorCommand::TurnRight: return ActionType::TurnRight;
        case MotorCommand::LookLeft: return ActionType::Look;
        case MotorCommand::LookRight: return ActionType::Look;
        case MotorCommand::Interact: return ActionType::Interact;
        case MotorCommand::Wait: return ActionType::Wait;
    }
    return ActionType::Wait;
}

} // namespace nlm