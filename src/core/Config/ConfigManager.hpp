#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <any>
#include <type_traits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>
#include <stdexcept>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string
>;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    SchemaDefault
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool isRequired = false;
    std::string validationPattern;
    std::any defaultValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), isRequired(false), validationPattern() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool required = false)
        : key(k), value(v), source(s), description(desc), isRequired(required) {}
};

// Configuration field schema for validation
struct ConfigField {
    std::string key;
    ConfigSource source;
    std::string description;
    bool isRequired = false;
    std::string validationPattern;
    std::any defaultValue;
    
    ConfigField(const std::string& k, ConfigSource s, const std::string& desc = "",
                bool required = false, const std::string& pattern = "",
                const std::any& defaultVal = std::any())
        : key(k), source(s), description(desc), isRequired(required),
          validationPattern(pattern), defaultValue(defaultVal) {}
};

// Configuration schema for validation rules
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add field with validation pattern
    void addField(const std::string& key, ConfigSource source, 
                  const std::string& description, bool required,
                  const std::string& validationPattern = "",
                  const std::any& defaultValue = std::any());
    
    // Add field without validation pattern (for backward compatibility)
    void addField(const std::string& key, ConfigSource source, 
                  const std::string& description, bool required) {
        addField(key, source, description, required, "", std::any());
    }
    
    const std::vector<ConfigEntry>& getFields() const { return fields; }
    bool hasField(const std::string& key) const;
    
private:
    std::vector<ConfigEntry> fields;
};

// Validation result
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
};

// Main enhanced configuration manager
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    // Loading from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromKeyValue(const std::string& filepath);
    
    // Saving to files
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    
    // Configuration management
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe get/set methods
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    void set(const std::string& key, const T& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters for common types
    void setString(const std::string& key, const std::string& value, 
                   ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, 
                ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, 
                   ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, 
                 ConfigSource source = ConfigSource::Runtime);
    
    // Check and manage configuration
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Information and utilities
    std::string summary() const;
    ConfigSource getSource(const std::string& key) const;
    
    // Schema validation
    void setSchema(const ConfigSchema& schema);
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Backward compatibility
    bool loadLegacyKeyValue(const std::string& filepath);
    
    // Static methods
    static std::string getCLIHelp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string valueToString(const ConfigValue& value) const;
    ValidationResult validateField(const ConfigEntry& entry) const;
    std::string generateValidationError(const ConfigEntry& entry, const ValidationResult& result) const;
    
    // Schema
    ConfigSchema schema;
};

} // namespace nlm
