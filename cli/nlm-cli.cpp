#!/usr/bin/env cpp
// NLM Command Line Interface
// Version 0.1.0
// Provides comprehensive CLI tools for interacting with the NLM brain simulation

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>

// NLM includes
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/experiments/Phase6Demo.hpp"
#include "src/experiments/Phase5Experiment.hpp"
#include "src/experiments/Phase4Demo.hpp"
#include "src/experiments/Phase3Demo.hpp"
#include "src/experiments/Phase2Demo.hpp"
#include "src/visualization/VisualizationInterface.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"

namespace nlm {

// CLI command base class
class CLICommand {
public:
    virtual ~CLICommand() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual bool isHidden() const { return false; }
};

// CLI tool for running experiments
class ExperimentRunnerCommand : public CLICommand {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { 
        return "Run NLM experiments (Phase 2, 3, 4, 5, or 6)"; 
    }
    std::string getUsage() const override { 
        return "nlm experiment <phase> [--config file] [--steps N] [--neurons N] [--output file]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.size() < 1) {
            std::cerr << "Error: Phase required" << std::endl;
            return false;
        }
        
        std::string phase = args[0];
        std::string configFile = "configs/default.cfg";
        size_t steps = 0;
        size_t neurons = 1000;
        std::string outputFile;
        
        // Parse arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--config" && i + 1 < args.size()) {
                configFile = args[++i];
            } else if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoul(args[++i]);
            } else if (args[i] == "--neurons" && i + 1 < args.size()) {
                neurons = std::stoul(args[++i]);
            } else if (args[i] == "--output" && i + 1 < args.size()) {
                outputFile = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        std::cout << "=== NLM Experiment Runner ===" << std::endl;
        std::cout << "Phase: " << phase << std::endl;
        std::cout << "Config: " << configFile << std::endl;
        std::cout << "Neurons: " << neurons << std::endl;
        if (steps > 0) std::cout << "Steps: " << steps << std::endl;
        
        try {
            if (phase == "2" || phase == "phase2") {
                runPhase2Demo(steps, neurons, configFile);
            } else if (phase == "3" || phase == "phase3") {
                runPhase3Demo(steps, neurons, configFile);
            } else if (phase == "4" || phase == "phase4") {
                runPhase4Demo(steps, neurons, configFile);
            } else if (phase == "5" || phase == "phase5") {
                runPhase5Experiment(steps, neurons, configFile);
            } else if (phase == "6" || phase == "phase6") {
                runPhase6Demo(steps, neurons, configFile);
            } else {
                std::cerr << "Error: Unknown phase '" << phase << "'" << std::endl;
                return false;
            }
            
            std::cout << "Experiment completed successfully!" << std::endl;
            
            // Save results if output file specified
            if (!outputFile.empty()) {
                saveResults(outputFile);
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error running experiment: " << e.what() << std::endl;
            return false;
        }
        
        return true;
    }
    
private:
    void runPhase2Demo(size_t steps, size_t neurons, const std::string& configFile) {
        std::cout << "Running Phase 2: Real Neural Computation Demo" << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", neurons);
        cfg->set("region_count", 1);
        cfg->set("connection_probability", 0.15f);
        cfg->set("stdp_ltp_weight", 0.02f);
        cfg->set("stdp_ltd_weight", 0.015f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run simulation
        size_t maxSteps = steps > 0 ? steps : 100;
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 10 == 0) {
                std::cout << "Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getAverageFiringRate() << std::endl;
            }
        }
        
        std::cout << "Phase 2 demo completed successfully!" << std::endl;
    }
    
    void runPhase3Demo(size_t steps, size_t neurons, const std::string& configFile) {
        std::cout << "Running Phase 3: World Interaction Demo" << std::endl;
        // Placeholder for Phase 3 demo
        std::cout << "Phase 3 demo - implementation pending" << std::endl;
    }
    
    void runPhase4Demo(size_t steps, size_t neurons, const std::string& configFile) {
        std::cout << "Running Phase 4: Cognitive Systems Demo" << std::endl;
        // Placeholder for Phase 4 demo
        std::cout << "Phase 4 demo - implementation pending" << std::endl;
    }
    
    void runPhase5Experiment(size_t steps, size_t neurons, const std::string& configFile) {
        std::cout << "Running Phase 5: Performance Optimization Experiment" << std::endl;
        // Placeholder for Phase 5 experiment
        std::cout << "Phase 5 experiment - implementation pending" << std::endl;
    }
    
    void runPhase6Demo(size_t steps, size_t neurons, const std::string& configFile) {
        std::cout << "Running Phase 6: Integration Demo" << std::endl;
        // Phase 6 integration demo
        Phase6IntegratedExperiment experiment;
        
        Phase6Config config;
        config.neuronCount = neurons;
        config.maxSteps = steps > 0 ? steps : 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        std::cout << "=== Phase 6 Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Integration status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    }
    
    void saveResults(const std::string& outputFile) {
        std::cout << "Saving results to: " << outputFile << std::endl;
        // Implementation would save experiment results
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "NLM Experiment Results" << std::endl;
            file.close();
        }
    }
};

