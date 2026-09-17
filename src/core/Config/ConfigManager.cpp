// NLM Configuration Manager Implementation
// Provides unified configuration management with command-line argument parsing,
// file loading, validation, and easy access to configuration values.

#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace nlm {

ConfigManager::ConfigManager() : config_(nullptr), initialized_(false) {
    config_ = std::make_shared<Config>();
    initializeDefaults();
}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadConfig(const std::string& configFile) {
    if (!configFileExists(configFile)) {
        NLM_LOG_WARNING("Configuration file not found: " + configFile);
        return false;
    }
    
    loadedConfigFiles.push_back(configFile);
    
    if (config_->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to parse configuration file: " + configFile);
        return false;
    }
}

void ConfigManager::initializeDefaults() {
    if (initialized_) return;
    
    setDefaultValues();
    initialized_ = true;
    
    NLM_LOG_INFO("Initialized default configuration");
}

void ConfigManager::resetToDefaults() {
    config_ = std::make_shared<Config>();
    initialized_ = false;
    loadedConfigFiles.clear();
    initializeDefaults();
    
    NLM_LOG_INFO("Reset configuration to defaults");
}

bool ConfigManager::parseCommandLine(int argc, char* argv[]) {
    if (argc <= 1) return true;
    
    applyCommandLineArgs(argc, argv);
    
    if (validateConfig()) {
        NLM_LOG_INFO("Configuration validation passed");
        return true;
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
        return false;
    }
}

void ConfigManager::applyCommandLineArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --help flag
        if (arg == "--help") {
            printConfigHelp(argv[0]);
            exit(0);
        }
        
        // Handle --verbose flag
        else if (arg == "--verbose") {
            setValue("verbose", true, ConfigSource::CommandLine);
        }
        
        // Handle --loglevel option
        else if (arg.find("--loglevel=") == 0) {
            std::string level = arg.substr(10);
            setLogLevel(level);
        }
        
        // Handle --loglevel with space
        else if (arg == "--loglevel" && i + 1 < argc) {
            std::string level = argv[++i];
            setLogLevel(level);
        }
        
        // Handle other options with equals sign
        else if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t eqPos = arg.find("=");
            std::string key = arg.substr(2, eqPos - 2);
            std::string value = arg.substr(eqPos + 1);
            
            applyConfigEntry(key, value);
        }
        
        // Handle options without equals sign (simple boolean flags)
        else if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            // Convert dashes to underscores for consistency
            std::replace(key.begin(), key.end(), '-', '_');
            setValue(key, true, ConfigSource::CommandLine);
        }
    }
}

void ConfigManager::setLogLevel(const std::string& level) {
    LogLevel logLevel;
    
    if (level == "DEBUG" || level == "debug") logLevel = LogLevel::Debug;
    else if (level == "INFO" || level == "info") logLevel = LogLevel::Info;
    else if (level == "WARNING" || level == "warning") logLevel = LogLevel::Warning;
    else if (level == "ERROR" || level == "error") logLevel = LogLevel::Error;
    else {
        NLM_LOG_WARNING("Unknown log level: " + level + ", defaulting to INFO");
        logLevel = LogLevel::Info;
    }
    
    // Note: This would need access to the global logger
    // For now, we'll store it as a config value
    setValue("log_level", static_cast<int>(logLevel), ConfigSource::CommandLine);
}

