#include "Action.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

Action::Action() : type_(ActionType::Wait) {}

Action::Action(ActionType type) : type_(type) {}

Action::Action(ActionType type, const std::vector<float>& parameters) 
    : type_(type), parameters_(parameters) {}

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

std::string Action::getName() const {
    static const std::unordered_map<ActionType, std::string> typeNames = {
        {ActionType::MoveForward, "MoveForward"},
        {ActionType::MoveBackward, "MoveBackward"},
        {ActionType::MoveLeft, "MoveLeft"},
        {ActionType::MoveRight, "MoveRight"},
        {ActionType::TurnLeft, "TurnLeft"},
        {ActionType::TurnRight, "TurnRight"},
        {ActionType::Look, "Look"},
        {ActionType::LookUp, "LookUp"},
        {ActionType::LookDown, "LookDown"},
        {ActionType::Interact, "Interact"},
        {ActionType::Eat, "Eat"},
        {ActionType::Drink, "Drink"},
        {ActionType::Rest, "Rest"},
        {ActionType::Wait, "Wait"},
        {ActionType::Custom, "Custom"}
    };
    
    auto it = typeNames.find(type_);
    return it != typeNames.end() ? it->second : "Unknown";
}

std::unique_ptr<Action> Action::clone() const {
    return std::make_unique<Action>(type_, parameters_);
}

} // namespace nlm
