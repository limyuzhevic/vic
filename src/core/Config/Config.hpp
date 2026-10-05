#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>
#include <algorithm>
#include <functional>

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
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

// Advanced configuration profiles
enum class AdvancedConfigProfile {
    Standard,      // Balanced performance and accuracy
    HighPrecision, // Maximum accuracy with performance trade-off
    HighPerformance, // Maximum speed with lower accuracy
    Custom        // User-defined parameters
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string summary;
    double score; // Validation score 0.0 to 1.0
};

// Configuration validation rules
struct ValidationRule {
    std::string parameter;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string warningMessage;
};

// Configuration optimizer
class ConfigOptimizer {
public:
    static ValidationResult validate(const Config& config);
    static std::shared_ptr<Config> optimizeForProfile(AdvancedConfigProfile profile);
    static std::shared_ptr<Config> autoTune(const Config& baseConfig, double targetPerformance);
    static std::vector<std::string> getRecommendedParameters();
    
private:
    static std::vector<ValidationRule> getDefaultValidationRules();
};

// Configuration profile definitions
class ConfigProfiles {
public:
    static std::shared_ptr<Config> getStandardProfile();
    static std::shared_ptr<Config> getHighPrecisionProfile();
    static std::shared_ptr<Config> getHighPerformanceProfile();
    static std::shared_ptr<Config> getResearchProfile(); // For experimental setups
    static std::shared_ptr<Config> getProductionProfile(); // For production systems
    
private:
    static std::shared_ptr<Config> createProfile(const std::string& name);
};

// Extended configuration class with advanced features
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Profile management
    void setProfile(AdvancedConfigProfile profile);
    AdvancedConfigProfile getProfile() const { return profile_; }
    std::string getProfileName() const;
    
    // Configuration validation and optimization
    ValidationResult validate() const;
    bool isValid() const { return validationResult_.isValid; }
    std::string getValidationSummary() const { return validationResult_.summary; }
    
    // Performance tuning
    void setTargetPerformance(double target); // 0.0 to 1.0
    double getTargetPerformance() const { return targetPerformance_; }
    
    // Memory management
    void enableMemoryPool(bool enable);
    bool isMemoryPoolEnabled() const { return memoryPoolEnabled_; }
    
    // Thread safety
    void setThreadSafe(bool enable);
    bool isThreadSafe() const { return threadSafe_; }
    
    // Debug and monitoring
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const { return debugMode_; }
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const { return logLevel_; }
    
    // Configuration export/import
    std::string exportToJson() const;
    bool importFromJson(const std::string& json);
    void saveProfile(const std::string& filename) const;
    bool loadProfile(const std::string& filename);
    
    // Configuration comparison
    double compareWith(const AdvancedConfig& other) const;
    std::vector<std::string> getDifferences(const AdvancedConfig& other) const;
    
    // Configuration utilities
    std::vector<std::string> getUnusedParameters() const;
    std::vector<std::string> getOverriddenParameters() const;
    size_t getTotalMemoryUsage() const;
    
    // Statistical analysis
    struct ConfigStats {
        size_t totalParameters = 0;
        size_t usedParameters = 0;
        size_t defaultParameters = 0;
        size_t userParameters = 0;
        double memoryUsageMB = 0.0;
        double performanceScore = 0.0;
        std::chrono::system_clock::time_point lastModified;
    };
    
    ConfigStats getStats() const;
    
    // Iteration and comparison operators
    bool operator==(const AdvancedConfig& other) const;
    bool operator!=(const AdvancedConfig& other) const;
    bool operator<(const AdvancedConfig& other) const;
    
private:
    AdvancedConfigProfile profile_;
    ValidationResult validationResult_;
    double targetPerformance_;
    bool memoryPoolEnabled_;
    bool threadSafe_;
    bool debugMode_;
    LogLevel logLevel_;
    
    // Extended configuration storage
    std::map<std::string, ConfigValue> advancedParameters_;
    std::map<std::string, std::chrono::system_clock::time_point> modificationHistory_;
    
    // Validation and optimization
    void runValidation();
    void updatePerformanceScore();
    void updateMemoryUsage();
    
    // Helper methods
    void initializeDefaultAdvancedParameters();
    void applyProfileSettings(AdvancedConfigProfile profile);
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

// Parameter validation rules
struct ParameterValidation {
    std::string name;
    std::string description;
    std::string unit;
    bool required;
    std::optional<std::function<bool(const ConfigValue&)>> validator;
    std::optional<std::function<double(const ConfigValue&)>> normalizer;
    ConfigSource defaultSource;
    std::string category;
    int order;
};

