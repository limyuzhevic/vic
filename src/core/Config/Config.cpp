#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>
#include <regex>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <random>
#include "Logger.hpp"
#include "NLMException.hpp"

namespace nlm {

using json = nlohmann::json;

// ConfigValidator implementation
struct ConfigValidator::Impl {
    std::map<std::string, ValidationRule> rules;
    int validationLevel;
    bool strictMode;
    
    Impl() : validationLevel(1), strictMode(false) {}
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
}

ConfigValidator::~ConfigValidator() {
}

void ConfigValidator::addValidationRule(const std::string& name, const ValidationRule& rule) {
    pImpl->rules[name] = rule;
}

void ConfigValidator::removeValidationRule(const std::string& name) {
    pImpl->rules.erase(name);
}

ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    
    for (const auto& [name, rule] : pImpl->rules) {
        if (!rule(config, result)) {
            if (pImpl->strictMode) {
                break; // Stop on first error in strict mode
            }
        }
    }
    
    return result;
}

std::vector<std::string> ConfigValidator::getRuleNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->rules.size());
    for (const auto& [name, rule] : pImpl->rules) {
        names.push_back(name);
    }
    return names;
}

void ConfigValidator::setValidationLevel(int level) {
    pImpl->validationLevel = level;
}

void ConfigValidator::setStrictMode(bool strict) {
    pImpl->strictMode = strict;
}

// ConfigRecovery implementation
struct ConfigRecovery::Impl {
    bool recoveryEnabled;
    RecoveryStrategy strategy;
    std::vector<std::string> checkpoints;
    std::map<std::string, Config> checkpointData;
    std::deque<ConfigChange> changeHistory;
    size_t maxHistorySize;
    
    Impl() : recoveryEnabled(true), strategy(RecoveryStrategy::AutoRevert), maxHistorySize(1000) {}
};

ConfigRecovery::ConfigRecovery() : pImpl(std::make_unique<Impl>()) {
}

ConfigRecovery::~ConfigRecovery() {
}

void ConfigRecovery::enableRecovery(bool enable) {
    pImpl->recoveryEnabled = enable;
}

bool ConfigRecovery::isRecoveryEnabled() const {
    return pImpl->recoveryEnabled;
}

void ConfigRecovery::setRecoveryStrategy(RecoveryStrategy strategy) {
    pImpl->strategy = strategy;
}

void ConfigRecovery::addCheckpoint(const std::string& name) {
    if (!pImpl->recoveryEnabled) {
        return;
    }
    
    // In a real implementation, this would save the current config state
    pImpl->checkpoints.push_back(name);
}

bool ConfigRecovery::restoreToCheckpoint(const std::string& name) {
    auto it = std::find(pImpl->checkpoints.begin(), pImpl->checkpoints.end(), name);
    if (it == pImpl->checkpoints.end()) {
        return false;
    }
    
    // In a real implementation, this would restore the checkpoint data
    return true;
}

std::vector<std::string> ConfigRecovery::getCheckpoints() const {
    return pImpl->checkpoints;
}

bool ConfigRecovery::attemptAutoRecovery(const ValidationResult& validationResult) {
    if (!pImpl->recoveryEnabled || validationResult.isValid) {
        return false;
    }
    
    switch (pImpl->strategy) {
        case RecoveryStrategy::AutoRevert:
            // Revert to last valid state
            if (!pImpl->changeHistory.empty()) {
                auto& lastChange = pImpl->changeHistory.back();
                lastChange.revert();
                return true;
            }
            break;
        
        case RecoveryStrategy::ManualReview:
            // Log error for manual intervention
            LoggerImpl::getGlobal().warning("Configuration validation failed - manual review required");
            break;
        
        case RecoveryStrategy::CheckpointBased:
            // Restore to most recent checkpoint
            if (!pImpl->checkpoints.empty()) {
                restoreToCheckpoint(pImpl->checkpoints.back());
                return true;
            }
            break;
        
        case RecoveryStrategy::None:
        default:
            break;
    }
    
    return false;
}

