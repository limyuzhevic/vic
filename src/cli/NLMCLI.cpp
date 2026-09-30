#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
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

// Command action for CLI
class CommandAction {
public:
    virtual ~CommandAction() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool execute(std::shared_ptr<Config> config, 
                        std::vector<std::string>& args,
                        std::ostream& output) = 0;
    virtual void showHelp(std::ostream& output) const = 0;
};

// Create brain command
class CreateBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "create"; }
    std::string getDescription() const override { return "Create and initialize a new brain"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        output << "Creating NLM brain..." << std::endl;
        
        // Set default configuration values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Check for custom neuron count
        if (args.size() > 0 && args[0].rfind("--neurons=", 0) == 0) {
            size_t neurons = std::stoul(args[0].substr(9));
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::User);
        }
        
        output << "Brain created with " << config->getOr<int64_t>("neuron_count", 1000) 
               << " neurons" << std::endl;
        return true;
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm create [--neurons=N] [--seed=N] [--region-count=N]" << std::endl;
        output << "  --neurons=N       Number of neurons (default: 1000)" << std::endl;
        output << "  --seed=N          Random seed (default: 42)" << std::endl;
        output << "  --region-count=N  Number of brain regions (default: 1)" << std::endl;
    }
};

// Initialize brain command
class InitializeBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "initialize"; }
    std::string getDescription() const override { return "Initialize a brain for simulation"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        output << "Initializing brain..." << std::endl;
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                output << "ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            output << "Brain initialized successfully" << std::endl;
            output << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            output << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
            
            // Save brain to file if requested
            std::string outputFile;
            for (size_t i = 0; i < args.size(); ++i) {
                if (args[i].rfind("--output=", 0) == 0) {
                    outputFile = args[i].substr(8);
                }
            }
            
            if (!outputFile.empty()) {
                if (brain->save(outputFile)) {
                    output << "Brain saved to: " << outputFile << std::endl;
                } else {
                    output << "WARNING: Failed to save brain to " << outputFile << std::endl;
                }
            }
            
            return true;
            
        } catch (const NLMError& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm initialize [--output=FILE]" << std::endl;
        output << "  --output=FILE     Save brain state to file" << std::endl;
    }
};

// Run simulation command
class RunSimulationCommand : public CommandAction {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run simulation for specified number of steps"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        int steps = 100;
        int logInterval = 10;
        
        // Parse command line arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoi(args[++i]);
            } else if (args[i] == "--log-interval" && i + 1 < args.size()) {
                logInterval = std::stoi(args[++i]);
            } else if (args[i].rfind("--steps=", 0) == 0) {
                steps = std::stoi(args[i].substr(7));
            } else if (args[i].rfind("--log-interval=", 0) == 0) {
                logInterval = std::stoi(args[i].substr(14));
            }
        }
        
        output << "Running simulation for " << steps << " steps..." << std::endl;
        output << "Logging statistics every " << logInterval << " steps" << std::endl;
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                output << "ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            // Run simulation
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
                
                if (step % logInterval == 0) {
                    output << "Step " << step << ": "
                           << brain->getFiringNeuronCount() << " firing neurons, "
                           << brain->getTotalSpikeCount() << " total spikes, "
                           << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
                }
            }
            
            output << "Simulation completed successfully" << std::endl;
            output << "Final statistics:" << std::endl;
            output << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            output << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            output << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            output << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            output << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            
            return true;
            
        } catch (const NLMError& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm run [--steps=N] [--log-interval=N]" << std::endl;
        output << "  --steps=N          Number of simulation steps (default: 100)" << std::endl;
        output << "  --log-interval=N   Log statistics every N steps (default: 10)" << std::endl;
    }
};

// Show brain status command
class ShowStatusCommand : public CommandAction {
public:
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Show brain status and statistics"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        output << "=== NLM Brain Status ===" << std::endl;
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                output << "ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            brain->logStatus();
            
            return true;
            
        } catch (const std::exception& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm status" << std::endl;
        output << "  Shows current brain status and statistics" << std::endl;
    }
};

// Save brain command
class SaveBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save brain state to file"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        if (args.size() < 1) {
            output << "ERROR: Missing output file" << std::endl;
            return false;
        }
        
        std::string outputFile = args[0];
        output << "Saving brain to " << outputFile << "..." << std::endl;
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                output << "ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            if (brain->save(outputFile)) {
                output << "Brain saved successfully" << std::endl;
                return true;
            } else {
                output << "ERROR: Failed to save brain" << std::endl;
                return false;
            }
            
        } catch (const NLMError& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm save FILE" << std::endl;
        output << "  FILE              File to save brain state to" << std::endl;
    }
};

