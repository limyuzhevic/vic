// NLM (熙然) - Neural Learning Machine
// CLI Version
//
// This file provides a command-line interface for controlling the NLM system with:
// - Command parsing for run, save, load, info, and help commands
// - Configuration file and command-line parameter support
// - Brain state management
// - User-friendly output formatting

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <filesystem>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>

using namespace nlm;

// Forward declarations
void printBanner();
void printHelp(const char* programName);
void showBrainInfo(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
void runSimulation(std::shared_ptr<Config> config);
void saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
void loadBrainState(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, const std::string& filepath);

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     CLI Interface                                             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const char* programName) {
    printBanner();
    std::cout << R"(
USAGE: 
    "programName" [--config FILE] COMMAND [OPTIONS]

COMMANDS:
    run                 Run simulation with specified parameters
    save [FILE]         Save brain state to file (default: brain_state.checkpoint)
    load FILE           Load brain state from file
    info                Show brain and system information
    help                Show this help message

OPTIONS:
    --neurons N         Number of neurons in the brain (default: 500)
    --timestep T        Simulation timestep in seconds (default: 0.001)
    --seed S            Random seed for reproducibility (default: 42)
    --steps N           Number of simulation steps to run (default: 100)
    --region-count N    Number of brain regions (default: 1)
    --conn-prob P       Connection probability (default: 0.15)

CONFIGURATION:
    --config FILE        Load configuration from file (default: configs/default.cfg)

EXAMPLES:
    ./programName --config myconfig.cfg run --neurons 1000 --steps 1000
    ./programName save my_brain.checkpoint
    ./programName load my_brain.checkpoint
    ./programName info
    ./programName --config custom.cfg run
    ./programName help

For more details on a specific command, run: "programName help COMMAND"
)";
}

void showBrainInfo(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "\n=== BRAIN INFORMATION ===" << std::endl;
    
    std::cout << "Basic Properties:" << std::endl;
    std::cout << "  Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Region Count: " << brain->getRegionCount() << std::endl;
    std::cout << "  Current Step: " << brain->getCurrentStep() << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
}

void runSimulation(std::shared_ptr<Config> config) {
    std::cout << "\n=== RUNNING SIMULATION ===" << std::endl;
    
    // Extract parameters from config
    uint64_t neuronCount = config->getOr<int64_t>("neuron_count", 500);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    uint64_t seed = config->getOr<int64_t>("random_seed", 42);
    size_t steps = config->getOr<size_t>("max_simulation_steps", 100);
    uint64_t regionCount = config->getOr<int64_t>("region_count", 1);
    float connectionProb = config->getOr<float>("connection_probability", 0.15f);
    
    std::cout << "Initializing simulation..." << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Timestep: " << timestep << "s" << std::endl;
    std::cout << "  Seed: " << seed << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    std::cout << "  Regions: " << regionCount << std::endl;
    std::cout << "  Connection Probability: " << connectionProb << std::endl;
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize random generator
    Random::seed(seed);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Run simulation
    std::cout << "Running simulation..." << std::endl;
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << "/" << steps
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    std::cout << "\nSimulation completed!" << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
}

void saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::cout << "\n=== SAVING BRAIN STATE ===" << std::endl;
    std::cout << "Saving brain to: " << filepath << std::endl;
    
    if (brain->save(filepath)) {
        std::cout << "✓ Brain state saved successfully!" << std::endl;
        
        // Show file info
        std::error_code ec;
        auto fileSize = std::filesystem::file_size(filepath, ec);
        if (!ec) {
            std::cout << "  File size: " << fileSize << " bytes" << std::endl;
        }
    } else {
        std::cerr << "✗ Failed to save brain state!" << std::endl;
    }
}

void loadBrainState(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, const std::string& filepath) {
    std::cout << "\n=== LOADING BRAIN STATE ===" << std::endl;
    std::cout << "Loading brain from: " << filepath << std::endl;
    
    if (brain->load(filepath)) {
        std::cout << "✓ Brain state loaded successfully!" << std::endl;
        brain->logStatus();
    } else {
        std::cerr << "✗ Failed to load brain state!" << std::endl;
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << "============================================================" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // If no arguments, show help
    if (argc < 2) {
        printHelp(argv[0]);
        return 0;
    }
    
    // Parse command line arguments
    std::string command;
    std::vector<std::string> args;
    std::string configFile = "configs/default.cfg";
    
    // First pass: parse --config and extract command
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "ERROR: --config requires a file path" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        } else if (arg.substr(0, 2) == "--config") {
            // Handle --config=file format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                configFile = arg.substr(pos + 1);
            }
        } else if (command.empty()) {
            // First non-option argument is the command
            command = arg;
        } else {
            // Additional arguments
            args.push_back(arg);
        }
    }
    
    // Load configuration from file
    auto config = std::make_shared<Config>();
    if (!configFile.empty() && config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line arguments
    // Parse command line options for config
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" || (arg.substr(0, 8) == "--config=")) {
            // Already handled
            continue;
        }
        
        if (arg == command) {
            // Command itself, skip
            continue;
        }
        
        // Parse other options
        if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            // --key=value format
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            config->set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            // -key value format
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            config->set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            std::cerr << "ERROR: Option " << arg << " requires a value" << std::endl;
            printHelp(argv[0]);
            return 1;
        }
    }
    
    // Set default values if not already set
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    if (!config->has("max_simulation_steps")) {
        config->set("max_simulation_steps", static_cast<int64_t>(100), ConfigSource::Default);
    }
    
    // Execute command
    if (command == "help") {
        printHelp(argv[0]);
        return 0;
    } else if (command == "info") {
        // Create a temporary brain to show info
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        showBrainInfo(brain, config);
        return 0;
    } else if (command == "run") {
        runSimulation(config);
        return 0;
    } else if (command == "save") {
        std::string filepath = "brain_state.checkpoint";
        if (!args.empty()) {
            filepath = args[0];
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        saveBrainState(brain, filepath);
        return 0;
    } else if (command == "load") {
        if (args.empty()) {
            std::cerr << "ERROR: load command requires a file path" << std::endl;
            printHelp(argv[0]);
            return 1;
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        loadBrainState(brain, config, args[0]);
        return 0;
    } else {
        std::cerr << "ERROR: Unknown command: " << command << std::endl;
        printHelp(argv[0]);
        return 1;
    }
}
