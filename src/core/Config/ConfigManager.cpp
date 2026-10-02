// Enhanced configuration management for NLM
// Provides better CLI options, validation, and utility functions

#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>

namespace nlm {

bool ConfigManager::loadConfig(const std::string& configFile, 
                              int argc, char** argv) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Clear previous configuration
    clearConfig();
    
    // Create new config object
    config_ = std::make_shared<Config>();
    
    // Load from file
    bool fileLoaded = config_->loadFromFile(configFile);
    
    // Load from command line arguments
    bool argsLoaded = false;
    if (argc > 0 && argv) {
        argsLoaded = config_->loadFromArgs(argc, argv);
    }
    
    // Apply command line overrides
    if (argsLoaded) {
        parseCommandLineArgs(argc, argv);
    }
    
    // Validate configuration
    validateConfig();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats_.configLoadTimeMs = static_cast<double>(duration.count());
    
    // Reset change tracking
    configChanges_.clear();
    lastChangeTime_ = std::chrono::steady_clock::now();
    
    return fileLoaded || argsLoaded;
}

bool ConfigManager::parseCommandLineArgs(int argc, char** argv) {
    bool parsed = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert value to appropriate type
                ConfigValue configValue;
                if (tryParseConfigValue(value, configValue)) {
                    setConfig(key, configValue, ConfigSource::CommandLine);
                    parsed = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            ConfigValue configValue;
            if (tryParseConfigValue(value, configValue)) {
                setConfig(key, configValue, ConfigSource::CommandLine);
                parsed = true;
            }
        }
    }
    
    return parsed;
}

bool ConfigManager::validateConfig() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("No configuration loaded");
        return false;
    }
    
    // Validate using registered rules
    for (const auto& rule : validationRules_) {
        if (!config_->has(rule.key)) {
            continue; // Skip if key doesn't exist
        }
        
        auto valueOpt = config_->get<ConfigValue>(rule.key);
        if (!valueOpt) {
            validationErrors_.push_back("Invalid value type for key: " + rule.key);
            continue;
        }
        
        // Create a temporary ConfigEntry for validation
        ConfigEntry entry(rule.key, *valueOpt, ConfigSource::Runtime);
        
        if (!validateSingleConfigEntry(entry)) {
            validationErrors_.push_back(rule.errorMessage);
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    return validationErrors_;
}

template<typename T>
T ConfigManager::getConfig(const std::string& key, const T& defaultValue) const {
    return config_->getOr<T>(key, defaultValue);
}

template<typename T>
bool ConfigManager::setConfig(const std::string& key, const T& value, ConfigSource source) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    ConfigValue oldValue;
    ConfigValue newValue = value;
    
    // Store old value for change tracking
    if (config_->has(key)) {
        oldValue = *config_->get<ConfigValue>(key);
    }
    
    // Set new value
    config_->set(key, value, source);
    
    // Track change
    if (oldValue != newValue) {
        trackConfigChange(key, oldValue, newValue);
    }
    
    return true;
}

std::string ConfigManager::getConfigSummary() const {
    if (!config_) {
        return "No configuration loaded";
    }
    
    std::ostringstream oss;
    oss << "=== NLM Configuration Summary ===" << std::endl;
    oss << "Total entries: " << config_->getKeys().size() << std::endl;
    
    // Count by source
    size_t fileEntries = 0, commandLineEntries = 0, defaultEntries = 0, runtimeEntries = 0;
    for (const auto& key : config_->getKeys()) {
        auto entryOpt = config_->get<ConfigValue>(key);
        if (!entryOpt) continue;
        
        // We need to find the source - this requires accessing the internal structure
        // For now, we'll just count total
    }
    
    oss << std::endl;
    oss << "Configuration entries:" << std::endl;
    for (const auto& key : config_->getKeys()) {
        auto valueOpt = config_->get<ConfigValue>(key);
        if (!valueOpt) {
            oss << "  " << key << " = [INVALID]" << std::endl;
            continue;
        }
        
        oss << "  " << key << " = " << ConfigUtils::configValueToString(*valueOpt) << std::endl;
    }
    
    return oss.str();
}

bool ConfigManager::saveConfig(const std::string& filepath) const {
    if (!config_) return false;
    return config_->saveToFile(filepath);
}