// Config::Impl implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChange> history;
    std::map<std::string, Config> templates;
    std::map<std::string, Config> presets;
    std::map<std::string, ValidationRule> validationRules;
    std::map<std::string, json> cachedJSON;
    mutable bool isLocked;
    
    // Debug and validation flags
    bool debugMode;
    bool validationMode;
    bool errorRecoveryMode;
    
    Impl() : isLocked(false), debugMode(false), validationMode(true), errorRecoveryMode(true) {
        // Initialize with some default validation rules
        registerDefaultValidationRules();
    }
    
    void registerDefaultValidationRules() {
        // Register some basic validation rules
        registerValidationRule("type_validation", [this](const Config& config, ValidationResult& result) {
            // Check type consistency
            for (const auto& entry : config.pImpl->entries) {
                if (!validateEntryType(entry, result)) {
                    return false;
                }
            }
            return true;
        });
        
        registerValidationRule("key_validation", [this](const Config& config, ValidationResult& result) {
            // Check key format
            for (const auto& entry : config.pImpl->entries) {
                if (!validateEntryKey(entry, result)) {
                    return false;
                }
            }
            return true;
        });
        
        registerValidationRule("value_validation", [this](const Config& config, ValidationResult& result) {
            // Check value ranges and constraints
            for (const auto& entry : config.pImpl->entries) {
                if (!validateEntryValue(entry, result)) {
                    return false;
                }
            }
            return true;
        });
    }
    
    bool validateEntryType(const ConfigEntry& entry, ValidationResult& result) {
        // Basic type validation based on key patterns
        if (entry.key.find("neuron_count") != std::string::npos) {
            if (!std::holds_alternative<int64_t>(entry.value) && 
                !std::holds_alternative<int>(entry.value)) {
                result.addError("neuron_count must be an integer: " + entry.key);
                return false;
            }
        } else if (entry.key.find("learning_rate") != std::string::npos) {
            if (!std::holds_alternative<double>(entry.value)) {
                result.addError("learning_rate must be a double: " + entry.key);
                return false;
            }
        } else if (entry.key.find("enabled") != std::string::npos) {
            if (!std::holds_alternative<bool>(entry.value)) {
                result.addError("enabled must be a boolean: " + entry.key);
                return false;
            }
        }
        return true;
    }
    
    bool validateEntryKey(const ConfigEntry& entry, ValidationResult& result) {
        if (entry.key.empty()) {
            result.addError("Empty configuration key");
            return false;
        }
        
        std::regex keyPattern("^[a-zA-Z0-9_.]+$");
        if (!std::regex_match(entry.key, keyPattern)) {
            result.addError("Invalid key format: " + entry.key);
            return false;
        }
        
        return true;
    }
    
    bool validateEntryValue(const ConfigEntry& entry, ValidationResult& result) {
        // Range validation for specific keys
        if (entry.key.find("neuron_count") != std::string::npos) {
            int64_t value = 0;
            if (std::holds_alternative<int64_t>(entry.value)) {
                value = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<int>(entry.value)) {
                value = std::get<int>(entry.value);
            }
            
            if (value <= 0 || value > 1000000) {
                result.addError("neuron_count out of valid range (1-1000000): " + std::to_string(value));
                return false;
            }
        } else if (entry.key.find("simulation_timestep") != std::string::npos) {
            double value = 0.0;
            if (std::holds_alternative<double>(entry.value)) {
                value = std::get<double>(entry.value);
            }
            
            if (value <= 0.0 || value > 1.0) {
                result.addError("simulation_timestep out of valid range (0-1.0): " + std::to_string(value));
                return false;
            }
        } else if (entry.key.find("connection_probability") != std::string::npos) {
            float value = 0.0f;
            if (std::holds_alternative<double>(entry.value)) {
                value = static_cast<float>(std::get<double>(entry.value));
            } else if (std::holds_alternative<int>(entry.value)) {
                value = static_cast<float>(std::get<int>(entry.value));
            }
            
            if (value < 0.0f || value > 1.0f) {
                result.addError("connection_probability out of valid range (0-1.0): " + std::to_string(value));
                return false;
            }
        }
        
        return true;
    }
    
    void registerValidationRule(const std::string& name, const ValidationRule& rule) {
        validationRules[name] = rule;
    }
    
    bool unregisterValidationRule(const std::string& name) {
        auto it = validationRules.find(name);
        if (it != validationRules.end()) {
            validationRules.erase(it);
            return true;
        }
        return false;
    }
    
    std::vector<std::string> getRegisteredValidationRules() const {
        std::vector<std::string> names;
        names.reserve(validationRules.size());
        for (const auto& [name, rule] : validationRules) {
            names.push_back(name);
        }
        return names;
    }
    
    bool isValueEqual(const ConfigValue& val1, const ConfigValue& val2) {
        return val1 == val2;
    }
    
    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << std::fixed << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << "?";
            }
        }, value);
        
        return oss.str();
    }
    
    json configValueToJSON(const ConfigValue& value) {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, int>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return json(arg);
            } else {
                return json(nullptr);
            }
        }, value);
    }
    
    double estimateMemoryUsage(const Config& config) {
        double totalSize = 0.0;
        
        for (const auto& entry : config.pImpl->entries) {
            // Estimate size based on value type
            std::visit([&totalSize](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    totalSize += arg.size() * sizeof(char);
                } else if constexpr (std::is_same_v<T, double>) {
                    totalSize += sizeof(double);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    totalSize += sizeof(int64_t);
                } else if constexpr (std::is_same_v<T, bool>) {
                    totalSize += sizeof(bool);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    totalSize += arg.size() * sizeof(int);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    totalSize += arg.size() * sizeof(double);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    for (const auto& str : arg) {
                        totalSize += str.size() * sizeof(char);
                    }
                }
            }, entry.value);
            
            totalSize += entry.key.size() * sizeof(char);
            totalSize += sizeof(ConfigSource);
            totalSize += sizeof(std::chrono::system_clock::time_point);
        }
        
        return totalSize;
    }
    
    std::string generateBackupFilename(const std::string& originalFile) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << originalFile;
        
        size_t dotPos = originalFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            ss << ".backup_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << originalFile.substr(dotPos);
        } else {
            ss << ".backup_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        }
        
        return ss.str();
    }
    
    bool createBackupDirectory(const std::string& filepath) {
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            return std::filesystem::create_directories(dir);
        }
        return true;
    }
    
    std::string generateSnapshotId() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "snapshot_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S_");
        
        // Add random component to ensure uniqueness
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000000);
        ss << dis(gen);
        
        return ss.str();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
}

