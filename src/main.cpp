// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include <chrono>

using namespace nlm;

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
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
===============================

USAGE:
    nlm [OPTIONS] [EXPERIMENT_NAME]

OPTIONS:
    --config <file>          Load configuration from file (default: configs/default.cfg)
    --config-json <file>      Load JSON configuration
    --neurons <count>         Set number of neurons (overrides config)
    --timestep <value>        Set simulation timestep in ms (overrides config)
    --seed <number>           Set random seed for reproducibility
    --help                    Show this help message
    --version                 Show version information
    --list-experiments        List available experiments
    --benchmark               Run performance benchmark
    --validate-config         Validate configuration before running

EXPERIMENTS:
    basic                    Run basic connectivity test
    plasticity               Run plasticity learning experiment
    stdp                     Run STDP verification
    all                      Run all tests (default if no experiment specified)

CONFIGURATION:
    Configuration files should be in key=value format:
        neuron_count = 1000
        simulation_timestep = 0.001
        random_seed = 42

    JSON format is also supported:
        {"neuron_count": 1000, "simulation_timestep": 0.001, "random_seed": 42}

EXAMPLES:
    # Run all tests with default config
    nlm

    # Use custom config file
    nlm --config my_config.cfg all

    # Quick benchmark
    nlm --neurons 500 --timestep 0.001 --benchmark

    # Just run basic test
    nlm basic

    # Run plasticity experiment with seed
    nlm --seed 123 plasticity

    # Validate config before running
    nlm --validate-config --config my_config.cfg

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine v0.1.0 (Phase 2)" << std::endl;
    std::cout << "Built with C++20 and real spiking neural computation." << std::endl;
    std::cout << "Project: https://github.com/nlm-project/nlm" << std::endl;
}

void validateConfig(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Validating configuration...");
    
    bool isValid = true;
    
    // Check for required parameters
    if (!config->has("neuron_count")) {
        NLM_LOG_WARNING("Missing required parameter: neuron_count (using default)");
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        isValid = false;
    }
    
    if (!config->has("simulation_timestep")) {
        NLM_LOG_WARNING("Missing required parameter: simulation_timestep (using default)");
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        isValid = false;
    }
    
    // Validate parameter ranges
    auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        isValid = false;
    } else if (neuronCount > 100000) {
        NLM_LOG_WARNING("Large neuron_count may impact performance: " + std::to_string(neuronCount));
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
        isValid = false;
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed!");
    } else {
        NLM_LOG_ERROR("Configuration validation failed!");
    }
    
    return isValid;
}

void logConfigSummary(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
}

void runBenchmark(std::shared_ptr<Brain> brain) {
    if (brain->getConfig()->getOr<bool>("benchmark_mode", false)) {
        NLM_LOG_INFO("Running benchmark mode...");
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run a few steps
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Benchmark completed in " + std::to_string(duration.count()) + " ms");
    }
}

void runExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName) {
    std::transform(experimentName.begin(), experimentName.end(), experimentName.begin(), ::tolower);
    
    if (experimentName == "basic") {
        runBasicConnectivityTest(brain);
    } else if (experimentName == "plasticity") {
        runPlasticityExperiment(brain);
    } else if (experimentName == "stdp") {
        runStdpVerification(brain);
    } else if (experimentName == "all") {
        // Run all tests with resets between them
        brain->reset();
        brain->initialize();
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + experimentName);
        printHelp();
        std::exit(1);
    }
}

