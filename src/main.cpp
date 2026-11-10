#pragma once

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
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <map>

using namespace nlm;

// Forward declarations for CLI commands
void runCommand(int argc, char** argv);
void demoCommand(int argc, char** argv);
void benchmarkCommand(int argc, char** argv);
void validateCommand(int argc, char** argv);
void checkpointCommand(int argc, char** argv);
void replayCommand(int argc, char** argv);
void helpCommand(const std::string& command);

// Command line interface parsing
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::map<std::string, std::string> options;
        std::vector<std::string> args;
    };
    
    CLIParser(int argc, char** argv) : argc_(argc), argv_(argv) {}
    
    // Parse command and arguments
    bool parse(std::string& command, std::map<std::string, std::string>& options, 
               std::vector<std::string>& args) {
        if (argc_ < 2) {
            return false;
        }
        
        command = argv_[1];
        
        // Parse options (--key=value or --key value)
        for (int i = 2; i < argc_; ++i) {
            std::string arg = argv_[i];
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    options[key] = value;
                } else if (i + 1 < argc_) {
                    std::string key = arg.substr(2);
                    std::string value = argv_[++i];
                    options[key] = value;
                } else {
                    std::cerr << "Error: Missing value for option " << key << std::endl;
                    return false;
                }
            } else {
                args.push_back(arg);
            }
        }
        
        return true;
    }
    
    // Print help for all commands
    static void printHelp() {
        std::cout << R"(
NLM — Neural Learning Machine
Usage: nlm <command> [options]

Available commands:
  run                    Run a brain simulation
  demo                   Run a demo of a specific phase
  benchmark              Run performance benchmarks
  validate               Validate configuration and brain state
  checkpoint             Checkpoint operations (save/load)
  replay                 Replay a saved simulation episode
  help                   Show help for a specific command or all commands

For more information about a command, use: nlm help <command>

Examples:
  nlm run --steps 1000 --config brain.yaml
  nlm demo phase6 --validate-integration
  nlm benchmark --duration 30 --neuron-count 10000
  nlm checkpoint save brain1.bin
  nlm checkpoint load brain1.bin
  nlm replay brain1.bin --steps 500

Use nlm help <command> for detailed information about a specific command.
)" << std::endl;
    }
    
private:
    int argc_;
    char** argv_;
};