Config::~Config() {
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, FileFormat::KeyValue);
}

bool Config::loadFromFile(const std::string& filepath, FileFormat format) {
    try {
        switch (format) {
            case FileFormat::KeyValue:
                return loadKeyValueFromFile(filepath);
            case FileFormat::JSON:
                return loadJSONFromFile(filepath);
            case FileFormat::YAML:
                // YAML not implemented
                return false;
            case FileFormat::TOML:
                // TOML not implemented
                return false;
        }
    } catch (const std::exception& e) {
        LoggerImpl::getGlobal().error("Failed to load configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
    return false;
}

bool Config::loadKeyValueFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadJSONFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Convert JSON to config
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            const json& value = it.value();
            
            // Recursively convert JSON values
            ConfigValue configValue;
            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Convert array based on element type
                if (!value.empty()) {
                    if (value[0].is_boolean()) {
                        std::vector<bool> vec;
                        for (const auto& item : value) {
                            vec.push_back(item.get<bool>());
                        }
                        configValue = vec;
                    } else if (value[0].is_number_integer()) {
                        std::vector<int64_t> vec;
                        for (const auto& item : value) {
                            vec.push_back(item.get<int64_t>());
                        }
                        configValue = vec;
                    } else if (value[0].is_number()) {
                        std::vector<double> vec;
                        for (const auto& item : value) {
                            vec.push_back(item.get<double>());
                        }
                        configValue = vec;
                    } else if (value[0].is_string()) {
                        std::vector<std::string> vec;
                        for (const auto& item : value) {
                            vec.push_back(item.get<std::string>());
                        }
                        configValue = vec;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        LoggerImpl::getGlobal().error("Failed to parse JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    
    for (const auto& entry : pImpl->entries) {
        std::string envKey = prefix + entry.key;
        const char* envValue = std::getenv(envKey.c_str());
        
        if (envValue != nullptr) {
            set(entry.key, std::string(envValue), ConfigSource::Environment);
            loaded = true;
        }
    }
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, FileFormat::KeyValue);
}

bool Config::saveToFile(const std::string& filepath, FileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case FileFormat::KeyValue:
            return saveAsKeyValue(file);
        case FileFormat::JSON:
            return saveAsJSON(file);
        case FileFormat::YAML:
            // YAML not implemented
            return false;
        case FileFormat::TOML:
            // TOML not implemented
            return false;
    }
    return false;
}

bool Config::saveAsKeyValue(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getConfigValueString(entry.value) << "\n\n";
    }
    
    return true;
}

bool Config::saveAsJSON(std::ofstream& file) const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJSON(entry.value);
    }
    
    file << j.dump(2);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::optional<ConfigValue> Config::getNested(const std::string& key) const {
    // For now, just return the regular value
    // In a full implementation, this would handle hierarchical keys
    return get<ConfigValue>(key);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->isLocked) {
        LoggerImpl::getGlobal().error("Configuration is locked. Cannot modify.");
        return;
    }
    
    std::string sanitizedKey = sanitizeKey(key);
    
    // Check if key already exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });n    
    if (it != pImpl->entries.end()) {
        // Key exists, update if value changed
        if (it->value != value || it->source != source) {
            ConfigChange change;
            change.key = sanitizedKey;
            change.oldValue = it->value;
            change.newValue = value;
            change.source = source;
            change.reason = "Updated via set()";
            
            it->value = value;
            it->source = source;
            it->timestamp = std::chrono::system_clock::now();
            
            pImpl->history.push_back(change);
            
            // Invalidate cached JSON
            pImpl->cachedJSON.clear();
        }
    } else {
        // New key, add it
        ConfigEntry entry(sanitizedKey, value, source);
        pImpl->entries.push_back(entry);
        
        ConfigChange change;
        change.key = sanitizedKey;
        change.oldValue = ConfigValue(); // Default value
        change.newValue = value;
        change.source = source;
        change.reason = "New key added via set()";
        
        pImpl->history.push_back(change);
        
        // Invalidate cached JSON
        pImpl->cachedJSON.clear();
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setNested(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Implementation for hierarchical configuration
    // This is a simplified version - full implementation would handle nested structures
    set(key, value, source);
}

bool Config::has(const std::string& key) const {
    std::string sanitizedKey = sanitizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });
}

