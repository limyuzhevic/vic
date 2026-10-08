// NLM Command Line Interface (CLI) Tool
// Provides common workflows and commands for different user levels
// Part of the NLM (Neural Learning Machine) system improvements

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

namespace nlm {

class NLMCLI {
public:
    NLMCLI() {
        // Initialize logger for CLI
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        printWelcomeMessage();
    }
    
    void run() {
        printHelp();
        
        while (true) {
            std::cout << "\nNLM CLI> ";
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            processCommand(command);
        }
    }
    
private:
    void printWelcomeMessage() {
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     NLM (熙然) - Neural Learning Machine CLI                      ║" << std::endl;
        std::cout << "║                     Command Line Interface for Common Workflows                 ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "Welcome to NLM CLI! This tool provides common workflows and commands for:" << std::endl;
        std::cout << "  • Quick start simulations for beginners" << std::endl;
        std::cout << "  • Configuration management" << std::endl;
        std::cout << "  • Experiment execution" << std::endl;
        std::cout << "  • Performance benchmarking" << std::endl;
        std::cout << "  • Help and documentation" << std::endl;
        std::cout << "  • Advanced features for power users" << std::endl;
    }
    
    void printHelp() {
        std::cout << "\nAvailable Commands:" << std::endl;
        std::cout << "══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        
        struct CommandInfo {
            std::string name;
            std::string description;
            std::string usage;
        };
        
        std::vector<CommandInfo> commands = {
            {"help", "Show this help message", "help [command]"},
            {"quick", "Quick start simulation for beginners", "quick [steps]"},
            {"demo", "Run demonstration with world interaction", "demo [steps]"},
            {"config", "Configuration management commands", "config <subcommand> [args]"},
            {"run", "Run experiment with current configuration", "run [steps]"},
            {"benchmark", "Run performance benchmarks", "benchmark [type]"},
            {"batch", "Run batch of experiments", "batch <count> [steps]"},
            {"monitor", "Monitor brain activity", "monitor [duration]"},
            {"save", "Save current state to file", "save <filename>"},
            {"load", "Load state from file", "load <filename>"},
            {"explore", "Explore NLM features", "explore [mode]"},
            {"advanced", "Advanced features for power users", "advanced <command>"},
            {"exit", "Exit NLM CLI", "exit"},
            {"quit", "Exit NLM CLI", "quit"}
        };
        
        for (const auto& cmd : commands) {
            std::cout << "  " << std::left << std::setw(12) << cmd.name 
                      << " - " << cmd.description << std::endl;
        }
        
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "\nFor more information on a specific command, type: help <command>" << std::endl;
    }
    
