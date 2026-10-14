#pragma once

#include "Command.hpp"
#include "Brain.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class BrainCommand;

// CommandManager - Manages command execution and undo/redo functionality
class CommandManager {
public:
    CommandManager() = default;
    ~CommandManager() = default;
    
    // Execute a command
    void execute(std::unique_ptr<BrainCommand> command);
    
    // Undo last command if possible
    bool undo();
    
    // Redo last undone command
    bool redo();
    
    // Clear command history
    void clearHistory();
    
    // Check if undo is available
    bool canUndo() const { return !undoStack.empty(); }
    
    // Check if redo is available
    bool canRedo() const { return !redoStack.empty(); }
    
    // Get command by name
    std::unique_ptr<BrainCommand> getCommand(const std::string& name) const;
    
    // Get all commands of a specific category
    std::vector<std::unique_ptr<BrainCommand>> getCommandsByCategory(CommandCategory category) const;
    
    // Get command history
    const std::vector<std::unique_ptr<BrainCommand>>& getHistory() const { return commandHistory; }
    
private:
    std::vector<std::unique_ptr<BrainCommand>> commandHistory;
    std::vector<std::unique_ptr<BrainCommand>> undoStack;
    std::vector<std::unique_ptr<BrainCommand>> redoStack;
    
    // Maximum history size to prevent memory issues
    static constexpr size_t MAX_HISTORY_SIZE = 10000;
};

} // namespace nlm
