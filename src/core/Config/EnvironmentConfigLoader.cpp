// NLM Environment Configuration Loader Implementation

#include "EnvironmentConfigLoader.hpp"
#include "Config.hpp"
#include "ConfigSchema.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>

namespace nlm {

// Environment variable prefix management
namespace {
    const std::string DEFAULT_ENVIRONMENT_PREFIX = "NLM_";
    std::string globalEnvironmentPrefix = DEFAULT_ENVIRONMENT_PREFIX;
    
    // Environment variable cache
    std::map<std::string, std::string> cachedEnvironmentVariables;
    bool environmentCacheEnabled = true;
    Timestamp environmentCacheTimestamp = 0.0;
    
    // Helper functions
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool isEnvironmentVariable(const std::string& key) {
        return std::getenv(key.c_str()) != nullptr;
    }
    
    std::string getEnvironmentVariable(const std::string& key) {
        const char* value = std::getenv(key.c_str());
        return value ? value : "";
    }
    
    std::string configKeyToEnvironmentVariable(const std::string& key) {
        // Convert configuration key to environment variable name
        std::string envVar = globalEnvironmentPrefix;
        
        // Replace dots and dashes with underscores
        for (char c : key) {
            if (c == '.' || c == '-') {
                envVar += '_';
            } else {
                envVar += c;
            }
        }
        
        // Convert to uppercase
        std::transform(envVar.begin(), envVar.end(), envVar.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        
        return envVar;
    }
    
    std::string environmentVariableToConfigKey(const std::string& envVar) {
        // Convert environment variable name to configuration key
        std::string key = envVar;
        
        // Remove prefix
        if (key.find(globalEnvironmentPrefix) == 0) {
            key = key.substr(globalEnvironmentPrefix.length());
        }
        
        // Convert to lowercase
        std::transform(key.begin(), key.end(), key.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        
        // Replace underscores with dots
        for (size_t i = 0; i < key.length(); ++i) {
            if (key[i] == '_') {
                key[i] = '.';
            }
        }
        
        return key;
    }
}

// EnvironmentConfigLoader implementation
EnvironmentConfigLoader::EnvironmentConfigLoader() : prefix_(DEFAULT_ENVIRONMENT_PREFIX) {}

EnvironmentConfigLoader::EnvironmentConfigLoader(const std::string& prefix) : prefix_(prefix) {}

EnvironmentConfigLoader::~EnvironmentConfigLoader() = default;

bool EnvironmentConfigLoader::loadEnvironmentConfig(std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    return loadEnvironmentConfig(config, std::vector<std::string>());
}

bool EnvironmentConfigLoader::loadEnvironmentConfig(std::shared_ptr<Config> config, 
                                                   const std::vector<std::string>& keys) {
    if (!config) {
        return false;
    }
    
    std::map<std::string, ConfigValue> envVars;
    
    if (keys.empty()) {
        // Load all environment variables with the prefix
        envVars = parseAllEnvironmentVariables();
    } else {
        // Load only specified environment variables
        envVars = parseSpecifiedEnvironmentVariables(keys);
    }
    
    // Apply environment variables to configuration
    for (const auto& pair : envVars) {
        config->set(pair.first, pair.second, ConfigSource::Environment);
    }
    
    return true;
}

bool EnvironmentConfigLoader::loadEnvironmentConfig(std::shared_ptr<Config> config, 
                                                   const ConfigSchema& schema) {
    if (!config) {
        return false;
    }
    
    // Get all environment variables that match the schema
    std::vector<std::string> matchingKeys = getMatchingKeys(schema);
    
    // Load and validate environment variables
    std::vector<ValidationError> errors = validateEnvironmentVariables(matchingKeys, schema);
    
    if (!errors.empty()) {
        // Report validation errors
        for (const auto& error : errors) {
            std::cerr << "Environment variable validation error: " << error.message << std::endl;
        }
        return false;
    }
    
    // Load environment variables
    std::map<std::string, ConfigValue> envVars = parseSpecifiedEnvironmentVariables(matchingKeys);
    
    // Apply to configuration
    for (const auto& pair : envVars) {
        config->set(pair.first, pair.second, ConfigSource::Environment);
    }
    
    return true;
}

void EnvironmentConfigLoader::setEnvironmentPrefix(const std::string& prefix) {
    prefix_ = prefix;
}

std::string EnvironmentConfigLoader::getEnvironmentPrefix() const {
    return prefix_;
}

std::vector<std::string> EnvironmentConfigLoader::getEnvironmentVariables() const {
    std::vector<std::string> envVars;
    
    // Get all environment variables
    for (const auto& pair : cachedEnvironmentVariables) {
        if (pair.first.find(prefix_) == 0) {
            envVars.push_back(pair.first);
        }
    }
    
    return envVars;
}

std::vector<std::string> EnvironmentConfigLoader::getMatchingEnvironmentVariables(const std::string& pattern) {
    std::vector<std::string> matchingVars;
    
    for (const auto& pair : cachedEnvironmentVariables) {
        if (pair.first.find(prefix_) == 0) {
            std::string varName = pair.first.substr(prefix_.length());
            
            // Simple pattern matching (supports * and ?)
            bool matches = true;
            for (size_t i = 0; i < pattern.size() && i < varName.size(); ++i) {
                if (pattern[i] != '*' && pattern[i] != '?' && 
                    std::tolower(pattern[i]) != std::tolower(varName[i])) {
                    matches = false;
                    break;
                }
            }
            
            if (matches && pattern.size() <= varName.size()) {
                matchingVars.push_back(pair.first);
            }
        }
    }
    
    return matchingVars;
}

bool EnvironmentConfigLoader::validateEnvironmentVariables(const std::vector<std::string>& keys) {
    std::vector<ValidationError> errors = validateEnvironmentVariables(keys, ConfigSchema());
    return errors.empty();
}

std::vector<ValidationError> EnvironmentConfigLoader::validateEnvironmentVariables(const std::vector<std::string>& keys, 
                                                                                 const ConfigSchema& schema) {
    std::vector<ValidationError> errors;
    
    for (const auto& key : keys) {
        std::string configKey = environmentVariableToConfigKey(key);
        
        if (schema.hasField(configKey)) {
            const SchemaField& field = schema.getField(configKey);
            std::string envValue = getEnvironmentVariable(key);
            
            // Validate type
            bool typeValid = validateEnvironmentVariableType(envValue, field.type);
            if (!typeValid) {
                errors.push_back({
                    "type",
                    "Type mismatch for environment variable " + key + ": expected " + 
                         typeToString(field.type) + ", got " + envValue,
                    configKey,
                    Severity::Error
                });
            }
            
            // Validate value
            bool valueValid = validateEnvironmentVariableValue(envValue, field);
            if (!valueValid) {
                errors.push_back({
                    "value",
                    "Value validation failed for environment variable " + key + ": " + envValue,
                    configKey,
                    Severity::Error
                });
            }
        }
    }
    
    return errors;
}

std::string EnvironmentConfigLoader::generateEnvironmentDocumentation(const std::vector<std::string>& keys) {
    std::ostringstream doc;
    
    doc << "Environment Variable Documentation for " << prefix_ << ":" << std::endl;
    doc << std::endl;
    
    for (const auto& key : keys) {
        std::string configKey = environmentVariableToConfigKey(key);
        std::string envValue = getEnvironmentVariable(key);
        
        doc << "Environment Variable: " << key << std::endl;
        doc << "  Configuration Key: " << configKey << std::endl;
        doc << "  Current Value: " << (envValue.empty() ? "(not set)" : envValue) << std::endl;
        doc << "  Description: " << getDescriptionForKey(configKey) << std::endl;
        doc << "  Type: " << getTypeForKey(configKey) << std::endl;
        doc << std::endl;
    }
    
    return doc.str();
}

std::string EnvironmentConfigLoader::generateEnvironmentDocumentation(const Config& config) {
    std::ostringstream doc;
    
    doc << "Environment Variable Documentation for " << prefix_ << ":" << std::endl;
    doc << std::endl;
    doc << "This file documents the environment variables that can be used to override "
        << "configuration settings for this application." << std::endl;
    doc << std::endl;
    
    // Get all configuration keys
    std::vector<std::string> keys = config.getKeys();
    
    for (const auto& key : keys) {
        std::string envVar = configKeyToEnvironmentVariable(key);
        
        doc << "Environment Variable: " << envVar << std::endl;
        doc << "  Configuration Key: " << key << std::endl;
        doc << "  Default Value: " << getConfigValueString(config, key) << std::endl;
        doc << "  Type: " << getTypeForKey(key) << std::endl;
        doc << "  Description: " << getDescriptionForKey(key) << std::endl;
        doc << std::endl;
    }
    
    return doc.str();
}

void EnvironmentConfigLoader::clearEnvironmentCache() {
    cachedEnvironmentVariables.clear();
    environmentCacheTimestamp = 0.0;
}

void EnvironmentConfigLoader::setEnvironmentCacheEnabled(bool enabled) {
    environmentCacheEnabled = enabled;
}

bool EnvironmentConfigLoader::isEnvironmentCacheEnabled() const {
    return environmentCacheEnabled;
}

// Helper methods implementation
std::map<std::string, std::string> EnvironmentConfigLoader::parseAllEnvironmentVariables() {
    std::map<std::string, std::string> envVars;
    
    // Update cache if enabled
    if (environmentCacheEnabled) {
        Timestamp currentTime = getCurrentTimestamp();
        if (currentTime - environmentCacheTimestamp > CACHE_REFRESH_INTERVAL) {
            updateEnvironmentCache();
            environmentCacheTimestamp = currentTime;
        }
    }
    
    // Filter and return variables with prefix
    for (const auto& pair : cachedEnvironmentVariables) {
        if (pair.first.find(prefix_) == 0) {
            envVars[pair.first] = pair.second;
        }
    }
    
    return envVars;
}

std::map<std::string, std::string> EnvironmentConfigLoader::parseSpecifiedEnvironmentVariables(const std::vector<std::string>& keys) {
    std::map<std::string, std::string> envVars;
    
    for (const auto& key : keys) {
        std::string envVar = configKeyToEnvironmentVariable(key);
        
        if (isEnvironmentVariable(envVar)) {
            envVars[envVar] = getEnvironmentVariable(envVar);
        }
    }
    
    return envVars;
}

std::map<std::string, ConfigValue> EnvironmentConfigLoader::parseToConfigValues(const std::map<std::string, std::string>& envVars) {
    std::map<std::string, ConfigValue> configValues;
    
    EnvironmentVariableParser parser;
    
    for (const auto& pair : envVars) {
        std::string configKey = environmentVariableToConfigKey(pair.first);
        ConfigValue value = parser.parseEnvironmentVariable(configKey, pair.second);
        configValues[configKey] = value;
    }
    
    return configValues;
}

void EnvironmentConfigLoader::updateEnvironmentCache() {
    cachedEnvironmentVariables.clear();
    
    // Get all environment variables
    for (char** env = std::getenv("PATH"); *env != nullptr; ++env) {
        std::string var = *env;
        size_t pos = var.find('=');
        if (pos != std::string::npos && pos + 1 < var.length()) {
            std::string key = var.substr(0, pos);
            std::string value = var.substr(pos + 1);
            cachedEnvironmentVariables[key] = value;
        }
    }
}

std::vector<std::string> EnvironmentConfigLoader::getMatchingKeys(const ConfigSchema& schema) {
    std::vector<std::string> matchingKeys;
    
    for (const auto& field : schema.getFields()) {
        std::string envVar = configKeyToEnvironmentVariable(field.key);
        if (isEnvironmentVariable(envVar)) {
            matchingKeys.push_back(field.key);
        }
    }
    
    return matchingKeys;
}

std::string EnvironmentConfigLoader::getDescriptionForKey(const std::string& key) const {
    // Return hardcoded descriptions for common keys
    if (key == "simulation.neuron_count") {
        return "Number of neurons in the brain";
    } else if (key == "simulation.timestep") {
        return "Simulation timestep in seconds";
    } else if (key == "plasticity.learning_rate") {
        return "Plasticity learning rate";
    } else if (key == "environment.name") {
        return "Environment name";
    } else if (key == "environment.width") {
        return "Environment width in grid cells";
    } else if (key == "environment.height") {
        return "Environment height in grid cells";
    }
    
    return "Configuration parameter";
}

std::string EnvironmentConfigLoader::getTypeForKey(const std::string& key) const {
    // Return hardcoded types for common keys
    if (key == "simulation.neuron_count") {
        return "int64_t";
    } else if (key == "simulation.timestep") {
        return "double";
    } else if (key == "plasticity.learning_rate") {
        return "float";
    } else if (key == "environment.name") {
        return "std::string";
    } else if (key == "environment.width" || key == "environment.height") {
        return "int";
    }
    
    return "unknown";
}

std::string EnvironmentConfigLoader::getConfigValueString(const Config& config, const std::string& key) const {
    auto value = config.get<std::string>(key);
    return value.has_value() ? value.value() : "(not set)";
}

bool EnvironmentConfigLoader::validateEnvironmentVariableType(const std::string& value, 
                                                                const std::type_info& type) {
    // Simple type validation
    if (type == typeid(int)) {
        return isInteger(value);
    } else if (type == typeid(int64_t)) {
        return isInteger(value);
    } else if (type == typeid(double)) {
        return isFloat(value);
    } else if (type == typeid(float)) {
        return isFloat(value);
    } else if (type == typeid(bool)) {
        return isBool(value);
    } else if (type == typeid(std::string)) {
        return true;  // Any string is valid
    }
    
    return false;
}

bool EnvironmentConfigLoader::validateEnvironmentVariableValue(const std::string& value,
                                                                const SchemaField& field) {
    // Range validation for numeric types
    if (field.type == typeid(int64_t)) {
        int64_t intValue = std::stoll(value);
        if (field.minValue.has_value() && intValue < field.minValue.value()) {
            return false;
        }
        if (field.maxValue.has_value() && intValue > field.maxValue.value()) {
            return false;
        }
    } else if (field.type == typeid(double)) {
        double doubleValue = std::stod(value);
        if (field.minValue.has_value() && doubleValue < field.minValue.value()) {
            return false;
        }
        if (field.maxValue.has_value() && doubleValue > field.maxValue.value()) {
            return false;
        }
    } else if (field.type == typeid(float)) {
        float floatValue = std::stof(value);
        if (field.minValue.has_value() && floatValue < field.minValue.value()) {
            return false;
        }
        if (field.maxValue.has_value() && floatValue > field.maxValue.value()) {
            return false;
        }
    }
    
    // Custom validation
    if (field.validator) {
        ConfigValue configValue;
        // Convert value to appropriate type
        if (field.type == typeid(int64_t)) {
            configValue = std::stoll(value);
        } else if (field.type == typeid(double)) {
            configValue = std::stod(value);
        } else if (field.type == typeid(float)) {
            configValue = std::stof(value);
        } else if (field.type == typeid(bool)) {
            configValue = (value == "true" || value == "1" || value == "yes");
        } else {
            configValue = value;
        }
        
        return field.validator(value, configValue);
    }
    
    return true;
}

bool EnvironmentConfigLoader::isInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-') {
        if (str.size() == 1) return false;
        pos = 1;
    }
    
