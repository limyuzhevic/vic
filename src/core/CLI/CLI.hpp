#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {
namespace cli {

// CLI option handling
struct CLIOptions {
    std::string configFile;
    bool showHelp;
    bool showVersion;
    bool validateConfig;
    bool listConfigs;
    std::vector<std::string> extraArgs;
    
    CLIOptions() : showHelp(false), showVersion(false), validateConfig(false), listConfigs(false) {}
};

// Parse command line arguments
CLIOptions parseArgs(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            options.showVersion = true;
        } else if (arg == "--validate") {
            options.validateConfig = true;
        } else if (arg == "--list-configs") {
            options.listConfigs = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else {
            options.extraArgs.push_back(arg);
        }
    }
    
    return options;
}

// Show help information
void showHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS] [ARGS...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message and exit" << std::endl;
    std::cout << "  -v, --version           Show version information and exit" << std::endl;
    std::cout << "      --config <file>     Load configuration from file" << std::endl;
    std::cout << "      --validate          Validate configuration and exit" << std::endl;
    std::cout << "      --list-configs      List all available configuration options" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2 Test Arguments:" << std::endl;
    std::cout << "  The following arguments are used for Phase 2 testing:" << std::endl;
    std::cout << "    --test-mode=<mode>    Select test mode (connectivity, learning, stdp)" << std::endl;
    std::cout << "    --steps=<n>           Number of simulation steps" << std::endl;
    std::cout << "    --neuron-count=<n>    Number of neurons to simulate" << std::endl;
    std::cout << "    --region-count=<n>    Number of brain regions" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm                          Run default Phase 2 tests" << std::endl;
    std::cout << "  nlm --config=myconfig.cfg     Load custom configuration" << std::endl;
    std::cout << "  nlm --validate                Validate current configuration" << std::endl;
    std::cout << "  nlm --list-configs            List all available configuration options" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Show version information
void showVersion() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase: 2 - Real Neural Computation" << std::endl;
    std::cout << "Build: Release" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM (熙然, meaning \"serene flow\") is an experimental computational brain project." << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Validate configuration
bool validateConfig(const std::shared_ptr<Config>& config) {
    std::cout << "=== Configuration Validation ===" << std::endl;
    
    bool isValid = true;
    
    // Check required configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            std::cout << "  ✗ Missing required key: " << key << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Found key: " << key << " = " << config->getKeys() << std::endl;
        }
    }
    
    // Check for reasonable values
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0 || neuronCount > 100000) {
        std::cout << "  ✗ Invalid neuron_count: " << neuronCount << std::endl;
        isValid = false;
    } else {
        std::cout << "  ✓ Valid neuron_count: " << neuronCount << std::endl;
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        std::cout << "  ✗ Invalid simulation_timestep: " << timestep << std::endl;
        isValid = false;
    } else {
        std::cout << "  ✓ Valid simulation_timestep: " << timestep << "s" << std::endl;
    }
    
    // Check for deprecated configuration keys
    std::vector<std::string> deprecatedKeys = {
        "deprecated_option"
    };
    
    for (const auto& key : deprecatedKeys) {
        if (config->has(key)) {
            std::cout << "  ⚠ Deprecated key: " << key << " (should be removed)" << std::endl;
        }
    }
    
    if (isValid) {
        std::cout << "  ✓ Configuration is valid" << std::endl;
    } else {
        std::cout << "  ✗ Configuration validation failed" << std::endl;
    }
    
    return isValid;
}

