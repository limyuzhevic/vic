#include "Action.hpp"

namespace nlm {

Action::Action() : type_(ActionType::Wait) {}

Action::Action(ActionType type) : type_(type) {}

Action::Action(ActionType type, const std::vector<float>& params) 
    : type_(type), parameters_(params) {}

Action::~Action() = default;

ActionType Action::getType() const {
    return type_;
}

void Action::setType(ActionType type) {
    type_ = type;
}

const std::vector<float>& Action::getParameters() const {
    return parameters_;
}

void Action::setParameters(const std::vector<float>& params) {
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