// Load brain command
class LoadBrainCommand : public CommandAction {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from file"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        if (args.size() < 1) {
            output << "ERROR: Missing input file" << std::endl;
            return false;
        }
        
        std::string inputFile = args[0];
        output << "Loading brain from " << inputFile << "..." << std::endl;
        
        try {
            auto brain = std::make_shared<Brain>(config);
            
            // Initialize first (create new brain)
            if (!brain->initialize()) {
                output << "ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            // Then load from file
            if (brain->load(inputFile)) {
                output << "Brain loaded successfully" << std::endl;
                
                // Show status after loading
                output << "Loaded brain status:" << std::endl;
                output << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
                output << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
                output << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                
                return true;
            } else {
                output << "ERROR: Failed to load brain" << std::endl;
                return false;
            }
            
        } catch (const NLMError& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            output << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm load FILE" << std::endl;
        output << "  FILE              File to load brain state from" << std::endl;
    }
};

// Show help command
class HelpCommand : public CommandAction {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information"; }
    
    bool execute(std::shared_ptr<Config> config, 
                std::vector<std::string>& args,
                std::ostream& output) override {
        output << "=== NLM (Neural Learning Machine) Command Line Interface ===" << std::endl;
        output << std::endl;
        output << "Available commands:" << std::endl;
        
        // Show all commands
        std::vector<CommandAction*> commands = {
            new CreateBrainCommand(),
            new InitializeBrainCommand(),
            new RunSimulationCommand(),
            new ShowStatusCommand(),
            new SaveBrainCommand(),
            new LoadBrainCommand(),
            new HelpCommand()
        };
        
        for (const auto& cmd : commands) {
            output << "  " << cmd->getName() << "    - " << cmd->getDescription() << std::endl;
        }
        
        output << std::endl;
        output << "Use 'nlm <command> --help' for detailed help on a specific command" << std::endl;
        
        // Clean up
        for (auto cmd : commands) {
            delete cmd;
        }
        
        return true;
    }
    
    void showHelp(std::ostream& output) const override {
        output << "Usage: nlm <command> [options]" << std::endl;
        output << "Commands:" << std::endl;
        output << "  help        Show this help message" << std::endl;
        output << "  create      Create and initialize a new brain" << std::endl;
        output << "  initialize  Initialize a brain for simulation" << std::endl;
        output << "  run         Run simulation for specified number of steps" << std::endl;
        output << "  status      Show brain status and statistics" << std::endl;
        output << "  save FILE   Save brain state to file" << std::endl;
        output << "  load FILE   Load brain state from file" << std::endl;
    }
};

// Main CLI class
class NLMCLI {
private:
    std::vector<std::shared_ptr<CommandAction>> commands;
    std::shared_ptr<Config> defaultConfig;
    
public:
    NLMCLI() {
        // Register all commands
        commands.push_back(std::make_shared<CreateBrainCommand>());
        commands.push_back(std::make_shared<InitializeBrainCommand>());
        commands.push_back(std::make_shared<RunSimulationCommand>());
        commands.push_back(std::make_shared<ShowStatusCommand>());
        commands.push_back(std::make_shared<SaveBrainCommand>());
        commands.push_back(std::make_shared<LoadBrainCommand>());
        commands.push_back(std::make_shared<HelpCommand>());
        
        defaultConfig = std::make_shared<Config>();
    }
    
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args,
                       std::ostream& output = std::cout) {
        // Find the command
        for (const auto& cmd : commands) {
            if (cmd->getName() == commandName) {
                return cmd->execute(defaultConfig, args, output);
            }
        }
        
        output << "Error: Unknown command '" << commandName << "'" << std::endl;
        output << "Use 'nlm help' for a list of available commands" << std::endl;
        return false;
    }
    
    void showHelp(const std::string& commandName = "", 
                  std::ostream& output = std::cout) const {
        if (commandName.empty()) {
            for (const auto& cmd : commands) {
                cmd->showHelp(output);
                output << std::endl;
            }
        } else {
            for (const auto& cmd : commands) {
                if (cmd->getName() == commandName) {
                    cmd->showHelp(output);
                    return;
                }
            }
            output << "Error: Unknown command '" << commandName << "'" << std::endl;
        }
    }
    
    void listCommands(std::ostream& output = std::cout) const {
        output << "Available commands:" << std::endl;
        for (const auto& cmd : commands) {
            output << "  " << cmd->getName() << "    - " << cmd->getDescription() << std::endl;
        }
    }
    
    std::shared_ptr<Config> getDefaultConfig() const { return defaultConfig; }
};

} // namespace nlm
