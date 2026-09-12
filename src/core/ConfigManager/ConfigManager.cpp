#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <zlib.h>
#include <sstream>
#include <codecvt>
#include <thread>

using json = nlohmann::json;

namespace nlm {

struct ConfigManager::Impl {
    Config config;
    std::vector<ConfigSchema> schemas;
    std::vector<ConfigChange> changeHistory;
    std::vector<ConfigChangeCallback> changeCallbacks;
    
    // Thread safety
    mutable std::mutex mutex;
    std::atomic<bool> monitoringEnabled;
    
    // Performance tracking
    size_t loadCount;
    size_t saveCount;
    size_t validationCount;
    double totalValidationTime;
    
    Impl() : monitoringEnabled(true), loadCount(0), saveCount(0), 
             validationCount(0), totalValidationTime(0.0) {}
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;

ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

bool ConfigManager::loadFromFile(const std::string& filepath, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->loadCount++;
    
    // Try to detect file format
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSONFile(filepath, validate);
    } else {
        success = pImpl->config.loadFromFile(filepath);
    }
    
    if (validate) {
        ValidationResult result = validate();
        if (!result.valid) {
            NLM_LOG_ERROR("Configuration validation failed after loading from " + filepath);
            for (const auto& error : result.errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
    }
    
    NLM_LOG_INFO("Configuration loaded from: " + filepath);
    return success;
}

bool ConfigManager::loadFromJSON(const std::string& jsonString, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        auto jsonData = json::parse(jsonString);
        pImpl->config.clear();
        
        // Parse JSON into config entries
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Check if it's a vector of simple types
                    std::vector<int> intArray;
                    std::vector<double> doubleArray;
                    std::vector<std::string> stringArray;
                    
                    for (const auto& item : value) {
                        if (item.is_number_integer()) {
                            intArray.push_back(item.get<int64_t>());
                        } else if (item.is_number_float()) {
                            doubleArray.push_back(item.get<double>());
                        } else if (item.is_string()) {
                            stringArray.push_back(item.get<std::string>());
                        }
                    }
                    
                    if (!intArray.empty()) {
                        configValue = intArray;
                    } else if (!doubleArray.empty()) {
                        configValue = doubleArray;
                    } else if (!stringArray.empty()) {
                        configValue = stringArray;
                    }
                }
                
                pImpl->config.set(key, configValue, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Configuration loaded from JSON string");
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

bool ConfigManager::loadFromJSONFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadFromJSON(buffer.str(), validate);
}

std::string ConfigManager::exportToJSON(bool pretty) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    json jsonData;
    
    for (const auto& entry : pImpl->config.getKeys()) {
        std::optional<ConfigValue> value = pImpl->config.get<ConfigValue>(entry);
        if (!value) continue;
        
        jsonData[entry] = value.value();
    }
    
    if (pretty) {
        return jsonData.dump(2);
    } else {
        return jsonData.dump();
    }
}

void ConfigManager::registerSchema(const ConfigSchema& schema) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove existing schema for this key
    pImpl->schemas.erase(
        std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
                      [&schema](const ConfigSchema& s) { return s.name == schema.name; }),
        pImpl->schemas.end()
    );
    
    pImpl->schemas.push_back(schema);
    
    // Validate if schema exists
    if (pImpl->config.has(schema.name)) {
        ValidationResult result = validate();
        if (!result.valid) {
            NLM_LOG_WARNING("New schema registered for existing key '" + schema.name + "' but validation failed");
        }
    }
}

ConfigManager::ValidationResult ConfigManager::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ValidationResult result;
    pImpl->validationCount++;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check required keys
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !pImpl->config.has(schema.name)) {
            result.errors.push_back("Required configuration key missing: '" + schema.name + "'");
        }
    }
    
    // Validate existing keys against schemas
    for (const auto& schema : pImpl->schemas) {
        if (pImpl->config.has(schema.name)) {
            std::optional<ConfigValue> value = pImpl->config.get<ConfigValue>(schema.name);
            if (!value) {
                result.errors.push_back("Cannot get value for key: '" + schema.name + "'");
                continue;
            }
            
            // Apply custom validator if provided
            if (schema.validator) {
                try {
                    if (!schema.validator(value.value())) {
                        result.errors.push_back("Validation failed for key '" + schema.name + "'");
                    }
                } catch (const std::exception& e) {
                    result.errors.push_back("Exception in validator for key '" + schema.name + "': " + e.what());
                }
            }
            
            // Check allowed values if specified
            if (!schema.allowedValues.empty()) {
                bool found = false;
                for (const auto& allowed : schema.allowedValues) {
                    if (allowed == std::to_string(value.value())) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    result.warnings.push_back("Value for key '" + schema.name + "' is not in allowed list");
                }
            }
        }
    }
    
    // Check for deprecated keys (future enhancement)
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    pImpl->totalValidationTime += duration.count();
    
    if (pImpl->schemas.empty()) {
        result.warnings.push_back("No configuration schemas registered");
    }
    
    if (result.valid) {
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    return result;
}