    for (size_t i = pos; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool EnvironmentConfigLoader::isFloat(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-') {
        if (str.size() == 1) return false;
        pos = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = pos; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false;  // Multiple decimals
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool EnvironmentConfigLoader::isBool(const std::string& str) {
    std::string lower = toLower(str);
    return (lower == "true" || lower == "false" || lower == "1" || lower == "0" || 
            lower == "yes" || lower == "no" || lower == "on" || lower == "off");
}

Timestamp EnvironmentConfigLoader::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() / 1000.0;
}

// EnvironmentVariableParser implementation
EnvironmentVariableParser::EnvironmentVariableParser() = default;

EnvironmentVariableParser::~EnvironmentVariableParser() = default;

ConfigValue EnvironmentVariableParser::parseEnvironmentVariable(const std::string& key,
                                                                const std::string& value) {
    // Parse based on key pattern
    if (key.find("neuron_count") != std::string::npos || 
        key.find("count") != std::string::npos) {
        return std::stoll(value);
    } else if (key.find("timestep") != std::string::npos || 
               key.find("time") != std::string::npos ||
               key.find("delay") != std::string::npos) {
        return std::stod(value);
    } else if (key.find("learning_rate") != std::string::npos || 
               key.find("rate") != std::string::npos ||
               key.find("weight") != std::string::npos) {
        return std::stof(value);
    } else if (key.find("enabled") != std::string::npos || 
               key.find("active") != std::string::npos ||
               key.find("flag") != std::string::npos) {
        std::string lower = toLower(value);
        return (lower == "true" || lower == "1" || lower == "yes");
    } else {
        return value;  // Default to string
    }
}

std::map<std::string, ConfigValue> EnvironmentVariableParser::parseEnvironmentVariables(
    const std::vector<std::string>& keys, const std::type_info& type) {
    std::map<std::string, ConfigValue> result;
    
    for (const auto& key : keys) {
        result[key] = parseEnvironmentVariable(key, "");  // Will be populated later
    }
    
    return result;
}

std::map<std::string, ConfigValue> EnvironmentVariableParser::parseEnvironmentVariables(
    const std::vector<std::string>& keys, const ConfigSchema& schema) {
    std::map<std::string, ConfigValue> result;
    
    for (const auto& key : keys) {
        if (schema.hasField(key)) {
            const SchemaField& field = schema.getField(key);
            std::string envValue = getEnvironmentVariable(key);
            result[key] = parseEnvironmentVariable(envValue, field.type);
        }
    }
    
    return result;
}

template<typename T>
T EnvironmentVariableParser::convertToType(const std::string& value) {
    std::istringstream iss(value);
    T result;
    iss >> result;
    return result;
}

bool EnvironmentVariableParser::matchesKeyPattern(const std::string& key, const std::string& pattern) {
    return key == pattern || key.find(pattern) != std::string::npos;
}

std::vector<std::string> EnvironmentVariableParser::findMatchingEnvironmentVariables(const std::string& pattern) {
    std::vector<std::string> matchingVars;
    
    for (const auto& pair : cachedEnvironmentVariables) {
        if (matchesKeyPattern(pair.first, pattern)) {
            matchingVars.push_back(pair.first);
        }
    }
    
    return matchingVars;
}

bool EnvironmentVariableParser::validateEnvironmentVariable(const std::string& key,
                                                             const std::string& value,
                                                             const TypeInfo& typeInfo) {
    // Simple validation based on type
    if (typeInfo.name() == typeid(int).name()) {
        return isInteger(value);
    } else if (typeInfo.name() == typeid(double).name()) {
        return isFloat(value);
    } else if (typeInfo.name() == typeid(bool).name()) {
        return isBool(value);
    }
    
    return true;  // Default to valid for unknown types
}

std::vector<ValidationError> EnvironmentVariableParser::validateEnvironmentVariables(
    const std::map<std::string, std::string>& envVars, const ConfigSchema& schema) {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : envVars) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        if (schema.hasField(key)) {
            const SchemaField& field = schema.getField(key);
            bool valid = validateEnvironmentVariable(key, value, field.type);
            if (!valid) {
                errors.push_back({
                    "type",
                    "Type validation failed for environment variable " + key + ": " + value,
                    key,
                    Severity::Error
                });
            }
        }
    }
    
