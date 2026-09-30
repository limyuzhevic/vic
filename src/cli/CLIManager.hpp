// NLM Core CLI module
// Comprehensive command-line interface for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Config/Config.hpp"
#include "cli/ArgParser.hpp"
#include "cli/HelpSystem.hpp"

namespace nlm::cli {

// Core CLI manager that orchestrates argument parsing, help, and configuration
class CLIManager {
private:
    std::string program_name;
    std::string program_description;
    std::string version;
    std::shared_ptr<ArgParser> arg_parser;
    std::shared_ptr<HelpSystem> help_system;
    std::shared_ptr<Config> config;
    bool verbose;
    bool quiet;
    bool help_requested;
    bool version_requested;
    
public:
    CLIManager(const std::string& name = "nlm", const std::string& desc = "Neural Learning Machine - Phase 2", 
               const std::string& ver = "1.0.0")
        : program_name(name), program_description(desc), version(ver)
        , verbose(false), quiet(false), help_requested(false), version_requested(false) {
        
        // Initialize help system
        help_system = std::make_shared<HelpSystem>();
        help_system->setProgramInfo(name, desc, ver);
        
        // Initialize argument parser
        arg_parser = std::make_shared<ArgParser>(name, desc, ver);
        
        // Initialize configuration
        config = std::make_shared<Config>();
    }
    
    // Add common arguments for NLM
    void addNLMArguments() {
        // Configuration arguments
        arg_parser->addArgument(Argument("config", "c", "Path to configuration file (JSON, YAML, or key=value format)", 
                           ArgumentType::FILEPATH, false, "Configuration"));
        arg_parser->addArgument(Argument("output", "o", "Output file path for results", 
                           ArgumentType::FILEPATH, false, "Output"));
        
        // Execution arguments
        arg_parser->addArgument(Argument("neurons", "n", "Number of neurons (default: 1000)", 
                           ArgumentType::INTEGER, false, "Execution"));
        arg_parser->addArgument(Argument("regions", "r", "Number of brain regions (default: 2)", 
                           ArgumentType::INTEGER, false, "Execution"));
        arg_parser->addArgument(Argument("timestep", "t", "Simulation timestep in seconds (default: 0.001)", 
                           ArgumentType::FLOAT, false, "Execution"));
        arg_parser->addArgument(Argument("steps", "", "Number of simulation steps (default: 1000)", 
                           ArgumentType::INTEGER, false, "Execution"));
        
        // Output and debugging
        arg_parser->addArgument(Argument("verbose", "v", "Enable verbose output (detailed logging)", 
                           ArgumentType::BOOLEAN, false, "Output"));
        arg_parser->addArgument(Argument("quiet", "q", "Suppress non-essential output (quiet mode)", 
                           ArgumentType::BOOLEAN, false, "Output"));
        arg_parser->addArgument(Argument("log_level", "", "Set log level: debug, info, warning, error", 
                           ArgumentType::ENUM, false, "Output"), 
                           std::vector<std::string>{"debug", "info", "warning", "error"});
        
        // Random seed and reproducibility
        arg_parser->addArgument(Argument("seed", "s", "Random seed for reproducible results", 
                           ArgumentType::INTEGER64, false, "General"));
        
        // Performance
        arg_parser->addArgument(Argument("performance", "p", "Performance mode (fast, balanced, quality)", 
                           ArgumentType::ENUM, false, "Performance"),
                           std::vector<std::string>{"fast", "balanced", "quality"});
        
        // Environment variables
        arg_parser->addEnvironmentVariable("NLM_CONFIG", "Path to default configuration file", ArgumentType::FILEPATH);
        arg_parser->addEnvironmentVariable("NLM_LOG_LEVEL", "Logging level (debug, info, warning, error)", ArgumentType::STRING);
        arg_parser->addEnvironmentVariable("NLM_SEED", "Random seed for reproducible results", ArgumentType::INTEGER64);
        arg_parser->addEnvironmentVariable("NLM_VERBOSE", "Enable verbose output", ArgumentType::BOOLEAN);
        arg_parser->addEnvironmentVariable("NLM_QUIET", "Suppress non-essential output", ArgumentType::BOOLEAN);
        arg_parser->addEnvironmentVariable("NLM_NEURONS", "Number of neurons (overrides command line)", ArgumentType::INTEGER);
        arg_parser->addEnvironmentVariable("NLM_REGIONS", "Number of brain regions (overrides command line)", ArgumentType::INTEGER);
        
        // Examples
        arg_parser->addExample("Basic Usage", program_name + " --config configs/default.cfg");
        arg_parser->addExample("Custom Brain", program_name + " --neurons 2000 --regions 3 --timestep 0.001");
        arg_parser->addExample("Verbose Debug", program_name + " --verbose --seed 123");
        arg_parser->addExample("Performance Mode", program_name + " --performance balanced");
        arg_parser->addExample("Quick Test", program_name + " --steps 100 --verbose");
        arg_parser->addExample("Output Results", program_name + " --output results.json --verbose");
    }
    