bool ConfigManager::setValidated(const std::string& key, const ConfigValue& value, 
                                 ConfigSource source, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if schema exists
    const ConfigSchema* schema = getSchema(key);
    
    if (schema) {
        // Apply validator if provided
        if (schema->validator && !schema->validator(value)) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "'");
            return false;
        }
        
        // Check allowed values
        if (!schema->allowedValues.empty()) {
            bool found = false;
            for (const auto& allowed : schema->allowedValues) {
                if (allowed == std::to_string(value)) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                NLM_LOG_WARNING("Value for key '" + key + "' is not in allowed list");
            }
        }
    }
    
    // Check for existing value
    bool hasOldValue = pImpl->config.has(key);
    ConfigValue oldValue;
    if (hasOldValue) {
        auto oldOpt = pImpl->config.get<ConfigValue>(key);
        if (oldOpt) oldValue = oldOpt.value();
    }
    
    // Set the value
    pImpl->config.set(key, value, source);
    
    // Record change
    if (hasOldValue && oldValue != value) {
        ConfigChange change;
        change.key = key;
        change.oldValue = oldValue;
        change.newValue = value;
        change.source = source;
        change.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        pImpl->changeHistory.push_back(change);
        
        // Notify callbacks
        if (pImpl->monitoringEnabled) {
            for (const auto& callback : pImpl->changeCallbacks) {
                callback(change);
            }
        }
        
        NLM_LOG_INFO("Configuration changed: '" + key + "' from " + 
                    std::to_string(oldValue) + " to " + std::to_string(value) +
                    " (source: " + std::to_string(static_cast<int>(source)) + ")");
    }
    
    return true;
}

const ConfigManager::ConfigSchema* ConfigManager::getSchema(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& schema : pImpl->schemas) {
        if (schema.name == key) {
            return &schema;
        }
    }
    
    return nullptr;
}

bool ConfigManager::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config.has(key);
}

std::vector<std::string> ConfigManager::getKeys(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pattern == "*") {
        return pImpl->config.getKeys();
    }
    
    std::vector<std::string> matchingKeys;
    for (const auto& key : pImpl->config.getKeys()) {
        // Simple wildcard matching
        bool matches = true;
        size_t patternPos = 0;
        size_t keyPos = 0;
        
        while (patternPos < pattern.size() && keyPos < key.size()) {
            if (pattern[patternPos] == '*') {
                // Skip consecutive *
                while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                    patternPos++;
                }
                
                // Match remaining pattern
                if (patternPos + 1 >= pattern.size()) {
                    return matchingKeys; // '*' matches anything
                }
                
                // Try to match the rest
                while (keyPos < key.size()) {
                    if (matchesPattern(pattern.substr(patternPos + 1), key.substr(keyPos))) {
                        return matchingKeys; // Found a match
                    }
                    keyPos++;
                }
                matches = false;
                break;
            } else if (pattern[patternPos] == '?') {
                if (keyPos < key.size()) {
                    keyPos++;
                } else {
                    matches = false;
                    break;
                }
            } else if (pattern[patternPos] != key[keyPos]) {
                matches = false;
                break;
            }
            
            patternPos++;
            keyPos++;
        }
        
        if (matches && patternPos == pattern.size() && keyPos == key.size()) {
            matchingKeys.push_back(key);
        }
    }
    
    return matchingKeys;
}

void ConfigManager::addChangeCallback(const ConfigChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->changeCallbacks.push_back(callback);
}

void ConfigManager::clearChangeCallbacks() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->changeCallbacks.clear();
}

std::string ConfigManager::getSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration Manager Summary (" << pImpl->config.getKeys().size() << " entries):";
    oss << "\n  Total loads: " << pImpl->loadCount;
    oss << "\n  Total saves: " << pImpl->saveCount;
    oss << "\n  Total validations: " << pImpl->validationCount;
    oss << "\n  Average validation time: " << (pImpl->validationCount > 0 ? 
        pImpl->totalValidationTime / pImpl->validationCount : 0) << " ms";
    
    oss << "\n\nConfiguration Entries:";
    for (const auto& key : pImpl->config.getKeys()) {
        auto valueOpt = pImpl->config.get<ConfigValue>(key);
        if (!valueOpt) continue;
        
        const ConfigValue& value = valueOpt.value();
        oss << "\n  " << key << " = ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
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
                    oss << arg[i];
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
                oss << arg;
            }
        }, value);
    }
    
    oss << "\n\nRegistered Schemas: " << pImpl->schemas.size();
    
    return oss.str();
}

