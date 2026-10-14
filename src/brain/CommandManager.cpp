#include "CommandManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void CommandManager::execute(std::unique_ptr<BrainCommand> command) {
    if (!command) return;
    
    // Check history size
    if (commandHistory.size() >= MAX_HISTORY_SIZE) {
        // Remove oldest command
        commandHistory.erase(commandHistory.begin());
    }
    
    // Execute the command
    command->execute();
    
    // Add to history
    commandHistory.push_back(std::move(command));
    
    // Clear redo stack when new command is executed
    redoStack.clear();
    
    NLM_LOG_INFO("Command executed: " + commandHistory.back()->getName());
}

bool CommandManager::undo() {
    if (undoStack.empty()) return false;
    
    auto command = std::move(undoStack.back());
    undoStack.pop_back();
    
    command->undo();
    redoStack.push_back(std::move(command));
    
    NLM_LOG_INFO("Command undone");
    return true;
}

bool CommandManager::redo() {
    if (redoStack.empty()) return false;
    
    auto command = std::move(redoStack.back());
    redoStack.pop_back();
    
    command->execute();
    undoStack.push_back(std::move(command));
    
    NLM_LOG_INFO("Command redone");
    return true;
}

void CommandManager::clearHistory() {
    commandHistory.clear();
    undoStack.clear();
    redoStack.clear();
    NLM_LOG_INFO("Command history cleared");
}

std::unique_ptr<BrainCommand> CommandManager::getCommand(const std::string& name) const {
    for (const auto& command : commandHistory) {
        if (command->getName() == name) {
            // Return a copy - this is a simplified implementation
            // In a real implementation, you might want to clone commands
            return nullptr;
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<BrainCommand>> CommandManager::getCommandsByCategory(CommandCategory category) const {
    // This is a simplified implementation
    // In a real implementation, commands would have category information
    return std::vector<std::unique_ptr<BrainCommand>>();
}

} // namespace nlm