void parseCommandLineArgs(const std::vector<std::string>& args, std::shared_ptr<Config> config, std::string& configFile) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            std::exit(0);
        } else if (arg == "--version") {
            printVersion();
            std::exit(0);
        } else if (arg == "--list-experiments") {
            std::cout << "Available experiments:" << std::endl;
            std::cout << "  basic    - Basic connectivity test" << std::endl;
            std::cout << "  plasticity - Plasticity learning experiment" << std::endl;
            std::cout << "  stdp      - STDP verification" << std::endl;
            std::cout << "  all       - Run all tests (default)" << std::endl;
            std::exit(0);
        } else if (arg == "--benchmark") {
            config->set("benchmark_mode", true, ConfigSource::CommandLine);
        } else if (arg == "--validate-config") {
            config->set("validate_config", true, ConfigSource::CommandLine);
        } else if (arg == "--config-json") {
            if (i + 1 < args.size()) {
                if (config->loadFromJsonFile(args[++i])) {
                    NLM_LOG_INFO("Loaded JSON configuration from: " + args[i]);
                } else {
                    NLM_LOG_WARNING("Failed to load JSON configuration: " + args[i]);
                }
            }
        } else if (arg.find("--config") == 0) {
            // Handle --config=file or --config file
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < args.size()) {
                value = args[++i];
            }
            if (!value.empty()) {
                configFile = value;
                NLM_LOG_INFO("Config file specified: " + configFile);
            }
        } else if (arg.find("--neurons") == 0) {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < args.size()) {
                value = args[++i];
            }
            if (!value.empty()) {
                try {
                    int64_t neuronCount = std::stoll(value);
                    config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Neuron count set to: " + value);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid neuron count: " + value);
                }
            }
        } else if (arg.find("--timestep") == 0) {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < args.size()) {
                value = args[++i];
            }
            if (!value.empty()) {
                try {
                    double timestep = std::stod(value);
                    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Timestep set to: " + value);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid timestep: " + value);
                }
            }
        } else if (arg.find("--seed") == 0) {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < args.size()) {
                value = args[++i];
            }
            if (!value.empty()) {
                try {
                    int64_t seed = std::stoll(value);
                    config->set("random_seed", seed, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Random seed set to: " + value);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid seed: " + value);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments into strings for easier processing
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Default values
    std::string experimentName = "all";
    std::string configFile = "configs/default.cfg";
    
    // Check for help/version before processing
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--list-experiments") {
            std::cout << "Available experiments:" << std::endl;
            std::cout << "  basic    - Basic connectivity test" << std::endl;
            std::cout << "  plasticity - Plasticity learning experiment" << std::endl;
            std::cout << "  stdp      - STDP verification" << std::endl;
            std::cout << "  all       - Run all tests (default)" << std::endl;
            return 0;
        }
    }
    
    // Parse experiment name (last positional argument)
    if (!args.empty()) {
        std::string lastArg = args.back();
        std::transform(lastArg.begin(), lastArg.end(), lastArg.begin(), ::tolower);
        if (lastArg == "all" || lastArg == "basic" || lastArg == "plasticity" || lastArg == "stdp") {
            experimentName = lastArg;
            // Remove from args so it doesn't interfere with other parsing
            args.pop_back();
        }
    }
    
    // Advanced experiment runner with power-user features
define_advanced_experiment_runner(std::vector<std::string> args) {
    // Parse advanced arguments
    std::string experimentName = "all";
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    bool enableCheckpointing = false;
    bool parallelExecution = false;
    int checkpointInterval = 1000;
    int maxWorkers = 4;
    bool exportConfig = false;
    bool saveProgress = false;
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--help-advanced") {
            printAdvancedHelp();
            return;
        } else if (arg == "--experiment") {
            if (i + 1 < args.size()) experimentName = args[++i];
        } else if (arg == "--config") {
            if (i + 1 < args.size()) configFile = args[++i];
        } else if (arg == "--output") {
            if (i + 1 < args.size()) outputFile = args[++i];
        } else if (arg == "--checkpoint") {
            enableCheckpointing = true;
            if (i + 1 < args.size() && std::isdigit(args[i + 1][0])) {
                checkpointInterval = std::stoi(args[++i]);
            }
        } else if (arg == "--parallel") {
            parallelExecution = true;
        } else if (arg == "--export-config") {
            exportConfig = true;
        } else if (arg == "--save-progress") {
            saveProgress = true;
        }
    }
    
    // Initialize and run
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    // Load configuration
    if (configFile != "configs/default.cfg" && config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr);
    
    // Set advanced options
    config->set("save_checkpoint", saveProgress, ConfigSource::CommandLine);
    
    // Initialize brain
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Run advanced experiment
    runExperimentWithSession(brain, experimentName, enableCheckpointing, checkpointInterval);
    
    // Export configuration if requested
    if (exportConfig && !outputFile.empty()) {
        exportConfiguration(config, outputFile);
    }
    
    NLM_LOG_INFO("Advanced experiment completed successfully!");
}

