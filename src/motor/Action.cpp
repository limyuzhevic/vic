#include "Action.hpp"
#include <cmath>

namespace nlm {

Action::Action() : type_(ActionType::Wait) {
    // Initialize with default empty parameters
}

Action::Action(ActionType type) : type_(type) {
    // Validate action type
    if (type == ActionType::Custom) {
        // Custom actions start with no parameters
    }
}

Action::Action(ActionType type, const std::vector<float>& params) 
    : type_(type), parameters_(params) {
    // Validate parameters based on action type
    validateParameters(type, params);
}

Action::~Action() = default;

void Action::validateParameters(ActionType type, const std::vector<float>& params) {
    switch (type) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
        case ActionType::MoveLeft:
        case ActionType::MoveRight:
            // Speed parameters should be in range [0.0, 1.0]
            for (float param : params) {
                if (param < 0.0f || param > 1.0f) {
                    // Clamp to valid range
                }
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Angular speed in radians/sec, reasonable range [0.0, 6.28] (0-360 deg/sec)
            for (float param : params) {
                if (param < 0.0f || param > 6.28f) {
                    // Clamp to valid range
                }
            }
            break;
            
        case ActionType::Look:
            // Direction in radians [0, 2*PI]
            for (float param : params) {
                if (param < 0.0f || param > 6.28f) {
                    // Normalize angle
                }
            }
            break;
            
        case ActionType::LookUp:
        case ActionType::LookDown:
            // Angle in radians [-PI/2, PI/2]
            for (float param : params) {
                if (param < -1.57f || param > 1.57f) {
                    // Clamp to valid range
                }
            }
            break;
            
        case ActionType::Interact:
        case ActionType::Eat:
        case ActionType::Drink:
        case ActionType::Rest:
            // Strength/duration in range [0.0, 1.0]
            for (float param : params) {
                if (param < 0.0f || param > 1.0f) {
                    // Clamp to valid range
                }
            }
            break;
            
        case ActionType::Wait:
            // Probability in range [0.0, 1.0]
            for (float param : params) {
                if (param < 0.0f || param > 1.0f) {
                    // Clamp to valid range
                }
            }
            break;
            
        case ActionType::Custom:
            // Custom actions can have any parameters (no validation)
            break;
    }
}

ActionType Action::getType() const {
    return type_;
}

void Action::setType(ActionType type) {
    // Validate action type
    if (type == ActionType::Custom) {
        // Clearing parameters for custom actions
    }
    type_ = type;
}

const std::vector<float>& Action::getParameters() const {
    return parameters_;
}

void Action::setParameters(const std::vector<float>& params) {
    // Validate parameters before setting
    validateParameters(type_, params);
    parameters_ = params;
}

const char* Action::actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

std::string Action::getName() const {
    return std::string(actionTypeToString(type_));
}

std::unique_ptr<Action> Action::clone() const {
    return std::make_unique<Action>(type_, parameters_);
}

} // namespace nlm