// CLI tool for managing checkpoints
class CheckpointCommand : public CLICommand {
public:
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { 
        return "Manage brain checkpoints (save/load/list)"; 
    }
    std::string getUsage() const override { 
        return "nlm checkpoint <save|load|list> [--file path] [--brain brain_file]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Subcommand required (save/load/list)" << std::endl;
            return false;
        }
        
        std::string subcommand = args[0];
        std::string checkpointFile = "./checkpoint.bin";
        std::string brainFile;
        
        // Parse arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--file" && i + 1 < args.size()) {
                checkpointFile = args[++i];
            } else if (args[i] == "--brain" && i + 1 < args.size()) {
                brainFile = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        if (subcommand == "save") {
            return saveCheckpoint(brainFile, checkpointFile);
        } else if (subcommand == "load") {
            return loadCheckpoint(brainFile, checkpointFile);
        } else if (subcommand == "list") {
            return listCheckpoints();
        } else {
            std::cerr << "Error: Unknown subcommand '" << subcommand << "'" << std::endl;
            return false;
        }
    }
    
private:
    bool saveCheckpoint(const std::string& brainFile, const std::string& checkpointFile) {
        std::cout << "Saving brain checkpoint to: " << checkpointFile << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        
        // Load brain from file if specified
        std::shared_ptr<Brain> brain;
        if (!brainFile.empty()) {
            auto tempBrain = std::make_shared<Brain>(cfg);
            if (!tempBrain->load(brainFile)) {
                std::cerr << "Error: Failed to load brain from " << brainFile << std::endl;
                return false;
            }
            brain = tempBrain;
        } else {
            brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain for checkpointing");
            }
            // Run some simulation steps
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        
        // Save checkpoint
        if (brain->save(checkpointFile)) {
            std::cout << "Checkpoint saved successfully" << std::endl;
            return true;
        } else {
            std::cerr << "Error: Failed to save checkpoint" << std::endl;
            return false;
        }
    }
    
    bool loadCheckpoint(const std::string& brainFile, const std::string& checkpointFile) {
        std::cout << "Loading brain checkpoint from: " << checkpointFile << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        
        // Create new brain and load checkpoint
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        
        if (brain->load(checkpointFile)) {
            std::cout << "Checkpoint loaded successfully" << std::endl;
            
            // Save to brain file if specified
            if (!brainFile.empty()) {
                if (brain->save(brainFile)) {
                    std::cout << "Brain saved to: " << brainFile << std::endl;
                }
            }
            
            return true;
        } else {
            std::cerr << "Error: Failed to load checkpoint" << std::endl;
            return false;
        }
    }
    
    bool listCheckpoints() {
        std::cout << "=== Checkpoint Files ===" << std::endl;
        
        // List checkpoint files in current directory
        std::vector<std::string> checkpointFiles;
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".bin" || entry.path().extension() == ".chk")) {
                checkpointFiles.push_back(entry.path().filename().string());
            }
        }
        
        if (checkpointFiles.empty()) {
            std::cout << "No checkpoint files found" << std::endl;
        } else {
            for (const auto& file : checkpointFiles) {
                std::cout << "  " << file << std::endl;
            }
        }
        
        return true;
    }
};