// Print advanced help information
void printAdvancedHelp() {
    std::cout << R"(
NLM Advanced Features Help
===========================

Advanced Command Line Options:

Experiment Management:
    --experiment <name>      Specify experiment (default: all)
    --checkpoint [interval]  Enable checkpointing (default interval: 1000)
    --parallel              Run experiments in parallel

Configuration Management:
    --output <file>         Export configuration to file
    --export-config         Export current configuration
    --save-progress         Save experiment progress

Performance:
    --benchmark             Run performance benchmark
    --validate-config       Validate configuration
    --list-experiments      List all experiments

Session Management:
    --load-session <file>   Load previous experiment session
    --checkpoint-dir <dir>  Set checkpoint directory

Configuration Export:
    --export-config [format] Export configuration (kv/json)
    --config <file>         Load configuration from file
    --set <key>=<value>      Set configuration parameter

Advanced Experiment Options:
    --neurons <count>        Number of neurons
    --timestep <value>       Simulation timestep
    --seed <number>          Random seed
    --plasticity <type>     Plasticity type (hebbian/stdp/both)
    --curiosity <bool>       Enable/disable curiosity
    --development <bool>     Enable/disable development

Batch Processing:
    --batch <exp1>,<exp2>,... Run multiple experiments in sequence
    --workers <n>            Number of parallel workers (default: 4)

Monitoring and Logging:
    --log-level <level>     Set log level (debug/info/warning/error)
    --log-file <file>        Write logs to file
    --progress-interval <n> Log progress every n steps

Checkpoint Options:
    --checkpoint-dir <dir>   Directory for checkpoint files
    --max-checkpoints <n>    Maximum number of checkpoints to keep
    --auto-save-interval <n> Auto-save interval in steps

Session Recovery:
    --resume <file>          Resume from saved session
    --session-id <id>        Specify session ID
    --keep-sessions <n>      Keep last n sessions

Performance Monitoring:
    --monitor-interval <n>   Performance monitoring interval
    --memory-limit <bytes>   Memory limit in bytes
    --cpu-limit <percent>    CPU usage limit (0-100)

Scripting and Automation:
    --script <file>         Run script for experiment control
    --python               Use Python for experiment control
    --json-config <file>    Load configuration from JSON file

Export Formats:
    --export-kv             Export as key-value pairs
    --export-json           Export as JSON
    --export-xml            Export as XML
    --export-yaml           Export as YAML

Experiment Templates:
    --template <name>       Use predefined experiment template
    --custom-template <file> Use custom experiment template

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

// Enhanced experiment runner with power-user features
define_advanced_experiment_runner(std::vector<std::string> args) {
    // Parse advanced arguments
    std::string experimentName = "all";
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    bool enableCheckpointing = false;
    bool parallelExecution = false;
    int checkpointInterval = 1000;
    int maxWorkers = 4;
    bool exportConfig = false;
    bool saveProgress = false;
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--help-advanced") {
            printAdvancedHelp();
            return;
        } else if (arg == "--experiment") {
            if (i + 1 < args.size()) experimentName = args[++i];
        } else if (arg == "--config") {
            if (i + 1 < args.size()) configFile = args[++i];
        } else if (arg == "--output") {
            if (i + 1 < args.size()) outputFile = args[++i];
        } else if (arg == "--checkpoint") {
            enableCheckpointing = true;
            if (i + 1 < args.size() && std::isdigit(args[i + 1][0])) {
                checkpointInterval = std::stoi(args[++i]);
            }
        } else if (arg == "--parallel") {
            parallelExecution = true;
        } else if (arg == "--export-config") {
            exportConfig = true;
        } else if (arg == "--save-progress") {
            saveProgress = true;
        }
    }
    
    // Initialize and run
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    // Load configuration
    if (configFile != "configs/default.cfg" && config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr);
    
    // Set advanced options
    config->set("save_checkpoint", saveProgress, ConfigSource::CommandLine);
    
    // Initialize brain
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Run advanced experiment
    runExperimentWithSession(brain, experimentName, enableCheckpointing, checkpointInterval);
    
    // Export configuration if requested
    if (exportConfig && !outputFile.empty()) {
        exportConfiguration(config, outputFile);
    }
    
    NLM_LOG_INFO("Advanced experiment completed successfully!");
}