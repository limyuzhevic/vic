// NLM Configuration Helper Implementation
// Phase 2: Real Neural Computation
//
// This file implements the configuration helper class for loading,
// validating, and managing configuration parameters with comprehensive
// error handling and logging.

#include "ConfigHelper.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

using namespace nlm;

// Implementation of ConfigHelper methods

std::shared_ptr<Config> ConfigHelper::parseCommandLineArgs(int argc, char** argv) {
    NLM_LOG_INFO("Initializing NLM Phase 2 Real Neural Computation...");
    
    // Create configuration object
    auto config = std::make_shared<Config>();
    
    // Process help and version flags
    bool showHelp = false;
    bool showVersion = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-help") {
            showHelp = true;
        } else if (arg == "--version") {
            showVersion = true;
        }
    }
    
    if (showHelp) {
        printConfigurationHelp();
        std::exit(0);
    }
    
    if (showVersion) {
        NLM_LOG_INFO("NLM (Neural Learning Machine) Phase 2 - Version 1.0.0");
        NLM_LOG_INFO("Real Neural Computation with Spiking Neurons");
        std::exit(0);
    }
    
    // Apply command-line overrides
    applyCommandLineOverrides(argc, argv, config);
    
    // Load configuration file
    std::string configFile = getDefaultConfigFilePath();
    
    // Check for --config command line argument
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            break;
        }
    }
    
    // Load configuration file with detailed error handling
    if (!loadConfigurationFile(configFile, config)) {
        NLM_LOG_WARNING("Failed to load configuration file: " + configFile);
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply Phase 2 specific defaults
    applyPhase2Defaults(config);
    
    // Validate configuration
    ValidationResult validationResult = validateConfiguration(config);
    if (!validationResult.isValid) {
        NLM_LOG_ERROR("Configuration validation failed: " + validationResult.errorMessage);
        throw ConfigError("Configuration validation failed: " + validationResult.errorMessage);
    }
    
    // Log warnings if any
    for (const auto& warning : validationResult.warnings) {
        NLM_LOG_WARNING("Configuration warning: " + warning);
    }
    
    // Log configuration summary
    logConfigurationSummary(config);
    
    return config;
}

bool ConfigHelper::loadConfigurationFile(const std::string& configFilePath, 
                                       std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot load configuration file: config object is null");
        return false;
    }
    
    NLM_LOG_INFO("Loading configuration from file: " + configFilePath);
    
    // Check if file exists
    if (!std::filesystem::exists(configFilePath)) {
        NLM_LOG_DEBUG("Configuration file not found: " + configFilePath);
        return false;
    }
    
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        showConfigurationLoadError(configFilePath, "Failed to open file");
        return false;
    }
    
    size_t loadedCount = 0;
    size_t errorCount = 0;
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                config->set(key, value, ConfigSource::File);
                loadedCount++;
            } catch (const std::exception& e) {
                errorCount++;
                NLM_LOG_ERROR(std::string("Failed to parse configuration at line ") + 
                            std::to_string(lineNumber) + ": " + e.what());
            }
        } else {
            errorCount++;
            NLM_LOG_ERROR(std::string("Invalid configuration format at line ") + 
                        std::to_string(lineNumber) + ": missing '='") ;
        }
    }
    
    if (errorCount > 0) {
        NLM_LOG_WARNING(std::string("Configuration loaded with ") + std::to_string(errorCount) + 
                      " errors. Loaded " + std::to_string(loadedCount) + " valid entries.");
    } else {
        NLM_LOG_INFO(std::string("Configuration loaded successfully: ") + 
                    std::to_string(loadedCount) + " entries");
    }
    
    return loadedCount > 0 || errorCount == 0;
}

void ConfigHelper::applyPhase2Defaults(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot apply defaults: config object is null");
        return;
    }
    
    NLM_LOG_INFO("Applying Phase 2 configuration defaults...");
    
    // Set default values for Phase 2
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
    
    NLM_LOG_INFO("Phase 2 defaults applied successfully");
}

