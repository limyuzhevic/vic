#include "CommandInvoker.hpp"

namespace nlm {

CommandInvoker::CommandInvoker() : currentCommandId(0) {}

CommandInvoker::~CommandInvoker() {
    clearHistory();
}

CommandResult CommandInvoker::execute(std::unique_ptr<Command> command) {
    if (!command) {
        return CommandResult(false, "Invalid command");
    }
    
    return executeAndStore(std::move(command));
}

CommandResult CommandInvoker::executeAndStore(std::unique_ptr<Command> command) {
    if (!command) {
        return CommandResult(false, "Invalid command");
    }
    
    // Record start time
    context.startTime = std::chrono::high_resolution_clock::now();
    
    // Execute the command
    CommandResult result = command->execute();
    
    // Record execution time
    context.executionTime = context.getElapsedTime();
    
    // Store in history
    if (commandHistory.size() >= MAX_HISTORY_SIZE) {
        commandHistory.pop();
    }
    commandHistory.push(std::move(command));
    
    // Store in executed commands list
    lastExecutedCommand = commandHistory.top().get();
    executedCommands.push_back(lastExecutedCommand);
    
    // Update command ID
    context.commandId++;
    
    return result;
}

std::vector<CommandResult> CommandInvoker::executeBatch(std::vector<std::unique_ptr<Command>> commands) {
    std::vector<CommandResult> results;
    for (auto& cmd : commands) {
        results.push_back(execute(std::move(cmd)));
    }
    return results;
}

std::vector<CommandResult> CommandInvoker::executeSequence(const std::vector<std::pair<std::unique_ptr<Command>, ConditionalExecution>>& sequence) {
    std::vector<CommandResult> results;
    for (const auto& pair : sequence) {
        if (pair.first) {
            results.push_back(pair.second.executeIf(*pair.first, context));
        }
    }
    return results;
}

CommandResult CommandInvoker::undo() {
    if (undoStack.empty()) {
        return CommandResult(false, "Nothing to undo");
    }
    
    auto command = std::move(undoStack.top());
    undoStack.pop();
    
    CommandResult result = command->undo();
    
    if (result.success) {
        redoStack.push(std::move(command));
    }
    
    return result;
}

CommandResult CommandInvoker::redo() {
    if (redoStack.empty()) {
        return CommandResult(false, "Nothing to redo");
    }
    
    auto command = std::move(redoStack.top());
    redoStack.pop();
    
    CommandResult result = command->execute();
    
    if (result.success) {
        undoStack.push(std::move(command));
    }
    
    return result;
}

void CommandInvoker::clearHistory() {
    while (!undoStack.empty()) {
        undoStack.pop();
    }
    while (!redoStack.empty()) {
        redoStack.pop();
    }
    while (!commandHistory.empty()) {
        commandHistory.pop();
    }
    lastExecutedCommand = nullptr;
    executedCommands.clear();
}

std::unique_ptr<CommandTemplate> CommandInvoker::createMacro(const std::string& name,
                                                   const std::vector<std::unique_ptr<Command>>& commands) {
    class MacroTemplate : public CommandTemplate {
    public:
        MacroTemplate(const std::string& name, const std::vector<std::unique_ptr<Command>>& cmds)
            : name_(name), commands_(cmds) {}
        
        std::unique_ptr<Command> instantiate(const std::unordered_map<std::string, std::string>& params) override {
            std::vector<std::unique_ptr<Command>> instantiatedCommands;
            for (const auto& cmd : commands_) {
                instantiatedCommands.push_back(cmd->clone());
            }
            
            // Apply parameters (simple substitution for now)
            // In a real implementation, this would be more sophisticated
            for (auto& param : params) {
                context.setVariable(param.first, param.second);
            }
            
            return std::make_unique<MacroCommand>(name_, std::move(instantiatedCommands));
        }
        
        std::string getTemplate() const override {
            return "Macro: " + name_ + " with " + std::to_string(commands_.size()) + " commands";
        }
        
        bool validateParameters(const std::unordered_map<std::string, std::string>& params) const override {
            return true; // Simple validation - all params are valid
        }
        
    private:
        class MacroCommand : public Command {
        public:
            MacroCommand(const std::string& name, std::vector<std::unique_ptr<Command>> cmds)
                : name_(name), commands_(std::move(cmds)) {}
            
            CommandResult execute() override {
                std::string output;
                for (auto& cmd : commands_) {
                    CommandResult result = cmd->execute();
                    if (!output.empty()) output += "\n";
                    output += result.output;
                    if (!result.success) {
                        return CommandResult(false, output + "\nError in macro execution: " + result.errorMessage);
                    }
                }
                return CommandResult(true, output);
            }
            
            std::string getName() const override { return name_; }
            std::string getDescription() const override { return "Macro command"; }
            std::unique_ptr<Command> clone() const override {
                return std::make_unique<MacroCommand>(*this);
            }
            
        private:
            std::string name_;
            std::vector<std::unique_ptr<Command>> commands_;
        };
        
        std::string name_;
        std::vector<std::unique_ptr<Command>> commands_;
        CommandContext context;
    };
    
    return std::make_unique<MacroTemplate>(name, commands);
}

CommandResult CommandInvoker::executeMacro(const std::string& name,
                                     const std::unordered_map<std::string, std::string>& params) {
    for (const auto& templateCmd : templates) {
        if (templateCmd && templateCmd->getTemplate().find(name) != std::string::npos) {
            auto instantiated = templateCmd->instantiate(params);
            return execute(std::move(instantiated));
        }
    }
    return CommandResult(false, "Macro not found: " + name);
}

} // namespace nlm