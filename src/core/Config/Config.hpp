#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", 
                         const std::string& expected = "") 
        : std::runtime_error(message), key_(key), expected_(expected) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getExpected() const { return expected_; }
    
private:
    std::string key_;
    std::string expected_;
};

// Configuration type
enum class ConfigDataType {
    Integer,
    Integer64,
    Float,
    Double,
    Boolean,
    String,
    StringVector,
    DoubleVector,
    IntVector
};

// Configuration entry metadata
struct ConfigMetadata {
    std::string description;
    std::string unit;
    ConfigDataType type;
    bool required;
    std::any defaultValue;
    std::string validationRegex;
    std::function<bool(const std::any&)> validator;
    
    ConfigMetadata()
        : description(""), unit(""), type(ConfigDataType::String)
        , required(false), validator(nullptr) {}
    
    ConfigMetadata(const std::string& desc, ConfigDataType t, bool req = false)
        : description(desc), unit(""), type(t), required(req), validator(nullptr) {}
    
    template<typename T>
    ConfigMetadata& withDefault(const T& value) {
        defaultValue = value;
        return *this;
    }
    
    ConfigMetadata& withUnit(const std::string& unit) {
        this->unit = unit;
        return *this;
    }
    
    ConfigMetadata& withValidator(std::function<bool(const std::any&)> validator) {
        this->validator = validator;
        return *this;
    }
    
    std::string getTypeString() const {
        switch (type) {
            case ConfigDataType::Integer: return "int";
            case ConfigDataType::Integer64: return "int64_t";
            case ConfigDataType::Float: return "float";
            case ConfigDataType::Double: return "double";
            case ConfigDataType::Boolean: return "bool";
            case ConfigDataType::String: return "string";
            case ConfigDataType::StringVector: return "vector<string>";
            case ConfigDataType::DoubleVector: return "vector<double>";
            case ConfigDataType::IntVector: return "vector<int>";
            default: return "unknown";
        }
    }
};

// Enhanced configuration class with type safety and validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading with format detection
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to various formats
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    std::string toJSON(bool pretty = true) const;
    std::string toINI() const;
    
    // Type-safe getters with validation
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getWithValidation(const std::string& key, const T& defaultValue = T()) const;
    
    // Set values with source tracking
    void set(const std::string& key, const std::any& value, 
             ConfigSource source = ConfigSource::Runtime, 
             const std::string& description = "");
    
    // Type-specific setters for convenience
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
                const std::string& description = "");
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
                  const std::string& description = "");
    void setFloat(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime,
                  const std::string& description = "");
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
                 const std::string& description = "");
    void setString(const std::string& key, const std::string& value,
                   ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    
    // Configuration schema management
    static Config& getDefaultConfig();
    static bool registerConfigSchema(const std::string& key, const ConfigMetadata& metadata);
    static const ConfigMetadata& getConfigMetadata(const std::string& key);
    static std::vector<std::string> getAllConfigKeys();
    
    // Configuration validation
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    
    // Configuration inspection
    std::string summary() const;
    std::string getInfo(const std::string& key) const;
    
    // Configuration source management
    ConfigSource getSource(const std::string& key) const;
    bool isOverridden(const std::string& key) const;
    std::string getSourceString(ConfigSource source) const;
    
    // Environment variable integration
    void setEnvPrefix(const std::string& prefix) { envPrefix_ = prefix; }
    static std::string toEnvVarName(const std::string& key);
    
    // Batch operations
    void setMultiple(const std::map<std::string, std::any>& values,
                     ConfigSource source = ConfigSource::Runtime);
    std::map<std::string, std::any> getMultiple(const std::vector<std::string>& keys) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string envPrefix_;
    
    // Internal helpers
    bool validateValue(const std::string& key, const std::any& value) const;
    std::string getTypeString(const std::any& value) const;
    bool convertToAny(const std::string& key, const std::string& value);
    
    // JSON conversion helpers
    static nlohmann::json anyToJSON(const std::any& value);
    static std::any jsonToAny(const nlohmann::json& json);
    
    // Configuration schema
    static std::map<std::string, ConfigMetadata> configSchema_;
};

// Configuration schema definitions
class ConfigSchema {
public:
    // Core configuration
    static ConfigMetadata NEURON_COUNT;
    static ConfigMetadata REGION_COUNT;
    static ConfigMetadata CONNECTION_PROBABILITY;
    static ConfigMetadata SIMULATION_TIMESTEP;
    static ConfigMetadata RANDOM_SEED;
    