void Config::remove(const std::string& key) {
    if (pImpl->isLocked) {
        LoggerImpl::getGlobal().error("Configuration is locked. Cannot modify.");
        return;
    }
    
    std::string sanitizedKey = sanitizeKey(key);
    
    // Find the entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });
    
    if (it != pImpl->entries.end()) {
        // Record change
        ConfigChange change;
        change.key = sanitizedKey;
        change.oldValue = it->value;
        change.newValue = ConfigValue(); // Default
        change.source = ConfigSource::Runtime;
        change.reason = "Removed via remove()";
        
        pImpl->history.push_back(change);
        
        // Remove the entry
        pImpl->entries.erase(it);
        
        // Invalidate cached JSON
        pImpl->cachedJSON.clear();
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::vector<std::string> Config::getNestedKeys(const std::string& parentKey) const {
    std::vector<std::string> nestedKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (parentKey.empty() || entry.key.find(parentKey + ".") == 0) {
            nestedKeys.push_back(entry.key);
        }
    }
    
    return nestedKeys;
}

void Config::clear() {
    if (pImpl->isLocked) {
        LoggerImpl::getGlobal().error("Configuration is locked. Cannot modify.");
        return;
    }
    
    // Record change
    ConfigChange change;
    change.key = "ALL";
    change.oldValue = ConfigValue(); // Represent clearing all
    change.newValue = ConfigValue(); // Represent clearing all
    change.source = ConfigSource::Runtime;
    change.reason = "Cleared via clear()";
    
    pImpl->history.push_back(change);
    
    pImpl->entries.clear();
    pImpl->cachedJSON.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << getConfigValueString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Apply registered validation rules
    for (const auto& [name, rule] : pImpl->validationRules) {
        if (!rule(*this, result)) {
            // Rule failed, result will contain error details
            if (pImpl->errorRecoveryMode) {
                // Attempt auto-recovery
                // Note: This would need access to recovery instance
                // For now, just log the error
                LoggerImpl::getGlobal().error("Configuration validation failed: " + result.errors.back());
            }
            break;
        }
    }
    
    return result;
}

bool Config::validate(const std::vector<ValidationRule>& customRules) const {
    ValidationResult result;
    
    // Apply registered validation rules
    for (const auto& [name, rule] : pImpl->validationRules) {
        if (!rule(*this, result)) {
            return false;
        }
    }
    
    // Apply custom validation rules
    for (const auto& rule : customRules) {
        if (!rule(*this, result)) {
            return false;
        }
    }
    
    return result.isValid;
}

ComparisonResult Config::compare(const Config& other) const {
    ComparisonResult result;
    
    // Compare entries in this config
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != other.pImpl->entries.end()) {
            if (entry.value != it->value) {
                result.differences[entry.key] = std::make_pair(entry.value, it->value);
            } else {
                result.identicalKeys.push_back(entry.key);
            }
            
            if (entry.source != it->source) {
                result.sourceConflicts[entry.key] = it->source;
            }
        } else {
            result.differences[entry.key] = std::make_pair(entry.value, ConfigValue());
        }
    }
    
    // Compare entries in other config
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            result.differences[entry.key] = std::make_pair(ConfigValue(), entry.value);
            result.onlyInSecond.push_back(entry.key);
        }
    }
    
    // Find keys only in first config
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            result.differences[entry.key] = std::make_pair(entry.value, ConfigValue());
            result.onlyInFirst.push_back(entry.key);
        }
    }
    
    return result;
}

