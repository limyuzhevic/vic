#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <limits>
#include <numeric>
#include <iomanip>

namespace nlm {

struct AdvancedConfig::Impl {
    // Core configuration data
    std::vector<ConfigEntry> entries;
    
    // Schema and metadata
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, ConfigMetadata> metadata;
    
    // Environment management
    std::unordered_map<std::string, EnvironmentConfig> environments;
    std::string currentEnvironment = "default";
    
    // Callbacks and validators
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::unordered_map<std::string, ConfigValidator> validators;
    std::unordered_map<std::string, ConfigModifier> modifiers;
    
    // Change tracking
    std::vector<ConfigChange> changeHistory;
    std::mutex changeHistoryMutex;
    
    // Performance optimization
    std::unordered_map<std::string, ConfigValue> normalizedValues;
    std::unordered_map<std::string, bool> validationCache;
    std::chrono::system_clock::time_point lastOptimization;
    
    // Statistics
    size_t accessCount = 0;
    double totalMemoryUsage = 0.0;
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default environment
    EnvironmentConfig defaultEnv;
    defaultEnv.name = "default";
    defaultEnv.description = "Default configuration environment";
    defaultEnv.isProduction = false;
    defaultEnv.isDevelopment = true;
    pImpl->environments["default"] = defaultEnv;
}

AdvancedConfig::~AdvancedConfig() = default;

AdvancedConfig::AdvancedConfig(AdvancedConfig&&) noexcept = default;
AdvancedConfig& AdvancedConfig::operator=(AdvancedConfig&&) noexcept = default;

void AdvancedConfig::setSchema(const std::string& key, const ConfigSchema& schema, const ConfigMetadata& metadata) {
    pImpl->schemas[key] = schema;
    pImpl->metadata[key] = metadata;
    
    // Validate existing values against new schema
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    if (it != pImpl->entries.end()) {
        if (!validateValue(key, it->value)) {
            spdlog::warn("Existing value for key '{}' does not match new schema", key);
        }
    }
}

bool AdvancedConfig::hasSchema(const std::string& key) const {
    return pImpl->schemas.find(key) != pImpl->schemas.end();
}

ConfigSchema AdvancedConfig::getSchema(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    return ConfigSchema{}; // Empty schema
}

ConfigMetadata AdvancedConfig::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    return ConfigMetadata{};
}

std::vector<std::string> AdvancedConfig::getSchemaKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->schemas.size());
    for (const auto& pair : pImpl->schemas) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool AdvancedConfig::validateValue(const std::string& key, const ConfigValue& value) const {
    // Quick validation for basic types
    if (!validateType(key, value)) {
        return false;
    }
    
    if (!validateRange(key, value)) {
        return false;
    }
    
    if (!validatePattern(key, value)) {
        return false;
    }
    
    // Check custom validator
    auto valIt = pImpl->validators.find(key);
    if (valIt != pImpl->validators.end()) {
        return valIt->second(value);
    }
    
    return true;
}

std::string AdvancedConfig::validateAll() const {
    std::ostringstream oss;
    bool hasErrors = false;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            hasErrors = true;
            oss << "Invalid value for key '" << entry.key << "'\n";
        }
    }
    
    if (!hasErrors) {
        oss << "All configuration values are valid.";
    }
    
    return oss.str();
}

template<typename T>
std::optional<T> AdvancedConfig::getValidated(const std::string& key) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        if (validateValue(key, value.value())) {
            return value;
        } else {
            spdlog::warn("Value for key '{}' is invalid", key);
        }
    }
    return std::nullopt;
}

void AdvancedConfig::addEnvironment(const EnvironmentConfig& env) {
    pImpl->environments[env.name] = env;
    
    // Set environment's default values
    for (const auto& pair : env.values) {
        set(pair.first, pair.second, ConfigSource::File);
    }
}

bool AdvancedConfig::setEnvironment(const std::string& envName) {
    auto it = pImpl->environments.find(envName);
    if (it == pImpl->environments.end()) {
        spdlog::warn("Environment '{}' not found", envName);
        return false;
    }
    
    pImpl->currentEnvironment = envName;
    
    // Apply environment-specific values
    for (const auto& pair : it->second.values) {
        set(pair.first, pair.second, ConfigSource::File);
    }
    
    return true;
}

std::string AdvancedConfig::getCurrentEnvironment() const {
    return pImpl->currentEnvironment;
}

std::vector<std::string> AdvancedConfig::getEnvironmentNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->environments.size());
    for (const auto& pair : pImpl->environments) {
        names.push_back(pair.first);
    }
    return names;
}

EnvironmentConfig AdvancedConfig::getEnvironment(const std::string& envName) const {
    auto it = pImpl->environments.find(envName);
    if (it != pImpl->environments.end()) {
        return it->second;
    }
    return EnvironmentConfig{};
}

