// Enhanced configuration system for NLM
// Provides validation, schema definition, and advanced configuration features

#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <optional>
#include <variant>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <functional>

namespace nlm {

// Configuration entry with validation and metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required = false;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> validator;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : source(ConfigSource::Default), required(false), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default)
        : key(key), value(value), source(source), required(false), timestamp(std::chrono::system_clock::now()) {}
    
    // Validation function
    bool validate() const {
        if (validator) {
            return validator(value);
        }
        
        // Basic type validation based on expected type
        if (!allowedValues.empty()) {
            if (std::holds_alternative<std::string>(value)) {
                return std::find(allowedValues.begin(), allowedValues.end(), std::get<std::string>(value)) != allowedValues.end();
            }
        }
        
        if (!validationRegex.empty() && std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            // Simple regex implementation (could be enhanced)
            return strValue.find(validationRegex) != std::string::npos;
        }
        
        return true;
    }
    
    // Get value as string
    std::string toString() const {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                oss << '"' << val << '"';
            } else {
                oss << val;
            }
        }, value);
        return oss.str();
    }
};

// Configuration schema for validation
struct ConfigSchema {
    std::string name;
    std::string version;
    std::vector<std::string> requiredKeys;
    std::map<std::string, ConfigEntry> entries;
    std::function<bool(const std::map<std::string, ConfigValue>&)> globalValidator;
    
    ConfigSchema(const std::string& name = "NLM Config")
        : name(name), version("1.0") {}
    
    // Add entry with validation
    void addEntry(const std::string& key, const ConfigValue& defaultValue,
                 ConfigSource source = ConfigSource::Default,
                 const std::string& description = "",
                 bool required = false,
                 const std::vector<std::string>& allowedValues = {}) {
        ConfigEntry entry(key, defaultValue, source);
        entry.description = description;
        entry.required = required;
        entry.allowedValues = allowedValues;
        entries[key] = entry;
        
        if (required) {
            requiredKeys.push_back(key);
        }
    }
    
    // Add entry with validator
    void addEntry(const std::string& key, const ConfigValue& defaultValue,
                 ConfigSource source,
                 const std::string& description,
                 std::function<bool(const ConfigValue&)> validator) {
        ConfigEntry entry(key, defaultValue, source);
        entry.description = description;
        entry.validator = validator;
        entries[key] = entry;
    }
    
    // Validate all entries
    std::vector<std::string> validateAll(const std::map<std::string, ConfigValue>& config) const {
        std::vector<std::string> errors;
        
        // Check required keys
        for (const auto& requiredKey : requiredKeys) {
            if (config.find(requiredKey) == config.end()) {
                errors.push_back("Missing required key: '" + requiredKey + "'");
            }
        }
        
        // Check each entry
        for (const auto& entry : entries) {
            auto it = config.find(entry.first);
            if (it != config.end()) {
                if (!entry.second.validate()) {
                    errors.push_back("Invalid value for key '" + entry.first + "'");
                }
            } else if (entry.second.required) {
                errors.push_back("Missing required key: '" + entry.first + "'");
            }
        }
        
        // Apply global validator
        if (globalValidator) {
            if (!globalValidator(config)) {
                errors.push_back("Global validation failed");
            }
        }
        
        return errors;
    }
    
    // Get entry by key
    const ConfigEntry* getEntry(const std::string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? &it->second : nullptr;
    }
    
    // Check if key exists
    bool hasEntry(const std::string& key) const {
        return entries.find(key) != entries.end();
    }
};n

// Enhanced Config class with validation and schema support
class Config {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Basic operations
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default);
    
    // Get operations with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Load/save operations
    bool loadFromFile(const std::string& filepath, const ConfigSchema* schema = nullptr);
    bool saveToFile(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, const ConfigSchema* schema = nullptr);
    
    // String processing utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringContains(const std::string& str, const std::string& substr);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // Configuration operations
    void clear();
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getAllEntries() const;
    std::string summary() const;
    
    // Validation and schema
    std::vector<std::string> validate(const ConfigSchema* schema = nullptr) const;
    bool validateEntry(const std::string& key) const;
    bool applySchema(const ConfigSchema& schema);
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Override);
    void override(const Config& other);
    
    // Configuration export/import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json, const ConfigSchema* schema = nullptr);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Statistics and metadata
    size_t getEntryCount() const;
    std::chrono::system_clock::time_point getLastModified() const;
    ConfigSource getSource(const std::string& key) const;
    
    // Template specializations for common types
    template<typename T>
    static std::optional<T> convertTo(const std::string& str);
    
    // Constants
    static constexpr const char* DEFAULT_CONFIG_FILE = "nlm.cfg";
    static constexpr const char* CONFIG_FILE_EXTENSION = ".cfg";
};

// Config utilities and helper functions
namespace ConfigUtils {
    // File utilities
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    bool ensureDirectoryExists(const std::string& path);
    std::string getFileExtension(const std::string& filename);
    std::string getFileNameWithoutExtension(const std::string& filename);
    std::string getAbsolutePath(const std::string& path);
    
    // String utilities
    std::string escapeString(const std::string& str);
    std::string unescapeString(const std::string& str);
    bool isWhitespace(char c);
    std::string removeWhitespace(const std::string& str);
    
    // Value conversion utilities
    std::string toString(const ConfigValue& value);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool parseBoolean(const std::string& str);
    int parseInt(const std::string& str);
    double parseDouble(const std::string& str);
    
    // Validation utilities
    bool isValidKey(const std::string& key);
    bool isValidValue(const std::string& value, const std::string& type);
    
    // Configuration merging utilities
    void mergeConfigs(Config& target, const Config& source, ConfigSource sourceType);
    
    // Export/import utilities
    std::string escapeJSON(const std::string& str);
    std::string unescapeJSON(const std::string& str);
}

} // namespace nlm

#endif // NLM_CONFIG_HPP