// utils/ActionConversion.cpp - Implementation of ActionConversion utility

#include "ActionConversion.hpp"
#include <stdexcept>
#include <string>

namespace nlm {

namespace {
    // Private helper to create conversion mappings
    const std::unordered_map<MotorCommand, ActionType> createMotorToActionMap() {
        std::unordered_map<MotorCommand, ActionType> map;
        map[MotorCommand::MoveForward] = ActionType::MoveForward;
        map[MotorCommand::MoveBackward] = ActionType::MoveBackward;
        map[MotorCommand::TurnLeft] = ActionType::TurnLeft;
        map[MotorCommand::TurnRight] = ActionType::TurnRight;
        map[MotorCommand::LookLeft] = ActionType::Look;
        map[MotorCommand::LookRight] = ActionType::Look;
        map[MotorCommand::Interact] = ActionType::Interact;
        map[MotorCommand::Wait] = ActionType::Wait;
        return map;
    }

    const std::unordered_map<ActionType, MotorCommand> createActionToMotorMap() {
        std::unordered_map<ActionType, MotorCommand> map;
        map[ActionType::MoveForward] = MotorCommand::MoveForward;
        map[ActionType::MoveBackward] = MotorCommand::MoveBackward;
        map[ActionType::MoveLeft] = MotorCommand::TurnLeft;  // MoveLeft = TurnLeft for compatibility
        map[ActionType::MoveRight] = MotorCommand::TurnRight; // MoveRight = TurnRight for compatibility
        map[ActionType::TurnLeft] = MotorCommand::TurnLeft;
        map[ActionType::TurnRight] = MotorCommand::TurnRight;
        map[ActionType::Look] = MotorCommand::Wait;        // Look has no direct MotorCommand equivalent
        map[ActionType::LookUp] = MotorCommand::Wait;     // LookUp has no direct MotorCommand equivalent
        map[ActionType::LookDown] = MotorCommand::Wait;   // LookDown has no direct MotorCommand equivalent
        map[ActionType::Interact] = MotorCommand::Interact;
        map[ActionType::Rest] = MotorCommand::Wait;
        map[ActionType::Wait] = MotorCommand::Wait;
        map[ActionType::Custom] = MotorCommand::Wait;
        return map;
    }

    const std::unordered_map<ActionType, std::string> createActionTypeStrings() {
        std::unordered_map<ActionType, std::string> map;
        map[ActionType::MoveForward] = "MoveForward";
        map[ActionType::MoveBackward] = "MoveBackward";
        map[ActionType::MoveLeft] = "MoveLeft";
        map[ActionType::MoveRight] = "MoveRight";
        map[ActionType::TurnLeft] = "TurnLeft";
        map[ActionType::TurnRight] = "TurnRight";
        map[ActionType::Look] = "Look";
        map[ActionType::LookUp] = "LookUp";
        map[ActionType::LookDown] = "LookDown";
        map[ActionType::Interact] = "Interact";
        map[ActionType::Rest] = "Rest";
        map[ActionType::Wait] = "Wait";
        map[ActionType::Custom] = "Custom";
        return map;
    }

    const std::unordered_map<MotorCommand, std::string> createMotorCommandStrings() {
        std::unordered_map<MotorCommand, std::string> map;
        map[MotorCommand::MoveForward] = "MoveForward";
        map[MotorCommand::MoveBackward] = "MoveBackward";
        map[MotorCommand::TurnLeft] = "TurnLeft";
        map[MotorCommand::TurnRight] = "TurnRight";
        map[MotorCommand::LookLeft] = "LookLeft";
        map[MotorCommand::LookRight] = "LookRight";
        map[MotorCommand::Interact] = "Interact";
        map[MotorCommand::Wait] = "Wait";
        return map;
    }
}

// MotorCommand -> ActionType conversion
ActionType ActionConversion::motorCommandToActionType(MotorCommand cmd) {
    const auto& map = getMotorToActionMap();
    auto it = map.find(cmd);
    if (it == map.end()) {
        throw std::invalid_argument("Unknown MotorCommand: " + std::to_string(static_cast<int>(cmd)));
    }
    return it->second;
}

// ActionType -> MotorCommand conversion
MotorCommand ActionConversion::actionTypeToMotorCommand(ActionType action) {
    const auto& map = getActionToMotorMap();
    auto it = map.find(action);
    if (it == map.end()) {
        throw std::invalid_argument("Unknown ActionType: " + std::to_string(static_cast<int>(action)));
    }
    return it->second;
}

// String conversion utilities
std::string ActionConversion::actionTypeToString(ActionType action) {
    const auto& map = getActionTypeStrings();
    auto it = map.find(action);
    if (it == map.end()) {
        return "Unknown";
    }
    return it->second;
}

std::string ActionConversion::motorCommandToString(MotorCommand cmd) {
    const auto& map = getMotorCommandStrings();
    auto it = map.find(cmd);
    if (it == map.end()) {
        return "Unknown";
    }
    return it->second;
}

// Validation utilities
bool ActionConversion::isValidAction(ActionType action) {
    return getActionToMotorMap().find(action) != getActionToMotorMap().end();
}

bool ActionConversion::isValidMotorCommand(MotorCommand cmd) {
    return getMotorToActionMap().find(cmd) != getMotorToActionMap().end();
}

// Compatibility mappings
std::vector<MotorCommand> ActionConversion::getMotorCommandsFromAction(ActionType action) {
    const auto& map = getActionToMotorMap();
    auto it = map.find(action);
    if (it == map.end()) {
        return {};
    }
    return {it->second};
}

std::vector<ActionType> ActionConversion::getActionTypesFromMotorCommand(MotorCommand cmd) {
    const auto& map = getMotorToActionMap();
    auto it = map.find(cmd);
    if (it == map.end()) {
        return {};
    }
    return {it->second};
}

// Safe conversion with error handling
std::optional<ActionType> ActionConversion::tryMotorCommandToActionType(MotorCommand cmd) {
    try {
        return motorCommandToActionType(cmd);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<MotorCommand> ActionConversion::tryActionTypeToMotorCommand(ActionType action) {
    try {
        return actionTypeToMotorCommand(action);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

// Convenience conversion functions
ActionType toActionType(MotorCommand cmd) {
    return ActionConversion::motorCommandToActionType(cmd);
}

MotorCommand toMotorCommand(ActionType action) {
    return ActionConversion::actionTypeToMotorCommand(action);
}

std::string toString(ActionType action) {
    return ActionConversion::actionTypeToString(action);
}

std::string toString(MotorCommand cmd) {
    return ActionConversion::motorCommandToString(cmd);
}

// Validation and safety checks
bool isCompatible(ActionType action, MotorCommand cmd) {
    return ActionConversion::tryMotorCommandToActionType(cmd) == action &&
           ActionConversion::tryActionTypeToMotorCommand(action) == cmd;
}

bool isCompatible(MotorCommand cmd, ActionType action) {
    return ActionConversion::tryMotorCommandToActionType(cmd) == action &&
           ActionConversion::tryActionTypeToMotorCommand(action) == cmd;
}

} // namespace nlm