ConfigHelper::ValidationResult ConfigHelper::validateConfiguration(const std::shared_ptr<Config>& config) {
    ValidationResult result;
    
    if (!config) {
        result.isValid = false;
        result.errorMessage = "Configuration object is null";
        return result;
    }
    
    NLM_LOG_INFO("Validating configuration parameters...");
    
    // Get required parameters with validation
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    // Validate neuron count
    if (neuronCount == 0) {
        result.isValid = false;
        result.errorMessage = "Cannot initialize brain with 0 neurons";
        return result;
    }
    
    // Validate region count
    if (regionCount == 0) {
        result.isValid = false;
        result.errorMessage = "Cannot initialize brain with 0 regions";
        return result;
    }
    
    // Check neurons per region
    if (neuronCount / regionCount == 0) {
        result.warnings.push_back("Not enough neurons for each region, adjusting allocation");
    }
    
    // Validate connection probability
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        result.isValid = false;
        result.errorMessage = "connection_probability must be in [0, 1], got: " + 
                           std::to_string(connectionProbability);
        return result;
    }
    
    // Validate individual parameters
    for (const auto& key : config->getKeys()) {
        ValidationResult paramResult = validateParameter(key, config->getValue(key));
        if (!paramResult.isValid) {
            result.isValid = false;
            result.errorMessage = "Invalid configuration parameter '" + key + "': " + 
                               paramResult.errorMessage;
            return result;
        }
        
        // Merge warnings
        for (const auto& warning : paramResult.warnings) {
            result.warnings.push_back("(Parameter '" + key + "') " + warning);
        }
    }
    
    NLM_LOG_INFO("Configuration validation successful");
    return result;
}

void ConfigHelper::logConfigurationSummary(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot log configuration: config object is null");
        return;
    }
    
    NLM_LOG_INFO("=== NLM Configuration Summary ===");
    NLM_LOG_INFO("Loaded " + std::to_string(config->getKeys().size()) + " configuration parameters");
    
    for (const auto& key : config->getKeys()) {
        std::string valueStr;
        auto value = config->getValue(key);
        
        try {
            if (std::holds_alternative<int>(value)) {
                valueStr = std::to_string(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                valueStr = std::to_string(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                valueStr = std::to_string(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                valueStr = std::get<bool>(value) ? "true" : "false";
            } else if (std::holds_alternative<std::string>(value)) {
                valueStr = std::get<std::string>(value);
            } else {
                valueStr = "UNKNOWN_TYPE";
            }
            
            NLM_LOG_INFO("  " + key + " = " + valueStr + 
                        " (Source: " + std::to_string(static_cast<int>(config->getSource(key))) + ")");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to log configuration parameter '" + key + "': ") + e.what());
        }
    }
    
    NLM_LOG_INFO("=== End Configuration Summary ===");
}

void ConfigHelper::printConfigurationHelp() {
    NLM_LOG_INFO("=== NLM Configuration Help ===");
    NLM_LOG_INFO("Usage: ./nlm [options]");
    NLM_LOG_INFO("\nAvailable Configuration Options:");
    NLM_LOG_INFO("  --config=<file>    Load configuration from file");
    NLM_LOG_INFO("  --config <file>    Load configuration from file");
    NLM_LOG_INFO("  --set <key> <value> Set configuration key");
    NLM_LOG_INFO("  --help, -help       Show this help message");
    NLM_LOG_INFO("  --version           Show version information");
    NLM_LOG_INFO("  --neuron_count <v>  Set number of neurons");
    NLM_LOG_INFO("  --region_count <v>  Set number of regions");
    NLM_LOG_INFO("  --log_level <v>    Set log level (Debug, Info, Warning, Error, Critical)");
    NLM_LOG_INFO("  --random_seed <v>   Set random seed");
    NLM_LOG_INFO("\nFor full parameter list: use --help-all");
}

ConfigHelper::ValidationResult ConfigHelper::validateParameter(const std::string& key, const ConfigValue& value) {
    ValidationResult result;
    
    try {
        if (key == "neuron_count" || key == "region_count" || key == "random_seed" || 
            key == "max_simulation_steps" || key == "replay_interval" || 
            key == "consolidation_interval") {
            
            // Integer parameters
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                if (val < 0) {
                    result.isValid = false;
                    result.errorMessage = "must be non-negative";
                }
            } else if (std::holds_alternative<int64_t>(value)) {
                int64_t val = std::get<int64_t>(value);
                if (val < 0) {
                    result.isValid = false;
                    result.errorMessage = "must be non-negative";
                }
            }
        } 
        else if (key == "simulation_timestep" || key == "stdp_time_constant" || 
                key == "reward_discount_factor" || key == "plasticity_learning_rate") {
            
            // Floating point parameters
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (val < 0.0) {
                    result.isValid = false;
                    result.errorMessage = "must be non-negative";
                }
            }
        }
        
        else if (key == "connection_probability" || key == "plasticity_learning_rate") {
            
            // Probability parameters (0-1)
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (val < 0.0 || val > 1.0) {
                    result.isValid = false;
                    result.errorMessage = "must be in [0, 1]";
                }
            }
        }
        
        else if (key == "log_level") {
            
            // Log level parameter
            if (std::holds_alternative<std::string>(value)) {
                std::string val = std::get<std::string>(value);
                if (val != "Debug" && val != "Info" && val != "Warning" && 
                    val != "Error" && val != "Critical") {
                    result.isValid = false;
                    result.errorMessage = "invalid log level (must be: Debug, Info, Warning, Error, Critical)";
                }
            }
        }
        
    } catch (const std::bad_variant_access& e) {
        result.isValid = false;
        result.errorMessage = "type mismatch";
    }
    
    return result;
}