// List available configuration options
void listConfigurationOptions() {
    std::cout << "=== Available Configuration Options ===" << std::endl;
    std::cout << std::endl;
    
    // Core simulation parameters
    std::cout << "Core Simulation Parameters:" << std::endl;
    std::cout << "  random_seed             = 42                           # Random seed for reproducible experiments" << std::endl;
    std::cout << "  simulation_timestep     = 0.001                        # Simulation timestep in seconds" << std::endl;
    std::cout << "  neuron_count            = 1000                          # Number of neurons in the brain" << std::endl;
    std::cout << "  region_count            = 2                             # Number of brain regions" << std::endl;
    std::cout << "  connection_probability  = 0.1                           # Connection probability for random connectivity" << std::endl;
    std::cout << std::endl;
    
    // Plasticity settings
    std::cout << "Plasticity Settings:" << std::endl;
    std::cout << "  plasticity_learning_rate = 0.01                        # Overall plasticity learning rate" << std::endl;
    std::cout << "  stdp_ltp_weight         = 0.02                         # STDP long-term potentiation weight" << std::endl;
    std::cout << "  stdp_ltd_weight         = 0.015                        # STDP long-term depression weight" << std::endl;
    std::cout << "  stdp_tau                = 20.0                         # STDP time constant" << std::endl;
    std::cout << std::endl;
    
    // Development settings
    std::cout << "Development Settings:" << std::endl;
    std::cout << "  synaptogenesis_rate     = 0.0001                       # Rate of new synapse formation" << std::endl;
    std::cout << "  pruning_rate            = 0.00001                      # Rate of synapse removal" << std::endl;
    std::cout << "  maturation_rate         = 0.001                        # Rate of adult property development" << std::endl;
    std::cout << std::endl;
    
    // Neuromodulation settings
    std::cout << "Neuromodulation Settings:" << std::endl;
    std::cout << "  dopamine_baseline       = 0.1                           # Baseline dopamine level" << std::endl;
    std::cout << "  curiosity_baseline      = 0.05                          # Baseline curiosity drive" << std::endl;
    std::cout << "  novelty_baseline        = 0.1                           # Baseline novelty detection" << std::endl;
    std::cout << "  reward_prediction_error = 0.0                           # Reward prediction error threshold" << std::endl;
    std::cout << std::endl;
    
    // Environment settings
    std::cout << "Environment Settings:" << std::endl;
    std::cout << "  environment_name        = GridWorld                   # Environment type" << std::endl;
    std::cout << "  environment_width       = 10                          # Environment width" << std::endl;
    std::cout << "  environment_height      = 10                          # Environment height" << std::endl;
    std::cout << "  observation_radius      = 2.0                          # Sensory observation radius" << std::endl;
    std::cout << "  action_radius           = 1.0                          # Action execution radius" << std::endl;
    std::cout << std::endl;
    
    // Simulation settings
    std::cout << "Simulation Settings:" << std::endl;
    std::cout << "  max_simulation_steps   = 10000                        # Maximum number of simulation steps" << std::endl;
    std::cout << "  simulation_time_limit   = 0.0                          # Time limit for simulation (seconds)" << std::endl;
    std::cout << "  checkpoint_interval     = 1000                         # Steps between checkpoints" << std::endl;
    std::cout << "  enable_checkpointing    = false                        # Enable checkpoint saving" << std::endl;
    std::cout << "  enable_replay           = false                        # Enable episodic memory replay" << std::endl;
    std::cout << std::endl;
    
    // Logging settings
    std::cout << "Logging Settings:" << std::endl;
    std::cout << "  log_level              = INFO                         # Logging level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  log_to_file            = false                        # Log to file instead of console" << std::endl;
    std::cout << "  log_filename           = nlm.log                     # Log file name" << std::endl;
    std::cout << "  log_file_max_size      = 1048576                      # Maximum log file size (bytes)" << std::endl;
    std::cout << "  log_backup_count        = 5                            # Number of backup log files" << std::endl;
    std::cout << std::endl;
    
    // Visualization settings
    std::cout << "Visualization Settings:" << std::endl;
    std::cout << "  visualization_enabled   = false                        # Enable visualization" << std::endl;
    std::cout << "  visualization_update_rate = 30.0                      # Visualization update rate (Hz)" << std::endl;
    std::cout << "  visualization_window    = 800                         # Visualization window width" << std::endl;
    std::cout << "  visualization_height   = 600                         # Visualization window height" << std::endl;
    std::cout << "  visualization_backend   = OpenGL                      # Visualization backend" << std::endl;
    std::cout << std::endl;
    
    // Development mode settings
    std::cout << "Development Mode Settings:" << std::endl;
    std::cout << "  development_mode        = false                        # Enable development mode features" << std::endl;
    std::cout << "  debug_neuron_connections = false                        # Log detailed neuron connection info" << std::endl;
    std::cout << "  enable_profiling       = false                        # Enable performance profiling" << std::endl;
    std::cout << "  profile_output_file     = profile.json                # Performance profile output file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Configuration File Format ===" << std::endl;
    std::cout << "Configuration files can be in simple key=value format:" << std::endl;
    std::cout << "  # Comment line" << std::endl;
    std::cout << "  key = value" << std::endl;
    std::cout << "  another_key = \"string value with spaces\"" << std::endl;
    std::cout << "  boolean_key = true" << std::endl;
    std::cout << "  integer_key = 42" << std::endl;
    std::cout << "  float_key = 3.14" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Command Line Arguments ===" << std::endl;
    std::cout << "Command line arguments can override configuration:" << std::endl;
    std::cout << "  nlm --key=value --another_key another_value -third third_value" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Configuration Sources (highest priority first) ===" << std::endl;
    std::cout << "1. Command line arguments" << std::endl;
    std::cout << "2. Configuration file" << std::endl;
    std::cout << "3. Default configuration" << std::endl;
    std::cout << "4. Runtime configuration" << std::endl;
    std::cout << std::endl;
}

}} // namespace nlm::cli