    void printCommandHelp(const std::string& command) {
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "quick") {
            std::cout << "\nQuick Start Command:" << std::endl;
            std::cout << "====================" << std::endl;
            std::cout << "Runs a simple brain simulation for beginners." << std::endl;
            std::cout << "\nUsage: quick [steps]" << std::endl;
            std::cout << "  steps: Number of simulation steps (default: 100)" << std::endl;
            std::cout << "\nThis command demonstrates the basic NLM workflow:" << std::endl;
            std::cout << "  1. Create a brain with default configuration" << std::endl;
            std::cout << "  2. Initialize the brain" << std::endl;
            std::cout << "  3. Run simulation for specified steps" << std::endl;
            std::cout << "  4. Display statistics about neural activity" << std::endl;
        }
        else if (command == "demo") {
            std::cout << "\nDemo Command:" << std::endl;
            std::cout << "=============" << std::endl;
            std::cout << "Runs a demonstration with world interaction." << std::endl;
            std::cout << "\nUsage: demo [steps]" << std::endl;
            std::cout << "  steps: Number of simulation steps (default: 50)" << std::endl;
            std::cout << "\nThis command shows the complete brain-world interaction loop:" << std::endl;
            std::cout << "  1. Create brain and world" << std::endl;
            std::cout << "  2. Initialize agent brain interface" << std::endl;
            std::cout << "  3. Run simulation with sensory input and motor output" << std::endl;
            std::cout << "  4. Display world state and brain statistics" << std::endl;
        }
        else if (command == "config") {
            std::cout << "\nConfiguration Management Commands:" << std::endl;
            std::cout << "===============================" << std::endl;
            std::cout << "Commands for managing NLM configuration:" << std::endl;
            std::cout << "\n  show     - Display current configuration" << std::endl;
            std::cout << "  set <key> <value> - Set configuration value" << std::endl;
            std::cout << "  validate - Validate current configuration" << std::endl;
            std::cout << "  presets  - List available configuration presets" << std::endl;
            std::cout << "  save <file> - Save configuration to file" << std::endl;
            std::cout << "  load <file> - Load configuration from file" << std::endl;
        }
        else if (command == "run") {
            std::cout << "\nRun Command:" << std::endl;
            std::cout << "===========" << std::endl;
            std::cout << "Run experiment with current configuration." << std::endl;
            std::cout << "\nUsage: run [steps]" << std::endl;
            std::cout << "  steps: Number of simulation steps (default: 1000)" << std::endl;
            std::cout << "\nThis command runs the brain simulation using the current configuration." << std::endl;
            std::cout << "It's similar to 'quick' but with more detailed output and statistics." << std::endl;
        }
        else if (command == "benchmark") {
            std::cout << "\nBenchmark Command:" << std::endl;
            std::cout << "===================" << std::endl;
            std::cout << "Run performance benchmarks." << std::endl;
            std::cout << "\nUsage: benchmark [type]" << std::endl;
            std::cout << "  type: Benchmark type (cpu, memory, network, all) (default: cpu)" << std::endl;
            std::cout << "\nRuns benchmarks for different network sizes and reports performance metrics." << std::endl;
        }
        else if (command == "batch") {
            std::cout << "\nBatch Command:" << std::endl;
            std::cout << "=============" << std::endl;
            std::cout << "Run batch of experiments." << std::endl;
            std::cout << "\nUsage: batch <count> [steps]" << std::endl;
            std::cout << "  count: Number of experiments to run" << std::endl;
            std::cout << "  steps: Steps per experiment (default: 100)" << std::endl;
            std::cout << "\nRuns multiple experiments and aggregates results." << std::endl;
        }
        else if (command == "monitor") {
            std::cout << "\nMonitor Command:" << std::endl;
            std::cout << "===============" << std::endl;
            std::cout << "Monitor brain activity in real-time." << std::endl;
            std::cout << "\nUsage: monitor [duration]" << std::endl;
            std::cout << "  duration: Monitoring duration in seconds (default: 10)" << std::endl;
            std::cout << "\nReal-time monitoring of brain statistics and neural activity." << std::endl;
        }
        else if (command == "save") {
            std::cout << "\nSave Command:" << std::endl;
            std::cout << "============" << std::endl;
            std::cout << "Save current state to file." << std::endl;
            std::cout << "\nUsage: save <filename>" << std::endl;
            std::cout << "  filename: Name of file to save to" << std::endl;
            std::cout << "\nSaves brain state, configuration, and statistics to file." << std::endl;
        }
        else if (command == "load") {
            std::cout << "\nLoad Command:" << std::endl;
            std::cout << "============" << std::endl;
            std::cout << "Load state from file." << std::endl;
            std::cout << "\nUsage: load <filename>" << std::endl;
            std::cout << "  filename: Name of file to load from" << std::endl;
            std::cout << "\nLoads brain state, configuration, and statistics from file." << std::endl;
        }
        else if (command == "explore") {
            std::cout << "\nExplore Command:" << std::endl;
            std::cout << "===============" << std::endl;
            std::cout << "Explore NLM features." << std::endl;
            std::cout << "\nUsage: explore [mode]" << std::endl;
            std::cout << "  mode: Exploration mode (memory, cognition, neuromodulation, all) (default: memory)" << std::endl;
            std::cout << "\nDemonstrates specific NLM features and capabilities." << std::endl;
        }
        else if (command == "advanced") {
            std::cout << "\nAdvanced Features Commands:" << std::endl;
            std::cout << "===========================" << std::endl;
            std::cout << "Advanced commands for power users:" << std::endl;
            std::cout << "\n  script <file> - Run NLM script" << std::endl;
            std::cout << "  export <format> - Export results to different formats" << std::endl;
            std::cout << "  parallel <count> - Run experiments in parallel" << std::endl;
            std::cout << "  profile - Profile performance and optimize" << std::endl;
            std::cout << "  debug - Enable debug output and run with validation" << std::endl;
        }
        else {
            std::cout << "\nCommand not found: " << command << std::endl;
            std::cout << "Type 'help' for a list of available commands." << std::endl;
        }
    }
    
