#pragma once

#include "CLIUtils.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace nlm {
namespace cli {

// Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual CLIResult execute(const std::vector<std::string>& args) = 0;
    virtual bool isHidden() const { return false; }
    virtual std::string getCategory() const { return "General"; }
};

// Demo mode command
class DemoCommand : public Command {
public:
    std::string getName() const override { return "demo"; }
    std::string getDescription() const override { 
        return "Run demonstration mode with basic neural computation"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli demo [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Modes"; }
};

// Integration mode command
class IntegrationCommand : public Command {
public:
    std::string getName() const override { return "integration"; }
    std::string getDescription() const override { 
        return "Run integration tests to verify all brain systems work together"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli integration [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Modes"; }
};

// Profiling mode command
class ProfilingCommand : public Command {
public:
    std::string getName() const override { return "profile"; }
    std::string getDescription() const override { 
        return "Run performance profiling of the brain system"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli profile [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Modes"; }
};

// Experiment mode command
class ExperimentCommand : public Command {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { 
        return "Run custom experiments with configurable parameters"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli experiment [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Modes"; }
};

// Config mode command
class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Manage and display configuration settings"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli config [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Settings"; }
};

// Checkpoint command
class CheckpointCommand : public Command {
public:
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { 
        return "Save and load brain checkpoints for experiments"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli checkpoint [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "State Management"; }
};

// Visualization command
class VisualizationCommand : public Command {
public:
    std::string getName() const override { return "viz"; }
    std::string getDescription() const override { 
        return "Run visualization modes (text-based)"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli viz [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Visualization"; }
};

// Brain state analysis command
class BrainAnalysisCommand : public Command {
public:
    std::string getName() const override { return "analysis"; }
    std::string getDescription() const override { 
        return "Analyze brain state and neural dynamics"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli analysis [options]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Analysis"; }
};

// Version command
class VersionCommand : public Command {
public:
    std::string getName() const override { return "version"; }
    std::string getDescription() const override { 
        return "Display version information"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli version";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Help"; }
    bool isHidden() const override { return false; }
};

// Help command
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Display help information for commands"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli help [command]";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "Help"; }
    bool isHidden() const override { return false; }
};

// Exit command
class ExitCommand : public Command {
public:
    std::string getName() const override { return "exit"; }
    std::string getDescription() const override { 
        return "Exit the application"; 
    }
    std::string getUsage() const override { 
        return "nlm-cli exit";
    }
    CLIResult execute(const std::vector<std::string>& args) override;
    std::string getCategory() const override { return "General"; }
    bool isHidden() const override { return true; }
};

// Command registry for managing commands
class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(std::unique_ptr<Command> command);
    std::vector<Command*> getCommands() const;
    std::vector<Command*> getCommandsByCategory(const std::string& category) const;
    Command* getCommand(const std::string& name) const;
    
private:
    CommandRegistry() = default;
    std::vector<std::unique_ptr<Command>> commands;
    std::unordered_map<std::string, Command*> commandMap;
};

} // namespace cli
} // namespace nlm