    return errors;
}

// EnvironmentConfigMonitor implementation
EnvironmentConfigMonitor::EnvironmentConfigMonitor() = default;

EnvironmentConfigMonitor::EnvironmentConfigMonitor(std::shared_ptr<Config> config) : config_(config) {}

EnvironmentConfigMonitor::~EnvironmentConfigMonitor() = default;

void EnvironmentConfigMonitor::startMonitoring() {
    isMonitoring_ = true;
    monitorThread_ = std::thread(&EnvironmentConfigMonitor::monitorEnvironment, this);
}

void EnvironmentConfigMonitor::stopMonitoring() {
    isMonitoring_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

bool EnvironmentConfigMonitor::isMonitoring() const {
    return isMonitoring_;
}

void EnvironmentConfigMonitor::addEnvironmentChangeCallback(const EnvironmentChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.push_back(callback);
}

void EnvironmentConfigMonitor::removeEnvironmentChangeCallback(const EnvironmentChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.erase(std::remove(callbacks_.begin(), callbacks_.end(), callback), callbacks_.end());
}

void EnvironmentConfigMonitor::clearEnvironmentChangeCallbacks() {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.clear();
}

std::vector<EnvironmentChange> EnvironmentConfigMonitor::getRecentChanges() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return recentChanges_;
}

void EnvironmentConfigMonitor::addRecentChange(const EnvironmentChange& change) {
    std::lock_guard<std::mutex> lock(mutex_);
    recentChanges_.push_back(change);
    
    // Keep only recent changes
    if (recentChanges_.size() > MAX_RECENT_CHANGES) {
        recentChanges_.erase(recentChanges_.begin());
    }
}

void EnvironmentConfigMonitor::updateConfigFromEnvironment() {
    if (!config_) return;
    
    // Get current environment variables
    auto environmentLoader = std::make_shared<EnvironmentConfigLoader>();
    
    // Parse environment variables
    std::vector<std::string> keys = getAllEnvironmentKeys();
    std::map<std::string, ConfigValue> envVars = environmentLoader->parseToConfigValues(
        environmentLoader->parseAllEnvironmentVariables()
    );
    
    // Apply changes
    for (const auto& pair : envVars) {
        config_->set(pair.first, pair.second, ConfigSource::Environment);
    }
}

bool EnvironmentConfigMonitor::hasEnvironmentConfigChanged() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return environmentConfigChanged_;
}

