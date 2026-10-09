#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Enhanced configuration value types with validation support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Enhanced configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Enhanced configuration entry with validation and metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationPattern;
    bool required;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          validationPattern(), required(false), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), 
          validationPattern(), required(req), validator(nullptr) {}
    
    bool validate() const {
        if (!validator) return true;
        return validator(value);
    }
    
    bool matchesPattern(const std::string& pattern) const {
        if (pattern.empty()) return true;
        // Simple pattern matching - could be enhanced with regex
        return key.find(pattern) != std::string::npos;
    }
};

// Configuration template for type-safe validation
struct ConfigParameter {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    ConfigSource source;
    std::string validationPattern;
    bool required;
    
    ConfigParameter(const std::string& k, const ConfigValue& def, 
                   const std::string& desc = "", ConfigSource src = ConfigSource::Default)
        : key(k), defaultValue(def), description(desc), source(src), 
          validationPattern(), required(false) {}
};

// Enhanced configuration change callback
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)>;

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with JSON support and validation
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromFile(const std::string& filepath, bool validateOnLoad);
    
    // Enhanced command line argument parsing with type conversion
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgs(const std::vector<std::string>& args);
    
    // Enhanced save to file with formatting
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    bool exportConfig(const std::string& filepath, const std::vector<std::string>& filterKeys = {}) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set methods with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced configuration management
    void registerParameter(const ConfigParameter& param);
    void unregisterParameter(const std::string& key);
    
    // Validation and error handling
    bool validate(const std::string& key = "") const;
    std::vector<std::string> getValidationErrors(const std::string& key = "") const;
    std::vector<std::string> getMissingRequiredKeys() const;
    
    // Configuration templates and presets
    bool saveTemplate(const std::string& name, const std::vector<std::string>& includeKeys = {}) const;
    bool loadTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Value manipulation
    void set(const std::string& key, const std::string& value, const std::string& unit = "");
    void increment(const std::string& key, double amount = 1.0);
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration comparison and diff
    std::string diff(const Config& other) const;
    bool isIdentical(const Config& other) const;
    
    // Configuration statistics and analytics
    std::string getStats() const;
    size_t getEntryCount() const;
    double getMemoryUsage() const;
    
    // Configuration change monitoring
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void clearChangeCallbacks();
    
    // Check existence and removal
    bool has(const std::string& key) const;
    
    // Remove key and history
    void remove(const std::string& key);
    void resetToDefault(const std::string& key);
    
    // Get all keys and entries
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    std::vector<ConfigEntry> getEntriesByPattern(const std::string& pattern) const;
    
    // Clear all and history
    void clear();
    
    // Get configuration summary with details
    std::string summary() const;
    std::string debugInfo() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool matchesPattern(const std::string& str, const std::string& pattern);
    
    // Command line argument parsing helpers
    static std::string parseConfigKey(const std::string& arg);
    static std::string parseConfigValue(const std::string& arg);
    
    // File I/O helpers
    static std::string getConfigDirectory();
    static std::string getDefaultConfigPath(const std::string& name = "default");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    static std::vector<std::string> validateValue(const ConfigValue& value, 
                                                  const ConfigParameter* param = nullptr);
};