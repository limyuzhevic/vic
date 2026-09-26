#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include "Schema.hpp"
#include "Validation.hpp"

namespace nlm {

// Forward declarations
class Config;

// Configuration value types (extended)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::vector<double>>  // For matrix values
>;

// Configuration source (extended)
enum class ConfigSource {
public:
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Override = 4  // User override
};

// Configuration entry (extended)
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string group;
    std::vector<std::string> tags;
    
    // Metadata
    bool read_only;
    bool requires_restart;
    std::string validation_rule;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), category("general"), group("basic")
        , read_only(false), requires_restart(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general"), group("basic")
        , read_only(false), requires_restart(false) {}
};

// Configuration change callback
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& json_str);
    bool loadFromYAMLString(const std::string& yaml_str);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Pythonic access
    std::optional<ConfigValue> getValue(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<std::string> getKeysByGroup(const std::string& group) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Safe set with validation
    bool safeSet(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool safeSet(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    ValidationResult validate() const;
    ValidationResult validate(const std::string& key) const;
    ValidationResult validateAll(const std::vector<std::pair<std::string, ConfigValue>>& key_values) const;
    
    // Runtime configuration management
    void enableRuntimeAdjustment(bool enabled = true);
    bool isRuntimeAdjustmentEnabled() const;
    
    // Callbacks
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const std::string& callback_id);
    void clearChangeCallbacks();
    
    // Schema access
    void setSchema(const ConfigurationSchema& schema);
    const ConfigurationSchema* getSchema() const;
    
    // Category/group utilities
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getGroups() const;
    
    // Export/import configuration
    std::string exportConfiguration() const;
    bool importConfiguration(const std::string& config_str);
    
    // Configuration search
    std::vector<std::pair<std::string, ConfigValue>> search(const std::string& pattern) const;
    std::vector<std::pair<std::string, ConfigValue>> getByCategory(const std::string& category) const;
    std::vector<std::pair<std::string, ConfigValue>> getByGroup(const std::string& group) const;
    
    // Statistics
    size_t getEntryCount() const;
    size_t getCategoryCount() const;
    size_t getGroupCount() const;
    double getMemoryUsage() const;
    size_t changeCount() const;
    
    // Configuration metadata
    void setDescription(const std::string& key, const std::string& description);
    void setCategory(const std::string& key, const std::string& category);
    void setGroup(const std::string& key, const std::string& group);
    void setReadOnly(const std::string& key, bool read_only);
    void setRequiresRestart(const std::string& key, bool requires_restart);
    
    std::string getDescription(const std::string& key) const;
    std::string getCategory(const std::string& key) const;
    std::string getGroup(const std::string& key) const;
    bool isReadOnly(const std::string& key) const;
    bool requiresRestart(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    bool differsFrom(const Config& other) const;
    
    // Configuration merge
    void merge(const Config& other, ConfigSource source = ConfigSource::Override);
    void merge(const std::vector<std::pair<std::string, ConfigValue>>& key_values, ConfigSource source = ConfigSource::Override);
    
    // Configuration reset
    void resetToDefaults();
    void resetToFile(const std::string& filepath);
    void resetToCommandLine(const std::vector<std::string>& args);
    
    // Configuration validation rules
    using ValidationRule = std::function<ValidationResult(const ConfigValue&)>;
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    void removeValidationRule(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void notifyChangeCallbacks(const std::string& key, const ConfigValue& old_value, const ConfigValue& new_value);
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    void applyValidationRules(const std::string& key, const ConfigValue& value);
    std::string generateUniqueCallbackId() const;
};

// Configuration template functions
inline Config createDefaultConfig() {
    Config config;
    // Set default Phase 6 configuration
    config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config.set("episodic_memory_max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    return config;
}

inline Config createAdvancedConfig() {
    Config config = createDefaultConfig();
    // Enhanced settings for advanced agents
    config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config.set("connection_probability", 0.2f, ConfigSource::Default);
    config.set("planning_depth", 5, ConfigSource::Default);
    config.set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    config.set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    return config;
}

} // namespace nlm
