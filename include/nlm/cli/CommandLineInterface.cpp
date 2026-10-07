// CommandLineInterface implementation
// Enhanced command-line interface for NLM

#include "cli/CommandLineInterface.hpp"
#include "core/Config/EnhancedConfig.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>
#include <filesystem>

namespace nlm {

CommandLineInterface::CommandLineInterface() : config(std::make_shared<EnhancedConfig>()), verbose(false), debug(false) {
    NLM_LOG_INFO("Command-line interface initialized");
}

CommandLineInterface::~CommandLineInterface() {
    NLM_LOG_INFO("Command-line interface destroyed");
}

bool CommandLineInterface::parseArguments(int argc, char** argv) {
    try {
        NLM_LOG_INFO("Parsing command line arguments...");
        
        // Check for help/version requests first
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false; // Exit normally after showing help
            } else if (arg == "--version" || arg == "-v") {
                showVersion();
                return false; // Exit normally after showing version
            } else if (arg == "--verbose" || arg == "-V") {
                verbose = true;
                config->set("verbose", true, ConfigSource::CommandLine);
                logMessage("Verbose mode enabled", LogLevel::Info);
            } else if (arg == "--debug" || arg == "-d") {
                debug = true;
                config->set("debug", true, ConfigSource::CommandLine);
                logMessage("Debug mode enabled", LogLevel::Debug);
            }
        }
        
        // Load configuration from arguments
        bool configSuccess = config->loadFromArgs(argc, argv);
        if (!configSuccess) {
            NLM_LOG_WARNING("Some command line arguments may not be in expected format");
        }
        
        // Load configuration from environment
        config->loadFromEnvironment();
        
        // Load configuration from file if specified
        std::string configFile = config->getOr<std::string>("config", "");
        if (!configFile.empty()) {
            NLM_LOG_INFO("Loading configuration from file: " + configFile);
            if (!config->loadFromFile(configFile)) {
                NLM_LOG_ERROR("Failed to load configuration from file: " + configFile);
                return false;
            }
        }
        
        // Validate configuration
        if (!validateArguments()) {
            return false;
        }
        
        NLM_LOG_INFO("Command line parsing completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception parsing command line arguments: ") + e.what());
        return false;
    }
}

void CommandLineInterface::showHelp() const {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << "  nlm --help                    Show this help message" << std::endl;
    std::cout << "  nlm --version                 Show version information" << std::endl;
    std::cout << "  nlm --verbose, -V             Enable verbose output" << std::endl;
    std::cout << "  nlm --debug, -d               Enable debug output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --key=value                  Set configuration key to value" << std::endl;
    std::cout << "  -k value                     Short form: set key to value" << std::endl;
    std::cout << "  --config=FILE                Load configuration from file" << std::endl;
    std::cout << "  --help-config                Show configuration help" << std::endl;
    std::cout << "  --list-config                List all configuration options" << std::endl;
    std::cout << std::endl;
    
    std::cout << "COMMON CONFIGURATION KEYS:" << std::endl;
    std::cout << "  --neuron_count=N             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region_count=N             Number of regions (default: 1)" << std::endl;
    std::cout << "  --connection_probability=X    Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --simulation_timestep=X       Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random_seed=N              Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << "  --enable_development=true     Enable developmental plasticity" << std::endl;
    std::cout << "  --enable_checkpointing=true    Enable checkpoint saving" << std::endl;
    std::cout << "  --checkpoint_dir=PATH         Checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ENVIRONMENT VARIABLES:" << std::endl;
    std::cout << "  NLM_NEURON_COUNT             Set neuron count via environment" << std::endl;
    std::cout << "  NLM_SIMULATION_TIMESTEP      Set timestep via environment" << std::endl;
    std::cout << "  NLM_ENABLE_DEVELOPMENT       Enable development via environment" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLE USAGE:" << std::endl;
    std::cout << "  nlm --neuron_count=2000 --simulation_timestep=0.0005" << std::endl;
    std::cout << "  NLM_NEURON_COUNT=5000 ./nlm" << std::endl;
    std::cout << "  nlm config_file.cfg --enable_development=true" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE MODES:" << std::endl;
    std::cout << "  --phase1                    Run Phase 1 (basic neural computation)" << std::endl;
    std::cout << "  --phase2                    Run Phase 2 (real neural computation)" << std::endl;
    std::cout << "  --phase3                    Run Phase 3 (world interaction)" << std::endl;
    std::cout << "  --phase4                    Run Phase 4 (prediction and cognition)" << std::endl;
    std::cout << "  --phase5                    Run Phase 5 (performance optimizations)" << std::endl;
    std::cout << "  --phase6                    Run Phase 6 (final integration test)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPERIMENT OPTIONS:" << std::endl;
    std::cout << "  --experiment=EXPERIMENT_NAME  Run specific experiment" << std::endl;
    std::cout << "  --experiment-config=FILE      Load experiment configuration" << std::endl;
    std::cout << "  --max_steps=N                Maximum simulation steps" << std::endl;
    std::cout << "  --enable_checkpointing=true    Enable checkpoint saving during experiment" << std::endl;
    std::cout << std::endl;
    
    std::cout << std::string(80, '=') << std::endl;
}