    // Add neural network specific arguments
    void addNeuralNetworkArguments() {
        // Neural network configuration
        arg_parser->addArgument(Argument("synapse_density", "", "Synapse connectivity density (default: 0.1)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("initial_weight_mean", "", "Mean initial synaptic weight (default: 0.5)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("initial_weight_std", "", "Standard deviation of initial weights (default: 0.1)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("v_thresh", "", "Neuron threshold potential in mV (default: -50.0)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("v_rest", "", "Resting potential in mV (default: -70.0)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("v_reset", "", "Reset potential after spike in mV (default: -75.0)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("tau_mem", "", "Membrane time constant in ms (default: 20.0)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        arg_parser->addArgument(Argument("tau_ref", "", "Refractory period in ms (default: 2.0)", 
                           ArgumentType::FLOAT, false, "Neural Network"));
        
        // Plasticity parameters
        arg_parser->addArgument(Argument("plasticity_learning_rate", "", "Plasticity learning rate (default: 0.01)", 
                           ArgumentType::FLOAT, false, "Plasticity"));
        arg_parser->addArgument(Argument("stdp_ltp_weight", "", "STDP LTP weight (default: 0.01)", 
                           ArgumentType::FLOAT, false, "Plasticity"));
        arg_parser->addArgument(Argument("stdp_ltd_weight", "", "STDP LTD weight (default: 0.012)", 
                           ArgumentType::FLOAT, false, "Plasticity"));
        arg_parser->addArgument(Argument("stdp_time_constant", "", "STDP time constant in ms (default: 20.0)", 
                           ArgumentType::FLOAT, false, "Plasticity"));
        
        // Development parameters
        arg_parser->addArgument(Argument("synaptogenesis_rate", "", "Synaptogenesis rate (default: 0.001)", 
                           ArgumentType::FLOAT, false, "Development"));
        arg_parser->addArgument(Argument("pruning_rate", "", "Pruning rate (default: 0.0001)", 
                           ArgumentType::FLOAT, false, "Development"));
    }
    
    // Add neuromodulation arguments
    void addNeuromodulationArguments() {
        arg_parser->addArgument(Argument("dopamine_scale", "", "Dopamine modulation scale (default: 1.0)", 
                           ArgumentType::FLOAT, false, "Neuromodulation"));
        arg_parser->addArgument(Argument("curiosity_enable", "", "Enable curiosity-driven exploration (default: true)", 
                           ArgumentType::BOOLEAN, false, "Neuromodulation"));
        arg_parser->addArgument(Argument("novelty_enable", "", "Enable novelty detection (default: true)", 
                           ArgumentType::BOOLEAN, false, "Neuromodulation"));
    }
    
    // Parse command line arguments
    bool parse(int argc, char** argv) {
        // Parse arguments
        if (!arg_parser->parse(argc, argv)) {
            return false;
        }
        
        // Update settings
        verbose = arg_parser->has("verbose") && arg_parser->getValue("verbose") == "true";
        quiet = arg_parser->has("quiet") && arg_parser->getValue("quiet") == "true";
        help_requested = arg_parser->isHelpRequested();
        version_requested = arg_parser->isVersionRequested();
        
        // Get configuration file
        std::string config_file = arg_parser->getConfigFile();
        
        // Load configuration
        if (!config_file.empty()) {
            if (config->loadFromFile(config_file)) {
                std::cout << "Loaded configuration from: " << config_file << std::endl;
            } else {
                std::cout << "Using default configuration (file not found: " << config_file << ")" << std::endl;
            }
        }
        
        // Override with command line arguments
        if (arg_parser->has("seed")) {
            config->set("random_seed", std::stoll(arg_parser->getValue("seed")), ConfigSource::CommandLine);
        }
        
        if (arg_parser->has("neurons")) {
            config->set("neuron_count", std::stoll(arg_parser->getValue("neurons")), ConfigSource::CommandLine);
        }
        
        if (arg_parser->has("regions")) {
            config->set("region_count", std::stoll(arg_parser->getValue("regions")), ConfigSource::CommandLine);
        }
        
        if (arg_parser->has("timestep")) {
            config->set("simulation_timestep", std::stod(arg_parser->getValue("timestep")), ConfigSource::CommandLine);
        }
        
        if (arg_parser->has("steps")) {
            config->set("max_simulation_steps", std::stoll(arg_parser->getValue("steps")), ConfigSource::CommandLine);
        }
        
        if (arg_parser->has("verbose")) {
            config->set("log_level", "INFO", ConfigSource::CommandLine);
        }
        
        // Set defaults for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return true;
    }
    
    // Validate configuration
    bool validate() const {
        // Check for conflicts
        if (verbose && quiet) {
            std::cerr << "Error: --verbose and --quiet cannot be used together" << std::endl;
            return false;
        }
        
        // Validate configuration values
        if (config->has("neuron_count")) {
            int64_t neurons = config->getOr<int64_t>("neuron_count", 1);
            if (neurons <= 0) {
                std::cerr << "Error: neuron_count must be positive" << std::endl;
                return false;
            }
        }
        
        if (config->has("simulation_timestep")) {
            double timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0) {
                std::cerr << "Error: simulation_timestep must be positive" << std::endl;
                return false;
            }
        }
        
        if (config->has("max_simulation_steps")) {
            int64_t steps = config->getOr<int64_t>("max_simulation_steps", 1000);
            if (steps <= 0) {
                std::cerr << "Error: max_simulation_steps must be positive" << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    // Show help
    void showHelp() const {
        help_system->showHelp();
    }
    
    // Show version
    void showVersion() const {
        help_system->showVersion();
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Get verbose flag
    bool isVerbose() const {
        return verbose;
    }
    
    // Get quiet flag
    bool isQuiet() const {
        return quiet;
    }
    
    // Check if help was requested
    bool isHelpRequested() const {
        return help_requested;
    }
    
    // Check if version was requested
    bool isVersionRequested() const {
        return version_requested;
    }
};