#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <set>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, std::string>
>;

// Configuration source enum - enhanced
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset,
    Inherited
};

// Configuration validation result
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (!valid) {
            oss << "Invalid configuration:\n";
            for (const auto& error : errors) {
                oss << "  ERROR: " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            if (!valid) oss << "Warnings:\n";
            for (const auto& warning : warnings) {
                oss << "  WARNING: " << warning << "\n";
            }
        }
        return oss.str();
    }
};

// Constraint types
enum class ConstraintType {
    Range,           // Range constraints (min, max)
    Enumeration,     // Allowed values
    Length,          // Min/max length
    Pattern,         // Regex pattern
    Custom           // Custom validation function
};

struct Constraint {
    ConstraintType type;
    std::string param1;
    std::string param2;
    std::function<bool(const ConfigValue&)> customValidator;
};

// Enhanced configuration entry with full metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::vector<Constraint> constraints;
    bool required = false;
    std::string parentKey;
    bool isInherited = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), category(), required(false), isInherited(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat), required(false), isInherited(false) {}
};

// Configuration preset
struct Preset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::vector<std::string> dependencies;
};

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, ConfigEntry> entries;
    std::map<std::string, std::vector<Constraint>> constraints;
    std::vector<Preset> presets;
};

// Main configuration class - enhanced
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Advanced configuration management
    void registerKey(const std::string& key, const ConfigValue& defaultValue, 
                    const std::string& description, const std::string& category = "General",
                    bool required = false);
    
    void registerKeyWithConstraints(const std::string& key, const ConfigValue& defaultValue,
                                   const std::string& description, const std::string& category = "General",
                                   const std::vector<Constraint>& constraints = {});
    
    // Enhanced file I/O with multiple formats
    bool loadFromFile(const std::string& filepath, const std::string& format = "json");
    bool loadFromJSON(const std::string& jsonStr);
    bool loadFromYAML(const std::string& yamlStr);
    
    // Export/import enhanced formats
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    std::string toJSON() const;
    std::string toYAML() const;
    bool fromJSON(const std::string& jsonStr);
    bool fromYAML(const std::string& yamlStr);
    
    // Command line and preset loading
    bool loadFromArgs(int argc, char** argv);
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    void clear();
    
    // Validation and schema
    ValidationResult validate() const;
    bool validateKey(const std::string& key) const;
    
    // Preset management
    bool addPreset(const Preset& preset);
    bool applyPreset(const std::string& presetName);
    std::vector<std::string> getPresetNames() const;
    
    // Hierarchical configuration
    void setParentConfig(std::shared_ptr<Config> parent);
    std::shared_ptr<Config> getParentConfig() const;
    bool inheritsFrom(const std::string& key) const;
    
    // Template management
    bool addTemplate(const std::string& name, const Config& templateConfig);
    bool applyTemplate(const std::string& templateName);
    std::vector<std::string> getTemplateNames() const;
    
    // Schema management
    bool addSchema(const ConfigSchema& schema);
    bool validateAgainstSchema(const std::string& schemaName) const;
    std::vector<std::string> getSchemaNames() const;
    
    // Configuration summary
    std::string summary() const;
    std::string summaryByCategory() const;
    
    // File I/O helpers
    static bool fileExists(const std::string& filepath);
    static std::string getConfigDirectory();
    
    // Static preset and schema management
    static std::map<std::string, Preset> getDefaultPresets();
    static ConfigSchema getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    
    // Validation helpers
    bool validateValueAgainstConstraints(const ConfigEntry& entry) const;
    bool checkRangeConstraint(const Constraint& constraint, const ConfigValue& value) const;
    bool checkEnumerationConstraint(const Constraint& constraint, const ConfigValue& value) const;
    bool checkLengthConstraint(const Constraint& constraint, const ConfigValue& value) const;
    bool checkPatternConstraint(const Constraint& constraint, const ConfigValue& value) const;
    
    // File I/O helpers
    bool loadJSON(const std::string& filepath, json& data);
    bool saveJSON(const std::string& filepath, const json& data) const;
    
    // Preset application helpers
    bool applyPresetRecursive(const std::string& presetName, std::set<std::string>& visited);
    
    // Schema validation helpers
    ValidationResult validateAgainstSchema(const ConfigSchema& schema) const;
};

} // namespace nlm
