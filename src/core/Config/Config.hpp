#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

// Forward declarations
class Config;
class AdvancedConfig;
class NetworkConfig;
class TrainingConfig;
class SystemConfig;
class ConfigValidator;

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
    Runtime,
    Preset,
    Inherited
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string type;
    
    // Validation constraints
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), category(), type() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "", 
                const std::string& t = "")
        : key(k), value(v), source(s), description(desc), category(cat), type(t) {}
};

// Validation error type
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& key = "", 
                    const std::string& path = "") 
        : std::runtime_error(message), key_(key), path_(path) {}
    
    std::string getKey() const { return key_; }
    std::string getPath() const { return path_; }
    
private:
    std::string key_;
    std::string path_;
};

// Parameter validator base class
class Validator {
public:
    virtual ~Validator() = default;
    virtual void validate(const ConfigValue& value, const std::string& key) const = 0;
    virtual std::string getError(const ConfigValue& value, const std::string& key) const = 0;
};

// Range validator
class RangeValidator : public Validator {
public:
    RangeValidator(double min, double max, bool inclusive = true)
        : min_(min), max_(max), inclusive_(inclusive) {}
    
    void validate(const ConfigValue& value, const std::string& key) const override {
        double val = toDouble(value);
        if (inclusive_) {
            if (val < min_ || val > max_) {
                throw ValidationError("Value " + std::to_string(val) + " is outside valid range [" + 
                                    std::to_string(min_) + ", " + std::to_string(max_) + "] for key " + key);
            }
        } else {
            if (val <= min_ || val >= max_) {
                throw ValidationError("Value " + std::to_string(val) + " is outside valid range (" + 
                                    std::to_string(min_) + ", " + std::to_string(max_) + "] for key " + key);
            }
        }
    }
    
    std::string getError(const ConfigValue& value, const std::string& key) const override {
        double val = toDouble(value);
        return "Value " + std::to_string(val) + " is outside valid range [" + 
               std::to_string(min_) + ", " + std::to_string(max_) + "] for key " + key;
    }
    
private:
    double toDouble(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        throw ValidationError("Cannot convert value to double for range validation");
    }
    
    double min_;
    double max_;
    bool inclusive_;
};

// Allowed values validator
class AllowedValuesValidator : public Validator {
public:
    AllowedValuesValidator(const std::vector<std::string>& allowed) 
        : allowedValues_(allowed) {}
    
    void validate(const ConfigValue& value, const std::string& key) const override {
        std::string val = toString(value);
        if (std::find(allowedValues_.begin(), allowedValues_.end(), val) == allowedValues_.end()) {
            throw ValidationError("Value \"" + val + "\" is not in allowed list for key " + key);
        }
    }
    
    std::string getError(const ConfigValue& value, const std::string& key) const override {
        std::string val = toString(value);
        std::string allowed = "";
        for (size_t i = 0; i < allowedValues_.size(); ++i) {
            if (i > 0) allowed += ", ";
            allowed += "\"" + allowedValues_[i] + "\"";
        }
        return "Value \"" + val + "\" is not in allowed list [" + allowed + "] for key " + key;
    }
    
private:
    std::string toString(const ConfigValue& value) const {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        throw ValidationError("Cannot convert value to string for allowed values validation");
    }
    
    std::vector<std::string> allowedValues_;
};

// Type converter utilities
class TypeConverter {
public:
    static std::string toString(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        if (std::holds_alternative<std::vector<int>>(value)) {
            return vectorToString(std::get<std::vector<int>>(value));
        }
        if (std::holds_alternative<std::vector<double>>(value)) {
            return vectorToString(std::get<std::vector<double>>(value));
        }
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            return vectorToString(std::get<std::vector<std::string>>(value));
        }
        return "";
    }
    
    static double toDouble(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        throw ValidationError("Cannot convert value to double");
    }
    
    static int toInt(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return std::get<int>(value);
        if (std::holds_alternative<int64_t>(value)) return static_cast<int>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return static_cast<int>(std::get<double>(value));
        throw ValidationError("Cannot convert value to int");
    }
    
    static bool toBool(const ConfigValue& value) {
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
        if (std::holds_alternative<std::string>(value)) {
            std::string str = std::get<std::string>(value);
            return str == "true" || str == "1" || str == "yes";
        }
        throw ValidationError("Cannot convert value to bool");
    }
    