bool ConfigManager::exportConfig(const std::string& filepath, bool includeComments) const {
    if (!config_) return false;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (includeComments) {
        file << "# NLM Configuration Export" << std::endl;
        file << "# Generated on: " << getCurrentTimeString() << std::endl;
        file << std::endl;
    }
    
    for (const auto& key : config_->getKeys()) {
        auto valueOpt = config_->get<ConfigValue>(key);
        if (!valueOpt) continue;
        
        if (includeComments) {
            file << "# " << key << std::endl;
        }
        
        file << key << " = " << ConfigUtils::configValueToString(*valueOpt) << std::endl;
        file << std::endl;
    }
    
    return true;
}

std::string ConfigManager::diffConfigs(const ConfigManager& other) const {
    if (!config_ || !other.config_) {
        return "Cannot diff: one or both configurations are empty";
    }
    
    std::ostringstream oss;
    oss << "=== Configuration Diff ===" << std::endl;
    
    // Get all keys from both configs
    auto keys1 = config_->getKeys();
    auto keys2 = other.config_->getKeys();
    
    std::unordered_set<std::string> allKeys;
    for (const auto& key : keys1) allKeys.insert(key);
    for (const auto& key : keys2) allKeys.insert(key);
    
    bool hasDifferences = false;
    
    for (const auto& key : allKeys) {
        auto value1 = config_->get<ConfigValue>(key);
        auto value2 = other.config_->get<ConfigValue>(key);
        
        if (!value1 && !value2) {
            continue; // Both missing
        } else if (!value1) {
            oss << "+ " << key << " = " << ConfigUtils::configValueToString(*value2) << std::endl;
            hasDifferences = true;
        } else if (!value2) {
            oss << "- " << key << " = " << ConfigUtils::configValueToString(*value1) << std::endl;
            hasDifferences = true;
        } else if (*value1 != *value2) {
            oss << "* " << key << std::endl;
            oss << "  - " << ConfigUtils::configValueToString(*value1) << std::endl;
            oss << "  + " << ConfigUtils::configValueToString(*value2) << std::endl;
            hasDifferences = true;
        }
    }
    
    if (!hasDifferences) {
        oss << "No differences found" << std::endl;
    }
    
    return oss.str();
}

bool ConfigManager::backupConfig(const std::string& backupDir) {
    if (!config_) return false;
    
    // Create backup directory if it doesn't exist
    if (!std::filesystem::exists(backupDir)) {
        std::filesystem::create_directories(backupDir);
    }
    
    // Generate backup filename with timestamp
    std::string timestamp = getCurrentTimeString();
    std::replace(timestamp.begin(), timestamp.end(), ':', '-');
    std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
    
    std::string backupFile = backupDir + "/nlm_config_" + timestamp + ".cfg";
    
    // Save configuration
    if (exportConfig(backupFile, true)) {
        oss << "Configuration backed up to: " << backupFile << std::endl;
        return true;
    }
    
    return false;
}

bool ConfigManager::restoreConfig(const std::string& backupFile) {
    if (!std::filesystem::exists(backupFile)) {
        return false;
    }
    
    clearConfig();
    return loadConfig(backupFile);
}

bool ConfigManager::loadPresetConfig(const std::string& presetName) {
    auto presets = getDefaultPresets();
    
    auto it = presets.find(presetName);
    if (it == presets.end()) {
        return false;
    }
    
    return applyConfigTemplate(presetName, it->second);
}

