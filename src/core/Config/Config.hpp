#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>
#include <set>

namespace nlm {

// Forward declarations
class Config;
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
    Runtime
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

// Configuration validation error types
enum class ValidationErrorType {
    MissingRequiredField,
    TypeMismatch,
    ValueOutOfRange,
    InvalidValue,
    ConfigurationConflict,
    UnknownField
};

// Configuration validation error
struct ValidationError {
    ValidationErrorType type;
    std::string field;
    std::string message;
    std::string value;
    
    ValidationError(ValidationErrorType t, const std::string& f, const std::string& msg, const std::string& val = "")
        : type(t), field(f), message(msg), value(val) {}
};

// Configuration validation rule
struct ValidationRule {
    std::string field;
    ValidationErrorType errorType;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    
    ValidationRule(const std::string& f, ValidationErrorType et, const std::string& desc,
                   std::function<bool(const ConfigValue&)> val)
        : field(f), errorType(et), description(desc), validator(val) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::vector<ValidationRule> rules;
    std::set<std::string> requiredFields;
    std::vector<std::string> recommendedFields;
    
    ConfigSchema(const std::string& n) : name(n) {}
    
    void addRule(const std::string& field, ValidationErrorType errorType, 
                 const std::string& description, std::function<bool(const ConfigValue&)>,
                 const std::string& ruleName = "");
    
    void addRequiredField(const std::string& field, const std::string& description = "");
    
    void addRecommendedField(const std::string& field, const std::string& description = "");
    
    std::vector<ValidationError> validate(const Config& config) const;
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
    
    // Validation methods
    std::vector<ValidationError> validate() const;
    bool isValid() const;
    void setSchema(const ConfigSchema& schema);
    void setValidator(const ConfigValidator& validator);
    
    // Configuration presets
    static Config createDefaultConfig();
    static Config createExplorationConfig();
    static Config createExploitationConfig();
    static Config createBalancedConfig();
    static Config createReproductionConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

// Common validation functions
namespace nlm {
    namespace validators {
        // Type validation
        bool isInteger(const ConfigValue& value);
        bool isDouble(const ConfigValue& value);
        bool isBoolean(const ConfigValue& value);
        bool isString(const ConfigValue& value);
        bool isArray(const ConfigValue& value);
        
        // Range validation
        bool inRange(int value, int min, int max);
        bool inRange(double value, double min, double max);
        bool inRange(size_t value, size_t min, size_t max);
        
        // String validation
        bool isNotEmpty(const ConfigValue& value);
        bool matchesPattern(const ConfigValue& value, const std::string& pattern);
        
        // Configuration-specific validators
        bool validNeuronCount(const ConfigValue& value);
        bool validRegionCount(const ConfigValue& value);
        bool validConnectionProbability(const ConfigValue& value);
        bool validLearningRate(const ConfigValue& value);
        bool validTimeStep(const ConfigValue& value);
        bool valid dopamineBaseline(const ConfigValue& value);
        bool validCuriosityRate(const ConfigValue& value);
        bool validNoveltyThreshold(const ConfigValue& value);
        
        // Compound validators
        bool validPlasticityParameters(const ConfigValue& ltp, const ConfigValue& ltd, const ConfigValue& tau);
        bool validMemoryConfiguration(const Config& config);
        bool validNeuromodulationConfiguration(const Config& config);
    }
    
    // Standard configuration schemas
    namespace schemas {
        extern ConfigSchema DefaultNLMConfig;
        extern ConfigSchema ExplorationConfig;
        extern ConfigSchema ExploitationConfig;
        extern ConfigSchema BalancedConfig;
        extern ConfigSchema ReproductionConfig;
    }
}
