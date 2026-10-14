#include "CommandManager.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

CommandRegistry::CommandRegistry() {
    // Create default groups
    createCommandGroup("Brain State", "Commands for managing brain state");
    createCommandGroup("Memory System", "Commands for memory systems");
    createCommandGroup("Neuromodulation", "Commands for neuromodulation systems");
    createCommandGroup("Cognition", "Commands for cognitive functions");
    createCommandGroup("Prediction", "Commands for prediction system");
    createCommandGroup("Development", "Commands for development system");
    createCommandGroup("Region/Connection", "Commands for brain regions and connections");
    createCommandGroup("Neuron/Population", "Commands for neurons and populations");
    createCommandGroup("Agent", "Commands for agent configuration");
    createCommandGroup("World", "Commands for world/environment manipulation");
}

bool CommandRegistry::registerCommand(std::unique_ptr<Command> command) {
    if (!command) return false;
    
    // Check if command already exists
    if (commands_.find(command->getName()) != commands_.end()) {
        return false;
    }
    
    // Register the command
    commands_[command->getName()] = std::move(command);
    return true;
}

bool CommandRegistry::registerCommand(std::unique_ptr<Command> command, const std::string& category) {
    if (!command || !registerCommand(std::move(command))) return false;
    
    // Find or create group for this category
    auto it = groups_.find(category);
    if (it == groups_.end()) {
        createCommandGroup(category, "Commands in " + category);
        it = groups_.find(category);
    }
    
    // Add to group
    auto& group = it->second;
    group.addCommand(command->getName(), std::move(command));
    
    return true;
}

Command* CommandRegistry::getCommand(const std::string& name) {
    auto it = commands_.find(name);
    return it != commands_.end() ? it->second.get() : nullptr;
}

const Command* CommandRegistry::getCommand(const std::string& name) const {
    auto it = commands_.find(name);
    return it != commands_.end() ? it->second.get() : nullptr;
}

bool CommandRegistry::hasCommand(const std::string& name) const {
    return commands_.find(name) != commands_.end();
}