std::optional<ConfigValue> AdvancedConfig::getWithFallback(const std::string& key, const ConfigValue& fallback) const {
    auto value = get<ConfigValue>(key);
    if (value.has_value()) {
        return value;
    }
    return fallback;
}

void AdvancedConfig::mergeConfig(const AdvancedConfig& otherConfig, ConfigSource source) {
    for (const auto& entry : otherConfig.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

void AdvancedConfig::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

std::vector<ConfigChange> AdvancedConfig::getRecentChanges(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->changeHistoryMutex);
    size_t start = std::max<size_t>(0, pImpl->changeHistory.size() - limit);
    return std::vector<ConfigChange>(pImpl->changeHistory.begin() + start, pImpl->changeHistory.end());
}

size_t AdvancedConfig::getChangeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->changeHistoryMutex);
    return pImpl->changeHistory.size();
}

void AdvancedConfig::setValidator(const std::string& key, const ConfigValidator& validator) {
    pImpl->validators[key] = validator;
}

void AdvancedConfig::setModifier(const std::string& key, const ConfigModifier& modifier) {
    pImpl->modifiers[key] = modifier;
}

bool AdvancedConfig::isValueValid(const std::string& key, const ConfigValue& value) const {
    if (pImpl->validationCache.find(key) != pImpl->validationCache.end()) {
        return pImpl->validationCache.at(key);
    }
    
    bool isValid = validateValue(key, value);
    pImpl->validationCache[key] = isValid;
    return isValid;
}

ConfigValue AdvancedConfig::applyModifier(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->modifiers.find(key);
    if (it != pImpl->modifiers.end()) {
        return it->second(value);
    }
    return value;
}

bool AdvancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Normalize the value first
    ConfigValue normalizedValue = normalizeValue(value);
    
    // Check if value has changed
    bool hasChanged = false;
    ConfigValue oldValue;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        if (oldValue != normalizedValue) {
            hasChanged = true;
            it->value = normalizedValue;
            it->source = source;
        }
    } else {
        hasChanged = true;
        pImpl->entries.emplace_back(key, normalizedValue, source);
    }
    
    // Apply modifier if exists
    ConfigValue finalValue = applyModifier(key, normalizedValue);
    
    // Validate if schema exists
    if (hasSchema(key) && !validateValue(key, finalValue)) {
        spdlog::error("Value for key '{}' is invalid after applying modifier", key);
        return false;
    }
    
    if (hasChanged) {
        ConfigChange change(key, oldValue, finalValue, source);
        
        // Update entries with final value after validation
        if (it != pImpl->entries.end()) {
            it->value = finalValue;
        }
        
        {
            std::lock_guard<std::mutex> lock(pImpl->changeHistoryMutex);
            pImpl->changeHistory.push_back(change);
        }
        
        // Notify callbacks
        for (const auto& callback : pImpl->changeCallbacks) {
            callback(key, oldValue, finalValue);
        }
    }
    
    return true;
}

bool AdvancedConfig::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!hasSchema(key)) {
        spdlog::warn("No schema defined for key '{}', performing basic validation only", key);
        return set(key, value, source);
    }
    
    if (!validateValue(key, value)) {
        std::string errorMsg = "Validation failed for key '" + key + "': ";
        // TODO: Provide specific validation error message
        spdlog::error(errorMsg);
        return false;
    }
    
    return set(key, value, source);
}

template<typename T>
std::optional<T> AdvancedConfig::get(const std::string& key) const {
    pImpl->accessCount++;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        spdlog::warn("Type mismatch for key '{}', expected type not found", key);
        return std::nullopt;
    }
}

template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Backward compatibility methods
void AdvancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool AdvancedConfig::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                       [&key](const ConfigEntry& e) { return e.key == key; });
}

void AdvancedConfig::remove(const std::string& key) {
    // Track change
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue value = it->value;
        pImpl->entries.erase(it);
        
        // Notify callbacks
        for (const auto& callback : pImpl->changeCallbacks) {
            callback(key, value, ConfigValue{});
        }
    }
}

std::vector<std::string> AdvancedConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::string AdvancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Advanced Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "  Environment: " << pImpl->currentEnvironment << "\n";
    oss << "  Schemas: " << pImpl->schemas.size() << " defined\n";
    oss << "  Validations: " << pImpl->validators.size() << " custom validators\n";
    oss << "  Changes: " << pImpl->changeHistory.size() << " in history\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

bool AdvancedConfig::operator==(const AdvancedConfig& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool AdvancedConfig::operator!=(const AdvancedConfig& other) const {
    return !(*this == other);
}

std::string AdvancedConfig::createSnapshot() const {
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::visit([&jsonConfig, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonConfig[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonConfig[key] = arg;
            }
        }, entry.value);
    }
    
    return jsonConfig.dump(2);
}

