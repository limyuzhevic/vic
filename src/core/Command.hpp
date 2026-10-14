#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Command;
class CommandInvoker;
class CommandHistory;

// CommandResult: Represents the result of command execution
struct CommandResult {
    bool success;
    std::string output;
    std::string errorMessage;
    std::unordered_map<std::string, std::string> metadata;
    
    CommandResult(bool s = false, const std::string& out = "", const std::string& err = "")
        : success(s), output(out), errorMessage(err) {}
};

// Command: Abstract base class for all commands
class Command {
public:
    virtual ~Command() = default;
    
    // Execute the command
    virtual CommandResult execute() = 0;
    
    // Undo the command (if supported)
    virtual CommandResult undo() { return CommandResult(false, "Undo not supported"); }
    
    // Redo the command (if supported)
    virtual CommandResult redo() { return CommandResult(false, "Redo not supported"); }
    
    // Get command name for help and registration
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Get command help text
    virtual std::string getHelp() const { return "Command help not available"; }
    
    // Check if command can be undone
    virtual bool isUndoable() const { return false; }
    
    // Get command category
    virtual std::string getCategory() const { return "General"; }
    
    // Clone the command (for scripting)
    virtual std::unique_ptr<Command> clone() const = 0;
    
    // Get command parameters as string (for serialization)
    virtual std::string getParameters() const { return ""; }
    
    // Set command parameters from string (for scripting)
    virtual bool setParameters(const std::string& params) { return false; }
    
    // Evaluate expression (for scripting)
    virtual std::string evaluateExpression(const std::string& expr) { return ""; }
    
    // Get command execution time in microseconds
    virtual long long getExecutionTime() const { return 0; }
    
    // Check if command is complete (for compound commands)
    virtual bool isComplete() const { return true; }
    
    // Add sub-command (for command chaining)
    virtual void addSubCommand(std::unique_ptr<Command> subCommand) {}
    
    // Get all sub-commands
    virtual std::vector<Command*> getSubCommands() const { return {}; }
};

// CommandContext: Provides context for command execution
struct CommandContext {
    Brain* brain = nullptr;
    std::unordered_map<std::string, std::string> variables;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    size_t commandId = 0;
    long long executionTime = 0;
    
    CommandContext() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    // Get elapsed time in microseconds
    long long getElapsedTime() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - startTime).count();
    }
    
    // Set a variable
    void setVariable(const std::string& name, const std::string& value) {
        variables[name] = value;
    }
    
    // Get a variable
    std::string getVariable(const std::string& name) const {
        auto it = variables.find(name);
        return it != variables.end() ? it->second : "";
    }
    
    // Evaluate expression with variables
    std::string evaluate(const std::string& expr) {
        // Simple expression evaluation with variable substitution
        std::string result = expr;
        for (const auto& [var, val] : variables) {
            std::string placeholder = "${" + var + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), val);
                pos += val.length();
            }
        }
        return result;
    }
};

// CommandCondition: Represents a conditional for command execution
class CommandCondition {
public:
    virtual ~CommandCondition() = default;
    virtual bool evaluate(CommandContext& context) = 0;
    virtual std::unique_ptr<CommandCondition> clone() const = 0;
    virtual std::string getDescription() const { return "Condition"; }
};

// ConditionalExecution: Represents conditional command execution
class ConditionalExecution {
public:
    enum class ConditionType { IF, IFNOT, WHILE, UNTIL };
    
    ConditionalExecution() : conditionType(ConditionType::IF), negate(false) {}
    ConditionalExecution(std::unique_ptr<CommandCondition> cond, ConditionType type, bool neg)
        : condition(cond), conditionType(type), negate(neg) {}
    
    CommandResult executeIf(Command& cmd, CommandContext& context) const;
    CommandResult executeWhile(Command& cmd, CommandContext& context) const;
    CommandResult executeUntil(Command& cmd, CommandContext& context) const;
    
    std::string getDescription() const {
        return condition ? condition->getDescription() : "No condition";
    }
    
    std::unique_ptr<CommandCondition> condition;
    ConditionType conditionType;
    bool negate;
};

// CommandTemplate: Represents a command template for macros
class CommandTemplate {
public:
    virtual ~CommandTemplate() = default;
    virtual std::unique_ptr<Command> instantiate(const std::unordered_map<std::string, std::string>& params) = 0;
    virtual std::string getTemplate() const { return "";
    }
    
    // Parameter validation for templates
    virtual bool validateParameters(const std::unordered_map<std::string, std::string>& params) const { return true; }
};

} // namespace nlm