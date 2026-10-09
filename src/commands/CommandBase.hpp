// Base command interface for NLM brain emulation
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <chrono>

namespace nlm {

// Command argument types
using CommandArgument = std::variant<std::string, int, double, bool>;

// Command result types
using CommandResult = std::variant<std::string, int, double, bool, std::vector<std::string>>;

// Command execution context
struct CommandContext {
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
    std::string user;
    std::map<std::string, std::string> metadata;
    
    CommandContext() : timestamp(std::chrono::system_clock::now()) {}
};

// Base command interface
class Command {
public:
    virtual ~Command() = default;
    
    // Get command name
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Get command usage
    virtual std::string getUsage() const = 0;
    
    // Get command arguments
    virtual std::vector<std::string> getArguments() const = 0;
    
    // Execute command with context and arguments
    virtual CommandResult execute(const CommandContext& context, 
                                 const std::map<std::string, CommandArgument>& arguments) = 0;
    
    // Validate arguments before execution
    virtual bool validateArguments(const std::map<std::string, CommandArgument>& arguments) const {
        return true;
    }
    
    // Check if command is available (for plugin system)
    virtual bool isAvailable() const {
        return true;
    }
};

// Command group for organization
class CommandGroup {
public:
    virtual ~CommandGroup() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::shared_ptr<Command>> getCommands() const = 0;
};

// Command factory for plugin system
class CommandFactory {
public:
    virtual ~CommandFactory() = default;
    virtual std::shared_ptr<Command> createCommand(const std::string& name) = 0;
    virtual bool hasCommand(const std::string& name) const = 0;
};

// Command registry for managing all commands
class CommandRegistry {
public:
    virtual ~CommandRegistry() = default;
    
    virtual void registerCommand(std::shared_ptr<Command> command) = 0;
    virtual void registerGroup(std::shared_ptr<CommandGroup> group) = 0;
    virtual std::shared_ptr<Command> getCommand(const std::string& name) = 0;
    virtual std::vector<std::shared_ptr<CommandGroup>> getGroups() = 0;
    virtual std::vector<std::shared_ptr<Command>> getAllCommands() = 0;
    virtual std::vector<std::string> getCommandNames() = 0;
};

} // namespace nlm
