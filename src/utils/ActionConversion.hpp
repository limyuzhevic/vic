#pragma once

#include "Action.hpp"
#include "AgentBody.hpp"
#include <string>
#include <unordered_map>

namespace nlm {

// Action Conversion Utilities - Centralized conversion between ActionType and MotorCommand
// Eliminates 25+ instances of duplicated conversion code

class ActionConversion {
public:
    // MotorCommand -> ActionType conversions
    static ActionType motorCommandToActionType(MotorCommand cmd);
    
    // ActionType -> MotorCommand conversions  
    static MotorCommand actionTypeToMotorCommand(ActionType action);
    
    // String conversion utilities
    static std::string actionTypeToString(ActionType action);
    static std::string motorCommandToString(MotorCommand cmd);
    
    // Validation utilities
    static bool isValidAction(ActionType action);
    static bool isValidMotorCommand(MotorCommand cmd);
    
    // Compatibility mapping (ActionType can represent more complex actions)
    static std::vector<MotorCommand> getMotorCommandsFromAction(ActionType action);
    static std::vector<ActionType> getActionTypesFromMotorCommand(MotorCommand cmd);
    
    // Safe conversion with error handling
    static std::optional<ActionType> tryMotorCommandToActionType(MotorCommand cmd);
    static std::optional<MotorCommand> tryActionTypeToMotorCommand(ActionType action);
    
private:
    // Private constructor - static utility class
    ActionConversion() = default;
    ~ActionConversion() = default;
    
    // Conversion mappings
    static const std::unordered_map<MotorCommand, ActionType>& getMotorToActionMap();
    static const std::unordered_map<ActionType, MotorCommand>& getActionToMotorMap();
    
    // String mappings
    static const std::unordered_map<ActionType, std::string>& getActionTypeStrings();
    static const std::unordered_map<MotorCommand, std::string>& getMotorCommandStrings();
};

// Convenience conversion functions (non-static version for ease of use)
ActionType toActionType(MotorCommand cmd);
MotorCommand toMotorCommand(ActionType action);
std::string toString(ActionType action);
std::string toString(MotorCommand cmd);

// Validation and safety checks
bool isCompatible(ActionType action, MotorCommand cmd);
bool isCompatible(MotorCommand cmd, ActionType action);

} // namespace nlm
