#include "core/Conversions.hpp"
#include "core/Constants.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Motor command and action type conversion implementation
// Replaces repetitive switch statements across the codebase

// Motor command to action type conversion table
static const std::unordered_map<MotorCommand, ActionType> MOTOR_COMMAND_TO_ACTION = {
    {MotorCommand::MoveForward, ActionType::MoveForward},
    {MotorCommand::MoveBackward, ActionType::MoveBackward},
    {MotorCommand::TurnLeft, ActionType::TurnLeft},
    {MotorCommand::TurnRight, ActionType::TurnRight},
    {MotorCommand::Interact, ActionType::Interact},
    {MotorCommand::LookLeft, ActionType::LookLeft},
    {MotorCommand::LookRight, ActionType::LookRight},
    {MotorCommand::Wait, ActionType::Wait}
};

// Action type to motor command conversion table
static const std::unordered_map<ActionType, MotorCommand> ACTION_TO_MOTOR_COMMAND = {
    {ActionType::MoveForward, MotorCommand::MoveForward},
    {ActionType::MoveBackward, MotorCommand::MoveBackward},
    {ActionType::TurnLeft, MotorCommand::TurnLeft},
    {ActionType::TurnRight, MotorCommand::TurnRight},
    {ActionType::Interact, MotorCommand::Interact},
    {ActionType::LookLeft, MotorCommand::LookLeft},
    {ActionType::LookRight, MotorCommand::LookRight},
    {ActionType::Wait, MotorCommand::Wait}
};

// Motor command string representations
static const std::unordered_map<MotorCommand, std::string> MOTOR_COMMAND_STRINGS = {
    {MotorCommand::MoveForward, "MoveForward"},
    {MotorCommand::MoveBackward, "MoveBackward"},
    {MotorCommand::TurnLeft, "TurnLeft"},
    {MotorCommand::TurnRight, "TurnRight"},
    {MotorCommand::Interact, "Interact"},
    {MotorCommand::LookLeft, "LookLeft"},
    {MotorCommand::LookRight, "LookRight"},
    {MotorCommand::Wait, "Wait"}
};

// Action type string representations
static const std::unordered_map<ActionType, std::string> ACTION_TYPE_STRINGS = {
    {ActionType::MoveForward, "MoveForward"},
    {ActionType::MoveBackward, "MoveBackward"},
    {ActionType::TurnLeft, "TurnLeft"},
    {ActionType::TurnRight, "TurnRight"},
    {ActionType::Interact, "Interact"},
    {ActionType::LookLeft, "LookLeft"},
    {ActionType::LookRight, "LookRight"},
    {ActionType::Wait, "Wait"}
};

// String to motor command lookup table
static const std::unordered_map<std::string, MotorCommand> STRING_TO_MOTOR_COMMAND = {
    {"MoveForward", MotorCommand::MoveForward},
    {"MoveBackward", MotorCommand::MoveBackward},
    {"TurnLeft", MotorCommand::TurnLeft},
    {"TurnRight", MotorCommand::TurnRight},
    {"Interact", MotorCommand::Interact},
    {"LookLeft", MotorCommand::LookLeft},
    {"LookRight", MotorCommand::LookRight},
    {"Wait", MotorCommand::Wait}
};

// String to action type lookup table
static const std::unordered_map<std::string, ActionType> STRING_TO_ACTION_TYPE = {
    {"MoveForward", ActionType::MoveForward},
    {"MoveBackward", ActionType::MoveBackward},
    {"TurnLeft", ActionType::TurnLeft},
    {"TurnRight", ActionType::TurnRight},
    {"Interact", ActionType::Interact},
    {"LookLeft", ActionType::LookLeft},
    {"LookRight", ActionType::LookRight},
    {"Wait", ActionType::Wait}
};

// Static vectors for easy access
static const std::vector<MotorCommand> ALL_MOTOR_COMMANDS = {
    MotorCommand::MoveForward,
    MotorCommand::MoveBackward,
    MotorCommand::TurnLeft,
    MotorCommand::TurnRight,
    MotorCommand::Interact,
    MotorCommand::LookLeft,
    MotorCommand::LookRight,
    MotorCommand::Wait
};

