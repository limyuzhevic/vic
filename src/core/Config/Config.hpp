// NLM - Enhanced Configuration System

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Enhanced Configuration Value Types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::map<std::string, std::string>
>;

// Enhanced Configuration Source with more options
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Programmatic,
    Imported,
    Environment
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v) : valid(v) {}
    ValidationResult(bool v, const std::vector<std::string>& e) : valid(v), errors(e) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration category for organization
enum class ConfigCategory {
    General,
    Brain,
    Memory,
    Neuromodulation,
    Prediction,
    Cognition,
    Development,
    Plasticity,
    Performance,
    Debugging,
    Environment,
    Custom
};

// Enhanced Configuration Entry with more metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    std::vector<std::string> tags;
    ValidationResult validation;
    std::function<bool(const ConfigValue&)> validator;
    bool isReadonly;
    std::chrono::system_clock::time_point lastModified;
    std::string helpText;
    bool isExperimental;
    std::string deprecatedBy;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), category(ConfigCategory::General),
          isReadonly(false), isExperimental(false), lastModified(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                ConfigCategory cat = ConfigCategory::General, const std::string& help = "")
        : key(k), value(v), source(s), description(desc), category(cat), isReadonly(false),
          isExperimental(false), lastModified(std::chrono::system_clock::now()), helpText(help) {}
    
    bool isValid() const { return validation.valid; }
    std::string getCategoryName() const;
    std::string getSourceName() const;
};

// Configuration templates for common use cases
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::vector<std::pair<std::string, ConfigValue>> values;
    ConfigCategory category;
    bool isExperimental;
    
    ConfigurationTemplate(const std::string& n, const std::string& desc,
                         const std::vector<std::pair<std::string, ConfigValue>>& vals,
                         ConfigCategory cat = ConfigCategory::General, bool exp = false)
        : name(n), description(desc), values(vals), category(cat), isExperimental(exp) {}
};

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
    
    // Load from file (JSON/YAML/TOML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from environment variables with prefix
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    
    // Convenience setters with type safety
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool validate = true);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, ConfigValue::variant_type type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys organized by category
    std::map<ConfigCategory, std::vector<std::string>> getKeysByCategory() const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigCategory category) const;
    
    // Configuration templates management
    bool addTemplate(const ConfigurationTemplate& template);
    bool removeTemplate(const std::string& name);
    std::optional<ConfigurationTemplate> getTemplate(const std::string& name) const;
    std::vector<ConfigurationTemplate> getTemplates(ConfigCategory category = ConfigCategory::General) const;
    
    // Apply template
    bool applyTemplate(const std::string& templateName, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced validation
    ValidationResult validate(const std::string& key) const;
    ValidationResult validateAll() const;
    
    // Set validator for a key
    void setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    
    // Get validation result
    ValidationResult getValidationResult(const std::string& key) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration entry
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Configuration presets for common scenarios
    static std::vector<ConfigurationTemplate> getCommonPresets();
    static ConfigurationTemplate getMinimalPreset();
    static ConfigurationTemplate getBalancedPreset();
    static ConfigurationTemplate getHighPerformancePreset();
    static ConfigurationTemplate getResearchPreset();
    
    // Configuration help and documentation
    static std::string getConfigHelp(const std::string& key = "");
    static std::vector<std::string> getConfigCategories();
    static std::vector<std::string> getConfigKeys(ConfigCategory category);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    size_t getEntryCount(ConfigCategory category) const;
    
    // Configuration comparison
    bool hasSameValues(const Config& other, const std::vector<std::string>& keys = {}) const;
    
    // Configuration merge
    void merge(const Config& other, ConfigSource source = ConfigSource::Imported);
    
    // Configuration import/export
    bool exportToJSON(const std::string& filepath, const std::vector<std::string>& keys = {}) const;
    bool importFromJSON(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Configuration backup
    bool createBackup(const std::string& filepath) const;
    bool restoreFromBackup(const std::string& filepath);
    
    // Configuration monitoring
    std::chrono::system_clock::time_point getLastModified(const std::string& key) const;
    std::vector<std::string> getModifiedKeys(const std::chrono::system_clock::time_point& since) const;
    
    // Configuration statistics
    double getMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string extractExtension(const std::string& filename);
    static std::string getFileContent(const std::string& filepath);
    static bool parseJSON(const std::string& jsonStr, std::map<std::string, ConfigValue>& outValues);
    static bool writeJSON(const std::map<std::string, ConfigEntry>& entries, const std::string& filepath);
    
    // JSON serialization helper
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static bool jsonToConfigValue(const nlohmann::json& json, ConfigValue& value);
};

} // namespace nlm

// Template implementations for Config.cpp
namespace nlm {

inline std::string ConfigEntry::getCategoryName() const {
    switch (category) {
        case ConfigCategory::General: return "General";
        case ConfigCategory::Brain: return "Brain";
        case ConfigCategory::Memory: return "Memory";
        case ConfigCategory::Neuromodulation: return "Neuromodulation";
        case ConfigCategory::Prediction: return "Prediction";
        case ConfigCategory::Cognition: return "Cognition";
        case ConfigCategory::Development: return "Development";
        case ConfigCategory::Plasticity: return "Plasticity";
        case ConfigCategory::Performance: return "Performance";
        case ConfigCategory::Debugging: return "Debugging";
        case ConfigCategory::Environment: return "Environment";
        case ConfigCategory::Custom: return "Custom";
        default: return "Unknown";
    }
}

inline std::string ConfigEntry::getSourceName() const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Programmatic: return "Programmatic";
        case ConfigSource::Imported: return "Imported";
        case ConfigSource::Environment: return "Environment";
        default: return "Unknown";
    }
}

} // namespace nlm