void ConfigManager::merge(const ConfigManager& other, bool overwrite) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& key : other.pImpl->config.getKeys()) {
        if (!overwrite && pImpl->config.has(key)) {
            continue;
        }
        
        auto valueOpt = other.pImpl->config.get<ConfigValue>(key);
        if (valueOpt) {
            pImpl->config.set(key, valueOpt.value(), ConfigSource::File);
        }
    }
    
    // Merge schemas
    for (const auto& schema : other.pImpl->schemas) {
        bool found = false;
        for (const auto& existing : pImpl->schemas) {
            if (existing.name == schema.name) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->schemas.push_back(schema);
        }
    }
}

std::unordered_map<std::string, std::string> ConfigManager::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::unordered_map<std::string, std::string> stats;
    
    stats["total_keys"] = std::to_string(pImpl->config.getKeys().size());
    stats["total_schemas"] = std::to_string(pImpl->schemas.size());
    stats["total_changes"] = std::to_string(pImpl->changeHistory.size());
    stats["monitoring_enabled"] = pImpl->monitoringEnabled ? "true" : "false";
    stats["load_count"] = std::to_string(pImpl->loadCount);
    stats["save_count"] = std::to_string(pImpl->saveCount);
    stats["validation_count"] = std::to_string(pImpl->validationCount);
    stats["average_validation_time_ms"] = std::to_string(
        pImpl->validationCount > 0 ? pImpl->totalValidationTime / pImpl->validationCount : 0);
    
    return stats;
}

void ConfigManager::resetToDefaults() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->config.clear();
    
    // Restore default values from schemas
    for (const auto& schema : pImpl->schemas) {
        if (!schema.defaultValue.empty()) {
            // Try to parse default value
            ConfigValue defaultValue;
            bool parsed = false;
            
            // Try different types
            try {
                if (schema.defaultValue == "true" || schema.defaultValue == "false") {
                    defaultValue = schema.defaultValue == "true";
                    parsed = true;
                } else if (schema.defaultValue.find('.') != std::string::npos) {
                    // Try double
                    size_t pos;
                    double doubleVal = std::stod(schema.defaultValue, &pos);
                    if (pos == schema.defaultValue.size()) {
                        defaultValue = doubleVal;
                        parsed = true;
                    }
                } else {
                    // Try integer
                    size_t pos;
                    int64_t intVal = std::stoll(schema.defaultValue, &pos);
                    if (pos == schema.defaultValue.size()) {
                        defaultValue = intVal;
                        parsed = true;
                    }
                }
            } catch (...) {
                // Ignore parsing errors
            }
            
            if (parsed) {
                pImpl->config.set(schema.name, defaultValue, ConfigSource::Default);
            }
        }
    }
    
    NLM_LOG_INFO("Configuration reset to defaults");
}

void ConfigManager::copyTo(ConfigManager& other) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    other.pImpl->config.clear();
    
    for (const auto& key : pImpl->config.getKeys()) {
        auto valueOpt = pImpl->config.get<ConfigValue>(key);
        if (valueOpt) {
            other.pImpl->config.set(key, valueOpt.value(), pImpl->config.getSource(key));
        }
    }
    
    other.pImpl->schemas = pImpl->schemas;
}

bool ConfigManager::matchesPattern(const std::string& pattern, const std::string& str) {
    // Simple implementation of wildcard matching
    size_t patternPos = 0;
    size_t strPos = 0;
    
    while (patternPos < pattern.size() && strPos < str.size()) {
        if (pattern[patternPos] == '*') {
            // Skip consecutive *
            while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                patternPos++;
            }
            
            // Match remaining pattern
            if (patternPos + 1 >= pattern.size()) {
                return true; // '*' matches anything
            }
            
            // Try to match the rest
            while (strPos < str.size()) {
                if (matchesPattern(pattern.substr(patternPos + 1), str.substr(strPos))) {
                    return true;
                }
                strPos++;
            }
            return false;
        } else if (pattern[patternPos] == '?') {
            if (strPos < str.size()) {
                strPos++;
            } else {
                return false;
            }
        } else if (pattern[patternPos] != str[strPos]) {
            return false;
        }
        
        patternPos++;
        strPos++;
    }
    
    return patternPos == pattern.size() && strPos == str.size();
}

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;

ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

} // namespace nlm