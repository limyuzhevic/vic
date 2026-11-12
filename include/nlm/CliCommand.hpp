// NLM Command Line Interface - Base Command Interface
// Defines the abstract interface for all subcommands

#ifndef NLM_CLI_COMMAND_HPP
#define NLM_CLI_COMMAND_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <functional>

namespace nlm {

class Command {
public:
    virtual ~Command() = default;
    
    // Execute the command
    virtual int execute(const std::vector<std::string>& args) = 0;
    
    // Get the command name
    virtual std::string getName() const = 0;
    
    // Get the command description
    virtual std::string getDescription() const = 0;
    
    // Get the command help
    virtual std::string getHelp() const = 0;
    
    // Check if command requires admin/root privileges
    virtual bool requiresRoot() const { return false; }
    
    // Check if command is for internal use only
    virtual bool isInternal() const { return false; }
};

using CommandPtr = std::unique_ptr<Command>;

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    // Register a command factory
    void registerCommand(const std::string& name, 
                        const std::string& description,
                        const std::string& help,
                        std::function<CommandPtr()> factory,
                        bool requiresRoot = false,
                        bool isInternal = false) {
        CommandInfo info;
        info.description = description;
        info.help = help;
        info.factory = std::move(factory);
        info.requiresRoot = requiresRoot;
        info.isInternal = isInternal;
        commands[name] = info;
    }
    
    // Get command factory
    CommandPtr getCommand(const std::string& name) {
        auto it = commands.find(name);
        if (it != commands.end()) {
            return it->second.factory();
        }
        return nullptr;
    }
    
    // Get all registered command names
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commands) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get command info
    CommandInfo* getCommandInfo(const std::string& name) {
        auto it = commands.find(name);
        if (it != commands.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    // Print all commands help
    void printHelp(std::ostream& out = std::cout) const {
        out << "NLM - Neural Learning Machine CLI" << std::endl;
        out << "===============================" << std::endl << std::endl;
        
        out << "Usage: nlm <command> [options] [args]" << std::endl;
        out << std::endl;
        
        for (const auto& pair : commands) {
            const auto& name = pair.first;
            const auto& info = pair.second;
            
            out << "  " << name << " - " << info.description << std::endl;
        }
        
        out << std::endl;
        out << "Use 'nlm <command> --help' for detailed help on each command." << std::endl;
    }
    
    // Print specific command help
    bool printCommandHelp(const std::string& name, std::ostream& out = std::cout) {
        CommandInfo* info = getCommandInfo(name);
        if (!info) {
            return false;
        }
        
        out << "NLM " << name << std::endl;
        out << std::string(20 + name.length(), '=') << std::endl << std::endl;
        out << info->help << std::endl << std::endl;
        
        // If the command has a getUsage method, call it
        if (auto cmd = info->factory()) {
            if (cmd->getHelp().find("--help") != std::string::npos) {
                out << "Usage: nlm " << name << " [options] [args]" << std::endl;
            }
        }
        
        return true;
    }

private:
    CommandRegistry() = default;
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;
    
    struct CommandInfo {
        std::string description;
        std::string help;
        std::function<CommandPtr()> factory;
        bool requiresRoot = false;
        bool isInternal = false;
    };
    
    std::map<std::string, CommandInfo> commands;
};

// Helper macro for command registration
#define REGISTER_COMMAND(name, description, help, factory, ...) \
    namespace { \
        struct Register##name { \
            Register##name() { \
                CommandRegistry::getInstance().registerCommand( \
                    #name, description, help, factory, __VA_ARGS__ \
                ); \
            } \
        }; \
        Register##name register_##name; \
    }

} // namespace nlm

#endif // NLM_CLI_COMMAND_HPP