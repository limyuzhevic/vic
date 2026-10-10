#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include "Config.hpp"

namespace nlm {

// Validation error
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& key = "") 
        : std::runtime_error("Validation Error: " + message + (key.empty() ? "" : " (key: " + key + ")")) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "vector<T>"
    bool required;
    std::string description;
    std::variant<
        int,
        double,
        std::string,
        bool,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    > defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigSchemaEntry() = default;
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, bool req, const std::string& desc,
                     const std::variant<int, double, std::string, bool, std::vector<int>, 
                                          std::vector<double>, std::vector<std::string>>& def,
                     std::function<bool(const ConfigValue&)> valid)
        : key(k), type(t), required(req), description(desc), 
          defaultValue(def), validator(valid) {}
};

// Enhanced configuration manager with validation and schema
class AdvancedConfig {
public:
    AdvancedConfig() = default;
    ~AdvancedConfig() = default;
    
    // Disable copying
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Add schema for validation
    void addSchema(const ConfigSchemaEntry& schema);
    
    // Validate current configuration against schema
    bool validate() const;
    
    // Get value with type checking and validation
    template<typename T>
    T get(const std::string& key) const;
    
    // Get value with default and validation
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set value with schema validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Extended set methods for common types
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if key exists and is valid according to schema
    bool has(const std::string& key) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get configuration summary with validation status
    std::string summary() const;
    
    // Save to file with validation
    bool saveToFile(const std::string& filepath) const;
    
    // Reset to defaults
    void resetToDefaults();
    
    // Merge with another configuration
    void merge(const AdvancedConfig& other);
    
private:
    // Internal config storage
    Config config_;
    
    // Schema for validation
    std::vector<ConfigSchemaEntry> schema_;
    
    // Validation helpers
    bool validateType(const std::string& expectedType, const ConfigValue& value) const;
    bool validateValue(const ConfigSchemaEntry& schema, const ConfigValue& value) const;
    std::string typeToString(const ConfigValue& value) const;
    
    // Schema-specific validators
    static bool validateInt(const ConfigValue& value);
    static bool validateDouble(const ConfigValue& value);
    static bool validateString(const ConfigValue& value);
    static bool validateBool(const ConfigValue& value);
    static bool validateVector(const ConfigValue& value);
    
    // Type checking helpers
    template<typename T>
    bool isType(const ConfigValue& value) const;
    
    // Helper to find schema entry for key
    const ConfigSchemaEntry* findSchema(const std::string& key) const;
};

// Template implementations for AdvancedConfig

inline template<typename T>
T AdvancedConfig::get(const std::string& key) const {
    auto value = config_.get<T>(key);
    if (!value.has_value()) {
        throw std::runtime_error("Config key not found: " + key);
    }
    
    // Validate against schema if present
    const ConfigSchemaEntry* schema = findSchema(key);
    if (schema && !validateValue(*schema, ConfigValue(value.value()))) {
        throw ValidationError("Value validation failed for key: " + key);
    }
    
    return value.value();
}

inline template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = config_.getOr<T>(key, defaultValue);
    
    // Validate against schema if present
    const ConfigSchemaEntry* schema = findSchema(key);
    if (schema && !validateValue(*schema, ConfigValue(value))) {
        // Fall back to default if validation fails
        return defaultValue;
    }
    
    return value;
}

// Static type validators
inline bool AdvancedConfig::validateInt(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
}

inline bool AdvancedConfig::validateDouble(const ConfigValue& value) {
    return std::holds_alternative<double>(value);
}

inline bool AdvancedConfig::validateString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

inline bool AdvancedConfig::validateBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

inline bool AdvancedConfig::validateVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

inline template<typename T>
bool AdvancedConfig::isType(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        return std::holds_alternative<T>(value) || std::holds_alternative<int>(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::holds_alternative<double>(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return std::holds_alternative<std::string>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return std::holds_alternative<bool>(value);
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        return std::holds_alternative<std::vector<int>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        return std::holds_alternative<std::vector<double>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

// Phase 6 config with validation
struct Phase6AdvancedConfig {
    // Neuron and region settings
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Performance settings
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableNovelty;
    bool enablePrediction;
    
    // Feature flags
    bool enableStructuralPlasticity;
    bool enableSynaptogenesis;
    bool enablePruning;
    bool enableSTDP;
    bool enableHebbian;
    
    // Simulation parameters
    double timestep;
    float simulationTimestep;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTau;
    float synaptogenesisRate;
    float pruningRate;
    
    // Memory settings
    size_t workingMemoryCapacity;
    size_t maxEpisodicEpisodes;
    
    // Development settings
    double developmentalDuration;
    float plasticityModulation;
    
    // Output and logging
    std::string checkpointPath;
    std::string logLevel;
    bool enableBenchmarking;
    
    Phase6AdvancedConfig() 
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableCuriosity(true)
        , enableNovelty(true)
        , enablePrediction(true)
        , enableStructuralPlasticity(true)
        , enableSynaptogenesis(true)
        , enablePruning(true)
        , enableSTDP(true)
        , enableHebbian(true)
        , timestep(1.0)
        , simulationTimestep(0.001f)
        , stdpLTPWeight(0.01f)
        , stdpLTDWeight(0.012f)
        , stdpTau(20.0f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
        , workingMemoryCapacity(100)
        , maxEpisodicEpisodes(1000)
        , developmentalDuration(60.0)
        , plasticityModulation(1.0f)
        , checkpointPath("./checkpoint_test.bin")
        , logLevel("INFO")
        , enableBenchmarking(false) {}
    
    // Convert to AdvancedConfig
    AdvancedConfig toAdvancedConfig() const;
};

// Advanced experiment runner
class AdvancedPhase6IntegratedExperiment {
public:
    AdvancedPhase6IntegratedExperiment();
    ~AdvancedPhase6IntegratedExperiment();
    
    // Run experiment with advanced configuration
    Phase6IntegrationResult run(const Phase6AdvancedConfig& config);
    
    // Validate configuration
    static bool validateConfig(const Phase6AdvancedConfig& config);
    
    // Get default configuration
    static Phase6AdvancedConfig getDefaultConfig();
    
    // Get validation errors for configuration
    static std::vector<std::string> validateConfigErrors(const Phase6AdvancedConfig& config);
    
    // Merge configuration with command line args
    static Phase6AdvancedConfig mergeWithArgs(const Phase6AdvancedConfig& config, int argc, char** argv);
    
private:
    // Internal implementation details
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