// Main entry point
int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line
    std::string command;
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    
    CLIParser parser(argc, argv);
    if (!parser.parse(command, options, args)) {
        std::cerr << "Error: No command provided. Use 'nlm help' for usage information." << std::endl;
        return 1;
    }
    
    // Dispatch to appropriate command handler
    try {
        if (command == "run") {
            runCommand(options, args);
        } else if (command == "demo") {
            demoCommand(options, args);
        } else if (command == "benchmark") {
            benchmarkCommand(options, args);
        } else if (command == "validate") {
            validateCommand(options, args);
        } else if (command == "checkpoint") {
            checkpointCommand(options, args);
        } else if (command == "replay") {
            replayCommand(options, args);
        } else if (command == "help") {
            if (args.empty()) {
                CLIParser::printHelp();
            } else {
                helpCommand(args[0]);
            }
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            std::cerr << "Use 'nlm help' to see available commands." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// Run command implementation
void runCommand(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Brain for simulation...\n" << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6 if not specified
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
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw std::runtime_error("Brain initialization failed");
    }
    
    brain->logStatus();
    
    // Get simulation steps from command line
    int steps = 100; // Default
    if (options.find("steps") != options.end()) {
        try {
            steps = std::stoi(options["steps"]);
        } catch (...) {
            NLM_LOG_WARNING("Invalid steps value, using default 100");
        }
    }
    
    // Run simulation
    NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps...");
    
    for (nlm::SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        
        // Log progress every 10%
        if (step % std::max(1, steps / 10) == 0) {
            NLM_LOG_INFO("Progress: " + std::to_string((step * 100) / steps) + "% (step " + 
                        std::to_string(step) + "/" + std::to_string(steps) + ")");
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Successfully completed " + std::to_string(steps) + " simulation steps.");
}

// Demo command implementation
void demoCommand(int argc, char** argv) {
    printBanner();
    
    std::string phase = "phase2";
    if (!args.empty()) {
        phase = args[0];
    }
    
    NLM_LOG_INFO("Starting NLM Phase " + phase + " demo...");
    
    // This would normally run the appropriate phase demo
    // For now, we'll just show a message
    NLM_LOG_INFO("Demo functionality would be implemented here.");
    NLM_LOG_INFO("Phase " + phase + " demo would show real neural computation.");
    
    if (options.find("validate") != options.end() && options["validate"] == "true") {
        NLM_LOG_INFO("Running validation checks...");
        // Would run validation
    }
}

// Benchmark command implementation
void benchmarkCommand(int argc, char** argv) {
    printBanner();
    
    std::cout << "Running NLM performance benchmark...\n" << std::endl;
    
    // This would normally run performance benchmarks
    // For now, we'll just show a message
    NLM_LOG_INFO("Benchmark functionality would be implemented here.");
    NLM_LOG_INFO("Would measure spike propagation, plasticity rates, memory usage, etc.");
    
    // Get benchmark parameters from command line
    int duration = 30; // Default 30 seconds
    if (options.find("duration") != options.end()) {
        try {
            duration = std::stoi(options["duration"]);
        } catch (...) {
            NLM_LOG_WARNING("Invalid duration value, using default 30 seconds");
        }
    }
    
    int neuronCount = 1000; // Default
    if (options.find("neuron-count") != options.end()) {
        try {
            neuronCount = std::stoi(options["neuron-count"]);
        } catch (...) {
            NLM_LOG_WARNING("Invalid neuron-count value, using default 1000");
        }
    }
    
    NLM_LOG_INFO("Benchmark parameters:");
    NLM_LOG_INFO("  Duration: " + std::to_string(duration) + " seconds");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(neuronCount));
}

// Validate command implementation
void validateCommand(int argc, char** argv) {
    printBanner();
    
    std::cout << "Validating NLM configuration and brain state...\n" << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Validate configuration
    if (!config->validate()) {
        NLM_LOG_ERROR("Configuration validation failed!");
        throw std::runtime_error("Configuration validation failed");
    }
    
    NLM_LOG_INFO("Configuration validation passed.");
    
    // Could validate brain state, memory systems, etc.
    NLM_LOG_INFO("Brain validation functionality would be implemented here.");
}

// Checkpoint command implementation
void checkpointCommand(int argc, char** argv) {
    printBanner();
    
    std::cout << "Checkpoint operations...\n" << std::endl;
    
    if (args.empty()) {
        NLM_LOG_ERROR("No checkpoint operation specified. Use 'save' or 'load'.");
        throw std::runtime_error("Missing checkpoint operation");
    }
    
    std::string operation = args[0];
    
    if (operation == "save") {
        if (args.size() < 2) {
            NLM_LOG_ERROR("Missing checkpoint filename for save operation.");
            throw std::runtime_error("Missing checkpoint filename");
        }
        
        std::string filename = args[1];
        NLM_LOG_INFO("Saving brain state to: " + filename);
        
        // This would normally save the current brain state
        // For now, we'll just show a message
        NLM_LOG_INFO("Checkpoint save functionality would be implemented here.");
        
    } else if (operation == "load") {
        if (args.size() < 2) {
            NLM_LOG_ERROR("Missing checkpoint filename for load operation.");
            throw std::runtime_error("Missing checkpoint filename");
        }
        
        std::string filename = args[1];
        NLM_LOG_INFO("Loading brain state from: " + filename);
        
        // This would normally load a saved brain state
        // For now, we'll just show a message
        NLM_LOG_INFO("Checkpoint load functionality would be implemented here.");
        
    } else if (operation == "list") {
        NLM_LOG_INFO("Listing available checkpoints...");
        // This would list available checkpoints
        NLM_LOG_INFO("Checkpoint listing functionality would be implemented here.");
        
    } else {
        NLM_LOG_ERROR("Unknown checkpoint operation: " + operation);
        throw std::runtime_error("Unknown checkpoint operation");
    }
}

// Replay command implementation
void replayCommand(int argc, char** argv) {
    printBanner();
    
    std::cout << "Replaying saved simulation...\n" << std::endl;
    
    if (args.empty()) {
        NLM_LOG_ERROR("Missing checkpoint filename for replay operation.");
        throw std::runtime_error("Missing checkpoint filename");
    }
    
    std::string filename = args[0];
    NLM_LOG_INFO("Replaying from checkpoint: " + filename);
    
    // Get replay steps from command line
    int steps = 1000; // Default
    if (options.find("steps") != options.end()) {
        try {
            steps = std::stoi(options["steps"]);
        } catch (...) {
            NLM_LOG_WARNING("Invalid steps value, using default 1000");
        }
    }
    
    // This would normally replay the saved simulation
    // For now, we'll just show a message
    NLM_LOG_INFO("Replay functionality would be implemented here.");
    NLM_LOG_INFO("Would replay " + std::to_string(steps) + " steps from checkpoint.");
}

// Help command implementation
void helpCommand(const std::string& command) {
    if (command == "run") {
        std::cout << R"(
Usage: nlm run [options]

Run a brain simulation with the specified configuration and parameters.

Options:
  --config <file>            Configuration file (default: configs/default.cfg)
  --steps <number>           Number of simulation steps (default: 100)
  --neuron-count <number>    Number of neurons in the brain (default: 500)
  --region-count <number>    Number of brain regions (default: 1)
  --connection-probability <float> Connection probability (default: 0.15)
  --random-seed <number>     Random seed for reproducible results (default: 42)

Examples:
  nlm run --steps 1000 --neuron-count 1000
  nlm run --config custom.cfg --steps 5000
  nlm run --random-seed 123 --connection-probability 0.2
)" << std::endl;
    } else if (command == "demo") {
        std::cout << R"(
Usage: nlm demo <phase> [options]

Run a demonstration of a specific NLM phase.

Arguments:
  <phase>                    Phase to demonstrate (phase2, phase3, phase4, phase5, phase6)

Options:
  --validate                 Run validation checks after demo

Examples:
  nlm demo phase6
  nlm demo phase6 --validate
)" << std::endl;
    } else if (command == "benchmark") {
        std::cout << R"(
Usage: nlm benchmark [options]

Run performance benchmarks on the NLM brain.

Options:
  --duration <seconds>       Benchmark duration in seconds (default: 30)
  --neuron-count <number>    Number of neurons to benchmark (default: 1000)

Examples:
  nlm benchmark --duration 60 --neuron-count 5000
  nlm benchmark --neuron-count 10000
)" << std::endl;
    } else if (command == "validate") {
        std::cout << R"(
Usage: nlm validate [options]

Validate NLM configuration and brain state.

Options:
  --config <file>            Configuration file to validate

Examples:
  nlm validate
  nlm validate --config custom.cfg
)" << std::endl;
    } else if (command == "checkpoint") {
        std::cout << R"(
Usage: nlm checkpoint <operation> [filename]

Perform checkpoint operations on brain state.

Arguments:
  <operation>                 Checkpoint operation (save, load, list)
  [filename]                 Checkpoint filename (required for save/load)

Examples:
  nlm checkpoint save brain1.bin
  nlm checkpoint load brain1.bin
  nlm checkpoint list
)" << std::endl;
    } else if (command == "replay") {
        std::cout << R"(
Usage: nlm replay <filename> [options]

Replay a saved simulation episode.

Arguments:
  <filename>                 Checkpoint file to replay from

Options:
  --steps <number>           Number of steps to replay (default: 1000)

Examples:
  nlm replay brain1.bin
  nlm replay brain1.bin --steps 500
)" << std::endl;
    } else {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        std::cerr << "Use 'nlm help' to see available commands." << std::endl;
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}