std::vector<std::string> ConfigManager::getAvailablePresets() const {
    auto presets = getDefaultPresets();
    std::vector<std::string> names;
    for (const auto& pair : presets) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::startConfigMonitoring(double intervalSeconds) {
    monitoringActive_ = true;
    monitoringThread_ = std::thread(&ConfigManager::monitoringThreadFunc, this);
}

void ConfigManager::stopConfigMonitoring() {
    monitoringActive_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

std::vector<std::string> ConfigManager::getConfigChanges() const {
    return configChanges_;
}

ConfigManager::ConfigStats ConfigManager::getConfigStats() const {
    if (!config_) {
        return {0, 0, 0, 0, 0.0, 0};
    }
    
    ConfigStats stats;
    stats.totalEntries = config_->getKeys().size();
    
    // Estimate based on source (simplified)
    stats.fileEntries = 0; // Would need to check each entry's source
    stats.commandLineEntries = 0;
    stats.defaultEntries = 0;
    stats.configLoadTimeMs = stats_.configLoadTimeMs;
    stats.memoryUsage = sizeof(ConfigManager) + (config_ ? sizeof(Config) * config_->getKeys().size() : 0);
    
    return stats;
}

bool ConfigManager::addConfigTemplate(const std::string& name, const std::string& description,
                                     const std::unordered_map<std::string, std::string>& defaults) {
    // This would require persistent storage of templates
    // For now, we'll just validate and apply
    return applyConfigTemplate(name, defaults);
}

bool ConfigManager::applyConfigTemplate(const std::string& name) {
    auto presets = getDefaultPresets();
    auto it = presets.find(name);
    if (it == presets.end()) {
        return false;
    }
    
    return applyConfigTemplate(name, it->second);
}

std::vector<std::string> ConfigManager::getConfigTemplates() const {
    auto presets = getDefaultPresets();
    std::vector<std::string> names;
    for (const auto& pair : presets) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::addValidationRule(const ValidationRule& rule) {
    validationRules_.push_back(rule);
}

void ConfigManager::clearValidationRules() {
    validationRules_.clear();
}

void ConfigManager::addConfigChangeCallback(const ConfigChangeCallback& callback) {
    changeCallbacks_.push_back(callback);
}

void ConfigManager::removeConfigChangeCallback(const ConfigChangeCallback& callback) {
    changeCallbacks_.erase(std::remove(changeCallbacks_.begin(), changeCallbacks_.end(), callback),
                          changeCallbacks_.end());
}

bool ConfigManager::applyConfigTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& defaults) {
    clearConfig();
    config_ = std::make_shared<Config>();
    
    bool success = true;
    for (const auto& pair : defaults) {
        ConfigValue value;
        if (tryParseConfigValue(pair.second, value)) {
            config_->set(pair.first, value, ConfigSource::Default);
        } else {
            // Try as string
            config_->set(pair.first, pair.second, ConfigSource::Default);
            success = false;
        }
    }
    
    validateConfig();
    return success;
}

bool ConfigManager::validateSingleConfigEntry(const ConfigEntry& entry) const {
    // Basic validation
    if (entry.key.empty()) {
        return false;
    }
    
    // Type-specific validation
    if (std::holds_alternative<int>(entry.value)) {
        int val = std::get<int>(entry.value);
        if (entry.key == "neuron_count" && (val <= 0 || val > 100000)) return false;
        if (entry.key == "region_count" && (val <= 0 || val > 100)) return false;
        if (entry.key == "random_seed" && (val < 0 || val > UINT64_MAX)) return false;
    } else if (std::holds_alternative<int64_t>(entry.value)) {
        int64_t val = std::get<int64_t>(entry.value);
        if (entry.key == "neuron_count" && (val <= 0 || val > 100000)) return false;
        if (entry.key == "region_count" && (val <= 0 || val > 100)) return false;
    } else if (std::holds_alternative<double>(entry.value)) {
        double val = std::get<double>(entry.value);
        if (entry.key == "simulation_timestep" && (val <= 0.0 || val > 1.0)) return false;
        if (entry.key == "stdp_ltp_weight" && (val < 0.0 || val > 1.0)) return false;
        if (entry.key == "stdp_ltd_weight" && (val < 0.0 || val > 1.0)) return false;
        if (entry.key == "stdp_time_constant" && (val <= 0.0 || val > 1000.0)) return false;
    } else if (std::holds_alternative<float>(entry.value)) {
        float val = std::get<float>(entry.value);
        if (entry.key == "connection_probability" && (val < 0.0f || val > 1.0f)) return false;
        if (entry.key == "synaptogenesis_rate" && (val < 0.0f || val > 1.0f)) return false;
        if (entry.key == "pruning_rate" && (val < 0.0f || val > 1.0f)) return false;
    }
    
    return true;
}

bool ConfigManager::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool ConfigManager::isBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

bool ConfigManager::isInteger(const std::string& str) {
    if (str.empty() || (str[0] == '-' && str.size() > 1)) return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); });
}