void ConfigHelper::applyCommandLineOverrides(int argc, char** argv, 
                                           std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot apply command-line overrides: config object is null");
        return;
    }
    
    NLM_LOG_INFO("Applying command-line configuration overrides...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --set command
        if (arg.substr(0, 5) == "--set") {
            if (i + 2 < argc) {
                std::string key = argv[++i];
                std::string value = argv[++i];
                
                NLM_LOG_DEBUG(std::string("Command-line override: ") + key + " = " + value);
                
                try {
                    config->set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Failed to apply command-line override for key '" + 
                                            key + "': " + e.what()));
                }
            } else {
                NLM_LOG_ERROR("Missing arguments for --set command");
            }
        }
        
        // Handle parameter-specific overrides
        else if (arg.substr(0, 13) == "--neuron_count") {
            size_t pos = arg.find('=');
            std::string value;
            if (pos != std::string::npos) {
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            
            if (!value.empty()) {
                try {
                    int64_t neuronCount = std::stoll(value);
                    NLM_LOG_INFO(std::string("Command-line override: neuron_count = ") + value);
                    config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Invalid neuron_count value ") + value + ": " + e.what());
                }
            }
        }
        
        else if (arg.substr(0, 12) == "--region_count") {
            size_t pos = arg.find('=');
            std::string value;
            if (pos != std::string::npos) {
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            
            if (!value.empty()) {
                try {
                    int64_t regionCount = std::stoll(value);
                    NLM_LOG_INFO(std::string("Command-line override: region_count = ") + value);
                    config->set("region_count", regionCount, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Invalid region_count value ") + value + ": " + e.what());
                }
            }
        }
        
        else if (arg.substr(0, 11) == "--log_level") {
            size_t pos = arg.find('=');
            std::string value;
            if (pos != std::string::npos) {
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            
            if (!value.empty()) {
                LogLevel level;
                if (value == "Debug") level = LogLevel::Debug;
                else if (value == "Info") level = LogLevel::Info;
                else if (value == "Warning") level = LogLevel::Warning;
                else if (value == "Error") level = LogLevel::Error;
                else if (value == "Critical") level = LogLevel::Critical;
                else {
                    NLM_LOG_ERROR(std::string("Invalid log level: ") + value);
                    continue;
                }
                
                // Note: Log level is handled by the main application, not config
                NLM_LOG_INFO(std::string("Command-line override: log_level = ") + value);
            }
        }
        
        else if (arg.substr(0, 11) == "--random_seed") {
            size_t pos = arg.find('=');
            std::string value;
            if (pos != std::string::npos) {
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            
            if (!value.empty()) {
                try {
                    int64_t seed = std::stoll(value);
                    NLM_LOG_INFO(std::string("Command-line override: random_seed = ") + value);
                    config->set("random_seed", seed, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Invalid random_seed value ") + value + ": " + e.what());
                }
            }
        }
    }
    
    NLM_LOG_INFO("Command-line override application complete");
}

void ConfigHelper::showConfigurationLoadError(const std::string& configFilePath, 
                                           const std::string& error) {
    NLM_LOG_ERROR(std::string("Failed to load configuration file: ") + configFilePath);
    NLM_LOG_ERROR(std::string("Error: ") + error);
    
    if (!std::filesystem::exists(configFilePath)) {
        NLM_LOG_DEBUG(std::string("Configuration file does not exist: ") + configFilePath);
        NLM_LOG_INFO("Creating default configuration...");
    } else {
        NLM_LOG_DEBUG(std::string("Configuration file exists but cannot be read: ") + configFilePath);
        NLM_LOG_INFO("Check file permissions and format.");
    }
}