// NLM Command Line Interface
// Comprehensive command-line interface for the Neural Learning Machine
// 
// Provides a unified interface for running simulations, managing brain state,
// configuring parameters, and accessing advanced features.

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include <algorithm>

// Forward declarations
class NLMCLI;

// Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(NLMCLI& cli, const std::vector<std::string>& args) = 0;
};

// CLI state and context
class NLMCLI {
public:
    NLMCLI() : currentBrain(nullptr), verboseMode(false), profilingEnabled(false) {}
    
    // Brain management
    void setBrain(std::shared_ptr<Brain> brain) { currentBrain = brain; }
    std::shared_ptr<Brain> getBrain() const { return currentBrain; }
    
    // Configuration
    void setVerbose(bool verbose) { verboseMode = verbose; }
    bool isVerbose() const { return verboseMode; }
    
    void setProfiling(bool profiling) { profilingEnabled = profiling; }
    bool isProfiling() const { return profilingEnabled; }
    
    // Command execution helpers
    bool runCommand(const std::string& commandName, const std::vector<std::string>& args);
    
    // Utility methods
    void printError(const std::string& message);
    void printInfo(const std::string& message);
    void printSuccess(const std::string& message);
    void printWarning(const std::string& message);
    
    // Configuration management
    bool loadConfig(const std::string& filepath);
    bool saveConfig(const std::string& filepath);
    
    // Simulation management
    bool runSimulation(size_t steps, size_t timestep, const std::string& outputFile = "");
    bool runInteractiveSimulation();
    
    // Checkpoint management
    bool saveCheckpoint(const std::string& name = "");
    bool loadCheckpoint(const std::string& name);
    void listCheckpoints();
    
    // Brain state management
    void resetBrain();
    void printBrainStats();
    
    // Help and documentation
    void printHelp();
    void printCommandHelp(const std::string& commandName);

private:
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Config> currentConfig;
    bool verboseMode;
    bool profilingEnabled;
    
    // Command registry
    std::map<std::string, std::unique_ptr<Command>> commands;
    
    // Initialize commands
    void initializeCommands();
};