bool ConfigManager::isFloat(const std::string& str) {
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool ConfigManager::isScientificNotation(const std::string& str) {
    // Check for scientific notation format (e.g., 1.23e-4)
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        return pos == str.size() && (str.find('e') != std::string::npos || str.find('E') != std::string::npos);
    } catch (...) {
        return false;
    }
}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ConfigManager::getDefaultPresets() {
    return {
        {"fast_debug", {
            {"random_seed", "42"},
            {"neuron_count", "100"},
            {"region_count", "1"},
            {"connection_probability", "0.2"},
            {"stdp_ltp_weight", "0.05"},
            {"stdp_ltd_weight", "0.06"},
            {"stdp_time_constant", "10.0"},
            {"synaptogenesis_rate", "0.01"},
            {"pruning_rate", "0.001"},
            {"log_level", "DEBUG"},
            {"visualization_enabled", "true"},
            {"max_simulation_steps", "1000"}
        }},
        {"production", {
            {"random_seed", "42"},
            {"neuron_count", "5000"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"stdp_time_constant", "20.0"},
            {"synaptogenesis_rate", "0.0001"},
            {"pruning_rate", "0.00001"},
            {"log_level", "INFO"},
            {"visualization_enabled", "false"},
            {"max_simulation_steps", "100000"}
        }},
        {"research", {
            {"random_seed", "12345"},
            {"neuron_count", "10000"},
            {"region_count", "4"},
            {"connection_probability", "0.15"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.018"},
            {"stdp_time_constant", "15.0"},
            {"synaptogenesis_rate", "0.001"},
            {"pruning_rate", "0.0001"},
            {"log_level", "INFO"},
            {"visualization_enabled", "true"},
            {"max_simulation_steps", "50000"}
        }},
        {"minimal", {
            {"random_seed", "42"},
            {"neuron_count", "50"},
            {"region_count", "1"},
            {"connection_probability", "0.3"},
            {"stdp_ltp_weight", "0.05"},
            {"stdp_ltd_weight", "0.06"},
            {"stdp_time_constant", "10.0"},
            {"log_level", "ERROR"},
            {"visualization_enabled", "false"},
            {"max_simulation_steps", "100"}
        }}
    };
}

void ConfigManager::trackConfigChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    std::ostringstream oss;
    oss << "Config changed: " << key << std::endl;
    oss << "  Old: " << ConfigUtils::configValueToString(oldValue) << std::endl;
    oss << "  New: " << ConfigUtils::configValueToString(newValue) << std::endl;
    oss << "  Time: " << getCurrentTimeString() << std::endl;
    
    configChanges_.push_back(oss.str());
    
    // Call callbacks
    for (const auto& callback : changeCallbacks_) {
        try {
            callback(key, oldValue, newValue);
        } catch (const std::exception& e) {
            // Log error but continue
        }
    }
}

void ConfigManager::monitoringThreadFunc() {
    while (monitoringActive_) {
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(10.0)));
        
        // Check for changes by monitoring file modification time
        // For now, just a placeholder
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastChangeTime_);
        
        if (duration.count() >= 30) {
            // Check if config has been modified
            // This would require file monitoring
        }
    }
}

// CommandLineOptions implementation functions
CommandLineOptions ConfigUtils::parseCommandLineArgs(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.customOptions["help"] = "true";
        } else if (arg == "--version") {
            options.customOptions["version"] = "true";
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--debug" || arg == "-d") {
            options.debug = true;
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.logLevel = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.substr(0, 7) == "--config" && arg.find('=') != std::string::npos) {
            options.configFile = arg.substr(arg.find('=') + 1);
        } else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options.customOptions[key] = value;
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            // Handle single-letter flags
            for (size_t j = 1; j < arg.size(); ++j) {
                options.customOptions[std::string(1, arg[j])] = "true";
            }
        } else {
            // Unknown argument, add to experiment args
            options.experimentArgs.push_back(arg);
        }
    }
    
    return options;
}

std::shared_ptr<Config> ConfigUtils::createConfigFromOptions(const CommandLineOptions& options) {
    auto config = std::make_shared<Config>();
    
    // Set basic options
    if (!options.configFile.empty()) {
        config->set("config_file", options.configFile, ConfigSource::CommandLine);
    }
    
    if (!options.logLevel.empty()) {
        config->set("log_level", options.logLevel, ConfigSource::CommandLine);
    }
    
    config->set("verbose", options.verbose, ConfigSource::CommandLine);
    config->set("debug", options.debug, ConfigSource::CommandLine);
    config->set("quiet", options.quiet, ConfigSource::CommandLine);
    
    // Set custom options
    for (const auto& pair : options.customOptions) {
        if (pair.first != "help" && pair.first != "version") {
            config->set(pair.first, pair.second, ConfigSource::CommandLine);
        }
    }
    
    return config;
}

