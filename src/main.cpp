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
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>

using namespace nlm;

// Enhanced Command Line Interface
class CommandLineInterface {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<bool(int, char**)> execute;
    };

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

    void printHelp() {
        std::cout << "NLM Command Line Interface - Available Commands:\n\n";
        
        for (const auto& command : commands) {
            std::cout << "  " << command.name << "\n";
            std::cout << "    " << command.description << "\n";
            std::cout << "    Usage: " << command.usage << "\n\n";
        }
        
        std::cout << "Examples:\n";
        std::cout << "  nlm --help                                    Show this help\n";
        std::cout << "  nlm --config=myconfig.json                 Load JSON config\n";
        std::cout << "  nlm --validate --config=myconfig.json       Validate config\n";
        std::cout << "  nlm --config-list                            List all config options\n";
        std::cout << "  nlm --generate-config                        Generate template config\n";
        std::cout << "  nlm --brain --steps=100 --record            Run brain simulation\n";
        std::cout << "  nlm --analyze --connectivity               Analyze brain connectivity\n";
    }

    void printVersion() {
        std::cout << "NLM Phase 2 - Neural Learning Machine\n";
        std::cout << "Version 0.1.0 (Phase 2)" << std::endl;
        std::cout << "Real Neural Computation with LIF neurons, STDP, and Hebbian plasticity\n";
    }

    bool validateConfig(const std::string& configFile) {
        std::cout << "Validating configuration file: " << configFile << std::endl;
        
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(configFile)) {
            std::cerr << "ERROR: Could not load configuration file: " << configFile << std::endl;
            return false;
        }
        
        // Validate required parameters
        std::vector<std::string> requiredParams = {
            "neuron_count", "region_count", "simulation_timestep", "random_seed"
        };
        
        bool allValid = true;
        for (const auto& param : requiredParams) {
            if (!config->has(param)) {
                std::cerr << "ERROR: Missing required parameter: " << param << std::endl;
                allValid = false;
            } else {
                std::cout << "✓ " << param << " = " << config->get<std::string>(param).value_or("?") << std::endl;
            }
        }
        
        // Validate parameter ranges
        if (auto neuronCountOpt = config->get<int64_t>("neuron_count")) {
            int64_t neurons = *neuronCountOpt;
            if (neurons < 10 || neurons > 1000000) {
                std::cerr << "ERROR: neuron_count " << neurons << " out of valid range [10, 1000000]" << std::endl;
                allValid = false;
            }
        }
        
        if (auto timestepOpt = config->get<double>("simulation_timestep")) {
            double timestep = *timestepOpt;
            if (timestep <= 0.0 || timestep > 1.0) {
                std::cerr << "ERROR: simulation_timestep " << timestep << " out of valid range (0, 1.0]" << std::endl;
                allValid = false;
            }
        }
        
        if (allValid) {
            std::cout << "\n✓ Configuration file is valid!" << std::endl;
        } else {
            std::cout << "\n✗ Configuration file has errors." << std::endl;
        }
        
        return allValid;
    }

    void listConfigOptions() {
        std::cout << "NLM Configuration Options:\n\n";
        
        // Core simulation parameters
        std::cout << "=== Core Simulation ===\n";
        std::cout << "  neuron_count              int64    Number of neurons (10-1000000)\n";
        std::cout << "  region_count              int64    Number of brain regions (1-100)\n";
        std::cout << "  simulation_timestep       double   Time step in seconds (0.0001-0.1)\n";
        std::cout << "  random_seed               int64    Random seed for reproducible results\n";
        
        std::cout << "\n=== Connectivity ===\n";
        std::cout << "  connection_probability    float    Probability of random connections (0.0-1.0)\n";
        
        std::cout << "\n=== Plasticity ===\n";
        std::cout << "  stdp_ltp_weight           float    STDP LTP weight (0.001-0.1)\n";
        std::cout << "  stdp_ltd_weight           float    STDP LTD weight (0.001-0.1)\n";
        std::cout << "  stdp_tau                  float    STDP time constant (1-100)\n";
        std::cout << "  plasticity_learning_rate  float    Hebbian learning rate (0.001-0.01)\n";
        
        std::cout << "\n=== Development ===\n";
        std::cout << "  synaptogenesis_rate       float    Synaptogenesis rate (0.00001-0.01)\n";
        std::cout << "  pruning_rate              float    Pruning rate (0.000001-0.001)\n";
        
        std::cout << "\n=== Neuromodulation ===\n";
        std::cout << "  dopamine_baseline         float    Baseline dopamine level (0.0-1.0)\n";
        std::cout << "  curiosity_baseline        float    Baseline curiosity (0.0-1.0)\n";
        
        std::cout << "\n=== Environment ===\n";
        std::cout << "  environment_name          string   Environment type\n";
        std::cout << "  environment_width         int64    Environment width\n";
        std::cout << "  environment_height        int64    Environment height\n";
        
        std::cout << "\n=== Logging ===\n";
        std::cout << "  log_level                 string   Log level (DEBUG, INFO, WARNING, ERROR)\n";
        std::cout << "  log_to_file               bool     Enable file logging\n";
        std::cout << "  log_filename              string   Log file name\n";
        
        std::cout << "\n=== Simulation ===\n";
        std::cout << "  max_simulation_steps     int64    Maximum simulation steps\n";
        std::cout << "  simulation_time_limit     double   Time limit in seconds (0.0 for unlimited)\n";
        
        std::cout << "\n=== Visualization ===\n";
        std::cout << "  visualization_enabled     bool     Enable visualization\n";
        std::cout << "  visualization_update_rate float    Visualization update rate (FPS)\n";
    }

    bool generateConfig(const std::string& outputFile) {
        std::cout << "Generating template configuration file: " << outputFile << std::endl;
        
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not create file: " << outputFile << std::endl;
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM Command Line Interface\n\n";
        
        file << "# Core Simulation Parameters\n";
        file << "random_seed = 42\n";
        file << "simulation_timestep = 0.001\n";
        file << "neuron_count = 1000\n";
        file << "region_count = 2\n\n";
        
        file << "# Connectivity Parameters\n";
        file << "connection_probability = 0.1\n\n";
        
        file << "# Plasticity Parameters\n";
        file << "plasticity_learning_rate = 0.01\n";
        file << "stdp_ltp_weight = 0.01\n";
        file << "stdp_ltd_weight = 0.012\n";
        file << "stdp_time_constant = 20.0\n\n";
        
        file << "# Development Parameters\n";
        file << "synaptogenesis_rate = 0.001\n";
        file << "pruning_rate = 0.0001\n\n";
        
        file << "# Neuromodulation Parameters\n";
        file << "dopamine_baseline = 0.1\n";
        file << "curiosity_baseline = 0.1\n\n";
        
        file << "# Reward Parameters\n";
        file << "reward_discount_factor = 0.99\n\n";
        
        file << "# Environment Parameters\n";
        file << "environment_name = GridWorld\n";
        file << "environment_width = 10\n";
        file << "environment_height = 10\n\n";
        
        file << "# Logging Parameters\n";
        file << "log_level = INFO\n";
        file << "log_to_file = false\n";
        file << "log_filename = nlm.log\n\n";
        
        file << "# Simulation Parameters\n";
        file << "max_simulation_steps = 10000\n";
        file << "simulation_time_limit = 0.0\n\n";
        
        file << "# Visualization Parameters\n";
        file << "visualization_enabled = false\n";
        file << "visualization_update_rate = 30.0\n";
        
        file.close();
        
        std::cout << "✓ Configuration file generated successfully!" << std::endl;
        return true;
    }

    bool runBrainSimulation(const std::string& configFile, int steps, bool record) {
        std::cout << "Running NLM Brain Simulation..." << std::endl;
        std::cout << "Config: " << configFile << "\n";
        std::cout << "Steps: " << steps << "\n";
        std::cout << "Recording: " << (record ? "Yes" : "No") << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set Phase 2 defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        
        // Run simulation
        NLM_LOG_INFO("Starting simulation with " + std::to_string(steps) + " steps...");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            // Create input pattern
            for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount() / 4); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Active: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("Simulation complete!");
        NLM_LOG_INFO("Final status:");
        brain->logStatus();
        
        // Save brain state if requested
        if (record) {
            std::string outputFile = "brain_state_checkpoint.bin";
            if (brain->save(outputFile)) {
                NLM_LOG_INFO("Brain state saved to: " + outputFile);
            } else {
                NLM_LOG_ERROR("Failed to save brain state");
            }
        }
        
        return true;
    }

    bool analyzeBrain(const std::string& analysisType, const std::string& outputFile) {
        std::cout << "Performing " << analysisType << " analysis..." << std::endl;
        
        if (analysisType == "connectivity") {
            std::cout << "Analyzing neural connectivity patterns..." << std::endl;
            
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Perform connectivity analysis
            size_t totalNeurons = brain->getTotalNeuronCount();
            size_t totalSynapses = brain->getTotalSynapseCount();
            float eI = brain->getExcitationInhibitionRatio();
            
            NLM_LOG_INFO("Connectivity Analysis Results:");
            NLM_LOG_INFO("  Total Neurons: " + std::to_string(totalNeurons));
            NLM_LOG_INFO("  Total Synapses: " + std::to_string(totalSynapses));
            NLM_LOG_INFO("  E/I Ratio: " + std::to_string(eI));
            NLM_LOG_INFO("  Average Connections per Neuron: " + std::to_string(totalSynapses / (float)totalNeurons));
            
            // Save analysis results if requested
            if (!outputFile.empty()) {
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << "Connectivity Analysis\n";
                    file << "===================\n\n";
                    file << "Total Neurons: " << totalNeurons << "\n";
                    file << "Total Synapses: " << totalSynapses << "\n";
                    file << "E/I Ratio: " << eI << "\n";
                    file << "Avg Connections/Neuron: " << totalSynapses / (float)totalNeurons << "\n";
                    file << "Analysis Time: " << std::time(nullptr) << "\n";
                    file.close();
                    NLM_LOG_INFO("Analysis results saved to: " + outputFile);
                }
            }
        }
        
        else if (analysisType == "plasticity") {
            std::cout << "Analyzing plasticity effects..." << std::endl;
            
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run plasticity experiment
            NLM_LOG_INFO("Running plasticity experiment...")
            
            // Record initial state
            size_t initialSynapses = brain->getTotalSynapseCount();
            
            for (SimulationStep step = 0; step < 500; ++step) {
                for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount() / 4); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
            }
            
            size_t finalSynapses = brain->getTotalSynapseCount();
            size_t synapseChange = finalSynapses - initialSynapses;
            
            NLM_LOG_INFO("Plasticity Analysis Results:");
            NLM_LOG_INFO("  Initial Synapses: " + std::to_string(initialSynapses));
            NLM_LOG_INFO("  Final Synapses: " + std::to_string(finalSynapses));
            NLM_LOG_INFO("  Change: " + std::to_string(synapseChange));
            NLM_LOG_INFO("  Relative Change: " + std::to_string(synapseChange / (float)initialSynapses * 100.0f) + "%");
            
            // Save analysis results if requested
            if (!outputFile.empty()) {
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << "Plasticity Analysis\n";
                    file << "==================\n\n";
                    file << "Initial Synapses: " << initialSynapses << "\n";
                    file << "Final Synapses: " << finalSynapses << "\n";
                    file << "Change: " << synapseChange << "\n";
                    file << "Relative Change: " << synapseChange / (float)initialSynapses * 100.0f << "%\n";
                    file << "Analysis Time: " << std::time(nullptr) << "\n";
                    file.close();
                    NLM_LOG_INFO("Analysis results saved to: " + outputFile);
                }
            }
        }
        
        else {
            std::cerr << "ERROR: Unknown analysis type: " << analysisType << std::endl;
            return false;
        }
        
        std::cout << "✓ Analysis complete!" << std::endl;
        return true;
    }

    // Main command execution
    int executeCommand(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return 1;
        }

        std::string command = argv[1];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        // Parse command line arguments into a map
        std::map<std::string, std::string> args;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                args[key] = value;
            } else {
                args[arg] = "";
            }
        }

        // Route commands
        if (command == "--help" || command == "-h") {
            printHelp();
            return 0;
        }
        else if (command == "--version" || command == "-v") {
            printVersion();
            return 0;
        }
        else if (command == "--config-list") {
            listConfigOptions();
            return 0;
        }
        else if (command == "--generate-config") {
            std::string outputFile = "config_template.cfg";
            if (args.count("--output")) {
                outputFile = args["--output"];
            }
            return generateConfig(outputFile) ? 0 : 1;
        }
        else if (command == "--validate") {
            std::string configFile;
            if (args.count("--config")) {
                configFile = args["--config"];
            } else {
                configFile = "configs/default.cfg";
            }
            return validateConfig(configFile) ? 0 : 1;
        }
        else if (command == "--brain") {
            std::string configFile = "configs/default.cfg";
            int steps = 100;
            bool record = false;
            
            if (args.count("--config")) {
                configFile = args["--config"];
            }
            if (args.count("--steps")) {
                steps = std::stoi(args["--steps"]);
            }
            if (args.count("--record")) {
                record = true;
            }
            
            return runBrainSimulation(configFile, steps, record) ? 0 : 1;
        }
        else if (command == "--analyze") {
            std::string analysisType;
            std::string outputFile;
            
            if (args.count("--type")) {
                analysisType = args["--type"];
            } else {
                std::cerr << "ERROR: Analysis type required (--type=connectivity or --type=plasticity)" << std::endl;
                return 1;
            }
            
            if (args.count("--output")) {
                outputFile = args["--output"];
            }
            
            return analyzeBrain(analysisType, outputFile) ? 0 : 1;
        }
        else {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
            std::cout << "\nType 'nlm --help' for available commands." << std::endl;
            return 1;
        }
    }

private:
    std::vector<Command> commands = {
        {"--help", "Show help information", "nlm --help", nullptr},
        {"--version", "Show version information", "nlm --version", nullptr},
        {"--config-list", "List all configuration options", "nlm --config-list", nullptr},
        {"--generate-config", "Generate template configuration file", "nlm --generate-config [--output=file.cfg]", nullptr},
        {"--validate", "Validate configuration file", "nlm --validate [--config=file.cfg]", nullptr},
        {"--brain", "Run brain simulation", "nlm --brain [--config=file.cfg] [--steps=N] [--record]", nullptr},
        {"--analyze", "Analyze brain state", "nlm --analyze --type=connectivity|plasticity [--output=file.txt]", nullptr}
    };
};

int main(int argc, char** argv) {
    CommandLineInterface cli;
    return cli.executeCommand(argc, argv);
}
}