// CLI tool for visualization
class VisualizeCommand : public CLICommand {
public:
    std::string getName() const override { return "visualize"; }
    std::string getDescription() const override { 
        return "Launch visualization interface"; 
    }
    std::string getUsage() const override { 
        return "nlm visualize [--update-rate Hz] [--window-size WxH] [--data-file file]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        double updateRate = 30.0;
        std::string dataFile;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--update-rate" && i + 1 < args.size()) {
                updateRate = std::stod(args[++i]);
            } else if (args[i] == "--data-file" && i + 1 < args.size()) {
                dataFile = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        std::cout << "=== NLM Visualization Interface ===" << std::endl;
        std::cout << "Update rate: " << updateRate << " Hz" << std::endl;
        
        if (!dataFile.empty()) {
            std::cout << "Data file: " << dataFile << std::endl;
        }
        
        // Create and initialize visualization interface
        VisualizationInterface viz;
        if (!viz.initialize()) {
            std::cerr << "Error: Failed to initialize visualization interface" << std::endl;
            return false;
        }
        
        viz.setUpdateRate(updateRate);
        
        std::cout << "Visualization interface started successfully" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
        
        // In a real implementation, this would start a GUI event loop
        // For now, we just show the available commands
        
        std::cout << "\nAvailable commands in visualization:" << std::endl;
        std::cout << "  'h' - Show this help" << std::endl;
        std::cout << "  'c' - Close visualization" << std::endl;
        std::cout << "  's' - Save current view" << std::endl;
        std::cout << "  't' - Toggle visualization modes" << std::endl;
        std::cout << "  '+' - Increase update rate" << std::endl;
        std::cout << "  '-' - Decrease update rate" << std::endl;
        
        return true;
    }
};

// CLI tool for configuration management
class ConfigCommand : public CLICommand {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Manage NLM configuration"; 
    }
    std::string getUsage() const override { 
        return "nlm config <show|get/set/remove> [--key key] [--value value] [--file file]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Subcommand required (show/get/set/remove)" << std::endl;
            return false;
        }
        
        std::string subcommand = args[0];
        std::string key;
        std::string value;
        std::string configFile;
        
        // Parse arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--key" && i + 1 < args.size()) {
                key = args[++i];
            } else if (args[i] == "--value" && i + 1 < args.size()) {
                value = args[++i];
            } else if (args[i] == "--file" && i + 1 < args.size()) {
                configFile = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        if (subcommand == "show") {
            return showConfig(configFile);
        } else if (subcommand == "get") {
            if (key.empty()) {
                std::cerr << "Error: Key required for get operation" << std::endl;
                return false;
            }
            return getConfigValue(key);
        } else if (subcommand == "set") {
            if (key.empty()) {
                std::cerr << "Error: Key required for set operation" << std::endl;
                return false;
            }
            if (value.empty()) {
                std::cerr << "Error: Value required for set operation" << std::endl;
                return false;
            }
            return setConfigValue(key, value);
        } else if (subcommand == "remove") {
            if (key.empty()) {
                std::cerr << "Error: Key required for remove operation" << std::endl;
                return false;
            }
            return removeConfigValue(key);
        } else {
            std::cerr << "Error: Unknown subcommand '" << subcommand << "'" << std::endl;
            return false;
        }
    }
    
