// Config.cpp
// Enhanced implementation of configuration system

#include "Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <regex>
#include <nlohmann/json.hpp>

namespace nlm {

// Config constructor
Config::Config() 
    : validationCount_(0)
    , errorCount_(0)
    , lastModified_(std::chrono::system_clock::now()) {
    
    // Load default configuration
    const auto& defaults = getDefaultValues();
    for (const auto& [key, value] : defaults) {
        config_[key] = ConfigEntry(value, ConfigSource::DEFAULT);
    }
    
    logWithContext("Configuration system initialized", LogLevel::Info);
}

// Move constructor
Config::Config(Config&& other) noexcept
    : config_(std::move(other.config_))
    , validationCount_(other.validationCount_)
    , errorCount_(other.errorCount_)
    , lastModified_(other.lastModified_) {
    other.validationCount_ = 0;
    other.errorCount_ = 0;
}

// Move assignment operator
Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        validationCount_ = other.validationCount_;
        errorCount_ = other.errorCount_;
        lastModified_ = other.lastModified_;
        other.validationCount_ = 0;
        other.errorCount_ = 0;
    }
    return *this;
}

Config::~Config() {
    logWithContext("Configuration system destroyed", LogLevel::Info);
}

bool Config::loadFromFile(const std::string& filename) {
    logWithContext("Loading configuration from file: " + filename, LogLevel::Info);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        logWithContext("ERROR: Cannot open configuration file: " + filename, LogLevel::Error);
        return false;
    }
    
    try {
        bool success = parseConfigFile(filename);
        if (success) {
            logWithContext("Configuration loaded successfully from: " + filename, LogLevel::Info);
            lastModified_ = std::chrono::system_clock::now();
        }
        return success;
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR loading configuration file: ") + e.what(), LogLevel::Error);
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    logWithContext("Loading configuration from command line arguments", LogLevel::Debug);
    
    try {
        bool success = parseCommandLineArgs(argc, argv);
        if (success) {
            logWithContext("Command line configuration loaded successfully", LogLevel::Debug);
            lastModified_ = std::chrono::system_clock::now();
        }
        return success;
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR loading command line configuration: ") + e.what(), LogLevel::Error);
        return false;
    }
}

bool Config::set(const std::string& key, int64_t value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = " + std::to_string(value) + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::set(const std::string& key, float value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = " + std::to_string(value) + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = " + std::to_string(value) + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = " + value + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = " + (value ? "true" : "false") + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::set(const std::string& key, const std::vector<float>& value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    if (!validateKey(normalizedKey) || !validateValue(normalizedKey, ConfigValue(value))) {
        logWithContext("ERROR: Invalid configuration key or value for: " + key, LogLevel::Error);
        errorCount_++;
        return false;
    }
    
    config_[normalizedKey] = ConfigEntry(ConfigValue(value), source, "Set via API");
    lastModified_ = std::chrono::system_clock::now();
    logWithContext("Configuration set: " + key + " = vector of size " + std::to_string(value.size()) + " (source: " + configSourceToString(source) + ")", LogLevel::Debug);
    return true;
}

bool Config::exists(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    return config_.find(normalizedKey) != config_.end();
}

ConfigSource Config::getSource(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end()) {
        return it->second.source;
    }
    return ConfigSource::DEFAULT;
}

std::chrono::system_clock::time_point Config::getTimestamp(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end()) {
        return it->second.timestamp;
    }
    return std::chrono::system_clock::time_point();
}

std::string Config::getDescription(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end()) {
        return it->second.description;
    }
    return "";
}

int64_t Config::getOr(const std::string& key, int64_t defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asInt();
    }
    validationCount_++;
    return defaultValue;
}

float Config::getOr(const std::string& key, float defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asFloat();
    }
    validationCount_++;
    return defaultValue;
}

double Config::getOr(const std::string& key, double defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asDouble();
    }
    validationCount_++;
    return defaultValue;
}

const std::string& Config::getOr(const std::string& key, const std::string& defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asString();
    }
    validationCount_++;
    static std::string emptyDefault = "";
    return emptyDefault;
}

bool Config::getOr(const std::string& key, bool defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asBool();
    }
    validationCount_++;
    return defaultValue;
}

const std::vector<float>& Config::getOr(const std::string& key, const std::vector<float>& defaultValue) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid()) {
        validationCount_++;
        return it->second.value.asVectorFloat();
    }
    validationCount_++;
    static std::vector<float> emptyDefault;
    return emptyDefault;
}

int64_t Config::getInt(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isInt()) {
        validationCount_++;
        return it->second.value.asInt();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get integer from non-integer key: " + key, LogLevel::Error);
    return 0;
}

