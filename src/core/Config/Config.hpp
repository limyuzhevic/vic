#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <filesystem>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Preset
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult(bool valid = true, const std::string& error = "",
                    const std::vector<std::string>& warnings = {})
        : isValid(valid), errorMessage(error), warnings(warnings) {}
};

// Configuration type for validation and schemas
enum class ConfigType {
    Integer,
    Float,
    Boolean,
    String,
    StringList,
    IntegerList,
    FloatList,
    Custom
};

// Configuration constraint
struct ConfigConstraint {
    std::string key;
    ConfigType type;
    std::string description;
    std::string defaultValue;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    bool required;
    
    ConfigConstraint(const std::string& k, ConfigType t, const std::string& desc = "",
                    const std::string& def = "", bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // ========== BASIC OPERATIONS ==========
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience typed setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setVector(const std::string& key, const std::vector<double>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // ========== ADVANCED OPERATIONS ==========
    
    // Configuration validation
    ValidationResult validate() const;
    ValidationResult validate(const std::vector<ConfigConstraint>& constraints) const;
    
    // Type-safe getter methods
    int getInt(const std::string& key, int defaultValue = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    std::vector<double> getVector(const std::string& key, const std::vector<double>& defaultValue = {}) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration presets
    bool loadPreset(const std::string& presetName);
    void addPreset(const std::string& name, const Config& config);
    std::vector<std::string> getPresetNames() const;
    
    // Configuration profiles
    std::string createProfile(const std::string& profileName) const;
    bool saveProfile(const std::string& profileName) const;
    bool loadProfile(const std::string& profileName);
    std::vector<std::string> getProfileNames() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Configuration information
    std::string getDescription(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    std::vector<ConfigEntry> getEntries() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    double getMemoryUsage() const;
    
    // ========== FACTORY AND PRESETS ==========
    
    // Factory methods for common configurations
    static Config createDefaultConfig();
    static Config createHighPerformanceConfig();
    static Config createBalancedConfig();
    static Config createResearchConfig();
    static Config createProductionConfig();
    
    // Neuron configuration presets
    static Config createNeuronConfig(int neuronCount, float connectionProbability = 0.15f);
    static Config createLIFConfig(float capacitance = 0.5f, float timeConstant = 20.0f);
    static Config createPlasticityConfig(bool hebbian = true, bool stdp = true, bool reward = true);
    
    // Agent configuration presets
    static Config createAgentConfig(const std::string& mode = "balanced");
    static Config createSimpleAgentConfig();
    static Config createAdvancedAgentConfig();
    
    // Environment configuration presets
    static Config createWorldConfig(int width = 20, int height = 20);
    static Config createSimpleWorldConfig();
    static Config createComplexWorldConfig();
    
    // ========== UTILITY AND HELP ==========
    
    // Configuration validation schemas
    static std::vector<ConfigConstraint> getDefaultConstraints();
    static std::vector<ConfigConstraint> getNeuronConstraints();
    static std::vector<ConfigConstraint> getAgentConstraints();
    static std::vector<ConfigConstraint> getWorldConstraints();
    
    // Configuration help and documentation
    static std::string getConfigHelp(const std::string& key = "");
    static std::string getConfigExamples();
    static std::string getPresetHelp(const std::string& presetName = "");
    
    // Configuration validation rules
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::string& key, const ConfigValue& value);
    static std::string validateKey(const std::string& key);
    static std::string validateValue(const std::string& key, const ConfigValue& value);
    
    // Configuration conversion
    static ConfigValue stringToConfigValue(const std::string& value);
    static std::string configValueToString(const ConfigValue& value);
    
    // Configuration search and filtering
    std::vector<std::string> findKeysByPattern(const std::string& pattern) const;
    std::vector<std::string> findKeysByType(ConfigType type) const;
    std::vector<ConfigEntry> filterBySource(ConfigSource source) const;
    
    // Configuration backup and recovery
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    static std::string getCurrentBackupPath();
    static void setBackupPath(const std::string& path);
    
    // Configuration export/import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json);
    std::string exportToINI() const;
    bool importFromINI(const std::string& ini);
    
    // Configuration monitoring
    void addChangeListener(const std::string& key, std::function<void(const ConfigEntry&)> listener);
    void removeChangeListener(const std::string& key);
    void clearChangeListeners();
    
    // Configuration statistics and monitoring
    std::string getStatistics() const;
    double getAverageValueForType(ConfigType type) const;
    size_t getConfigurationComplexity() const;
    
    // Configuration validation and debugging
    bool validateConstraints(const std::vector<ConfigConstraint>& constraints) const;
    std::vector<std::string> getConstraintViolations(const std::vector<ConfigConstraint>& constraints) const;
    std::string getValidationReport(const std::vector<ConfigConstraint>& constraints) const;
    
    // Configuration optimization
    Config optimizeForPerformance() const;
    Config optimizeForMemory() const;
    Config optimizeForBalanced() const;
    
    // Configuration profiling
    static Config createProfilingConfig(const std::string& profileName);
    bool isProfilingEnabled() const;
    void enableProfiling(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKeyName(const std::string& key);
    static ConfigType inferConfigType(const ConfigValue& value);
    static std::string configValueToStringInternal(const ConfigValue& value);
    
    // Change notification
    void notifyChange(const ConfigEntry& entry);
    
    // Validation helpers
    static ValidationResult validateValue(const ConfigConstraint& constraint, const ConfigValue& value) const;
    static std::string getConfigTypeName(ConfigType type);
    
    // Preset management
    static std::unordered_map<std::string, Config> getBuiltInPresets();
    static std::unordered_map<std::string, std::vector<ConfigConstraint>> getBuiltInConstraintSets();
    
    // Profile management
    static std::filesystem::path getProfilesDirectory();
    static std::filesystem::path getBackupDirectory();
    
    friend class ConfigImpl;
};

// Configuration implementation class
class ConfigImpl {
public:
    static ValidationResult validate(const Config& config, const std::vector<ConfigConstraint>& constraints);
    static std::string generateReport(const Config& config, const std::vector<ConfigConstraint>& constraints);
    static Config optimize(const Config& config, const std::string& optimizationGoal);
    
private:
    static bool validateAgainstConstraint(const ConfigEntry& entry, const ConfigConstraint& constraint);
    static std::string getValueString(const ConfigValue& value);
};

} // namespace nlm