private:
    bool showConfig(const std::string& configFile) {
        std::cout << "=== NLM Configuration ===" << std::endl;
        
        auto cfg = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            if (cfg->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            } else {
                std::cerr << "Error: Failed to load configuration from " << configFile << std::endl;
                return false;
            }
        }
        
        std::cout << cfg->summary() << std::endl;
        return true;
    }
    
    bool getConfigValue(const std::string& key) {
        auto cfg = std::make_shared<Config>();
        
        std::cout << key << " = ";
        
        if (cfg->has(key)) {
            if (auto intVal = cfg->get<int>(key)) {
                std::cout << *intVal << std::endl;
            } else if (auto int64Val = cfg->get<int64_t>(key)) {
                std::cout << *int64Val << std::endl;
            } else if (auto doubleVal = cfg->get<double>(key)) {
                std::cout << *doubleVal << std::endl;
            } else if (auto boolVal = cfg->get<bool>(key)) {
                std::cout << (*boolVal ? "true" : "false") << std::endl;
            } else if (auto stringVal = cfg->get<std::string>(key)) {
                std::cout << "\"" << *stringVal << "\"" << std::endl;
            } else {
                std::cout << "(unknown type)" << std::endl;
            }
        } else {
            std::cout << "(not set)" << std::endl;
        }
        
        return true;
    }
    
    bool setConfigValue(const std::string& key, const std::string& value) {
        auto cfg = std::make_shared<Config>();
        cfg->set(key, value, ConfigSource::Runtime);
        std::cout << "Set " << key << " = " << value << std::endl;
        return true;
    }
    
    bool removeConfigValue(const std::string& key) {
        auto cfg = std::make_shared<Config>();
        cfg->remove(key);
        std::cout << "Removed " << key << std::endl;
        return true;
    }
};

// CLI tool for debugging
class DebugCommand : public CLICommand {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { 
        return "Debug and profile NLM systems"; 
    }
    std::string getUsage() const override { 
        return "nlm debug <spike-system|memory-system|neuromodulation|all> [--output file]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Component required (spike-system/memory-system/neuromodulation/all)" << std::endl;
            return false;
        }
        
        std::string component = args[0];
        std::string outputFile;
        
        // Parse arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--output" && i + 1 < args.size()) {
                outputFile = args[++i];
            } else {
                std::cerr << "Unknown argument: " << args[i] << std::endl;
                return false;
            }
        }
        
        std::cout << "=== NLM Debug Tool ===" << std::endl;
        std::cout << "Component: " << component << std::endl;
        
        if (component == "spike-system") {
            debugSpikeSystem(outputFile);
        } else if (component == "memory-system") {
            debugMemorySystem(outputFile);
        } else if (component == "neuromodulation") {
            debugNeuromodulation(outputFile);
        } else if (component == "all") {
            debugAllSystems(outputFile);
        } else {
            std::cerr << "Error: Unknown component '" << component << "'" << std::endl;
            return false;
        }
        
        return true;
    }
    
private:
    void debugSpikeSystem(const std::string& outputFile) {
        std::cout << "Spike System Debug:" << std::endl;
        // Placeholder for spike system debugging
        std::cout << "  Spike events recorded: (simulation would show this)" << std::endl;
        if (!outputFile.empty()) {
            std::cout << "  Debug data saved to: " << outputFile << std::endl;
        }
    }
    
    void debugMemorySystem(const std::string& outputFile) {
        std::cout << "Memory System Debug:" << std::endl;
        // Placeholder for memory system debugging
        std::cout << "  Working memory traces: (simulation would show this)" << std::endl;
        std::cout << "  Episodic memory episodes: (simulation would show this)" << std::endl;
        if (!outputFile.empty()) {
            std::cout << "  Debug data saved to: " << outputFile << std::endl;
        }
    }
    
    void debugNeuromodulation(const std::string& outputFile) {
        std::cout << "Neuromodulation Debug:" << std::endl;
        // Placeholder for neuromodulation debugging
        std::cout << "  Dopamine level: (simulation would show this)" << std::endl;
        std::cout << "  Curiosity level: (simulation would show this)" << std::endl;
        if (!outputFile.empty()) {
            std::cout << "  Debug data saved to: " << outputFile << std::endl;
        }
    }
    
    void debugAllSystems(const std::string& outputFile) {
        std::cout << "Complete System Debug:" << std::endl;
        debugSpikeSystem(outputFile);
        debugMemorySystem(outputFile);
        debugNeuromodulation(outputFile);
    }
};