float Config::getFloat(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isFloat()) {
        validationCount_++;
        return it->second.value.asFloat();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get float from non-float key: " + key, LogLevel::Error);
    return 0.0f;
}

double Config::getDouble(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isDouble()) {
        validationCount_++;
        return it->second.value.asDouble();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get double from non-double key: " + key, LogLevel::Error);
    return 0.0;
}

const std::string& Config::getString(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isString()) {
        validationCount_++;
        return it->second.value.asString();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get string from non-string key: " + key, LogLevel::Error);
    static std::string emptyString = "";
    return emptyString;
}

bool Config::getBool(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isBool()) {
        validationCount_++;
        return it->second.value.asBool();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get bool from non-bool key: " + key, LogLevel::Error);
    return false;
}

const std::vector<float>& Config::getVectorFloat(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end() && it->second.value.isValid() && it->second.value.isVectorFloat()) {
        validationCount_++;
        return it->second.value.asVectorFloat();
    }
    errorCount_++;
    logWithContext("ERROR: Attempted to get vector from non-vector key: " + key, LogLevel::Error);
    static std::vector<float> emptyVector;
    return emptyVector;
}

bool Config::validate() const {
    logWithContext("Validating configuration", LogLevel::Debug);
    size_t validationErrors = 0;
    
    for (const auto& [key, entry] : config_) {
        if (!entry.value.isValid()) {
            logWithContext("ERROR: Invalid configuration value for key: " + key, LogLevel::Error);
            validationErrors++;
        }
    }
    
    validationCount_++;
    if (validationErrors > 0) {
        errorCount_ += validationErrors;
        logWithContext(std::to_string(validationErrors) + " configuration validation errors found", LogLevel::Error);
        return false;
    }
    
    logWithContext("Configuration validation successful", LogLevel::Info);
    return true;
}

std::string Config::validateReport() const {
    std::ostringstream report;
    report << "=== Configuration Validation Report ===" << std::endl;
    report << "Total entries: " << config_.size() << std::endl;
    report << "Validation attempts: " << validationCount_ << std::endl;
    report << "Validation errors: " << errorCount_ << std::endl;
    
    size_t validEntries = 0;
    size_t invalidEntries = 0;
    
    for (const auto& [key, entry] : config_) {
        if (entry.value.isValid()) {
            validEntries++;
            report << "✓ " << key << " (" << entry.value.typeName() << ") - VALID" << std::endl;
        } else {
            invalidEntries++;
            report << "✗ " << key << " - INVALID" << std::endl;
        }
    }
    
    report << "Valid entries: " << validEntries << std::endl;
    report << "Invalid entries: " << invalidEntries << std::endl;
    report << "Validation score: " << (validEntries * 100.0 / std::max(config_.size(), 1)) << "%" << std::endl;
    
    return report.str();
}

std::vector<std::string> Config::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(config_.size());
    for (const auto& pair : config_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> Config::getKeysOfType(ConfigType type) const {
    std::vector<std::string> keys;
    
    for (const auto& [key, entry] : config_) {
        ConfigType currentType;
        if (entry.value.isInt()) currentType = ConfigType::INTEGER;
        else if (entry.value.isFloat()) currentType = ConfigType::FLOAT;
        else if (entry.value.isDouble()) currentType = ConfigType::DOUBLE;
        else if (entry.value.isString()) currentType = ConfigType::STRING;
        else if (entry.value.isBool()) currentType = ConfigType::BOOLEAN;
        else if (entry.value.isVectorFloat()) currentType = ConfigType::VECTOR_FLOAT;
        else continue;
        
        if (currentType == type) {
            keys.push_back(key);
        }
    }
    
    return keys;
}

size_t Config::getEntryCount() const {
    return config_.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& pair : config_) {
        if (pair.second.source == source) {
            count++;
        }
    }
    return count;
}

void Config::clear() {
    logWithContext("Clearing configuration", LogLevel::Info);
    config_.clear();
    previousValues_.clear();
    lastModified_ = std::chrono::system_clock::now();
}

bool Config::remove(const std::string& key) {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it != config_.end()) {
        config_.erase(it);
        lastModified_ = std::chrono::system_clock::now();
        logWithContext("Configuration removed: " + key, LogLevel::Debug);
        return true;
    }
    return false;
}

bool Config::hasChanged(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    auto it = config_.find(normalizedKey);
    if (it == config_.end()) {
        return false;
    }
    
    auto prevIt = previousValues_.find(normalizedKey);
    if (prevIt == previousValues_.end()) {
        return true;  // Changed from non-existent to existing
    }
    
    return prevIt->second.value.asInt() != it->second.value.asInt();
}