bool AdvancedConfig::restoreFromSnapshot(const std::string& snapshot) {
    try {
        auto jsonConfig = nlohmann::json::parse(snapshot);
        clear();
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            const auto& value = it.value();
            
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_array()) {
                // For arrays, try to infer type
                if (!value.empty()) {
                    if (value[0].is_number_integer()) {
                        std::vector<int> intArray = value.get<std::vector<int>>();
                        // Note: ConfigValue doesn't directly support vector<int>, need to handle this differently
                    } else if (value[0].is_number_float()) {
                        std::vector<double> doubleArray = value.get<std::vector<double>>();
                    } else if (value[0].is_string()) {
                        std::vector<std::string> stringArray = value.get<std::vector<std::string>>();
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to restore configuration from snapshot: {}", e.what());
        return false;
    }
}

bool AdvancedConfig::exportToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonConfig;
        
        // Export configuration entries
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            std::visit([&jsonConfig, &key](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfig[key] = arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    jsonConfig[key] = arg;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    jsonConfig[key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfig[key] = arg;
                }
            }, entry.value);
        }
        
        // Export schema information
        nlohmann::json jsonSchemas;
        for (const auto& pair : pImpl->schemas) {
            jsonSchemas[pair.first] = std::visit([](auto&& arg) {
                return arg;
            }, pair.second);
        }
        jsonConfig["_schemas"] = jsonSchemas;
        
        // Export metadata
        nlohmann::json jsonMetadata;
        for (const auto& pair : pImpl->metadata) {
            nlohmann::json metaJson;
            metaJson["description"] = pair.second.description;
            metaJson["category"] = pair.second.category;
            metaJson["unit"] = pair.second.unit;
            metaJson["validation_pattern"] = pair.second.validationPattern;
            metaJson["example_value"] = pair.second.exampleValue;
            metaJson["required"] = pair.second.required;
            metaJson["is_readonly"] = pair.second.isReadOnly;
            metaJson["is_advanced"] = pair.second.isAdvanced;
            jsonMetadata[pair.first] = metaJson;
        }
        jsonConfig["_metadata"] = jsonMetadata;
        
        // Export environment info
        nlohmann::json jsonEnv;
        jsonEnv["current"] = pImpl->currentEnvironment;
        for (const auto& pair : pImpl->environments) {
            nlohmann::json envJson;
            envJson["description"] = pair.second.description;
            envJson["is_production"] = pair.second.isProduction;
            envJson["is_development"] = pair.second.isDevelopment;
            jsonEnv[pair.second.name] = envJson;
        }
        jsonConfig["_environments"] = jsonEnv;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonConfig.dump(2);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to export configuration to JSON: {}", e.what());
        return false;
    }
}

bool AdvancedConfig::importFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        auto jsonConfig = nlohmann::json::parse(file);
        
        // Clear existing configuration
        clear();
        pImpl->schemas.clear();
        pImpl->metadata.clear();
        pImpl->environments.clear();
        
        // Import configuration entries
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            
            // Skip metadata and environment keys (prefixed with _)
            if (key.substr(0, 1) == "_") {
                continue;
            }
            
            const auto& value = it.value();
            
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            }
        }
        
        // Import schemas if present
        if (jsonConfig.contains("_schemas") && jsonConfig["_schemas"].is_object()) {
            auto schemas = jsonConfig["_schemas"];
            for (auto it = schemas.begin(); it != schemas.end(); ++it) {
                std::string key = it.key();
                pImpl->schemas[key] = std::visit([](auto&& arg) {
                    return arg;
                }, ConfigSchema{}); // Default schema
            }
        }
        
        // Import environments if present
        if (jsonConfig.contains("_environments") && jsonConfig["_environments"].is_object()) {
            auto envs = jsonConfig["_environments"];
            if (envs.contains("current")) {
                pImpl->currentEnvironment = envs["current"].get<std::string>();
            }
            
            for (auto it = envs.begin(); it != envs.end(); ++it) {
                std::string envName = it.key();
                if (envName == "current") continue;
                
                EnvironmentConfig env;
                env.name = envName;
                env.description = it.value().value_or("").get<std::string>("").find("description") != std::string::npos ? 
                    it.value()["description"].get<std::string>() : "";
                env.isProduction = it.value().value_or("").get<bool>("").find("is_production") != std::string::npos ? 
                    it.value()["is_production"].get<bool>() : false;
                env.isDevelopment = it.value().value_or("").get<bool>("").find("is_development") != std::string::npos ? 
                    it.value()["is_development"].get<bool>() : true;
                
                pImpl->environments[envName] = env;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to import configuration from JSON: {}", e.what());
        return false;
    }
}