static const std::vector<ActionType> ALL_ACTION_TYPES = {
    ActionType::MoveForward,
    ActionType::MoveBackward,
    ActionType::TurnLeft,
    ActionType::TurnRight,
    ActionType::Interact,
    ActionType::LookLeft,
    ActionType::LookRight,
    ActionType::Wait
};

ActionType motorCommandToActionType(MotorCommand command) {
    auto it = MOTOR_COMMAND_TO_ACTION.find(command);
    return (it != MOTOR_COMMAND_TO_ACTION.end()) ? it->second : ActionType::Wait;
}

MotorCommand actionTypeToMotorCommand(ActionType action) {
    auto it = ACTION_TO_MOTOR_COMMAND.find(action);
    return (it != ACTION_TO_MOTOR_COMMAND.end()) ? it->second : MotorCommand::Wait;
}

std::string motorCommandToString(MotorCommand command) {
    auto it = MOTOR_COMMAND_STRINGS.find(command);
    return (it != MOTOR_COMMAND_STRINGS.end()) ? it->second : "";
}

std::string actionTypeToString(ActionType action) {
    auto it = ACTION_TYPE_STRINGS.find(action);
    return (it != ACTION_TYPE_STRINGS.end()) ? it->second : "";
}

MotorCommand stringToMotorCommand(const std::string& str) {
    auto it = STRING_TO_MOTOR_COMMAND.find(str);
    return (it != STRING_TO_MOTOR_COMMAND.end()) ? it->second : MotorCommand::Wait;
}

ActionType stringToActionType(const std::string& str) {
    auto it = STRING_TO_ACTION_TYPE.find(str);
    return (it != STRING_TO_ACTION_TYPE.end()) ? it->second : ActionType::Wait;
}

bool motorCommandsEqual(MotorCommand cmd1, MotorCommand cmd2) {
    return cmd1 == cmd2;
}

bool actionTypesEqual(ActionType act1, ActionType act2) {
    return act1 == act2;
}

size_t getMotorCommandCount() {
    return ALL_MOTOR_COMMANDS.size();
}

size_t getActionTypeCount() {
    return ALL_ACTION_TYPES.size();
}

const std::vector<MotorCommand>& getAllMotorCommands() {
    return ALL_MOTOR_COMMANDS;
}

const std::vector<ActionType>& getAllActionTypes() {
    return ALL_ACTION_TYPES;
}

bool isMovementCommand(MotorCommand command) {
    return command == MotorCommand::MoveForward || 
           command == MotorCommand::MoveBackward;
}

bool isInteractCommand(MotorCommand command) {
    return command == MotorCommand::Interact;
}

bool isWaitCommand(MotorCommand command) {
    return command == MotorCommand::Wait;
}

std::vector<MotorCommand> getMovementCommands() {
    std::vector<MotorCommand> movementCommands;
    movementCommands.push_back(MotorCommand::MoveForward);
    movementCommands.push_back(MotorCommand::MoveBackward);
    movementCommands.push_back(MotorCommand::TurnLeft);
    movementCommands.push_back(MotorCommand::TurnRight);
    return movementCommands;
}

std::vector<MotorCommand> getInteractCommands() {
    std::vector<MotorCommand> interactCommands;
    interactCommands.push_back(MotorCommand::Interact);
    return interactCommands;
}

MotorCommand normalizeMotorCommand(MotorCommand command) {
    return isValidMotorCommand(command) ? command : MotorCommand::Wait;
}

ActionType normalizeActionType(ActionType action) {
    return isValidActionType(action) ? action : ActionType::Wait;
}

bool isValidMotorCommand(MotorCommand command) {
    return MOTOR_COMMAND_TO_ACTION.find(command) != MOTOR_COMMAND_TO_ACTION.end();
}

bool isValidActionType(ActionType action) {
    return ACTION_TO_MOTOR_COMMAND.find(action) != ACTION_TO_MOTOR_COMMAND.end();
}