bool Config::equals(const Config& other) const {
    return compare(other).differences.empty();
}

std::string Config::diff(const Config& other) const {
    ComparisonResult result = compare(other);
    
    std::ostringstream oss;
    oss << "=== Configuration Differences ===" << std::endl;
    
    if (result.differences.empty()) {
        oss << "No differences found." << std::endl;
        return oss.str();
    }
    
    for (const auto& [key, values] : result.differences) {
        oss << "Key: " << key << std::endl;
        oss << "  " << (std::get<0>(values) == ConfigValue() ? "MISSING" : "OLD: ") 
            << (std::get<0>(values) != ConfigValue() ? valueToString(std::get<0>(values)) : "") << std::endl;
        oss << "  " << (std::get<1>(values) == ConfigValue() ? "NEW" : "NEW: ") 
            << (std::get<1>(values) != ConfigValue() ? valueToString(std::get<1>(values)) : "") << std::endl;
    }
    
    if (result.sourceConflicts.empty()) {
        oss << "\nNo source conflicts." << std::endl;
    } else {
        oss << "\nSource conflicts:" << std::endl;
        for (const auto& [key, source] : result.sourceConflicts) {
            oss << "  " << key << " has conflicting sources" << std::endl;
        }
    }
    
    return oss.str();
}

std::vector<ConfigChange> Config::getHistory() const {
    return pImpl->history;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

Config Config::createDefaultConfig() {
    Config config;
    
    // Add default settings
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    
    // Add minimal settings
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    return config;
}

Config Config::createFromJSON(const std::string& jsonString) {
    Config config;
    
    try {
        json j = json::parse(jsonString);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            const json& value = it.value();
            
            ConfigValue configValue;
            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // For arrays, we'll use a simpler approach
                std::vector<double> vec;
                for (const auto& item : value) {
                    if (item.is_number()) {
                        vec.push_back(item.get<double>());
                    }
                }
                if (!vec.empty()) {
                    configValue = vec;
                }
            }
            
            config.set(key, configValue, ConfigSource::File);
        }
    } catch (const std::exception& e) {
        LoggerImpl::getGlobal().error("Failed to parse JSON configuration string: " + std::string(e.what()));
    }
    
    return config;
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    
    stats.totalEntries = pImpl->entries.size();
    
    // Count unique keys
    std::set<std::string> uniqueKeys;
    for (const auto& entry : pImpl->entries) {
        uniqueKeys.insert(entry.key);
    }
    stats.uniqueKeys = uniqueKeys.size();
    
    // Count sources
    for (const auto& entry : pImpl->entries) {
        stats.sourceCounts[entry.source]++;
    }
    
    stats.lastModified = std::chrono::system_clock::now();
    stats.memoryUsage = estimateMemoryUsage(*this);
    
    return stats;
}

