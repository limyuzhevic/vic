#include "Action.hpp"
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

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

std::string Action::toString() const {
    std::ostringstream oss;
    oss << actionTypeToString(type_);
    
    if (!parameters_.empty()) {
        oss << "(";
        for (size_t i = 0; i < parameters_.size(); ++i) {
            oss << parameters_[i];
            if (i < parameters_.size() - 1) {
                oss << ",";
            }
        }
        oss << ")";
    }
    
    return oss.str();
}

std::unique_ptr<Action> Action::fromString(const std::string& str) {
    try {
        std::istringstream iss(str);
        std::string actionName;
        std::getline(iss, actionName, '(');
        
        // Trim whitespace
        actionName.erase(0, actionName.find_first_not_of(" \t\n\r"));
        actionName.erase(actionName.find_last_not_of(" \t\n\r") + 1);
        
        // Map string to ActionType
        ActionType type;
        if (actionName == "MoveForward") type = ActionType::MoveForward;
        else if (actionName == "MoveBackward") type = ActionType::MoveBackward;
        else if (actionName == "MoveLeft") type = ActionType::MoveLeft;
        else if (actionName == "MoveRight") type = ActionType::MoveRight;
        else if (actionName == "TurnLeft") type = ActionType::TurnLeft;
        else if (actionName == "TurnRight") type = ActionType::TurnRight;
        else if (actionName == "Look") type = ActionType::Look;
        else if (actionName == "LookUp") type = ActionType::LookUp;
        else if (actionName == "LookDown") type = ActionType::LookDown;
        else if (actionName == "Interact") type = ActionType::Interact;
        else if (actionName == "Eat") type = ActionType::Eat;
        else if (actionName == "Drink") type = ActionType::Drink;
        else if (actionName == "Rest") type = ActionType::Rest;
        else if (actionName == "Wait") type = ActionType::Wait;
        else if (actionName == "Custom") type = ActionType::Custom;
        else {
            // Return null action for unknown type
            return nullptr;
        }
        
        // Extract parameters if present
        std::vector<float> parameters;
        size_t parenPos = str.find('(');
        if (parenPos != std::string::npos) {
            std::string paramsStr = str.substr(parenPos + 1);
            paramsStr = paramsStr.substr(0, paramsStr.find(')'));
            
            std::istringstream paramStream(paramsStr);
            std::string param;
            while (std::getline(paramStream, param, ',')) {
                try {
                    parameters.push_back(std::stof(param));
                } catch (...) {
                    // Skip invalid parameters
                }
            }
        }
        
        return std::make_unique<Action>(type, parameters);
        
    } catch (...) {
        // Return null action for any parsing error
        return nullptr;
    }
}

std::unique_ptr<Action> Action::clone() const {
    return std::make_unique<Action>(type_, parameters_);
}

} // namespace nlm