std::vector<std::string> CommandRegistry::getCommandNames() const {
    std::vector<std::string> names;
    for (const auto& pair : commands_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<Command*> CommandRegistry::getCommandsByCategory(const std::string& category) {
    std::vector<Command*> result;
    auto it = groups_.find(category);
    if (it != groups_.end()) {
        const auto& group = it->second;
        for (const auto& pair : group.commands_) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

CommandGroup* CommandRegistry::getCommandGroup(const std::string& name) {
    auto it = groups_.find(name);
    return it != groups_.end() ? &it->second : nullptr;
}

void CommandRegistry::createCommandGroup(const std::string& name, const std::string& description) {
    // Remove existing group if present
    auto it = groups_.find(name);
    if (it != groups_.end()) {
        groups_.erase(it);
        // Also remove from groupsPtr_
        groupsPtr_.erase(std::remove(groupsPtr_.begin(), groupsPtr_.end(), &groups_.at(name)), groupsPtr_.end());
    }
    
    // Create new group
    groups_[name] = CommandGroup(name, description);
    groupsPtr_.push_back(&groups_[name]);
}

bool CommandRegistry::addCommandToGroup(const std::string& groupName, const std::string& commandName) {
    auto groupIt = groups_.find(groupName);
    if (groupIt == groups_.end()) return false;
    
    auto cmdIt = commands_.find(commandName);
    if (cmdIt == commands_.end()) return false;
    
    auto& group = groupIt->second;
    group.addCommand(commandName, cmdIt->second->clone());
    
    return true;
}

std::vector<Command*> CommandRegistry::getAllCommands() {
    std::vector<Command*> result;
    for (auto& pair : commands_) {
        result.push_back(pair.second.get());
    }
    return result;
}

bool CommandRegistry::removeCommand(const std::string& name) {
    auto it = commands_.find(name);
    if (it == commands_.end()) return false;
    
    // Remove from all groups
    for (auto& [groupName, group] : groups_) {
        group.commands_.erase(name);
    }
    
    commands_.erase(it);
    return true;
}

bool CommandRegistry::removeGroup(const std::string& name) {
    auto it = groups_.find(name);
    if (it == groups_.end()) return false;
    
    // Remove all commands in this group from registry
    auto& group = it->second;
    for (const auto& pair : group.commands_) {
        commands_.erase(pair.first);
    }
    
    groups_.erase(it);
    groupsPtr_.erase(std::remove(groupsPtr_.begin(), groupsPtr_.end(), &groups_.at(name)), groupsPtr_.end());
    
    return true;
}

std::string CommandRegistry::getCommandHelp(const std::string& commandName) const {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) return "Command not found: " + commandName;
    
    const auto& command = it->second;
    std::string help = "Command: " + command->getName() + "\n";
    help += "Description: " + command->getDescription() + "\n";
    help += "Category: " + command->getCategory() + "\n";
    help += "Help: " + command->getHelp() + "\n";
    help += "Parameters: " + command->getParameters() + "\n";
    
    return help;
}

std::string CommandRegistry::getGroupHelp(const std::string& groupName) const {
    auto it = groups_.find(groupName);
    if (it == groups_.end()) return "Group not found: " + groupName;
    
    const auto& group = it->second;
    std::string help = "Group: " + group.getName() + "\n";
    help += "Description: " + group.getDescription() + "\n";
    help += "Commands (" + std::to_string(group.getCommandCount()) + "):\n";
    
    for (const auto& cmdName : group.getCommandNames()) {
        help += "  - " + cmdName + "\n";
    }
    
    return help;
}

std::string CommandRegistry::getAllHelp() const {
    std::string help = "Available Commands:\n";
    help += "==================\n\n";
    
    for (const auto& groupPtr : groupsPtr_) {
        const auto& group = *groupPtr;
        help += group.getDescription() + "\n";
        for (const auto& cmdName : group.getCommandNames()) {
            help += "  " + cmdName + "\n";
        }
        help += "\n";
    }
    
    return help;
}

std::vector<Command*> CommandRegistry::searchCommands(const std::string& keyword) {
    std::vector<Command*> result;
    std::string lowerKeyword = keyword;
    for (char& c : lowerKeyword) c = std::tolower(c);
    
    for (auto& pair : commands_) {
        const auto& command = pair.second;
        std::string name = command->getName();
        std::string desc = command->getDescription();
        std::string help = command->getHelp();
        
        for (char& c : name) c = std::tolower(c);
        for (char& c : desc) c = std::tolower(c);
        for (char& c : help) c = std::tolower(c);
        
        if (name.find(lowerKeyword) != std::string::npos ||
            desc.find(lowerKeyword) != std::string::npos ||
            help.find(lowerKeyword) != std::string::npos) {
            result.push_back(command.get());
        }
    }
    
    return result;
}

bool CommandRegistry::validateCommandParameters(const std::string& commandName,
                                                const std::string& parameters) const {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) return false;
    
    const auto& command = it->second;
    return command->setParameters(parameters);
}

void CommandRegistry::discoverBrainCommands(Brain* brain) {
    if (!brain) return;
    
    // Discover brain components for command creation
    // This is a placeholder for actual command discovery logic
    // In a real implementation, this would introspect the Brain class
    // and create appropriate command objects for each method
    
    // For now, just create some basic commands
    class SaveCommand : public Command {
    public:
        CommandResult execute() override {
            return CommandResult(true, "Brain saved");
        }
        std::string getName() const override { return "save"; }
        std::string getDescription() const override { return "Save brain state to file"; }
        std::unique_ptr<Command> clone() const override {
            return std::make_unique<SaveCommand>(*this);
        }
        std::string getCategory() const override { return "Brain State"; }
    };
    
    registerCommand(std::make_unique<SaveCommand>(), "Brain State");
}

std::vector<CommandRegistry::CommandStats> CommandRegistry::getCommandStats() const {
    std::vector<CommandStats> stats;
    
    // Note: In a real implementation, this would track execution statistics
    // For now, return empty stats
    
    return stats;
}

} // namespace nlm