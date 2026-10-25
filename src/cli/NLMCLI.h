// NLM Command Line Interface Tools
// Provides enhanced command-line interface for NLM operations

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations
class NLMCLI;
class Command;

// Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args, NLMCLI& cli) = 0;
    virtual bool requiresBrain() const { return false; }
};

// Configuration Command
class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Manage configuration files and settings"; 
    }
    std::string getUsage() const override { 
        return "config <command> [options]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigLoadCommand : public Command {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { 
        return "Load configuration from file"; 
    }
    std::string getUsage() const override { 
        return "config load <file> [--format json|yaml|simple] [--validate]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigSaveCommand : public Command {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { 
        return "Save configuration to file"; 
    }
    std::string getUsage() const override { 
        return "config save <file> [--format json|yaml|simple] [--compress]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigSetCommand : public Command {
public:
    std::string getName() const override { return "set"; }
    std::string getDescription() const override { 
        return "Set configuration value"; 
    }
    std::string getUsage() const override { 
        return "config set <key> <value> [--source command-line|runtime]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigGetCommand : public Command {
public:
    std::string getName() const override { return "get"; }
    std::string getDescription() const override { 
        return "Get configuration value"; 
    }
    std::string getUsage() const override { 
        return "config get <key> [--format human|json|raw]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigListCommand : public Command {
public:
    std::string getName() const override { return "list"; }
    std::string getDescription() const override { 
        return "List all configuration keys"; 
    }
    std::string getUsage() const override { 
        return "config list [--format human|table|json] [--source source]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigValidateCommand : public Command {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { 
        return "Validate configuration values"; 
    }
    std::string getUsage() const override { 
        return "config validate [--strict] [--fix]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ConfigAutoSaveCommand : public Command {
public:
    std::string getName() const override { return "autosave"; }
    std::string getDescription() const override { 
        return "Configure automatic configuration saving"; 
    }
    std::string getUsage() const override { 
        return "config autosave <path> <interval> [--enabled]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class SimulationCommand : public Command {
public:
    std::string getName() const override { return "simulate"; }
    std::string getDescription() const override { 
        return "Run neural simulation"; 
    }
    std::string getUsage() const override { 
        return "simulate [--config file] [--steps N] [--brain brain_file] [--output output] [--benchmark]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainCommand : public Command {
public:
    std::string getName() const override { return "brain"; }
    std::string getDescription() const override { 
        return "Manage brain instances and operations"; 
    }
    std::string getUsage() const override { 
        return "brain <command> [options]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainCreateCommand : public Command {
public:
    std::string getName() const override { return "create"; }
    std::string getDescription() const override { 
        return "Create new brain instance"; 
    }
    std::string getUsage() const override { 
        return "brain create [--neurons N] [--regions R] [--density D] [--output brain_file]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainLoadCommand : public Command {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { 
        return "Load brain from file"; 
    }
    std::string getUsage() const override { 
        return "brain load <brain_file> [--replace]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainStepCommand : public Command {
public:
    std::string getName() const override { return "step"; }
    std::string getDescription() const override { 
        return "Run brain simulation step"; 
    }
    std::string getUsage() const override { 
        return "brain step [--steps N] [--time T] [--monitor]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainSaveCommand : public Command {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { 
        return "Save brain state to file"; 
    }
    std::string getUsage() const override { 
        return "brain save <brain_file> [--compress] [--metadata]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class BrainStatsCommand : public Command {
public:
    std::string getName() const override { return "stats"; }
    std::string getDescription() const override { 
        return "Show brain statistics and diagnostics"; 
    }
    std::string getUsage() const override { 
        return "brain stats [--format human|json|csv] [--detailed]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Show help information"; 
    }
    std::string getUsage() const override { 
        return "help [command]"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

class ExitCommand : public Command {
public:
    std::string getName() const override { return "exit"; }
    std::string getDescription() const override { 
        return "Exit NLM CLI"; 
    }
    std::string getUsage() const override { 
        return "exit"; 
    }
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
};

// Main CLI class
class NLMCLI {
private:
    std::shared_ptr<::nlm::Config> currentConfig;
    std::shared_ptr<::nlm::Brain> currentBrain;
    std::map<std::string, std::unique_ptr<Command>> commands;
    bool running;
    std::string currentOutput;
    
public:
    NLMCLI();
    void run();
    
    void setConfig(std::shared_ptr<::nlm::Config> config);
    void setBrain(std::shared_ptr<::nlm::Brain> brain);
    std::shared_ptr<::nlm::Config> getConfig() const;
    std::shared_ptr<::nlm::Brain> getBrain() const;
    void setOutput(const std::string& output);
    std::string getOutput() const;
    bool isRunning() const;
    void stop();
    
    // Helper method for processing commands
    void processCommand(const std::string& input);
    
    const std::map<std::string, std::unique_ptr<Command>>& getCommands() const;
    
    // Forward declaration for command processing
    bool processCommand(const std::vector<std::string>& args);
};

// Forward declaration for friend class
class Command;

// Global instance
extern NLMCLI gNLMCLI;

#endif // NLM_CLI_H