private:
    static std::string vectorToString(const std::vector<int>& vec) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    }
    
    static std::string vectorToString(const std::vector<double>& vec) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    }
    
    static std::string vectorToString(const std::vector<std::string>& vec) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
        return ss.str();
    }
};

// Configuration presets
struct PresetConfig {
    std::string name;
    std::string description;
    std::unordered_map<std::string, ConfigValue> values;
    ConfigSource source = ConfigSource::Preset;
    
    PresetConfig(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced set with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", 
             const std::string& category = "", 
             const std::string& type = "",
             const std::optional<double>& minValue = std::nullopt,
             const std::optional<double>& maxValue = std::nullopt,
             const std::vector<std::string>& allowedValues = {});
    
    // Convenience methods
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Existing methods (backward compatibility)
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // New enhanced methods
    void validate() const;
    std::vector<ValidationError> validateWithErrors() const;
    std::string validateReport() const;
    
    // Configuration presets
    static bool savePreset(const PresetConfig& preset);
    static std::optional<PresetConfig> loadPreset(const std::string& name);
    static std::vector<std::string> getPresetNames();
    
    // Inheritance and composition
    bool mergeFrom(const Config& other, ConfigSource source = ConfigSource::Inherited);
    std::shared_ptr<Config> createSubset(const std::vector<std::string>& keys) const;
    
    // Runtime modification
    void applyPreset(const PresetConfig& preset);
    void optimize(const std::string& optimizationStrategy = "balance");
    
    // Configuration templates
    bool createTemplate(const std::string& name, const std::string& description);
    bool applyTemplate(const std::string& name);
    static std::vector<std::string> getTemplateNames();
    
    // Expert features
    std::vector<std::string> searchKeys(const std::string& pattern) const;
    std::string getConfigurationId() const;
    std::tm getCreationTime() const;
    void setCreationTime(const std::tm& time);
    
    // Validation utilities
    static bool validateValue(const ConfigValue& value, const std::string& type);
    static std::string getDefaultValue(const std::string& key);
    static std::string getValueTypeName(const ConfigValue& value);
    
    // Comparison utilities
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Utility functions
    static std::string generateConfigId();
    static std::tm currentTime();
    
    // Configuration categories
    static const std::string NEURAL_NETWORK_CATEGORY;
    static const std::string TRAINING_CATEGORY;
    static const std::string MEMORY_CATEGORY;
    static const std::string NEUROMODULATION_CATEGORY;
    static const std::string SYSTEM_CATEGORY;
    static const std::string PLASTICITY_CATEGORY;
    static const std::string DEVELOPMENT_CATEGORY;
    
    // Configuration types
    static const std::string TYPE_INT;
    static const std::string TYPE_DOUBLE;
    static const std::string TYPE_STRING;
    static const std::string TYPE_BOOL;
    static const std::string TYPE_INT_VECTOR;
    static const std::string TYPE_DOUBLE_VECTOR;
    static const std::string TYPE_STRING_VECTOR;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::vector<Validator*> createValidators(const ConfigEntry& entry);
    
    // Static configuration stores
    static std::unordered_map<std::string, PresetConfig> presets_;
    static std::unordered_map<std::string, std::string> templates_;
    static std::unordered_map<std::string, std::string> defaultValues_;
};

// Advanced configuration with additional features
class AdvancedConfig : public Config {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Advanced validation with detailed error reporting
    std::string detailedValidationReport() const;
    
    // Performance optimization
    double benchmark() const;
    
    // Configuration search and filtering
    std::vector<std::string> findByCategory(const std::string& category) const;
    std::vector<std::string> findByType(const std::string& type) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::string getSourceDistribution() const;
    
