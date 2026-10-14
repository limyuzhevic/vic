#pragma once

#include "Command.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

namespace nlm {

// CommandGroup: Organizes commands into logical groups
class CommandGroup {
public:
    CommandGroup(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    // Add a command to the group
    void addCommand(const std::string& name, std::unique_ptr<Command> command) {
        commands_[name] = std::move(command);
    }
    
    // Get a command by name
    Command* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        return it != commands_.end() ? it->second.get() : nullptr;
    }
    
    // Get all command names
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get group name
    const std::string& getName() const { return name_; }
    
    // Get group description
    const std::string& getDescription() const { return description_; }
    
    // Get number of commands
    size_t getCommandCount() const { return commands_.size(); }
    
    // Check if command exists
    bool hasCommand(const std::string& name) const {
        return commands_.find(name) != commands_.end();
    }
    
private:
    std::string name_;
    std::string description_;
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
    friend class CommandManager;
};

// CommandRegistry: Central registry for all available commands
class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry() = default;
    
    // Register a command
    bool registerCommand(std::unique_ptr<Command> command);
    
    // Register a command with category
    bool registerCommand(std::unique_ptr<Command> command, const std::string& category);
    
    // Get command by name
    Command* getCommand(const std::string& name);
    
    // Get command by name (const version)
    const Command* getCommand(const std::string& name) const;
    
    // Check if command exists
    bool hasCommand(const std::string& name) const;
    
    // Get all command names
    std::vector<std::string> getCommandNames() const;
    
    // Get all command groups
    const std::vector<CommandGroup*>& getCommandGroups() const { return groups_; }
    
    // Get commands in a category
    std::vector<Command*> getCommandsByCategory(const std::string& category);
    
    // Get command group by name
    CommandGroup* getCommandGroup(const std::string& name);
    
    // Create a command group
    void createCommandGroup(const std::string& name, const std::string& description = "");
    
    // Add command to group
    bool addCommandToGroup(const std::string& groupName, const std::string& commandName);
    
    // Get all commands
    std::vector<Command*> getAllCommands();
    
    // Get command count
    size_t getCommandCount() const { return commands_.size(); }
    
    // Get group count
    size_t getGroupCount() const { return groups_.size(); }
    
    // Remove command
    bool removeCommand(const std::string& name);
    
    // Remove group
    bool removeGroup(const std::string& name);
    
    // Get command help
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Get group help
    std::string getGroupHelp(const std::string& groupName) const;
    
    // Get all help text
    std::string getAllHelp() const;
    
    // Search commands by keyword
    std::vector<Command*> searchCommands(const std::string& keyword);
    
    // Validate command parameters
    bool validateCommandParameters(const std::string& commandName,
                                   const std::string& parameters) const;
    
    // Auto-discover commands from brain API
    void discoverBrainCommands(Brain* brain);
    
    // Get command statistics
    struct CommandStats {
        std::string commandName;
        std::string category;
        size_t executionCount = 0;
        float successRate = 0.0f;
        long long averageExecutionTime = 0;
    };
    
    std::vector<CommandStats> getCommandStats() const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
    std::unordered_map<std::string, CommandGroup> groups_;
    std::vector<CommandGroup*> groupsPtr_;
};

} // namespace nlm