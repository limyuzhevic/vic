#pragma once

#include <string>
#include <vector>
#include "core/Types/Types.hpp"

namespace nlm {

// Motor command and action type conversion utilities
// Eliminates repetitive switch statements across the codebase

// Convert MotorCommand to ActionType
ActionType motorCommandToActionType(MotorCommand command);

// Convert ActionType to MotorCommand  
MotorCommand actionTypeToMotorCommand(ActionType action);

// String representation utilities
std::string motorCommandToString(MotorCommand command);
std::string actionTypeToString(ActionType action);

MotorCommand stringToMotorCommand(const std::string& str);
ActionType stringToActionType(const std::string& str);

// Motor command comparison and hashing
bool motorCommandsEqual(MotorCommand cmd1, MotorCommand cmd2);
bool actionTypesEqual(ActionType act1, ActionType act2);

// Motor command enumeration utilities
size_t getMotorCommandCount();
size_t getActionTypeCount();

// Get all motor commands
const std::vector<MotorCommand>& getAllMotorCommands();
const std::vector<ActionType>& getAllActionTypes();

// Check if motor command is a movement (not wait/interact)
bool isMovementCommand(MotorCommand command);
bool isInteractCommand(MotorCommand command);
bool isWaitCommand(MotorCommand command);

// Get movement commands (forward, backward, turn left/right)
std::vector<MotorCommand> getMovementCommands();

// Get interact-related commands
std::vector<MotorCommand> getInteractCommands();

// Normalize motor command (ensure valid)
MotorCommand normalizeMotorCommand(MotorCommand command);
ActionType normalizeActionType(ActionType action);

// Validate motor command
bool isValidMotorCommand(MotorCommand command);
bool isValidActionType(ActionType action);

// Get string representation for debugging
std::string motorCommandToDescription(MotorCommand command);
std::string actionTypeToDescription(ActionType action);

// Parse command from string with error checking
bool tryParseMotorCommand(const std::string& str, MotorCommand& out);
bool tryParseActionType(const std::string& str, ActionType& out);

// Command type categories
enum MotorCommandCategory {
    Move,
    Turn,
    Interact,
    Wait,
    LookLeft,
    LookRight,
    CategoryCount
};

MotorCommandCategory getMotorCommandCategory(MotorCommand command);

// Similarity and distance between commands
float motorCommandSimilarity(MotorCommand cmd1, MotorCommand cmd2);
float actionTypeSimilarity(ActionType act1, ActionType act2);

// Command conversion for different contexts
MotorCommand toMovementCommand(MotorCommand command);
MotorCommand toTurnCommand(MotorCommand command);

// Legacy compatibility functions (for deprecated APIs)
MotorCommand legacyMotorCommandFix(MotorCommand command);
ActionType legacyActionTypeFix(ActionType action);

// Command sequence utilities
std::vector<MotorCommand> rotateCommandSequence(const std::vector<MotorCommand>& sequence, size_t rotation);
bool containsCommand(const std::vector<MotorCommand>& sequence, MotorCommand command);
size_t countMovementCommands(const std::vector<MotorCommand>& sequence);

// Command pattern matching
bool matchesPattern(const std::vector<MotorCommand>& sequence, const std::vector<MotorCommand>& pattern);
std::vector<MotorCommand> findFirstMatch(const std::vector<MotorCommand>& sequence, const std::vector<MotorCommand>& pattern);

// Command statistics and analysis
size_t countCommandFrequency(const std::vector<MotorCommand>& sequence, MotorCommand command);
float calculateCommandDiversity(const std::vector<MotorCommand>& sequence);
MotorCommand getMostFrequentCommand(const std::vector<MotorCommand>& sequence);

// Command encoding/decoding for storage
std::string encodeCommandSequence(const std::vector<MotorCommand>& sequence);
bool decodeCommandSequence(const std::string& encoded, std::vector<MotorCommand>& out);

// Command utilities for simulation
bool isCommandAllowedInState(MotorCommand command, const std::string& state);
std::vector<MotorCommand> getAllowedCommands(const std::string& state);

// Command transformation utilities
std::vector<MotorCommand> transformCommands(const std::vector<MotorCommand>& commands, const std::string& transformation);

// Command validation for different contexts
bool isSafeCommand(MotorCommand command, const std::vector<float>& sensoryInput);
bool isAppropriateCommand(MotorCommand command, float curiosityLevel, float noveltyLevel);

// Command generation for planning
std::vector<MotorCommand> generateCommandSequence(size_t length, float explorationRate = 0.1f);

// Command analysis for debugging
void analyzeCommandSequence(const std::vector<MotorCommand>& sequence);
std::string commandSequenceToString(const std::vector<MotorCommand>& sequence);

} // namespace nlm