    // Version control
    std::string getVersion() const;
    void setVersion(const std::string& version);
    
    // Metadata
    std::string getAuthor() const;
    void setAuthor(const std::string& author);
    
    std::string getDescription() const;
    void setDescription(const std::string& description);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImplAdvanced;
};

// Neural network specific configuration
class NetworkConfig : public AdvancedConfig {
public:
    NetworkConfig();
    ~NetworkConfig();
    
    // Neural architecture parameters
    void setLayerCount(int layers);
    void setNeuronCount(int neurons);
    void setInputSize(int size);
    void setOutputSize(int size);
    
    // Connection parameters
    void setConnectionProbability(double prob);
    void setMaxConnectionsPerNeuron(int max);
    void setRecurrentConnections(bool recurrent);
    
    // Neuron parameters
    void setNeuronThreshold(double threshold);
    void setNeuronDecay(double decay);
    void setNeuronRefractoryPeriod(double period);
    
    // Network topology
    void setTopologyType(const std::string& topology);
    void setSmallWorldParameter(double parameter);
    void setClusteringCoefficient(double coefficient);
    
    // Get network configuration summary
    std::string getNetworkSummary() const;
};

// Training specific configuration
class TrainingConfig : public AdvancedConfig {
public:
    TrainingConfig();
    ~TrainingConfig();
    
    // Training hyperparameters
    void setLearningRate(double rate);
    void setBatchSize(int size);
    void setEpochs(int epochs);
    void setOptimizerType(const std::string& optimizer);
    
    // Regularization
    void setL1Regularization(double lambda);
    void setL2Regularization(double lambda);
    void setDropoutRate(double rate);
    void setEarlyStoppingPatience(int patience);
    
    // Learning rules
    void setLearningRule(const std::string& rule);
    void setLearningRateSchedule(const std::string& schedule);
    void setMinibatchScheduling(const std::string& scheduling);
    
    // Get training configuration summary
    std::string getTrainingSummary() const;
};

// System configuration
class SystemConfig : public AdvancedConfig {
public:
    SystemConfig();
    ~SystemConfig();
    
    // Runtime system parameters
    void setThreadCount(int threads);
    void setMemoryLimit(size_t limit);
    void setTimeStep(double timestep);
    void setMaxTime(double maxTime);
    
    // Logging and monitoring
    void setLogLevel(const std::string& level);
    void setLogFile(const std::string& file);
    void setPerformanceMonitoring(bool enabled);
    void setProfilingEnabled(bool enabled);
    
    // Get system configuration summary
    std::string getSystemSummary() const;
};

// Configuration utilities
namespace ConfigUtils {
    // Configuration validation functions
    std::vector<ValidationError> validateConfig(const Config& config);
    std::string generateValidationReport(const Config& config);
    
    // Configuration comparison functions
    bool configsEqual(const Config& config1, const Config& config2);
    std::string compareConfigs(const Config& config1, const Config& config2);
    
    // Configuration merging functions
    Config mergeConfigs(const Config& config1, const Config& config2, 
                        ConfigSource source = ConfigSource::Inherited);
    
    // Configuration optimization functions
    Config optimizeConfig(const Config& config, 
                         const std::string& strategy = "balance");
    
    // Configuration search and filtering
    std::vector<std::string> searchConfigs(const Config& config, 
                                           const std::string& pattern);
    
    // Configuration benchmarking
    class Benchmarker {
    public:
        Benchmarker(Config config);
        double runBenchmark() const;
        std::string getBenchmarkReport() const;
        
    private:
        Config config_;
    };
    
    // Configuration export/import
    bool exportConfig(const Config& config, const std::string& filepath);
    std::optional<Config> importConfig(const std::string& filepath);
    
    // Configuration templates
    bool saveTemplate(const std::string& name, const std::string& description, 
                      const Config& config);
    std::optional<Config> applyTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates();
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}