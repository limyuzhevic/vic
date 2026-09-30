#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <map>
#include "../core/Config/Config.hpp"
#include "Exceptions/NLMExceptions.hpp"

namespace nlm {

// Command line option types
enum class OptionType {
    Flag,
    Value,
    String,
    Int,
    Float,
    File,
    Directory
};

// Command line option definition
struct CommandOption {
    std::string name;
    std::string shortName;
    OptionType type;
    std::string description;
    std::string defaultValue;
    bool required;
    std::function<void(std::shared_ptr<Config>)> configSetter;
    std::function<bool(std::string)> valueValidator;
};

// Command action interface
class CommandAction {
public:
    virtual ~CommandAction() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool execute(std::shared_ptr<Config> config, 
                        const std::vector<std::string>& args,
                        std::ostream& output) = 0;
    virtual void showHelp(std::ostream& output) const = 0;
};

// Create brain command
class CreateBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "create"; }
    std::string getDescription() const override { return "Create and initialize a new brain"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Initialize brain command
class InitializeBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "initialize"; }
    std::string getDescription() const override { return "Initialize a brain for simulation"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Run simulation command
class RunSimulationCommand : public CommandAction {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run simulation for specified number of steps"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Show brain status command
class ShowStatusCommand : public CommandAction {
public:
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Show brain status and statistics"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Save brain command
class SaveBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save brain state to file"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Load brain command
class LoadBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from file"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Show help command
class HelpCommand : public CommandAction {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information"; }
    
    bool execute(std::shared_ptr<Config> config, 
                const std::vector<std::string>& args,
                std::ostream& output) override;
    
    void showHelp(std::ostream& output) const override;
};

// Main CLI class
class NLMCLI {
private:
    std::vector<std::shared_ptr<CommandAction>> commands;
    std::shared_ptr<Config> defaultConfig;
    
public:
    NLMCLI();
    
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args,
                       std::ostream& output = std::cout);
    
    void showHelp(const std::string& commandName = "", 
                  std::ostream& output = std::cout) const;
    
    void listCommands(std::ostream& output = std::cout) const;
    
    std::shared_ptr<Config> getDefaultConfig() const { return defaultConfig; }
};

} // namespace nlm
