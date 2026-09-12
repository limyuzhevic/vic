#include "Action.hpp"
#include <algorithm>

namespace nlm {

std::unique_ptr<Action> Action::create(ActionType type) {
    return std::make_unique<Action>(type);
}

std::unique_ptr<Action> Action::create(ActionType type, const std::vector<float>& params) {
    return std::make_unique<Action>(type, params);
}

std::unique_ptr<const Action> Action::createConst(ActionType type) {
    return std::make_unique<const Action>(type);
}

std::unique_ptr<const Action> Action::createConst(ActionType type, const std::vector<float>& params) {
    return std::make_unique<const Action>(type, params);
}

std::vector<ActionType> Action::getAllActionTypes() {
    return {
        ActionType::MoveForward,
        ActionType::MoveBackward,
        ActionType::MoveLeft,
        ActionType::MoveRight,
        ActionType::TurnLeft,
        ActionType::TurnRight,
        ActionType::Look,
        ActionType::LookUp,
        ActionType::LookDown,
        ActionType::Interact,
        ActionType::Eat,
        ActionType::Drink,
        ActionType::Rest,
        ActionType::Wait,
        ActionType::Custom
    };
}

std::string Action::toString(ActionType type) {
    return std::string(actionTypeToString(type));
}

ActionType Action::fromString(const std::string& str) {
    for (auto type : getAllActionTypes()) {
        if (std::string(actionTypeToString(type)) == str) {
            return type;
        }
    }
    return ActionType::Wait; // Default
}

bool Action::isMovementAction(ActionType type) {
    return type == ActionType::MoveForward ||
           type == ActionType::MoveBackward ||
           type == ActionType::MoveLeft ||
           type == ActionType::MoveRight;
}

bool Action::isRotationAction(ActionType type) {
    return type == ActionType::TurnLeft ||
           type == ActionType::TurnRight;
}

bool Action::isInteractionAction(ActionType type) {
    return type == ActionType::Interact ||
           type == ActionType::Eat ||
           type == ActionType::Drink ||
           type == ActionType::Rest;
}

int Action::getParameterCount(ActionType type) {
    switch (type) {
        case ActionType::Look:
        case ActionType::LookUp:
        case ActionType::LookDown:
        case ActionType::Custom:
            return 1;
        default:
            return 0;
    }
}

} // namespace nlm