void EnvironmentConfigMonitor::setEnvironmentConfigChanged(bool changed) {
    std::lock_guard<std::mutex> lock(mutex_);
    environmentConfigChanged_ = changed;
}

void EnvironmentConfigMonitor::saveState() {
    std::lock_guard<std::mutex> lock(mutex_);
    savedState_ = config_;
}

void EnvironmentConfigMonitor::restoreState() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (savedState_) {
        config_ = savedState_;
    }
}

void EnvironmentConfigMonitor::clearHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    recentChanges_.clear();
    environmentConfigChanged_ = false;
}

void EnvironmentConfigMonitor::monitorEnvironment() {
    while (isMonitoring_) {
        // Check for environment variable changes
        checkForEnvironmentChanges();
        
        // Sleep for a short period
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void EnvironmentConfigMonitor::checkForEnvironmentChanges() {
    if (!config_) return;
    
    // Get current environment variables
    auto environmentLoader = std::make_shared<EnvironmentConfigLoader>();
    std::map<std::string, std::string> currentEnvVars = environmentLoader->parseAllEnvironmentVariables();
    
    // Get saved environment variables
    std::map<std::string, std::string> savedEnvVars = environmentVariables_;
    
    // Compare and detect changes
    bool changed = false;
    for (const auto& pair : currentEnvVars) {
        if (savedEnvVars.find(pair.first) == savedEnvVars.end() || 
            savedEnvVars[pair.first] != pair.second) {
            changed = true;
            break;
        }
    }
    
    for (const auto& pair : savedEnvVars) {
        if (currentEnvVars.find(pair.first) == currentEnvVars.end()) {
            changed = true;
            break;
        }
    }
    
    if (changed) {
        setEnvironmentConfigChanged(true);
        
        // Record changes
        EnvironmentChange change;
        change.timestamp = getCurrentTimestamp();
        change.type = EnvironmentChangeType::EnvironmentVariable;
        change.key = "environment_variables";
        change.oldValue = ""; // Simplified
        change.newValue = "";  // Simplified
        
        addRecentChange(change);
        
        // Notify callbacks
        notifyEnvironmentChangeCallbacks(change);
    }
}

void EnvironmentConfigMonitor::notifyEnvironmentChangeCallbacks(const EnvironmentChange& change) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& callback : callbacks_) {
        try {
            callback(change);
        } catch (const std::exception& e) {
            std::cerr << "Error in environment change callback: " << e.what() << std::endl;
        }
    }
}

