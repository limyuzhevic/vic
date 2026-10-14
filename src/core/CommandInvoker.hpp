#pragma once

#include "Command.hpp"
#include <vector>
#include <memory>
#include <stack>
#include <queue>
#include <algorithm>

namespace nlm {

class CommandInvoker {
public:
    CommandInvoker();
    ~CommandInvoker();
    
    // Execute a command
    CommandResult execute(std::unique_ptr<Command> command);
    
    // Execute a batch of commands
    std::vector<CommandResult> executeBatch(std::vector<std::unique_ptr<Command>> commands);
    
    // Execute a sequence of commands with conditional execution
    std::vector<CommandResult> executeSequence(const std::vector<std::pair<std::unique_ptr<Command>, ConditionalExecution>>& sequence);
    
    // Undo the last command
    CommandResult undo();
    
    // Redo the last undone command
    CommandResult redo();
    
    // Clear command history
    void clearHistory();
    
    // Get command history size
    size_t getHistorySize() const { return commandHistory.size(); }
    
    // Check if undo is possible
    bool canUndo() const { return !undoStack.empty(); }
    
    // Check if redo is possible
    bool canRedo() const { return !redoStack.empty(); }
    
    // Get the last executed command
    Command* getLastExecutedCommand() const {
        return lastExecutedCommand ? lastExecutedCommand.get() : nullptr;
    }
    
    // Set the brain context for commands
    void setBrain(Brain* brain) { context.brain = brain; }
    
    // Set a variable in the command context
    void setVariable(const std::string& name, const std::string& value) {
        context.setVariable(name, value);
    }
    
    // Get command execution context
    CommandContext& getContext() { return context; }
    
    // Get all available command templates
    const std::vector<std::unique_ptr<CommandTemplate>>& getCommandTemplates() const { return templates; }
    
    // Add a command template
    void addCommandTemplate(std::unique_ptr<CommandTemplate> templateCommand) {
        templates.push_back(std::move(templateCommand));
    }
    
    // Create a macro from a sequence of commands
    std::unique_ptr<CommandTemplate> createMacro(const std::string& name, 
                                                 const std::vector<std::unique_ptr<Command>>& commands);
    
    // Execute a macro by name with parameters
    CommandResult executeMacro(const std::string& name, const std::unordered_map<std::string, std::string>& params);
    
    // Get all executed commands for logging
    const std::vector<Command*>& getExecutedCommands() const { return executedCommands; }
    
private:
    // Execute a command and store in history
    CommandResult executeAndStore(std::unique_ptr<Command> command);
    
    // Max history size to prevent memory issues
    static constexpr size_t MAX_HISTORY_SIZE = 10000;
    
    CommandContext context;
    std::vector<Command*> executedCommands;  // Raw pointers for easy access
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;
    std::stack<std::unique_ptr<Command>> commandHistory;
    std::unique_ptr<Command> lastExecutedCommand;
    std::vector<std::unique_ptr<CommandTemplate>> templates;
    size_t currentCommandId;
};

} // namespace nlm