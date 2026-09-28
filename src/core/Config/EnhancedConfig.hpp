// Nested Configuration Support
#include <unordered_map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace nlm {

// Enhanced Configuration value types with nested support
using NestedConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::unordered_map<std::string, NestedConfigValue> // Nested configuration
>;

// Path-based key system for nested configurations
struct ConfigPath {
    std::vector<std::string> pathParts;
    
    ConfigPath() = default;
    ConfigPath(const std::string& singleKey) {
        pathParts.push_back(singleKey);
    }
    ConfigPath(const std::vector<std::string>& parts) : pathParts(parts) {}
    
    std::string toString() const {
        std::ostringstream oss;
        for (size_t i = 0; i < pathParts.size(); ++i) {
            if (i > 0) oss << ".";
            oss << pathParts[i];
        }
        return oss.str();
    }
    
    bool isEmpty() const { return pathParts.empty(); }
    bool isRoot() const { return pathParts.size() == 1; }
    std::string getLastPart() const {
        return pathParts.empty() ? "" : pathParts.back();
    }
};

// Configuration entry with enhanced metadata
struct EnhancedConfigEntry {
    ConfigPath key;
    NestedConfigValue value;
    ConfigSource source;
    std::string description;
    std::string dataType;
    bool required = false;
    std::string validationPattern;
    std::vector<std::string> allowedValues;
    std::pair<double, double> rangeValidation;
    
    EnhancedConfigEntry() : source(ConfigSource::Default), required(false) {}
    EnhancedConfigEntry(const ConfigPath& k, const NestedConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), required(false) {}
};

// Configuration schema with nested support
class EnhancedConfigSchema {
public:
    EnhancedConfigSchema() = default;
    
    // Add nested configuration support
    void addNestedConfig(const std::string& key, const EnhancedConfigSchema& nestedSchema) {
        nestedConfigs_[key] = std::make_shared<EnhancedConfigSchema>(nestedSchema);
    }
    
    // Add validation for nested key
    void addNestedValidation(const std::string& key, const std::string& nestedKey, 
                           const std::string& errorMessage, const std::string& suggestion = "") {
        nestedValidations_[key].push_back({nestedKey, errorMessage, suggestion});
    }
    
    // Check if key has nested configuration
    bool hasNestedConfig(const std::string& key) const {
        return nestedConfigs_.find(key) != nestedConfigs_.end();
    }
    
    // Get nested configuration schema
    std::shared_ptr<EnhancedConfigSchema> getNestedConfig(const std::string& key) const {
        auto it = nestedConfigs_.find(key);
        return it != nestedConfigs_.end() ? it->second : nullptr;
    }
    
    // Get nested validations
    const std::vector<std::tuple<std::string, std::string, std::string>>& getNestedValidations(const std::string& key) const {
        static const std::vector<std::tuple<std::string, std::string, std::string>> empty;
        auto it = nestedValidations_.find(key);
        return it != nestedValidations_.end() ? it->second : empty;
    }
    
    // Clear all nested configurations
    void clearNestedConfigs() {
        nestedConfigs_.clear();
        nestedValidations_.clear();
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<EnhancedConfigSchema>> nestedConfigs_;
    std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>> nestedValidations_;
};

// Enhanced Configuration class with nested support
class EnhancedConfig {
public:
    EnhancedConfig() = default;
    
    // Set nested configuration value
    void setNested(const std::string& parentKey, const std::string& nestedKey, const NestedConfigValue& value) {
        ConfigPath fullPath;
        fullPath.pathParts.push_back(parentKey);
        fullPath.pathParts.push_back(nestedKey);
        
        setValue(fullPath, value, ConfigSource::Runtime);
    }
    
    // Get nested configuration value
    std::optional<NestedConfigValue> getNested(const std::string& parentKey, const std::string& nestedKey) const {
        ConfigPath fullPath;
        fullPath.pathParts.push_back(parentKey);
        fullPath.pathParts.push_back(nestedKey);
        
        return getValue(fullPath);
    }
    
    // Check if nested configuration exists
    bool hasNested(const std::string& parentKey, const std::string& nestedKey) const {
        ConfigPath fullPath;
        fullPath.pathParts.push_back(parentKey);
        fullPath.pathParts.push_back(nestedKey);
        
        return hasKey(fullPath);
    }
    
    // Get all nested keys for a parent
    std::vector<std::string> getNestedKeys(const std::string& parentKey) const {
        std::vector<std::string> nestedKeys;
        
        for (const auto& pair : configEntries_) {
            if (pair.first.pathParts.size() > 1 && pair.first.pathParts[0] == parentKey) {
                nestedKeys.push_back(pair.first.getLastPart());
            }
        }
        
        return nestedKeys;
    }
    
    // Set nested schema
    void setNestedSchema(const std::string& parentKey, const EnhancedConfigSchema& schema) {
        nestedSchema_[parentKey] = std::make_shared<EnhancedConfigSchema>(schema);
    }
    
    // Validate nested configuration
    bool validateNested(const std::string& parentKey) const {
        auto schemaIt = nestedSchema_.find(parentKey);
        if (schemaIt == nestedSchema_.end()) {
            return true; // No schema, skip validation
        }
        
        const auto& schema = *schemaIt->second;
        
        // Validate nested entries
        for (const auto& nestedKey : getNestedKeys(parentKey)) {
            auto value = getNested(parentKey, nestedKey);
            if (!value.has_value()) {
                continue; // Skip missing values
            }
            
            // Here you would add nested validation logic
            // This can be expanded to support nested schema validation
        }
        
        return true;
    }
    
    // Remove nested configuration
    void removeNested(const std::string& parentKey, const std::string& nestedKey) {
        ConfigPath fullPath;
        fullPath.pathParts.push_back(parentKey);
        fullPath.pathParts.push_back(nestedKey);
        
        removeKey(fullPath);
    }
    
    // Clear all nested configurations
    void clearNested(const std::string& parentKey) {
        std::vector<std::string> keysToRemove;
        
        for (const auto& pair : configEntries_) {
            if (pair.first.pathParts.size() > 1 && pair.first.pathParts[0] == parentKey) {
                keysToRemove.push_back(pair.first.toString());
            }
        }
        
        for (const auto& key : keysToRemove) {
            configEntries_.erase(key);
        }
    }
    
    // Helper methods for path-based operations
private:
    void setValue(const ConfigPath& key, const NestedConfigValue& value, ConfigSource source) {
        std::string keyStr = key.toString();
        configEntries_[keyStr] = EnhancedConfigEntry(key, value, source);
    }
    
    std::optional<NestedConfigValue> getValue(const ConfigPath& key) const {
        std::string keyStr = key.toString();
        auto it = configEntries_.find(keyStr);
        if (it != configEntries_.end()) {
            return it->second.value;
        }
        return std::nullopt;
    }
    
    bool hasKey(const ConfigPath& key) const {
        std::string keyStr = key.toString();
        return configEntries_.find(keyStr) != configEntries_.end();
    }
    
    void removeKey(const ConfigPath& key) {
        std::string keyStr = key.toString();
        configEntries_.erase(keyStr);
    }
    
protected:
    std::unordered_map<std::string, EnhancedConfigEntry> configEntries_;
    std::unordered_map<std::string, std::shared_ptr<EnhancedConfigSchema>> nestedSchema_;
};

} // namespace nlm