void ConfigManager::applyConfigEntry(const std::string& key, const std::string& value) {
    try {
        // Try to parse as boolean
        if (value == "1" || value == "true" || value == "yes" || value == "on") {
            setValue(key, true, ConfigSource::CommandLine);
        } else if (value == "0" || value == "false" || value == "no" || value == "off") {
            setValue(key, false, ConfigSource::CommandLine);
        }
        // Try to parse as integer
        else {
            size_t pos;
            int intValue = std::stoi(value, &pos);
            if (pos == value.length()) {
                setValue(key, intValue, ConfigSource::CommandLine);
            } else {
                // Try to parse as float
                float floatValue = std::stof(value, &pos);
                if (pos == value.length()) {
                    setValue(key, floatValue, ConfigSource::CommandLine);
                } else {
                    // Fall back to string
                    setValue(key, value, ConfigSource::CommandLine);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse config entry " + key + " = " + value + ": " + e.what());
    }
}

bool ConfigManager::validateConfig() const {
    // Validate critical configuration values
    bool valid = true;
    
    // Check that neuron_count is positive
    int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
        valid = false;
    }
    
    // Check that simulation_timestep is positive
    double timestep = config_->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep));
        valid = false;
    }
    
    // Check that connection_probability is between 0 and 1
    float connectionProb = config_->getOr<float>("connection_probability", 0.0f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProb));
        valid = false;
    }
    
    // Check that stdp parameters are reasonable
    float stdpLtp = config_->getOr<float>("stdp_ltp_weight", 0.0f);
    float stdpLtd = config_->getOr<float>("stdp_ltd_weight", 0.0f);
    if (stdpLtp < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: " + std::to_string(stdpLtp));
        valid = false;
    }
    if (stdpLtd < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltd_weight: " + std::to_string(stdpLtd));
        valid = false;
    }
    
    return valid;
}

std::string ConfigManager::getConfigSummary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary" << std::endl;
    ss << "=========================" << std::endl;
    ss << "Loaded from: ";
    for (size_t i = 0; i < loadedConfigFiles.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << loadedConfigFiles[i];
    }
    if (loadedConfigFiles.empty()) ss << "Defaults only";
    ss << std::endl;
    
    ss << "\nKey Configuration Values:" << std::endl;
    ss << "  neuron_count: " << config_->getOr<int64_t>("neuron_count", 0) << std::endl;
    ss << "  region_count: " << config_->getOr<int64_t>("region_count", 0) << std::endl;
    ss << "  connection_probability: " << config_->getOr<float>("connection_probability", 0.0f) << std::endl;
    ss << "  simulation_timestep: " << config_->getOr<double>("simulation_timestep", 0.0) << std::endl;
    ss << "  random_seed: " << config_->getOr<int64_t>("random_seed", 0) << std::endl;
    
    return ss.str();
}

void ConfigManager::printConfigSummary(std::ostream& out) const {
    out << getConfigSummary() << std::endl;
}

void ConfigManager::printConfigHelp(const char* programName) {
    std::cout << "=== NLM Configuration Manager ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --verbose                    Enable verbose logging" << std::endl;
    std::cout << "  --loglevel <level>          Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --config <file>             Load configuration from file" << std::endl;
    std::cout << "  --help                      Show this help message" << std::endl;
    std::cout << "  --neuron_count <value>      Number of neurons" << std::endl;
    std::cout << "  --region_count <value>      Number of brain regions" << std::endl;
    std::cout << "  --connection_probability <value> Connection probability (0-1)" << std::endl;
    std::cout << "  --simulation_timestep <value> Simulation timestep (seconds)" << std::endl;
    std::cout << "  --random_seed <value>       Random seed for reproducibility" << std::endl;
    std::cout << "  --stdp_ltp_weight <value>    STDP LTP weight factor" << std::endl;
    std::cout << "  --stdp_ltd_weight <value>    STDP LTD weight factor" << std::endl;
    std::cout << "  --stdp_time_constant <value> STDP time constant" << std::endl;
    std::cout << "  --synaptogenesis_rate <value> Structural synaptogenesis rate" << std::endl;
    std::cout << "  --pruning_rate <value>      Structural pruning rate" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verbose --neuron_count 1000" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg" << std::endl;
    std::cout << "  " << programName << " --connection_probability 0.15 --random_seed 12345" << std::endl;
}

bool ConfigManager::shouldShowHelp(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            return true;
        }
    }
    return false;
}

bool ConfigManager::hasOption(int argc, char* argv[], const std::string& option) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--" + option || arg == "-" + option.substr(0, 1)) {
            return true;
        }
    }
    return false;
}

std::string ConfigManager::getOptionValue(int argc, char* argv[], 
                                        const std::string& option,
                                        const std::string& defaultValue) {
    return getOptionValueHelper(argc, argv, option, defaultValue);
}