std::vector<std::string> EnvironmentConfigMonitor::getAllEnvironmentKeys() {
    std::vector<std::string> keys;
    
    // Get all environment variables with prefix
    auto environmentLoader = std::make_shared<EnvironmentConfigLoader>();
    std::map<std::string, std::string> envVars = environmentLoader->parseAllEnvironmentVariables();
    
    for (const auto& pair : envVars) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

Timestamp EnvironmentConfigMonitor::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() / 1000.0;
}

// Helper functions
const std::string EnvironmentPrefixManager::DEFAULT_PREFIX = "NLM_";
std::string EnvironmentPrefixManager::globalEnvironmentPrefix = DEFAULT_PREFIX;

void EnvironmentPrefixManager::setGlobalPrefix(const std::string& prefix) {
    globalEnvironmentPrefix = prefix;
}

std::string EnvironmentPrefixManager::getGlobalPrefix() {
    return globalEnvironmentPrefix;
}

std::string EnvironmentPrefixManager::configKeyToEnvironmentVariable(const std::string& key) {
    std::string envVar = globalEnvironmentPrefix;
    
    for (char c : key) {
        if (c == '.' || c == '-') {
            envVar += '_';
        } else {
            envVar += c;
        }
    }
    
    std::transform(envVar.begin(), envVar.end(), envVar.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    return envVar;
}

std::string EnvironmentPrefixManager::environmentVariableToConfigKey(const std::string& envVar) {
    std::string key = envVar;
    
    if (key.find(globalEnvironmentPrefix) == 0) {
        key = key.substr(globalEnvironmentPrefix.length());
    }
    
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    for (size_t i = 0; i < key.length(); ++i) {
        if (key[i] == '_') {
            key[i] = '.';
        }
    }
    
    return key;
}

// EnvironmentVariableTypeConverter implementation
int EnvironmentVariableTypeConverter::convertToInt(const std::string& value) {
    return std::stoi(value);
}

int64_t EnvironmentVariableTypeConverter::convertToInt64(const std::string& value) {
    return std::stoll(value);
}

float EnvironmentVariableTypeConverter::convertToFloat(const std::string& value) {
    return std::stof(value);
}

double EnvironmentVariableTypeConverter::convertToDouble(const std::string& value) {
    return std::stod(value);
}

bool EnvironmentVariableTypeConverter::convertToBool(const std::string& value) {
    std::string lower = toLower(value);
    return (lower == "true" || lower == "1" || lower == "yes" || 
            lower == "on");
}

std::string EnvironmentVariableTypeConverter::convertToString(const std::string& value) {
    return value;
}

std::vector<int> EnvironmentVariableTypeConverter::convertToVectorInt(const std::string& value) {
    std::vector<int> result;
    std::istringstream iss(value);
    int num;
    char delim;
    
    while (iss >> num) {
        result.push_back(num);
        if (iss >> delim && delim != ',') {
            iss.putback(delim);
            break;
        }
    }
    
    return result;
}

std::vector<double> EnvironmentVariableTypeConverter::convertToVectorDouble(const std::string& value) {
    std::vector<double> result;
    std::istringstream iss(value);
    double num;
    char delim;
    
    while (iss >> num) {
        result.push_back(num);
        if (iss >> delim && delim != ',') {
            iss.putback(delim);
            break;
        }
    }
    
    return result;
}

std::vector<std::string> EnvironmentVariableTypeConverter::convertToVectorString(const std::string& value) {
    std::vector<std::string> result;
    std::istringstream iss(value);
    std::string token;
    
    while (iss >> token) {
        result.push_back(token);
    }
    
    return result;
}

bool EnvironmentVariableTypeConverter::isInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-') {
        if (str.size() == 1) return false;
        pos = 1;
    }
    
    for (size_t i = pos; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool EnvironmentVariableTypeConverter::isFloat(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-') {
        if (str.size() == 1) return false;
        pos = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = pos; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

// Constants
const size_t EnvironmentConfigMonitor::MAX_RECENT_CHANGES = 100;
const size_t EnvironmentConfigLoader::CACHE_REFRESH_INTERVAL = 60;  // seconds

}} // namespace nlm