std::string motorCommandToDescription(MotorCommand command) {
    switch (command) {
        case MotorCommand::MoveForward: return "Move forward in the current direction";
        case MotorCommand::MoveBackward: return "Move backward in the current direction";
        case MotorCommand::TurnLeft: return "Rotate left by 90 degrees";
        case MotorCommand::TurnRight: return "Rotate right by 90 degrees";
        case MotorCommand::Interact: return "Interact with nearby object";
        case MotorCommand::LookLeft: return "Look to the left";
        case MotorCommand::LookRight: return "Look to the right";
        case MotorCommand::Wait: return "Do nothing, wait for next step";
        default: return "Unknown command";
    }
}

std::string actionTypeToDescription(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return "Move forward in the current direction";
        case ActionType::MoveBackward: return "Move backward in the current direction";
        case ActionType::TurnLeft: return "Rotate left by 90 degrees";
        case ActionType::TurnRight: return "Rotate right by 90 degrees";
        case ActionType::Interact: return "Interact with nearby object";
        case ActionType::LookLeft: return "Look to the left";
        case ActionType::LookRight: return "Look to the right";
        case ActionType::Wait: return "Do nothing, wait for next step";
        default: return "Unknown action";
    }
}

bool tryParseMotorCommand(const std::string& str, MotorCommand& out) {
    auto it = STRING_TO_MOTOR_COMMAND.find(str);
    if (it != STRING_TO_MOTOR_COMMAND.end()) {
        out = it->second;
        return true;
    }
    return false;
}

bool tryParseActionType(const std::string& str, ActionType& out) {
    auto it = STRING_TO_ACTION_TYPE.find(str);
    if (it != STRING_TO_ACTION_TYPE.end()) {
        out = it->second;
        return true;
    }
    return false;
}

MotorCommandCategory getMotorCommandCategory(MotorCommand command) {
    if (isMovementCommand(command)) return Move;
    if (isInteractCommand(command)) return Interact;
    if (isWaitCommand(command)) return Wait;
    if (command == MotorCommand::LookLeft || command == MotorCommand::LookRight) return LookLeft;
    return Turn;
}

float motorCommandSimilarity(MotorCommand cmd1, MotorCommand cmd2) {
    if (cmd1 == cmd2) return 1.0f;
    if ((cmd1 == MotorCommand::MoveForward && cmd2 == MotorCommand::MoveBackward) ||
        (cmd1 == MotorCommand::MoveBackward && cmd2 == MotorCommand::MoveForward)) return 0.3f;
    if ((cmd1 == MotorCommand::TurnLeft && cmd2 == MotorCommand::TurnRight) ||
        (cmd1 == MotorCommand::TurnRight && cmd2 == MotorCommand::TurnLeft)) return 0.3f;
    return 0.0f;
}

float actionTypeSimilarity(ActionType act1, ActionType act2) {
    if (act1 == act2) return 1.0f;
    if ((act1 == ActionType::MoveForward && act2 == ActionType::MoveBackward) ||
        (act1 == ActionType::MoveBackward && act2 == ActionType::MoveForward)) return 0.3f;
    if ((act1 == ActionType::TurnLeft && act2 == ActionType::TurnRight) ||
        (act1 == ActionType::TurnRight && act2 == ActionType::TurnLeft)) return 0.3f;
    return 0.0f;
}

MotorCommand toMovementCommand(MotorCommand command) {
    return isMovementCommand(command) ? command : MotorCommand::Wait;
}

MotorCommand toTurnCommand(MotorCommand command) {
    if (command == MotorCommand::TurnLeft || command == MotorCommand::TurnRight) {
        return command;
    }
    return MotorCommand::Wait;
}

MotorCommand legacyMotorCommandFix(MotorCommand command) {
    // Legacy compatibility fixes
    return command;  // Currently no fixes needed
}

ActionType legacyActionTypeFix(ActionType action) {
    // Legacy compatibility fixes
    return action;  // Currently no fixes needed
}

std::vector<MotorCommand> rotateCommandSequence(const std::vector<MotorCommand>& sequence, size_t rotation) {
    std::vector<MotorCommand> result = sequence;
    if (rotation >= result.size()) rotation = rotation % result.size();
    std::rotate(result.begin(), result.begin() + rotation, result.end());
    return result;
}

bool containsCommand(const std::vector<MotorCommand>& sequence, MotorCommand command) {
    return std::find(sequence.begin(), sequence.end(), command) != sequence.end();
}