std::string Config::toJSON() const {
    nlohmann::json json;
    
    for (const auto& [key, entry] : config_) {
        if (entry.value.isInt()) {
            json[key] = entry.value.asInt();
        } else if (entry.value.isFloat()) {
            json[key] = entry.value.asFloat();
        } else if (entry.value.isDouble()) {
            json[key] = entry.value.asDouble();
        } else if (entry.value.isString()) {
            json[key] = entry.value.asString();
        } else if (entry.value.isBool()) {
            json[key] = entry.value.asBool();
        } else if (entry.value.isVectorFloat()) {
            json[key] = entry.value.asVectorFloat();
        }
    }
    
    return json.dump(2);
}

bool Config::fromJSON(const std::string& json) {
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::CONFIG_FILE);
            } else if (it->is_number_float()) {
                set(key, static_cast<float>(it->get<double>()), ConfigSource::CONFIG_FILE);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::CONFIG_FILE);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::CONFIG_FILE);
            } else if (it->is_array()) {
                std::vector<float> vec = it->get<std::vector<float>>();
                set(key, vec, ConfigSource::CONFIG_FILE);
            }
        }
        
        logWithContext("Configuration loaded from JSON successfully", LogLevel::Info);
        return true;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR parsing JSON configuration: ") + e.what(), LogLevel::Error);
        return false;
    }
}

double Config::getValidationScore() const {
    if (config_.empty()) return 0.0;
    
    size_t validCount = 0;
    for (const auto& entry : config_) {
        if (entry.second.value.isValid()) {
            validCount++;
        }
    }
    
    return static_cast<double>(validCount) / config_.size() * 100.0;
}

std::chrono::system_clock::time_point Config::getLastModified() const {
    return lastModified_;
}

void Config::logConfiguration() const {
    logWithContext("=== Current Configuration ===", LogLevel::Info);
    
    for (const auto& [key, entry] : config_) {
        std::string valueStr;
        if (entry.value.isInt()) {
            valueStr = std::to_string(entry.value.asInt());
        } else if (entry.value.isFloat()) {
            valueStr = std::to_string(entry.value.asFloat());
        } else if (entry.value.isDouble()) {
            valueStr = std::to_string(entry.value.asDouble());
        } else if (entry.value.isString()) {
            valueStr = entry.value.asString();
        } else if (entry.value.isBool()) {
            valueStr = entry.value.asBool() ? "true" : "false";
        } else if (entry.value.isVectorFloat()) {
            valueStr = "vector[" + std::to_string(entry.value.asVectorFloat().size()) + "]";
        }
        
        logWithContext(key + " = " + valueStr + " (source: " + configSourceToString(entry.source) + ") - " + entry.description, LogLevel::Info);
    }
}

void Config::logConfigurationChanges() const {
    if (!hasConfigurationChanged()) {
        logWithContext("No configuration changes detected", LogLevel::Debug);
        return;
    }
    
    logWithContext("=== Configuration Changes ===", LogLevel::Info);
    
    for (const auto& [key, entry] : config_) {
        auto prevIt = previousValues_.find(key);
        if (prevIt != previousValues_.end()) {
            logWithContext("Changed: " + key + " -> " + entry.value.typeName(), LogLevel::Info);
        }
    }
}

void Config::configureForPhase(int phase) {
    logWithContext("Configuring for Phase " + std::to_string(phase), LogLevel::Info);
    
    switch (phase) {
        case 1:
            // Phase 1: Basic functionality
            set("neuron_count", 1000, ConfigSource::DEFAULT);
            set("region_count", 2, ConfigSource::DEFAULT);
            set("connection_probability", 0.1, ConfigSource::DEFAULT);
            set("plasticity_learning_rate", 0.01, ConfigSource::DEFAULT);
            break;
            
        case 2:
            // Phase 2: Real neural computation
            set("neuron_count", 500, ConfigSource::DEFAULT);
            set("region_count", 1, ConfigSource::DEFAULT);
            set("connection_probability", 0.15f, ConfigSource::DEFAULT);
            set("stdp_ltp_weight", 0.02f, ConfigSource::DEFAULT);
            set("stdp_ltd_weight", 0.015f, ConfigSource::DEFAULT);
            set("stdp_tau", 20.0f, ConfigSource::DEFAULT);
            set("synaptogenesis_rate", 0.0001f, ConfigSource::DEFAULT);
            set("pruning_rate", 0.00001f, ConfigSource::DEFAULT);
            set("learning_rate", 0.01f, ConfigSource::DEFAULT);
            set("curiosity_threshold", 0.3f, ConfigSource::DEFAULT);
            set("exploration_rate", 0.1f, ConfigSource::DEFAULT);
            break;
            
        case 3:
            // Phase 3: Advanced features
            set("neuron_count", 2000, ConfigSource::DEFAULT);
            set("region_count", 3, ConfigSource::DEFAULT);
            set("connection_probability", 0.12f, ConfigSource::DEFAULT);
            set("working_memory_size", 100, ConfigSource::DEFAULT);
            set("episodic_memory_capacity", 1000, ConfigSource::DEFAULT);
            set("novelty_threshold", 0.5f, ConfigSource::DEFAULT);
            set("reward_prediction_error", true, ConfigSource::DEFAULT);
            break;
            
        default:
            logWithContext("Unknown phase: " + std::to_string(phase), LogLevel::Warning);
            break;
    }
}