std::string ConfigManager::getOptionWithArg(int argc, char* argv[], 
                                           const std::string& option,
                                           const std::string& defaultValue) {
    return getOptionValueHelper(argc, argv, option, defaultValue);
}

std::string ConfigManager::getOptionValueHelper(int argc, char* argv[], 
                                               const std::string& option,
                                               const std::string& defaultValue) {
    std::string prefix = "--" + option + "=";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find(prefix) == 0) {
            return arg.substr(prefix.length());
        }
    }
    
    // Check for --option value format
    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if (arg == "--" + option || arg == "-" + option.substr(0, 1)) {
            return argv[i + 1];
        }
    }
    
    return defaultValue;
}

bool ConfigManager::applyFileConfig(const std::string& configFile) {
    return loadConfig(configFile);
}

void ConfigManager::applySystemDefaults() {
    // Apply system-specific defaults
    setDefaultValues();
}

bool ConfigManager::saveConfig(const std::string& configFile) const {
    // Note: This would require additional implementation in the Config class
    NLM_LOG_WARNING("saveConfig not yet implemented");
    return false;
}

std::vector<std::string> ConfigManager::getAvailableConfigFiles() {
    std::vector<std::string> configFiles;
    
    // Add standard configuration files
    std::string currentDir = "./configs";
    if (std::filesystem::exists(currentDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".cfg" || 
                 entry.path().extension() == ".conf" ||
                 entry.path().extension() == ".txt")) {
                configFiles.push_back(entry.path().string());
            }
        }
    }
    
    return configFiles;
}

bool ConfigManager::configFileExists(const std::string& configFile) {
    std::ifstream file(configFile);
    return file.good();
}

ConfigManager::ConfigStats ConfigManager::getStats() const {
    ConfigStats stats;
    stats.totalEntries = 0;  // Would need to be implemented in Config class
    stats.fileLoaded = loadedConfigFiles.size();
    stats.commandLineApplied = 0;  // Would need to track this
    stats.defaultsApplied = initialized_ ? 1 : 0;
    stats.memoryUsage = 0.0f;  // Would need to measure this
    
    return stats;
}

void ConfigManager::setDefaultValues() {
    // Set default configuration values
    setValue("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    setValue("simulation_timestep", 0.001, ConfigSource::Default);
    setValue("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    setValue("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    setValue("connection_probability", 0.1f, ConfigSource::Default);
    setValue("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    setValue("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    setValue("stdp_time_constant", 20.0, ConfigSource::Default);
    setValue("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
    setValue("development_pruning_rate", 0.0001f, ConfigSource::Default);
    setValue("dopamine_baseline", 0.1f, ConfigSource::Default);
    setValue("reward_discount_factor", 0.99f, ConfigSource::Default);
    setValue("environment_width", 10, ConfigSource::Default);
    setValue("environment_height", 10, ConfigSource::Default);
    setValue("log_level", static_cast<int>(LogLevel::Info), ConfigSource::Default);
    setValue("log_to_file", false, ConfigSource::Default);
    setValue("log_filename", "nlm.log", ConfigSource::Default);
    setValue("max_simulation_steps", 10000, ConfigSource::Default);
    setValue("visualization_enabled", false, ConfigSource::Default);
    setValue("visualization_update_rate", 30.0f, ConfigSource::Default);
    setValue("verbose", false, ConfigSource::Default);
    setValue("trace_plasticity", false, ConfigSource::Default);
    setValue("enable_hebbian", true, ConfigSource::Default);
    setValue("enable_structural", true, ConfigSource::Default);
}

bool ConfigManager::validateEntry(const std::string& key, const std::string& value) const {
    // Basic validation for common configuration keys
    if (key == "neuron_count") {
        try {
            int64_t count = std::stoll(value);
            return count > 0;
        } catch (...) {
            return false;
        }
    } else if (key == "connection_probability") {
        try {
            float prob = std::stof(value);
            return prob >= 0.0f && prob <= 1.0f;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return timestep > 0.0;
        } catch (...) {
            return false;
        }
    }
    return true;  // Unknown keys pass through
}

} // namespace nlm