#pragma once

/**
 * Command System for NLM Brain
 * 
 * Provides advanced command-line interface for power users
 * Builds on existing checkpointing infrastructure
 */

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace nlm {

// Forward declarations
class Brain;
class CheckpointManager;
class Config;
class ILogger;

// Command result structure
struct CommandResult {
    bool success;
    std::string output;
    std::string errorMessage;
    std::chrono::milliseconds executionTime;
    
    CommandResult() : success(false), executionTime(0) {}
    CommandResult(bool s, const std::string& out = "", const std::string& err = "", 
                  const std::chrono::milliseconds& time = std::chrono::milliseconds(0))
        : success(s), output(out), errorMessage(err), executionTime(time) {}
};

// Command interface
class ICommand {
public:
    virtual ~ICommand() = default;
    
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual const std::vector<std::string>& getAliases() const = 0;
    virtual const std::string& getUsage() const = 0;
    
    // Execute the command
    virtual CommandResult execute(const std::vector<std::string>& args) = 0;
    
    // Check if command requires a brain instance
    virtual bool requiresBrain() const { return false; }
    
    // Check if command is global (doesn't require brain)
    virtual bool isGlobal() const { return false; }
};

// Command manager for handling command execution
class CommandManager {
public:
    CommandManager(std::shared_ptr<Brain> brain, 
                   std::shared_ptr<CheckpointManager> checkpointManager,
                   std::shared_ptr<Config> config,
                   std::shared_ptr<ILogger> logger);
    
    // Register a command
    void registerCommand(std::unique_ptr<ICommand> command);
    
    // Execute a command by name
    CommandResult executeCommand(const std::string& commandName, 
                                 const std::vector<std::string>& args);
    
    // List available commands
    std::vector<std::string> getAvailableCommands() const;
    
    // Get command details
    struct CommandInfo {
        std::string name;
        std::string description;
        std::vector<std::string> aliases;
        std::string usage;
        bool requiresBrain;
        bool isGlobal;
    };
    
    std::vector<CommandInfo> getCommandInfo() const;
    
    // Help for a specific command
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Global command execution (no brain required)
    CommandResult executeGlobalCommand(const std::string& commandName,
                                       const std::vector<std::string>& args);
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<CheckpointManager> checkpointManager_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<ILogger> logger_;
    
    std::map<std::string, std::unique_ptr<ICommand>> commands_;
    std::map<std::string, std::string> commandAliases_;
};

} // namespace nlm