void Config::configureForPerformance() {
    logWithContext("Configuring for optimal performance", LogLevel::Info);
    
    set("simulation_timestep", 0.001f, ConfigSource::DEFAULT);
    set("max_simulation_steps", 10000, ConfigSource::DEFAULT);
    set("neuron_count", 1000, ConfigSource::DEFAULT);
    set("log_level", "INFO", ConfigSource::DEFAULT);
    set("log_to_file", false, ConfigSource::DEFAULT);
}

void Config::configureForLearning() {
    logWithContext("Configuring for enhanced learning", LogLevel::Info);
    
    set("plasticity_learning_rate", 0.02f, ConfigSource::DEFAULT);
    set("stdp_ltp_weight", 0.025f, ConfigSource::DEFAULT);
    set("stdp_ltd_weight", 0.020f, ConfigSource::DEFAULT);
    set("stdp_time_constant", 25.0f, ConfigSource::DEFAULT);
    set("learning_rate", 0.02f, ConfigSource::DEFAULT);
    set("curiosity_threshold", 0.2f, ConfigSource::DEFAULT);
    set("exploration_rate", 0.15f, ConfigSource::DEFAULT);
    set("reward_discount_factor", 0.95f, ConfigSource::DEFAULT);
}

void Config::configureForDevelopment() {
    logWithContext("Configuring for developmental processes", LogLevel::Info);
    
    set("development_synaptogenesis_rate", 0.002f, ConfigSource::DEFAULT);
    set("development_pruning_rate", 0.0002f, ConfigSource::DEFAULT);
    set("maturation_period", 300.0, ConfigSource::DEFAULT);
    set("critical_period_start", 60.0, ConfigSource::DEFAULT);
    set("developmental_plasticity_modifier", 0.8f, ConfigSource::DEFAULT);
}

bool Config::equals(const Config& other) const {
    if (config_.size() != other.config_.size()) {
        return false;
    }
    
    for (const auto& [key, entry] : config_) {
        auto it = other.config_.find(key);
        if (it == other.config_.end()) {
            return false;
        }
        
        if (entry.value.isInt() && it->second.value.isInt()) {
            if (entry.value.asInt() != it->second.value.asInt()) {
                return false;
            }
        } else if (entry.value.isFloat() && it->second.value.isFloat()) {
            if (entry.value.asFloat() != it->second.value.asFloat()) {
                return false;
            }
        } else if (entry.value.isDouble() && it->second.value.isDouble()) {
            if (entry.value.asDouble() != it->second.value.asDouble()) {
                return false;
            }
        } else if (entry.value.isString() && it->second.value.isString()) {
            if (entry.value.asString() != it->second.value.asString()) {
                return false;
            }
        } else if (entry.value.isBool() && it->second.value.isBool()) {
            if (entry.value.asBool() != it->second.value.asBool()) {
                return false;
            }
        }
    }
    
    return true;
}

size_t Config::hash() const {
    size_t hash = std::hash<size_t>{} (config_.size());
    for (const auto& pair : config_) {
        hash ^= std::hash<std::string>{}(pair.first);
    }
    return hash;
}



} // namespace nlm

// Template instantiations for common types (to avoid linker errors)
namespace nlm {
    template std::optional<int64_t> Config::get<int64_t>(const std::string& key) const;
    template std::optional<float> Config::get<float>(const std::string& key) const;
    template std::optional<double> Config::get<double>(const std::string& key) const;
    template std::optional<std::string> Config::get<std::string>(const std::string& key) const;
    template std::optional<bool> Config::get<bool>(const std::string& key) const;
    template std::optional<std::vector<float>> Config::get<std::vector<float>>(const std::string& key) const;
    
    template int64_t Config::getOr<int64_t>(const std::string& key, int64_t defaultValue) const;
    template float Config::getOr<float>(const std::string& key, float defaultValue) const;
    template double Config::getOr<double>(const std::string& key, double defaultValue) const;
    template std::string Config::getOr<std::string>(const std::string& key, std::string defaultValue) const;
    template bool Config::getOr<bool>(const std::string& key, bool defaultValue) const;
    template std::vector<float> Config::getOr<std::vector<float>>(const std::string& key, std::vector<float> defaultValue) const;
}