std::string Config::exportToString() const {
    return summary();
}

bool Config::importFromString(const std::string& configString) {
    // Simple import from string format
    // This is a basic implementation - could be enhanced
    clear();
    
    std::istringstream iss(configString);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
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
            
            set(key, value, ConfigSource::Runtime);
        }
    }
    
    return true;
}

void Config::registerValidationRule(const std::string& name, const ValidationRule& rule) {
    pImpl->registerValidationRule(name, rule);
}

bool Config::unregisterValidationRule(const std::string& name) {
    return pImpl->unregisterValidationRule(name);
}

std::vector<std::string> Config::getRegisteredValidationRules() {
    return pImpl->getRegisteredValidationRules();
}

std::string Config::createSnapshot() {
    return pImpl->generateSnapshotId();
}

bool Config::restoreSnapshot(const std::string& snapshot) {
    // In a real implementation, this would restore a previously saved snapshot
    // For now, just log that it's not implemented
    LoggerImpl::getGlobal().warning("Snapshot restore not implemented yet: " + snapshot);
    return false;
}

void Config::logChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue,
                       ConfigSource source, const std::string& reason) {
    addToHistory(key, oldValue, newValue, source, reason);
}

bool Config::isValueEqual(const ConfigValue& val1, const ConfigValue& val2) {
    return val1 == val2;
}

std::string Config::valueToString(const ConfigValue& value) {
    return pImpl->valueToString(value);
}

void Config::lock() const {
    pImpl->isLocked = true;
}

void Config::unlock() const {
    pImpl->isLocked = false;
}

bool Config::isLocked() const {
    return pImpl->isLocked;
}

ValidationResult Config::validateKey(const std::string& key) {
    ValidationResult result;
    if (!isValidKey(key)) {
        result.addError("Invalid configuration key: " + key);
    }
    return result;
}

ValidationResult Config::validateValue(const ConfigValue& value, ValidationResult& result, const std::string& key) {
    // Validate value based on type
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            int64_t intVal = static_cast<int64_t>(arg);
            if (intVal <= 0) {
                result.addError("Value must be positive: " + std::to_string(intVal));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            double doubleVal = arg;
            if (doubleVal < 0.0) {
                result.addError("Value cannot be negative: " + std::to_string(doubleVal));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                result.addError("String value cannot be empty");
            }
        }
    }, value);
    
    return result;
}

// Additional helper methods
std::string Config::getConfigValueString(const ConfigValue& value) const {
    return pImpl->valueToString(value);
}

json Config::configValueToJSON(const ConfigValue& value) const {
    return pImpl->configValueToJSON(value);
}

void Config::addToHistory(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue,
                          ConfigSource source, const std::string& reason) {
    if (pImpl->history.size() >= pImpl->maxHistorySize) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    ConfigChange change(key, oldValue, newValue, source, reason);
    pImpl->history.push_back(change);
}

void Config::setDebugMode(bool enabled) {
    LoggerImpl::getGlobal().debug("Debug mode " + std::string(enabled ? "enabled" : "disabled"));
}

bool Config::isDebugModeEnabled() {
    return LoggerImpl::getGlobal().isEnabled(LogLevel::Debug);
}

void Config::setValidationMode(bool enabled) {
    // Note: This would need access to validation instance
    // For now, just log the change
    LoggerImpl::getGlobal().debug("Validation mode " + std::string(enabled ? "enabled" : "disabled"));
}

bool Config::isValidationModeEnabled() {
    return LoggerImpl::getGlobal().isEnabled(LogLevel::Info);
}

void Config::setErrorRecoveryMode(bool enabled) {
    // Note: This would need access to recovery instance
    // For now, just log the change
    LoggerImpl::getGlobal().debug("Error recovery mode " + std::string(enabled ? "enabled" : "disabled"));
}

bool Config::isErrorRecoveryModeEnabled() {
    return LoggerImpl::getGlobal().isEnabled(LogLevel::Warning);
}

// Explicit template instantiations
// These would need to be implemented for the specific types

} // namespace nlm