    // Neuron dynamics
    static ConfigMetadata LIF_RESTING_POTENTIAL;
    static ConfigMetadata LIF_THRESHOLD;
    static ConfigMetadata LIF_RESET_POTENTIAL;
    static ConfigMetadata LIF_LEAK_CONDUCTANCE;
    static ConfigMetadata LIF_REFACTORY_PERIOD;
    
    // Synaptic properties
    static ConfigMetadata SYNAPSE_WEIGHT_MIN;
    static ConfigMetadata SYNAPSE_WEIGHT_MAX;
    static ConfigMetadata SYNAPSE_DELAY_MIN;
    static ConfigMetadata SYNAPSE_DELAY_MAX;
    static ConfigMetadata STDP_LTP_WEIGHT;
    static ConfigMetadata STDP_LTD_WEIGHT;
    static ConfigMetadata STDP_TIME_CONSTANT;
    
    // Plasticity
    static ConfigMetadata SYNAPTOGENESIS_RATE;
    static ConfigMetadata PRUNING_RATE;
    static ConfigMetadata HEBBIAN_LEARNING_RATE;
    
    // Memory systems
    static ConfigMetadata WORKING_MEMORY_CAPACITY;
    static ConfigMetadata EPISODIC_MEMORY_DURATION;
    static ConfigMetadata EPISODIC_MEMORY_COMPRESSION;
    
    // Neuromodulation
    static ConfigMetadata DOPAMINE_GAIN;
    static ConfigMetadata NOVELTY_SENSITIVITY;
    static ConfigMetadata CURIOSITY_THRESHOLD;
    static ConfigMetadata PREDICTION_ERROR_DECAY;
    
    // Development
    static ConfigMetadata DEVELOPMENT_DURATION;
    static ConfigMetadata PLASTICITY_MODIFIER_RATE;
    static ConfigMetadata CRITICAL_PERIOD_START;
    static ConfigMetadata CRITICAL_PERIOD_END;
    
    // Environment
    static ConfigMetadata WORLD_WIDTH;
    static ConfigMetadata WORLD_HEIGHT;
    static ConfigMetadata SENSOR_RANGE;
    static ConfigMetadata MOTOR_SPEED;
    
    // Experiment configuration
    static ConfigMetadata EXPERIMENT_DURATION;
    static ConfigMetadata REWARD_FUNCTION_TYPE;
    static ConfigMetadata NOISE_LEVEL;
    static ConfigMetadata CHECKPOINT_INTERVAL;
    
    // Performance
    static ConfigMetadata MAX_THREADS;
    static ConfigMetadata CACHE_SIZE;
    static ConfigMetadata ENABLE_SIMD;
    static ConfigMetadata ENABLE_OPENMP;
    
    // Debug and logging
    static ConfigMetadata LOG_LEVEL;
    static ConfigMetadata LOG_FILE;
    static ConfigMetadata ENABLE_PERFORMANCE_METRICS;
    static ConfigMetadata METRICS_INTERVAL;
    
    // Advanced features
    static ConfigMetadata ENABLE_HEDONIC_REWARD;
    static ConfigMetadata ENABLE_TEMPORAL_DIFFRENCE_LEARNING;
    static ConfigMetadata ENABLE_HIERARCHICAL_SYSTEMS;
    static ConfigMetadata ENABLE_TRANSFER_LEARNING;
};

// Configuration helper functions
namespace ConfigHelper {
    // Configuration validation functions
    bool isPositive(const std::any& value);
    bool isNonNegative(const std::any& value);
    bool isInRange(const std::any& value, double min, double max);
    bool isStringMatchRegex(const std::any& value, const std::string& regex);
    bool isVectorNonEmpty(const std::any& value);
    
    // Configuration conversion functions
    std::string toString(const std::any& value);
    double toDouble(const std::any& value);
    int toInt(const std::any& value);
    bool toBool(const std::any& value);
    
    // Configuration merging
    void mergeConfigs(Config& target, const Config& source, ConfigSource sourcePriority);
    
    // Configuration comparison
    bool configsEqual(const Config& config1, const Config& config2);
    
    // Configuration cloning
    Config cloneConfig(const Config& original);
}

} // namespace nlm