// Default parameter definitions
class DefaultParameters {
public:
    static std::vector<ParameterValidation> getAll() {
        return {
            // Brain parameters
            ParameterValidation{"brain.neuron_count", "Total number of neurons", "neurons", true, 
                [](const ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) > 0; },
                nullptr, ConfigSource::Default, "brain", 10},
            
            ParameterValidation{"brain.region_count", "Number of brain regions", "regions", true,
                [](const ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) >= 1; },
                nullptr, ConfigSource::Default, "brain", 20},
            
            ParameterValidation{"brain.synapse_density", "Synapse connection probability", "0.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                nullptr, ConfigSource::File, "brain", 30},
            
            // Neuron dynamics parameters
            ParameterValidation{"neuron.v_thresh", "Neuron threshold potential", "mV", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v); },
                nullptr, ConfigSource::Default, "neuron", 40},
            
            ParameterValidation{"neuron.v_rest", "Resting potential", "mV", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v); },
                nullptr, ConfigSource::Default, "neuron", 50},
            
            ParameterValidation{"neuron.v_reset", "Reset potential", "mV", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v); },
                nullptr, ConfigSource::Default, "neuron", 60},
            
            // Plasticity parameters
            ParameterValidation{"plasticity.stdp.learning_rate", "STDP learning rate", "0.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
                nullptr, ConfigSource::Default, "plasticity", 70},
            
            ParameterValidation{"plasticity.stdp.tau", "STDP time constant", "ms", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
                nullptr, ConfigSource::Default, "plasticity", 80},
            
            ParameterValidation{"plasticity.hebbian.learning_rate", "Hebbian learning rate", "0.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
                nullptr, ConfigSource::Default, "plasticity", 90},
            
            // Neuromodulation parameters
            ParameterValidation{"neuromod.dopamine.scale", "Dopamine modulation scale", "-1.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= -1.0 && std::get<double>(v) <= 1.0; },
                nullptr, ConfigSource::Default, "neuromod", 100},
            
            ParameterValidation{"neuromod.curiosity.scale", "Curiosity exploration scale", "0.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                nullptr, ConfigSource::Default, "neuromod", 110},
            
            // Development parameters
            ParameterValidation{"development.critical_period.duration", "Critical period duration", "s", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
                nullptr, ConfigSource::Default, "development", 120},
            
            ParameterValidation{"development.maturation.rate", "Maturation rate", "0.0-1.0", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                nullptr, ConfigSource::Default, "development", 130},
            
            // Memory parameters
            ParameterValidation{"memory.working.capacity", "Working memory capacity", "slots", false,
                [](const ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) > 0; },
                nullptr, ConfigSource::Default, "memory", 140},
            
            ParameterValidation{"memory.episodic.max_entries", "Maximum episodic memory entries", "entries", false,
                [](const ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) > 0; },
                nullptr, ConfigSource::Default, "memory", 150},
            
            // Simulation parameters
            ParameterValidation{"simulation.timestep", "Simulation timestep", "ms", false,
                [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
                nullptr, ConfigSource::Default, "simulation", 160},
            
            ParameterValidation{"simulation.max_steps", "Maximum simulation steps", "steps", false,
                [](const ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) > 0; },
                nullptr, ConfigSource::Default, "simulation", 170}
        };
    }
};

// Advanced neuron parameters for fine-tuning
class AdvancedNeuronParameters {
public:
    static std::map<std::string, ConfigValue> getDefaults() {
        return {
            {"neuron.leak_conductance", 12.0},
            {"neuron.refractory_period", 8},
            {"neuron.adaptation_strength", 0.5},
            {"neuron.spike_detection_threshold", 1.0},
            {"neuron.numerical_stability_threshold", 50.0},
            {"neuron.max_spike_history", 200},
            {"neuron.memory_optimization_level", 1},
            {"neuron.thread_safety_level", 2},
            {"neuron.debug_level", 1},
            {"neuron.performance_target", 0.8},
            {"neuron.automatic_optimization", true},
            {"neuron.stability_checking", true},
            {"neuron.error_reporting", true},
            {"neuron.metrics_collection", true},
            {"neuron.checkpoint_frequency", 1000},
            {"neuron.history_retention_days", 7}
        };
    }
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Advanced configuration methods
    void setAdvancedParameter(const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getAdvancedParameter(const std::string& key) const;
    bool hasAdvancedParameter(const std::string& key) const;
    void removeAdvancedParameter(const std::string& key);
    
    // Configuration profiling
    void setProfile(AdvancedConfigProfile profile);
    AdvancedConfigProfile getProfile() const { return profile_; }
    
    // Performance optimization
    void autoOptimize();
    void setTargetPerformance(double target);
    double getCurrentPerformance() const;
    
    // Validation
    ValidationResult validateAdvanced() const;
    bool isAdvancedValid() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Profile management
    AdvancedConfigProfile profile_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