bool ConfigUtils::validateExperimentConfig(const Config& config, const std::string& experimentType) {
    // Basic validation based on experiment type
    if (experimentType == "plasticity") {
        if (!config.has("neuron_count") || config.getOr<int64_t>("neuron_count", 0) < 10) {
            return false;
        }
        if (!config.has("max_simulation_steps") || config.getOr<int64_t>("max_simulation_steps", 0) < 100) {
            return false;
        }
    } else if (experimentType == "learning") {
        if (!config.has("plasticity_learning_rate") || config.getOr<double>("plasticity_learning_rate", 0.0) <= 0.0) {
            return false;
        }
    }
    
    return true;
}

bool ConfigUtils::validateSimulationConfig(const Config& config) {
    if (!config.has("neuron_count") || config.getOr<int64_t>("neuron_count", 0) <= 0) {
        return false;
    }
    
    if (!config.has("simulation_timestep") || config.getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        return false;
    }
    
    if (!config.has("region_count") || config.getOr<int64_t>("region_count", 0) <= 0) {
        return false;
    }
    
    if (config.has("connection_probability")) {
        float prob = config.getOr<float>("connection_probability", 0.0f);
        if (prob < 0.0f || prob > 1.0f) return false;
    }
    
    return true;
}

bool ConfigUtils::validateVisualizationConfig(const Config& config) {
    if (!config.has("visualization_enabled")) return false;
    
    bool enabled = config.getOr<bool>("visualization_enabled", false);
    if (enabled && !config.has("visualization_update_rate")) return false;
    
    if (config.has("visualization_update_rate")) {
        double rate = config.getOr<double>("visualization_update_rate", 0.0);
        if (rate <= 0.0) return false;
    }
    
    return true;
}

template<typename T>
T ConfigUtils::getRequiredConfig(const Config& config, const std::string& key) {
    auto value = config.get<T>(key);
    if (!value) {
        throw std::runtime_error("Missing required configuration: " + key);
    }
    return *value;
}

void ConfigUtils::mergeConfigs(Config& target, const Config& source, ConfigSource sourcePriority) {
    for (const auto& key : source.getKeys()) {
        auto value = source.get<ConfigValue>(key);
        if (value) {
            target.set(key, *value, sourcePriority);
        }
    }
}

std::string ConfigUtils::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, float>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "[unknown]";
        }
    }, value);
    
    return oss.str();
}

bool ConfigUtils::tryParseConfigValue(const std::string& str, ConfigValue& value) {
    std::string trimmed = Config::trim(str);
    
    // Try boolean
    if (isBoolean(trimmed)) {
        value = (toLower(trimmed) == "true" || toLower(trimmed) == "1");
        return true;
    }
    
    // Try integer
    if (isInteger(trimmed)) {
        value = std::stoi(trimmed);
        return true;
    }
    
    // Try int64_t
    if (isInteger(trimmed)) {
        try {
            size_t pos;
            long long val = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = static_cast<int64_t>(val);
                return true;
            }
        } catch (...) {}
    }
    
    // Try float/double
    if (isFloat(trimmed) || isScientificNotation(trimmed)) {
        try {
            size_t pos;
            float val = std::stof(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = val;
                return true;
            }
        } catch (...) {}
    }
    
    // Try double
    if (isFloat(trimmed) || isScientificNotation(trimmed)) {
        try {
            size_t pos;
            double val = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = val;
                return true;
            }
        } catch (...) {}
    }
    
    // Default to string
    value = trimmed;
    return true;
}

// Helper function implementations
std::string ConfigManager::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

// Explicit template instantiations for ConfigManager
template int ConfigManager::getConfig<int>(const std::string&, const int&) const;
template int64_t ConfigManager::getConfig<int64_t>(const std::string&, const int64_t&) const;
template double ConfigManager::getConfig<double>(const std::string&, const double&) const;
template float ConfigManager::getConfig<float>(const std::string&, const float&) const;
template bool ConfigManager::getConfig<bool>(const std::string&, const bool&) const;
template std::string ConfigManager::getConfig<std::string>(const std::string&, const std::string&) const;

// Explicit template instantiations for ConfigUtils
template int ConfigUtils::getRequiredConfig<int>(const Config&, const std::string&);
template int64_t ConfigUtils::getRequiredConfig<int64_t>(const Config&, const std::string&);
template double ConfigUtils::getRequiredConfig<double>(const Config&, const std::string&);
template float ConfigUtils::getRequiredConfig<float>(const Config&, const std::string&);
template bool ConfigUtils::getRequiredConfig<bool>(const Config&, const std::string&);
template std::string ConfigUtils::getRequiredConfig<std::string>(const Config&, const std::string&);

} // namespace nlm