void CommandLineInterface::showVersion() const {
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase: 6 (Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;
    std::cout << "Homepage: https://github.com/nlm-project/nlm" << std::endl;
    std::cout << "Documentation: https://nlm.readthedocs.io" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::showConfigurationHelp() const {
    std::cout << std::endl;
    std::cout << "NLM CONFIGURATION HELP" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration keys can be set via:" << std::endl;
    std::cout << "  1. Command line (--key=value)" << std::endl;
    std::cout << "  2. Environment variables (NLM_ prefix)" << std::endl;
    std::cout << "  3. Configuration file" << std::endl;
    std::cout << "  4. Runtime API" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Key types and validation:" << std::endl;
    std::cout << "  int, int64_t, double, bool, string, std::vector<T>" << std::endl;
    std::cout << "  Ranges and valid values can be specified in configuration options" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration file format:" << std::endl;
    std::cout << "  # comment or empty lines are ignored" << std::endl;
    std::cout << "  key = value" << std::endl;
    std::cout << "  value can be quoted for strings" << std::endl;
    std::cout << "  Example: neuron_count = 1000" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Environment variable format:" << std::endl;
    std::cout << "  NLM_<KEY_NAME> = value" << std::endl;
    std::cout << "  Example: NLM_NEURON_COUNT = 2000" << std::endl;
    std::cout << "  Keys are automatically uppercase and dash/underscore normalized" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available presets:" << std::endl;
    std::cout << "  --preset=quick              Quick test mode (small scale)" << std::endl;
    std::cout << "  --preset=standard           Standard configuration" << std::endl;
    std::cout << "  --preset=production         Production configuration" << std::endl;
    std::cout << "  --preset=research           Research configuration (larger scale)" << std::endl;
    std::cout << std::endl;
}

bool CommandLineInterface::validateArguments() {
    std::cout << "Validating command line arguments..." << std::endl;
    
    std::vector<std::string> errors;
    
    // Validate neuron count
    auto neuronCount = config->getOr<int>("neuron_count", -1);
    if (neuronCount < 1) {
        errors.push_back("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be >= 1");
    } else if (neuronCount > 100000) {
        errors.push_back("Neuron count " + std::to_string(neuronCount) + " is too large (max: 100000)");
    }
    
    // Validate region count
    auto regionCount = config->getOr<int>("region_count", -1);
    if (regionCount < 1) {
        errors.push_back("Invalid region_count: " + std::to_string(regionCount) + ". Must be >= 1");
    } else if (regionCount > 100) {
        errors.push_back("Region count " + std::to_string(regionCount) + " is too large (max: 100)");
    }
    
    // Validate connection probability
    auto connectionProb = config->getOr<double>("connection_probability", -1.0);
    if (connectionProb < 0.0 || connectionProb > 1.0) {
        errors.push_back("Invalid connection_probability: " + std::to_string(connectionProb) + ". Must be between 0.0 and 1.0");
    }
    
    // Validate simulation timestep
    auto timestep = config->getOr<double>("simulation_timestep", -1.0);
    if (timestep <= 0.0) {
        errors.push_back("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be > 0.0");
    } else if (timestep > 1.0) {
        errors.push_back("Simulation timestep " + std::to_string(timestep) + " is too large (max: 1.0)");
    }
    
    // Validate random seed
    auto seed = config->getOr<int64_t>("random_seed", -1);
    if (seed < 0) {
        errors.push_back("Invalid random_seed: " + std::to_string(seed) + ". Must be >= 0");
    }
    
    // Check for conflicting configurations
    auto enableDev = config->getOr<bool>("enable_development", false);
    auto enableCheckpoint = config->getOr<bool>("enable_checkpointing", false);
    auto enableReplay = config->getOr<bool>("enable_replay", false);
    
    if (enableCheckpoint && enableDev) {
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "");
        if (checkpointDir.empty()) {
            errors.push_back("Checkpoint directory not specified when checkpointing and development are both enabled");
        }
    }
    
    // Validate phase-specific requirements
    if (config->has("phase")) {
        std::string phase = config->getOr<std::string>("phase", "");
        if (phase != "1" && phase != "2" && phase != "3" && phase != "4" && phase != "5" && phase != "6") {
            errors.push_back("Invalid phase: " + phase + ". Must be 1, 2, 3, 4, 5, or 6");
        }
    }
    
    // Report errors
    if (!errors.empty()) {
        std::cout << std::endl;
        std::cout << "VALIDATION ERRORS:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Use --help for more information about configuration options." << std::endl;
        return false;
    }
    
    std::cout << "Command line validation passed successfully" << std::endl;
    return true;
}

void CommandLineInterface::logMessage(const std::string& message, LogLevel level) const {
    if (logger && level >= logLevel) {
        logger->log(message, level);
    }
    
    // Also output to stdout for important messages
    if (level == LogLevel::Info || level == LogLevel::Warning || level == LogLevel::Error) {
        std::cout << message << std::endl;
    }
}

const std::shared_ptr<EnhancedConfig>& CommandLineInterface::getConfig() const {
    return config;
}

void CommandLineInterface::setConfig(const std::shared_ptr<EnhancedConfig>& newConfig) {
    config = newConfig;
}

void CommandLineInterface::setLogger(std::unique_ptr<Logger> logger) {
    this->logger = std::move(logger);
}

void CommandLineInterface::setLogLevel(LogLevel level) {
    logLevel = level;
}

} // namespace nlm