size_t countMovementCommands(const std::vector<MotorCommand>& sequence) {
    return (size_t)std::count_if(sequence.begin(), sequence.end(), 
        [](MotorCommand cmd) { return isMovementCommand(cmd); });
}

bool matchesPattern(const std::vector<MotorCommand>& sequence, const std::vector<MotorCommand>& pattern) {
    if (pattern.size() > sequence.size()) return false;
    
    for (size_t i = 0; i <= sequence.size() - pattern.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < pattern.size(); ++j) {
            if (sequence[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

std::vector<MotorCommand> findFirstMatch(const std::vector<MotorCommand>& sequence, const std::vector<MotorCommand>& pattern) {
    std::vector<MotorCommand> result;
    if (pattern.size() > sequence.size()) return result;
    
    for (size_t i = 0; i <= sequence.size() - pattern.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < pattern.size(); ++j) {
            if (sequence[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            result.assign(sequence.begin() + i, sequence.begin() + i + pattern.size());
            return result;
        }
    }
    return result;
}

size_t countCommandFrequency(const std::vector<MotorCommand>& sequence, MotorCommand command) {
    return (size_t)std::count(sequence.begin(), sequence.end(), command);
}

float calculateCommandDiversity(const std::vector<MotorCommand>& sequence) {
    if (sequence.empty()) return 0.0f;
    
    std::vector<MotorCommand> uniqueCommands;
    for (MotorCommand cmd : sequence) {
        if (std::find(uniqueCommands.begin(), uniqueCommands.end(), cmd) == uniqueCommands.end()) {
            uniqueCommands.push_back(cmd);
        }
    }
    
    return (float)uniqueCommands.size() / (float)sequence.size();
}

MotorCommand getMostFrequentCommand(const std::vector<MotorCommand>& sequence) {
    if (sequence.empty()) return MotorCommand::Wait;
    
    std::unordered_map<MotorCommand, size_t> frequency;
    for (MotorCommand cmd : sequence) {
        frequency[cmd]++;
    }
    
    MotorCommand mostFrequent = MotorCommand::Wait;
    size_t maxCount = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostFrequent = pair.first;
        }
    }
    
    return mostFrequent;
}

std::string encodeCommandSequence(const std::vector<MotorCommand>& sequence) {
    std::string encoded;
    for (MotorCommand cmd : sequence) {
        encoded += motorCommandToString(cmd) + ",";
    }
    if (!encoded.empty()) encoded.pop_back();  // Remove trailing comma
    return encoded;
}

bool decodeCommandSequence(const std::string& encoded, std::vector<MotorCommand>& out) {
    out.clear();
    if (encoded.empty()) return true;
    
    size_t pos = 0;
    size_t start = 0;
    while (pos < encoded.size()) {
        pos = encoded.find(',', pos);
        if (pos == std::string::npos) {
            // Last element
            std::string token = encoded.substr(start);
            MotorCommand cmd = stringToMotorCommand(token);
            if (cmd == MotorCommand::Wait && token != "Wait") return false;  // Failed to parse
            out.push_back(cmd);
            break;
        } else {
            std::string token = encoded.substr(start, pos - start);
            MotorCommand cmd = stringToMotorCommand(token);
            if (cmd == MotorCommand::Wait && token != "Wait") return false;  // Failed to parse
            out.push_back(cmd);
            start = pos + 1;
            pos = start;
        }
    }
    return true;
}

void analyzeCommandSequence(const std::vector<MotorCommand>& sequence) {
    std::cout << "Command sequence analysis:" << std::endl;
    std::cout << "  Length: " << sequence.size() << std::endl;
    std::cout << "  Unique commands: " << calculateCommandDiversity(sequence) << std::endl;
    std::cout << "  Most frequent: " << motorCommandToString(getMostFrequentCommand(sequence)) << std::endl;
    std::cout << "  Movement commands: " << countMovementCommands(sequence) << std::endl;
}

std::string commandSequenceToString(const std::vector<MotorCommand>& sequence) {
    std::string result;
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (i > 0) result += ", ";
        result += motorCommandToString(sequence[i]);
    }
    return result;
}

} // namespace nlm