// Concrete command implementations
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information"; }
    std::string getUsage() const override { return "help [command]"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class RunCommand : public Command {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run simulation" };
    std::string getUsage() const override { return "run [steps] [output_file]"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class ResetCommand : public Command {
public:
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset brain state"; }
    std::string getUsage() const override { return "reset"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class SaveCommand : public Command {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save brain state to checkpoint"; }
    std::string getUsage() const override { return "save [name]"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class LoadCommand : public Command {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from checkpoint"; }
    std::string getUsage() const override { return "load <name>"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Manage configuration"; }
    std::string getUsage() const override { return "config <command> [args]"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class StatsCommand : public Command {
public:
    std::string getName() const override { return "stats"; }
    std::string getDescription() const override { return "Print brain statistics"; }
    std::string getUsage() const override { return "stats"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class InteractiveCommand : public Command {
public:
    std::string getName() const override { return "interactive"; }
    std::string getDescription() const override { return "Run interactive simulation session"; }
    std::string getUsage() const override { return "interactive"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

class ListCommand : public Command {
public:
    std::string getName() const override { return "list"; }
    std::string getDescription() const override { return "List available checkpoints"; }
    std::string getUsage() const override { return "list"; }
    
    bool execute(NLMCLI& cli, const std::vector<std::string>& args) override;
};

// Implementation of NLMCLI methods
bool NLMCLI::runCommand(const std::string& commandName, const std::vector<std::string>& args) {
    auto it = commands.find(commandName);
    if (it == commands.end()) {
        printError("Unknown command: " + commandName);
        return false;
    }
    
    return it->second->execute(*this, args);
}

void NLMCLI::printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void NLMCLI::printInfo(const std::string& message) {
    if (verboseMode) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void NLMCLI::printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void NLMCLI::printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

bool NLMCLI::loadConfig(const std::string& filepath) {
    if (verboseMode) {
        printInfo("Loading configuration from: " + filepath);
    }
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        printError("Failed to load configuration: " + filepath);
        return false;
    }
    
    currentConfig = config;
    printSuccess("Configuration loaded successfully");
    return true;
}

bool NLMCLI::saveConfig(const std::string& filepath) {
    if (!currentConfig) {
        printError("No configuration to save");
        return false;
    }
    
    if (verboseMode) {
        printInfo("Saving configuration to: " + filepath);
    }
    
    if (!currentConfig->saveToFile(filepath)) {
        printError("Failed to save configuration: " + filepath);
        return false;
    }
    
    printSuccess("Configuration saved successfully");
    return true;
}

bool NLMCLI::runSimulation(size_t steps, size_t timestep, const std::string& outputFile) {
    if (!currentBrain) {
        printError("No brain initialized. Load a brain first.");
        return false;
    }
    
    if (verboseMode) {
        printInfo(std::string("Running simulation: ") + std::to_string(steps) + 
            " steps with timestep: " + std::to_string(timestep));
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < steps; ++i) {
        currentBrain->step(i, static_cast<Timestamp>(i) * timestep);
        
        if (profilingEnabled && i % 1000 == 0) {
            printInfo(std::string("Progress: ") + std::to_string(i) + "/" + 
                std::to_string(steps) + " steps");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (verboseMode) {
        printSuccess(std::string("Simulation completed in ") + std::to_string(duration.count()) + 
            " ms (" + std::to_string(steps / (duration.count() / 1000.0)) + " steps/sec)");
    }
    
    if (!outputFile.empty()) {
        if (!currentBrain->save(outputFile)) {
            printError("Failed to save simulation output: " + outputFile);
            return false;
        }
        printSuccess("Simulation output saved to: " + outputFile);
    }
    
    return true;
}

bool NLMCLI::runInteractiveSimulation() {
    if (!currentBrain) {
        printError("No brain initialized. Load a brain first.");
        return false;
    }
    
    printInfo("Starting interactive simulation session...")
    printInfo("Type 'help' for available commands, 'exit' to quit");
    
    std::string line;
    size_t step = 0;
    
    while (true) {
        std::cout << ">>> Step " << step << ": ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        if (line == "exit" || line == "quit") {
            break;
        } else if (line == "help") {
            printHelp();
        } else if (line == "step") {
            currentBrain->step(step);
            step++;
            printInfo(std::string("Completed step ") + std::to_string(step - 1));
        } else if (line == "stats") {
            printBrainStats();
        } else if (line.substr(0, 4) == "load ") {
            std::string name = line.substr(5);
            if (loadCheckpoint(name)) {
                step = 0;
            }
        } else if (line.substr(0, 5) == "save ") {
            std::string name = line.substr(6);
            saveCheckpoint(name);
        } else {
            printError("Unknown command: " + line);
        }
    }
    
    printInfo("Interactive simulation ended");
    return true;
}

bool NLMCLI::saveCheckpoint(const std::string& name) {
    if (!currentBrain) {
        printError("No brain to save");
        return false;
    }
    
    std::string filename = "checkpoint_" + name + "_" + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".nlmckpt";
    
    if (verboseMode) {
        printInfo(std::string("Saving checkpoint to: ") + filename);
    }
    
    if (!currentBrain->save(filename)) {
        printError("Failed to save checkpoint");
        return false;
    }
    
    printSuccess("Checkpoint saved: " + filename);
    return true;
}

bool NLMCLI::loadCheckpoint(const std::string& name) {
    std::string filename = "checkpoint_" + name + ".nlmckpt";
    
    if (verboseMode) {
        printInfo(std::string("Loading checkpoint from: ") + filename);
    }
    
    if (!currentBrain->load(filename)) {
        printError("Failed to load checkpoint: " + filename);
        return false;
    }
    
    printSuccess("Checkpoint loaded: " + filename);
    return true;
}

void NLMCLI::listCheckpoints() {
    // In a real implementation, would read from checkpoint directory
    // For now, just show info message
    printInfo("Available checkpoints (use 'help list' for details)");
}

void NLMCLI::resetBrain() {
    if (currentBrain) {
        currentBrain->reset();
        printSuccess("Brain reset completed");
    } else {
        printWarning("No brain to reset");
    }
}

void NLMCLI::printBrainStats() {
    if (currentBrain) {
        currentBrain->logStatus();
    } else {
        printWarning("No brain to display stats");
    }
}

void NLMCLI::printHelp() {
    std::cout << "Neural Learning Machine (NLM) Command Line Interface" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available commands:" << std::endl;
    for (const auto& pair : commands) {
        std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Use 'help <command>' for more details about a specific command." << std::endl;
}

void NLMCLI::printCommandHelp(const std::string& commandName) {
    auto it = commands.find(commandName);
    if (it == commands.end()) {
        printError("Unknown command: " + commandName);
        return;
    }
    
    std::cout << "Command: " << it->second->getName() << std::endl;
    std::cout << "Description: " << it->second->getDescription() << std::endl;
    std::cout << "Usage: " << it->second->getUsage() << std::endl;
}

// HelpCommand implementation
bool HelpCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    if (args.empty()) {
        cli.printHelp();
        return true;
    }
    
    cli.printCommandHelp(args[0]);
    return true;
}

// RunCommand implementation
bool RunCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    size_t steps = 1000;  // Default steps
    std::string outputFile;
    
    if (!args.empty()) {
        try {
            steps = std::stoul(args[0]);
        } catch (...) {
            cli.printError("Invalid step count: " + args[0]);
            return false;
        }
    }
    
    if (args.size() > 1) {
        outputFile = args[1];
    }
    
    return cli.runSimulation(steps, 1, outputFile);
}

// ResetCommand implementation
bool ResetCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    cli.resetBrain();
    return true;
}

// SaveCommand implementation
bool SaveCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    std::string name;
    if (!args.empty()) {
        name = args[0];
    }
    
    return cli.saveCheckpoint(name);
}

// LoadCommand implementation
bool LoadCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    if (args.empty()) {
        cli.printError("Checkpoint name required");
        return false;
    }
    
    return cli.loadCheckpoint(args[0]);
}

// ConfigCommand implementation
bool ConfigCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    // For now, just show help
    cli.printInfo("Configuration management commands:")
    cli.printInfo("  config load <file> - Load configuration from file")
    cli.printInfo("  config save <file> - Save configuration to file")
    return true;
}

// StatsCommand implementation
bool StatsCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    cli.printBrainStats();
    return true;
}

// InteractiveCommand implementation
bool InteractiveCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    return cli.runInteractiveSimulation();
}

// ListCommand implementation
bool ListCommand::execute(NLMCLI& cli, const std::vector<std::string>& args) {
    cli.listCheckpoints();
    return true;
}

// Initialize commands
void NLMCLI::initializeCommands() {
    commands["help"] = std::make_unique<HelpCommand>();
    commands["run"] = std::make_unique<RunCommand>();
    commands["reset"] = std::make_unique<ResetCommand>();
    commands["save"] = std::make_unique<SaveCommand>();
    commands["load"] = std::make_unique<LoadCommand>();
    commands["config"] = std::make_unique<ConfigCommand>();
    commands["stats"] = std::make_unique<StatsCommand>();
    commands["interactive"] = std::make_unique<InteractiveCommand>();
    commands["list"] = std::make_unique<ListCommand>();
}

// Main function
int main(int argc, char* argv[]) {
    NLMCLI cli;
    cli.initializeCommands();
    
    // Parse command line arguments
    std::string command;
    std::vector<std::string> args;
    
    if (argc < 2) {
        cli.printHelp();
        return 0;
    }
    
    command = argv[1];
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Parse flags
    bool verbose = false;
    bool profiling = false;
    std::string configFile;
    
    for (const auto& arg : args) {
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--profile" || arg == "-p") {
            profiling = true;
        } else if (arg.substr(0, 8) == "--config=") {
            configFile = arg.substr(8);
        } else if (arg.substr(0, 2) == "-c") {
            if (arg.size() > 2) {
                configFile = arg.substr(2);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    cli.setVerbose(verbose);
    cli.setProfiling(profiling);
    
    // Load configuration if specified
    if (!configFile.empty()) {
        if (!cli.loadConfig(configFile)) {
            return 1;
        }
    }
    
    // Create a default brain for now
    // In a real implementation, would use configuration to create brain
    auto config = std::make_shared<Config>();
    config->set<size_t>("neuron_count", 1000);
    config->set<size_t>("region_count", 1);
    config->set<float>("simulation_timestep", 0.001f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "[ERROR] Failed to initialize brain" << std::endl;
        return 1;
    }
    
    cli.setBrain(brain);
    
    // Execute command
    if (!cli.runCommand(command, args)) {
        return 1;
    }
    
    return 0;
}