    void processCommand(const std::string& input) {
        std::istringstream iss(input);
        std::string command;
        iss >> command;
        
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "help") {
            std::string subcmd;
            iss >> subcmd;
            if (!subcmd.empty()) {
                printCommandHelp(subcmd);
            } else {
                printHelp();
            }
        }
        else if (command == "quick") {
            int steps;
            iss >> steps;
            if (steps <= 0) steps = 100;
            runQuickSimulation(steps);
        }
        else if (command == "demo") {
            int steps;
            iss >> steps;
            if (steps <= 0) steps = 50;
            runDemo(steps);
        }
        else if (command == "config") {
            std::string subcommand;
            iss >> subcommand;
            processConfigCommand(subcommand, iss);
        }
        else if (command == "run") {
            int steps;
            iss >> steps;
            if (steps <= 0) steps = 1000;
            runExperiment(steps);
        }
        else if (command == "benchmark") {
            std::string type;
            iss >> type;
            if (type.empty()) type = "cpu";
            runBenchmark(type);
        }
        else if (command == "batch") {
            int count;
            int steps;
            iss >> count >> steps;
            if (count <= 0) {
                std::cout << "Error: Batch count must be positive." << std::endl;
                return;
            }
            if (steps <= 0) steps = 100;
            runBatchExperiments(count, steps);
        }
        else if (command == "monitor") {
            int duration;
            iss >> duration;
            if (duration <= 0) duration = 10;
            monitorBrain(duration);
        }
        else if (command == "save") {
            std::string filename;
            iss >> filename;
            if (filename.empty()) {
                std::cout << "Error: Filename required for save command." << std::endl;
                return;
            }
            saveState(filename);
        }
        else if (command == "load") {
            std::string filename;
            iss >> filename;
            if (filename.empty()) {
                std::cout << "Error: Filename required for load command." << std::endl;
                return;
            }
            loadState(filename);
        }
        else if (command == "explore") {
            std::string mode;
            iss >> mode;
            if (mode.empty()) mode = "memory";
            exploreFeatures(mode);
        }
        else if (command == "advanced") {
            std::string subcmd;
            iss >> subcmd;
            processAdvancedCommand(subcmd, iss);
        }
        else if (command == "exit" || command == "quit") {
            std::cout << "Goodbye! Exiting NLM CLI." << std::endl;
            exit(0);
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for a list of available commands." << std::endl;
        }
    }
    
    void runQuickSimulation(int steps) {
        std::cout << "\nRunning quick simulation with " << steps << " steps..." << std::endl;
        
        try {
            // Create default configuration
            auto config = Config::createDefaultConfig();
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run simulation
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
                if (step % 100 == 0) {
                    std::cout << "  Step " << step << "/" << steps << ": "
                              << brain->getFiringNeuronCount() << " firing neurons, "
                              << brain->getTotalSpikeCount() << " total spikes" << std::endl;
                }
            }
            
            // Display final statistics
            std::cout << "\nQuick Simulation Results:" << std::endl;
            std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            
            std::cout << "\nQuick simulation complete!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error running quick simulation: " << e.what() << std::endl;
        }
    }
    
    void runDemo(int steps) {
        std::cout << "\nRunning demo simulation with world interaction..." << std::endl;
        
        try {
            // Create components
            auto config = Config::createDefaultConfig();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            
            // Run simulation
            std::cout << "Simulation started. World: " << world->getWidth() << "x" << world->getHeight() << std::endl;
            
            for (int step = 0; step < steps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Brain thinks
                brain->step(step);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, world->getSimulationTime());
                
                if (step % 10 == 0) {
                    auto body = world->getAgentBody();
                    std::cout << "  Step " << step << ": Agent at (" << body.x << ", " << body.y << ")"
                              << ", Energy: " << body.energy << "/" << world->getMaxEnergy()
                              << ", Firing neurons: " << brain->getFiringNeuronCount()
                              << ", Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                }
            }
            
            // Display final statistics
            std::cout << "\nDemo Simulation Results:" << std::endl;
            auto body = world->getAgentBody();
            std::cout << "  Final position: (" << body.x << ", " << body.y << ")" << std::endl;
            std::cout << "  Final orientation: " << body.orientation << "°" << std::endl;
            std::cout << "  Final energy: " << body.energy << "/" << world->getMaxEnergy() << std::endl;
            std::cout << "  Total simulation time: " << world->getSimulationTime() << "s" << std::endl;
            std::cout << "  Total brain spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Final development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            
            std::cout << "\nDemo simulation complete!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error running demo simulation: " << e.what() << std::endl;
        }
    }
    
    void processConfigCommand(const std::string& subcommand, std::istringstream& iss) {
        std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
        
        if (subcommand == "show") {
            showConfiguration();
        }
        else if (subcommand == "set") {
            std::string key, value;
            iss >> key >> value;
            setConfiguration(key, value);
        }
        else if (subcommand == "validate") {
            validateConfiguration();
        }
        else if (subcommand == "presets") {
            showPresets();
        }
        else if (subcommand == "save") {
            std::string filename;
            iss >> filename;
            if (!filename.empty()) {
                saveConfiguration(filename);
            } else {
                std::cout << "Error: Filename required for config save command." << std::endl;
            }
        }
        else if (subcommand == "load") {
            std::string filename;
            iss >> filename;
            if (!filename.empty()) {
                loadConfiguration(filename);
            } else {
                std::cout << "Error: Filename required for config load command." << std::endl;
            }
        }
        else {
            std::cout << "Unknown config subcommand: " << subcommand << std::endl;
            std::cout << "Available config commands: show, set, validate, presets, save, load" << std::endl;
        }
    }
    
    void showConfiguration() {
        std::cout << "\nCurrent Configuration:" << std::endl;
        std::cout << "=====================" << std::endl;
        
        auto config = Config::createDefaultConfig();
        std::cout << config->summary() << std::endl;
    }
    
    void setConfiguration(const std::string& key, const std::string& value) {
        std::cout << "\nSetting configuration: " << key << " = " << value << std::endl;
        
        try {
            if (key == "neuron_count") {
                size_t neurons = std::stoul(value);
                auto config = Config::createDefaultConfig();
                config->set(key, neurons, ConfigSource::Runtime);
                std::cout << "Set neuron count to " << neurons << std::endl;
            }
            else if (key == "region_count") {
                size_t regions = std::stoul(value);
                auto config = Config::createDefaultConfig();
                config->set(key, regions, ConfigSource::Runtime);
                std::cout << "Set region count to " << regions << std::endl;
            }
            else if (key == "connection_probability") {
                float prob = std::stof(value);
                auto config = Config::createDefaultConfig();
                config->set(key, prob, ConfigSource::Runtime);
                std::cout << "Set connection probability to " << prob << std::endl;
            }
            else if (key == "learning_rate") {
                float rate = std::stof(value);
                auto config = Config::createDefaultConfig();
                config->set(key, rate, ConfigSource::Runtime);
                std::cout << "Set learning rate to " << rate << std::endl;
            }
            else {
                std::cout << "Unknown configuration key: " << key << std::endl;
                std::cout << "Common keys: neuron_count, region_count, connection_probability, learning_rate" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error setting configuration: " << e.what() << std::endl;
        }
    }
    
    void validateConfiguration() {
        std::cout << "\nValidating configuration..." << std::endl;
        
        try {
            auto config = Config::createDefaultConfig();
            auto errors = config->validate();
            
            if (errors.empty()) {
                std::cout << "✓ Configuration is valid!" << std::endl;
            } else {
                std::cout << "✗ Configuration has errors:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  - " << error.message << " (field: " << error.field << ")" << std::endl;
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error validating configuration: " << e.what() << std::endl;
        }
    }
    
    void showPresets() {
        std::cout << "\nAvailable Configuration Presets:" << std::endl;
        std::cout << "================================" << std::endl;
        
        std::vector<std::pair<std::string, std::string>> presets = {
            {"default", "Standard NLM configuration"},
            {"exploration", "Optimized for exploration and learning"},
            {"exploitation", "Optimized for exploitation and efficiency"},
            {"balanced", "Balanced exploration/exploitation"},
            {"reproduction", "Conservative, reproduction-focused"},
            {"playful", "Highly exploratory and curious"}
        };
        
        for (const auto& preset : presets) {
            std::cout << "  " << std::left << std::setw(15) << preset.first 
                      << "- " << preset.second << std::endl;
        }
        
        std::cout << "\nUsage: config set <key> <value> to modify specific settings" << std::endl;
    }
    
    void saveConfiguration(const std::string& filename) {
        std::cout << "\nSaving configuration to: " << filename << std::endl;
        
        try {
            auto config = Config::createDefaultConfig();
            if (config->saveToFile(filename)) {
                std::cout << "✓ Configuration saved successfully!" << std::endl;
            } else {
                std::cout << "✗ Failed to save configuration!" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error saving configuration: " << e.what() << std::endl;
        }
    }
    
    void loadConfiguration(const std::string& filename) {
        std::cout << "\nLoading configuration from: " << filename << std::endl;
        
        try {
            auto config = Config::createDefaultConfig();
            if (config->loadFromFile(filename)) {
                std::cout << "✓ Configuration loaded successfully!" << std::endl;
                std::cout << config->summary() << std::endl;
            } else {
                std::cout << "✗ Failed to load configuration!" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error loading configuration: " << e.what() << std::endl;
        }
    }
    
    void runExperiment(int steps) {
        std::cout << "\nRunning experiment with " << steps << " steps..." << std::endl;
        
        try {
            // Create default configuration
            auto config = Config::createDefaultConfig();
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create world and agent for more realistic simulation
            auto world = std::make_shared<SimpleWorld>();
            world->configure(15, 15, 6, 6);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            
            // Enable learning
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            
            std::cout << "Experiment started. Monitoring brain activity..." << std::endl;
            
            // Run simulation
            for (int step = 0; step < steps; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Brain thinks
                brain->step(step);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, world->getSimulationTime());
                
                if (step % 100 == 0) {
                    auto body = world->getAgentBody();
                    std::cout << "  Step " << step << "/" << steps 
                              << ": Energy " << body.energy << "/" << world->getMaxEnergy()
                              << ", Spikes " << brain->getTotalSpikeCount()
                              << ", Dev stage " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
                }
            }
            
            // Display final statistics
            std::cout << "\nExperiment Results:" << std::endl;
            auto body = world->getAgentBody();
            std::cout << "  Final position: (" << body.x << ", " << body.y << ")" << std::endl;
            std::cout << "  Final energy: " << body.energy << "/" << world->getMaxEnergy() << std::endl;
            std::cout << "  Total steps: " << steps << std::endl;
            std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Final development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            std::cout << "\nExperiment complete!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error running experiment: " << e.what() << std::endl;
        }
    }
    
    void runBenchmark(const std::string& type) {
        std::cout << "\nRunning " << type << " benchmark..." << std::endl;
        
        try {
            // Use existing benchmark.cpp functionality
            // For now, just show message
            std::cout << "Benchmark type: " << type << std::endl;
            std::cout << "Note: Full benchmarking requires integration with benchmark.cpp" << std::endl;
            std::cout << "Available benchmark types: cpu, memory, network, all" << std::endl;
            
            // Run quick benchmark using the existing benchmark system
            auto benchmark = std::make_shared<Benchmark>(type);
            benchmark->run();
            
        } catch (const std::exception& e) {
            std::cout << "Error running benchmark: " << e.what() << std::endl;
        }
    }
    
    void runBatchExperiments(int count, int steps) {
        std::cout << "\nRunning batch of " << count << " experiments (" << steps << " steps each)..." << std::endl;
        
        try {
            std::vector<double> allSpikes;
            std::vector<int> allNeurons;
            
            for (int i = 0; i < count; ++i) {
                std::cout << "  Running experiment " << (i + 1) << "/" << count << "..." << std::endl;
                
                // Create fresh configuration for each experiment
                auto config = Config::createDefaultConfig();
                
                // Add some variation to configuration
                auto neuronCount = config->getOr<size_t>("neuron_count", 1000);
                auto regionCount = config->getOr<size_t>("region_count", 1);
                
                // Create brain
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Run simulation
                for (int step = 0; step < steps; ++step) {
                    brain->step(step);
                }
                
                // Collect statistics
                allSpikes.push_back(static_cast<double>(brain->getTotalSpikeCount()));
                allNeurons.push_back(static_cast<int>(brain->getTotalNeuronCount()));
            }
            
            // Display batch results
            std::cout << "\nBatch Experiment Results:" << std::endl;
            std::cout << "  Experiments run: " << count << std::endl;
            std::cout << "  Steps per experiment: " << steps << std::endl;
            
            if (!allSpikes.empty()) {
                double totalSpikes = 0.0;
                double avgSpikes = 0.0;
                double totalNeurons = 0.0;
                double avgNeurons = 0.0;
                
                for (double spikes : allSpikes) {
                    totalSpikes += spikes;
                }
                for (int neurons : allNeurons) {
                    totalNeurons += neurons;
                }
                
                avgSpikes = totalSpikes / allSpikes.size();
                avgNeurons = totalNeurons / allNeurons.size();
                
                std::cout << "  Total spikes: " << static_cast<long>(totalSpikes) << std::endl;
                std::cout << "  Average spikes per experiment: " << std::fixed << std::setprecision(1) << avgSpikes << std::endl;
                std::cout << "  Average neurons per experiment: " << static_cast<int>(avgNeurons) << std::endl;
                std::cout << "  Average spikes per neuron: " << std::fixed << std::setprecision(3) << avgSpikes / avgNeurons << std::endl;
            }
            
            std::cout << "\nBatch experiments complete!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error running batch experiments: " << e.what() << std::endl;
        }
    }
    
    void monitorBrain(int duration) {
        std::cout << "\nMonitoring brain activity for " << duration << " seconds..." << std::endl;
        
        try {
            // Create configuration
            auto config = Config::createDefaultConfig();
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create world for more realistic monitoring
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 4, 4);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            
            std::cout << "Monitoring started. Press Ctrl+C to stop." << std::endl;
            std::cout << "Showing real-time statistics..." << std::endl;
            
            // Monitor for specified duration
            auto startTime = std::chrono::steady_clock::now();
            auto endTime = startTime + std::chrono::seconds(duration);
            
            while (std::chrono::steady_clock::now() < endTime) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                // Brain thinks
                static int step = 0;
                brain->step(step++);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Display current statistics
                auto body = world->getAgentBody();
                std::cout << "\rTime: " << std::fixed << std::setprecision(1) << world->getSimulationTime() << "s | "
                          << "Energy: " << body.energy << "/" << world->getMaxEnergy() << " | "
                          << "Firing: " << brain->getFiringNeuronCount() << " | "
                          << "Total spikes: " << brain->getTotalSpikeCount() << " | "
                          << "Dev stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "   ";
                
                std::cout.flush();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            std::cout << std::endl << "Monitoring complete." << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error monitoring brain: " << e.what() << std::endl;
        }
    }
    
    void saveState(const std::string& filename) {
        std::cout << "\nSaving brain state to: " << filename << std::endl;
        
        try {
            // Create configuration
            auto config = Config::createDefaultConfig();
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run some simulation steps to create interesting state
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            // Save brain state
            if (brain->save(filename)) {
                std::cout << "✓ Brain state saved successfully!" << std::endl;
            } else {
                std::cout << "✗ Failed to save brain state!" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error saving brain state: " << e.what() << std::endl;
        }
    }
    
    void loadState(const std::string& filename) {
        std::cout << "\nLoading brain state from: " << filename << std::endl;
        
        try {
            // Create configuration
            auto config = Config::createDefaultConfig();
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Load brain state
            if (brain->load(filename)) {
                std::cout << "✓ Brain state loaded successfully!" << std::endl;
                brain->logStatus();
            } else {
                std::cout << "✗ Failed to load brain state!" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error loading brain state: " << e.what() << std::endl;
        }
    }
    
    void exploreFeatures(const std::string& mode) {
        std::cout << "\nExploring NLM features in mode: " << mode << std::endl;
        
        try {
            if (mode == "memory") {
                std::cout << "Memory System Exploration:" << std::endl;
                std::cout << "  Demonstrating working memory, episodic memory, and associative memory." << std::endl;
                
                // Create configuration
                auto config = Config::createDefaultConfig();
                
                // Create brain
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Access memory systems
                auto workingMem = brain->getWorkingMemory();
                auto episodicMem = brain->getEpisodicMemory();
                auto assocMem = brain->getAssociativeMemory();
                
                std::cout << "  - Working memory capacity: " << workingMem->getCapacity() << std::endl;
                std::cout << "  - Episodic memory episodes stored: " << episodicMem->getEpisodeCount() << std::endl;
                
            } else if (mode == "cognition") {
                std::cout << "Cognitive System Exploration:" << std::endl;
                std::cout << "  Demonstrating neural planning, concept formation, and attention." << std::endl;
                
                // Create configuration
                auto config = Config::createDefaultConfig();
                
                // Create brain
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Access cognition systems
                auto planner = brain->getPlanner();
                auto conceptForm = brain->getConceptFormation();
                auto attention = brain->getAttention();
                
                std::cout << "  - Planning depth: " << planner->getPlanningDepth() << std::endl;
                std::cout << "  - Attention inhibition strength: " << attention->getInhibitionStrength() << std::endl;
                
            } else if (mode == "neuromodulation") {
                std::cout << "Neuromodulation System Exploration:" << std::endl;
                std::cout << "  Demonstrating dopamine, curiosity, novelty, and prediction error." << std::endl;
                
                // Create configuration
                auto config = Config::createDefaultConfig();
                
                // Create brain
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Access neuromodulation systems
                auto dopamine = brain->getDopamine();
                auto curiosity = brain->getCuriosity();
                auto novelty = brain->getNovelty();
                auto predError = brain->getPredictionErrorSignal();
                
                std::cout << "  - Dopamine level: " << dopamine->getLevel() << std::endl;
                std::cout << "  - Curiosity level: " << curiosity->getLevel() << std::endl;
                std::cout << "  - Novelty level: " << novelty->getLevel() << std::endl;
                
            } else if (mode == "all") {
                std::cout << "Complete NLM Feature Exploration:" << std::endl;
                std::cout << "  Demonstrating integration of all brain systems." << std::endl;
                
                // Create configuration
                auto config = Config::createDefaultConfig();
                
                // Create brain
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Create world for complete demonstration
                auto world = std::make_shared<SimpleWorld>();
                world->configure(12, 12, 6, 6);
                world->reset();
                
                auto agent = std::make_shared<AgentBrain>(brain);
                agent->initialize(world);
                
                // Enable all learning features
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
                agent->enableDevelopment(true);
                
                // Run demonstration
                for (int step = 0; step < 50; ++step) {
                    // Update world
                    world->update(0.1);
                    
                    // Get sensory input
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    // Brain thinks
                    brain->step(step);
                    
                    // Get action
                    auto action = agent->decodeMotorCommand();
                    
                    // Apply action
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
                
                // Display final statistics
                auto body = world->getAgentBody();
                std::cout << "  Final position: (" << body.x << ", " << body.y << ")" << std::endl;
                std::cout << "  Final energy: " << body.energy << "/" << world->getMaxEnergy() << std::endl;
                std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                std::cout << "  Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
                
            } else {
                std::cout << "Unknown exploration mode: " << mode << std::endl;
                std::cout << "Available modes: memory, cognition, neuromodulation, all" << std::endl;
                return;
            }
            
            std::cout << "\nFeature exploration complete!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error exploring features: " << e.what() << std::endl;
        }
    }
    
    void processAdvancedCommand(const std::string& command, std::istringstream& iss) {
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "script") {
            std::string filename;
            iss >> filename;
            if (!filename.empty()) {
                runScript(filename);
            } else {
                std::cout << "Error: Script filename required." << std::endl;
            }
        }
        else if (command == "export") {
            std::string format;
            iss >> format;
            exportResults(format);
        }
        else if (command == "parallel") {
            int count;
            iss >> count;
            if (count > 0) {
                runParallelExperiments(count);
            } else {
                std::cout << "Error: Parallel count must be positive." << std::endl;
            }
        }
        else if (command == "profile") {
            profilePerformance();
        }
        else if (command == "debug") {
            enableDebugMode();
        }
        else {
            std::cout << "Unknown advanced command: " << command << std::endl;
            std::cout << "Available advanced commands: script, export, parallel, profile, debug" << std::endl;
        }
    }
    
    void runScript(const std::string& filename) {
        std::cout << "Running NLM script: " << filename << std::endl;
        // TODO: Implement script execution
        std::cout << "Script execution not yet implemented." << std::endl;
    }
    
    void exportResults(const std::string& format) {
        std::cout << "Exporting results in format: " << format << std::endl;
        // TODO: Implement result export
        std::cout << "Result export not yet implemented." << std::endl;
    }
    
    void runParallelExperiments(int count) {
        std::cout << "Running " << count << " experiments in parallel..." << std::endl;
        // TODO: Implement parallel execution
        std::cout << "Parallel execution not yet implemented." << std::endl;
    }
    
    void profilePerformance() {
        std::cout << "Profiling NLM performance..." << std::endl;
        // TODO: Implement performance profiling
        std::cout << "Performance profiling not yet implemented." << std::endl;
    }
    
    void enableDebugMode() {
        std::cout << "Enabling debug mode..." << std::endl;
        // TODO: Implement debug mode
        std::cout << "Debug mode not yet implemented." << std::endl;
    }
    
    // Placeholder for Benchmark class
    class Benchmark {
    public:
        Benchmark(const std::string& type) : type(type) {}
        void run() {
            std::cout << "Benchmarking " << type << " performance..." << std::endl;
            // TODO: Implement actual benchmarking
        }
    private:
        std::string type;
    };
};

int main(int argc, char** argv) {
    try {
        NLMCLI cli;
        cli.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
