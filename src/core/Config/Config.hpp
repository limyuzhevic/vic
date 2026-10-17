#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types with support for arrays
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Enhanced configuration source with more granularity
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Preset = 4,
    Environment = 5,
    Compilation = 6
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRules;
    std::string defaultValue;
    bool isRequired = false;
    bool isReadOnly = false;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationRules(), 
                   defaultValue(), isRequired(false), isReadOnly(false), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRules(), defaultValue(), 
          isRequired(false), isReadOnly(false), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, 
                const std::string& rules, const std::string& def, bool req, bool readOnly)
        : key(k), value(v), source(s), description(desc), validationRules(rules), defaultValue(def),
          isRequired(req), isReadOnly(readOnly), timestamp(std::chrono::system_clock::now()) {}
    
    // Helper method to get value as string for saving
    std::string getStringValue() const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << (arg[i] ? "true" : "false");
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
};

// Configuration validation result
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (isValid) {
            oss << "✓ Configuration is valid";
            if (!warnings.empty()) {
                oss << " (" << warnings.size() << " warnings)";
            }
            return oss.str();
        }
        oss << "✗ Configuration is invalid:\n";
        for (const auto& error : errors) {
            oss << "  ERROR: " << error << "\n";
        }
        for (const auto& warning : warnings) {
            oss << "  WARNING: " << warning << "\n";
        }
        return oss.str();
    }
};

// Configuration statistics
struct ConfigStatistics {
    size_t totalEntries = 0;
    size_t uniqueKeys = 0;
    std::map<ConfigSource, size_t> entriesBySource;
    std::map<std::string, size_t> keysByType;
    double memoryUsage = 0.0;
    std::chrono::system_clock::time_point lastModified;
    
    void update(const std::vector<ConfigEntry>& entries) {
        totalEntries = entries.size();
        std::set<std::string> uniqueKeysSet;
        
        for (const auto& entry : entries) {
            uniqueKeysSet.insert(entry.key);
            entriesBySource[entry.source]++;
            
            // Count by type (simplified)
            if (std::holds_alternative<int>(entry.value)) keysByType["int"]++;
            else if (std::holds_alternative<int64_t>(entry.value)) keysByType["int64_t"]++;
            else if (std::holds_alternative<double>(entry.value)) keysByType["double"]++;
            else if (std::holds_alternative<bool>(entry.value)) keysByType["bool"]++;
            else if (std::holds_alternative<std::string>(entry.value)) keysByType["string"]++;
            else if (std::holds_alternative<std::vector<int>>(entry.value)) keysByType["vector<int>"]++;
            else if (std::holds_alternative<std::vector<double>>(entry.value)) keysByType["vector<double>"]++;
            else if (std::holds_alternative<std::vector<std::string>>(entry.value)) keysByType["vector<string>"]++;
            else if (std::holds_alternative<std::vector<bool>>(entry.value)) keysByType["vector<bool>"]++;
        }
        
        uniqueKeys = uniqueKeysSet.size();
        lastModified = std::chrono::system_clock::now();
        
        // Estimate memory usage (rough approximation)
        memoryUsage = sizeof(ConfigEntry) * entries.size() * 1.2; // +20% overhead
    }
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // ==================== CORE CONFIGURATION FUNCTIONS ====================
    
    // Load from file (JSON/YAML format - simplified for now)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from file with preset support
    bool loadFromFileWithPresets(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from string
    bool loadFromString(const std::string& configString);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to string
    std::string saveToString() const;
    
    // ==================== CORE GET/SET OPERATIONS ====================
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety and validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced set with metadata
    void set(const std::string& key, const ConfigValue& value, const std::string& description, 
             ConfigSource source = ConfigSource::Runtime, const std::string& validationRules = "",
             const std::string& defaultValue = "", bool isRequired = false, bool isReadOnly = false);
    
    // ==================== VALIDATION AND VERIFICATION ====================
    
    // Comprehensive validation
    ValidationResult validate() const;
    
    // Key validation
    bool validateKey(const std::string& key) const;
    
    // Value validation
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Schema validation
    ValidationResult validateSchema(const std::vector<std::pair<std::string, std::string>>& schema) const;
    
    // Range validation
    bool validateRange(const std::string& key, const ConfigValue& value) const;
    
    // ==================== CONFIGURATION PRESET MANAGEMENT ====================
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    bool loadPreset(const std::string& presetName);
    void saveAsPreset(const std::string& presetName) const;
    void removePreset(const std::string& presetName);
    
    // Built-in presets
    static void createDefaultPreset();
    static void createPerformancePreset();
    static void createAccuracyPreset();
    static void createMinimalPreset();
    
    // ==================== CONFIGURATION SEARCH AND FILTERING ====================
    
    // Search for keys containing substring
    std::vector<std::string> findKeysContaining(const std::string& substring) const;
    
    // Search for keys matching pattern
    std::vector<std::string> findKeysMatching(const std::string& pattern) const;
    
    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Get keys by type
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    // Filter entries by source
    std::vector<ConfigEntry> filterBySource(ConfigSource source) const;
    
    // Filter entries by type
    std::vector<ConfigEntry> filterByType(const std::string& type) const;
    
    // ==================== CONFIGURATION MERGING AND COMPARISON ====================
    
    // Merge another configuration
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Compare configurations
    bool isIdentical(const Config& other) const;
    double similarity(const Config& other) const;
    
    // Get differences
    std::vector<std::string> getDifferences(const Config& other) const;
    
    // ==================== STATISTICS AND METADATA ====================
    
    // Get configuration statistics
    ConfigStatistics getStatistics() const;
    
    // Get last modified time
    std::chrono::system_clock::time_point getLastModified() const;
    
    // Check if configuration is empty
    bool isEmpty() const;
    
    // ==================== BASIC INFORMATION ====================
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get entry by key
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // ==================== UTILITY FUNCTIONS ====================
    
    // Get configuration summary
    std::string summary() const;
    
    // Get formatted entry list
    std::string getFormattedEntryList() const;
    
    // Get source string for display
    static std::string getSourceString(ConfigSource source);
    
    // ==================== PRIVATE IMPLEMENTATION ====================
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
    
    // Validation helpers
    static bool validateIntRange(const ConfigValue& value, const std::string& rules);
    static bool validateDoubleRange(const ConfigValue& value, const std::string& rules);
    static bool validateStringLength(const ConfigValue& value, const std::string& rules);
    static bool validateEnum(const ConfigValue& value, const std::string& rules);
    static bool validateArraySize(const ConfigValue& value, const std::string& rules);
    
    // Preset file management
    static std::string getPresetDirectory();
    static std::string getPresetFilePath(const std::string& presetName);
    static bool loadPresetsFromDirectory();
};