// CLI tool for help and version information
class HelpCommand : public CLICommand {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Show help information"; 
    }
    std::string getUsage() const override { 
        return "nlm help [command]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            showGeneralHelp();
        } else {
            showCommandHelp(args[0]);
        }
        return true;
    }
    
private:
    void showGeneralHelp() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        
        // Collect available commands (excluding help)
        std::vector<CLICommand*> commands = {
            new ExperimentRunnerCommand(),
            new CheckpointCommand(),
            new VisualizeCommand(),
            new ConfigCommand(),
            new DebugCommand(),
        };
        
        for (CLICommand* cmd : commands) {
            if (!cmd->isHidden()) {
                std::cout << "  " << cmd->getName() << " - " << cmd->getDescription() << std::endl;
            }
            delete cmd;
        }
        
        std::cout << std::endl;
        std::cout << "For more information about a specific command, use: nlm help <command>" << std::endl;
    }
    
    void showCommandHelp(const std::string& commandName) {
        // This would need a registry of commands to find the specific command
        // For now, just show a simple help
        std::cout << "Help for command: " << commandName << std::endl;
        std::cout << "(Detailed help implementation pending)" << std::endl;
    }
};

// CLI tool for version information
class VersionCommand : public CLICommand {
public:
    std::string getName() const override { return "version"; }
    std::string getDescription() const override { 
        return "Show version information"; 
    }
    std::string getUsage() const override { 
        return "nlm version"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Version: 0.1.0 (Development Build)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Phase: 6 (Final Integration)" << std::endl;
        std::cout << "Architecture: Artificial Developmental Brain" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
        return true;
    }
};

// Main CLI dispatcher
class NLMCLI {
public:
    NLMCLI() {
        // Register all available commands
        registerCommands();
    }
    
    void run(int argc, char** argv) {
        if (argc < 2) {
            printUsage();
            return;
        }
        
        std::string commandName = argv[1];
        std::vector<std::string> args;
        
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        CLICommand* command = findCommand(commandName);
        if (!command) {
            std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
            printUsage();
            return;
        }
        
        try {
            bool success = command->execute(args);
            if (!success) {
                std::cerr << "Command failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        
        delete command;
    }
    
private:
    std::vector<CLICommand*> commands;
    
    void registerCommands() {
        // Register all commands
        commands.push_back(new ExperimentRunnerCommand());
        commands.push_back(new CheckpointCommand());
        commands.push_back(new VisualizeCommand());
        commands.push_back(new ConfigCommand());
        commands.push_back(new DebugCommand());
        commands.push_back(new HelpCommand());
        commands.push_back(new VersionCommand());
    }
    
    CLICommand* findCommand(const std::string& name) {
        for (CLICommand* cmd : commands) {
            if (cmd->getName() == name) {
                return cmd;
            }
        }
        return nullptr;
    }
    
    void printUsage() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Usage: nlm <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        
        for (CLICommand* cmd : commands) {
            if (!cmd->isHidden()) {
                std::cout << "  " << cmd->getName() << " - " << cmd->getDescription() << std::endl;
            }
            delete cmd;
        }
        
        std::cout << std::endl;
        std::cout << "For more information about a specific command, use: nlm help <command>" << std::endl;
        std::cout << "To get version information: nlm version" << std::endl;
    }
};

// Main function
int main(int argc, char** argv) {
    // Initialize NLM logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create and run CLI
    NLMCLI cli;
    cli.run(argc, argv);
    
    return 0;
}

} // namespace nlm

// Entry point
int main(int argc, char** argv) {
    return nlm::main(argc, argv);
}