void AdvancedConfig::optimizeAccess() {
    auto now = std::chrono::system_clock::now();
    auto timeSinceLastOpt = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastOptimization).count();
    
    if (timeSinceLastOpt > 60) { // Optimize every minute
        // Clear validation cache
        pImpl->validationCache.clear();
        
        // Rebuild normalized values cache
        pImpl->normalizedValues.clear();
        for (const auto& entry : pImpl->entries) {
            pImpl->normalizedValues[entry.key] = normalizeValue(entry.value);
        }
        
        pImpl->lastOptimization = now;
    }
}

AdvancedConfig::ConfigStats AdvancedConfig::getStats() const {
    ConfigStats stats;
    stats.totalKeys = pImpl->entries.size();
    stats.invalidKeys = 0;
    stats.readonlyKeys = 0;
    stats.memoryUsage = 0.0;
    stats.lastAccessTime = 0.0;
    
    // Calculate statistics
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            stats.invalidKeys++;
        }
        
        const auto& meta = pImpl->metadata.find(entry.key);
        if (meta != pImpl->metadata.end() && meta->second.isReadOnly) {
            stats.readonlyKeys++;
        }
    }
    
    stats.validKeys = stats.totalKeys - stats.invalidKeys;
    
    // Estimate memory usage
    for (const auto& entry : pImpl->entries) {
        stats.memoryUsage += sizeof(entry.key) + sizeof(entry.value);
    }
    
    auto now = std::chrono::system_clock::now();
    stats.lastAccessTime = std::chrono::duration<double>(now - pImpl->lastOptimization).count();
    
    return stats;
}

ConfigValue AdvancedConfig::normalizeValue(const ConfigValue& value) const {
    // Apply modifiers if needed
    return value;
}

bool AdvancedConfig::validateType(const std::string& key, const ConfigValue& value) const {
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt == pImpl->schemas.end()) {
        return true; // No schema defined, accept any type
    }
    
    const ConfigSchema& schema = schemaIt->second;
    
    // Check type match
    bool typeMatches = false;
    std::visit([&typeMatches, &value](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            typeMatches = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            typeMatches = std::holds_alternative<double>(value) || std::holds_alternative<float>(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            typeMatches = std::holds_alternative<bool>(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            typeMatches = std::holds_alternative<std::string>(value);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            typeMatches = std::holds_alternative<std::vector<int>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            typeMatches = std::holds_alternative<std::vector<double>>(value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            typeMatches = std::holds_alternative<std::vector<std::string>>(value);
        }
    }, schema);
    
    return typeMatches;
}

bool AdvancedConfig::validateRange(const std::string& key, const ConfigValue& value) const {
    auto metaIt = pImpl->metadata.find(key);
    if (metaIt == pImpl->metadata.end()) {
        return true; // No metadata, no range validation
    }
    
    const ConfigMetadata& metadata = metaIt->second;
    
    // Convert to double for range comparison if numeric
    bool inRange = true;
    std::visit([&inRange, &metadata](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                      std::is_same_v<T, double> || std::is_same_v<T, float>) {
            double val = (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) ? 
                         static_cast<double>(arg) : static_cast<double>(arg);
            if (metadata.minValue > 0 || metadata.maxValue > 0) {
                inRange = (val >= metadata.minValue && val <= metadata.maxValue);
            }
        }
    }, value);
    
    return inRange;
}

bool AdvancedConfig::validatePattern(const std::string& key, const ConfigValue& value) const {
    auto metaIt = pImpl->metadata.find(key);
    if (metaIt == pImpl->metadata.end() || metaIt->second.validationPattern.empty()) {
        return true; // No pattern validation
    }
    
    const ConfigMetadata& metadata = metaIt->second;
    
    bool patternValid = false;
    if (std::holds_alternative<std::string>(value)) {
        const std::string& strValue = std::get<std::string>(value);
        // Simple regex pattern matching (in real implementation, use proper regex)
        patternValid = metadata.validationPattern.empty() || 
                      strValue.find(metadata.validationPattern) != std::string::npos;
    }
    
    return patternValid;
}

// Explicit template instantiations
template std::optional<int> AdvancedConfig::get<int>(const std::string&) const;
template std::optional<int64_t> AdvancedConfig::get<int64_t>(const std::string&) const;
template std::optional<double> AdvancedConfig::get<double>(const std::string&) const;
template std::optional<bool> AdvancedConfig::get<bool>(const std::string&) const;
template std::optional<std::string> AdvancedConfig::get<std::string>(const std::string&) const;

template int AdvancedConfig::getOr<int>(const std::string&, const int&) const;
template int64_t AdvancedConfig::getOr<int64_t>(const std::string&, const int64_t&) const;
template double AdvancedConfig::getOr<double>(const std::string&, const double&) const;
template bool AdvancedConfig::getOr<bool>(const std::string&, const bool&) const;
template std::string AdvancedConfig::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
