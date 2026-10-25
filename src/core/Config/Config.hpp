// Config.hpp enhancements - add JSON/YAML support
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types with type safety
class ConfigValue {
public:
    enum class Type {
        Int,
        Int64,
        Double,
        Bool,
        String,
        StringArray,
        DoubleArray,
        IntArray,
        Unknown
    };

private:
    Type type_;
    std::any value_;

public:
    // Constructors
    ConfigValue() : type_(Type::Unknown) {}
    ConfigValue(int value) : type_(Type::Int), value_(value) {}
    ConfigValue(int64_t value) : type_(Type::Int64), value_(value) {}
    ConfigValue(double value) : type_(Type::Double), value_(value) {}
    ConfigValue(bool value) : type_(Type::Bool), value_(value) {}
    ConfigValue(const std::string& value) : type_(Type::String), value_(value) {}
    ConfigValue(const std::vector<int>& value) : type_(Type::IntArray), value_(value) {}
    ConfigValue(const std::vector<double>& value) : type_(Type::DoubleArray), value_(value) {}
    ConfigValue(const std::vector<std::string>& value) : type_(Type::StringArray), value_(value) {}

    // Type checking
    Type type() const { return type_; }
    
    bool isInt() const { return type_ == Type::Int; }
    bool isInt64() const { return type_ == Type::Int64; }
    bool isDouble() const { return type_ == Type::Double; }
    bool isBool() const { return type_ == Type::Bool; }
    bool isString() const { return type_ == Type::String; }
    bool isStringArray() const { return type_ == Type::StringArray; }
    bool isDoubleArray() const { return type_ == Type::DoubleArray; }
    bool isIntArray() const { return type_ == Type::IntArray; }
    
    // Value access
    int asInt() const { return std::any_cast<int>(value_); }
    int64_t asInt64() const { return std::any_cast<int64_t>(value_); }
    double asDouble() const { return std::any_cast<double>(value_); }
    bool asBool() const { return std::any_cast<bool>(value_); }
    const std::string& asString() const { return std::any_cast<std::string>(value_); }
    const std::vector<int>& asIntArray() const { return std::any_cast<std::vector<int>>(value_); }
    const std::vector<double>& asDoubleArray() const { return std::any_cast<std::vector<double>>(value_); }
    const std::vector<std::string>& asStringArray() const { return std::any_cast<std::vector<std::string>>(value_); }
    
    // Default values
    static ConfigValue defaultInt(int val = 0) { return ConfigValue(val); }
    static ConfigValue defaultDouble(double val = 0.0) { return ConfigValue(val); }
    static ConfigValue defaultBool(bool val = false) { return ConfigValue(val); }
    static ConfigValue defaultString(const std::string& val = "") { return ConfigValue(val); }
    
    // Type name for debugging
    static const char* typeName(Type type) {
        switch (type) {
            case Type::Int: return "int";
            case Type::Int64: return "int64_t";
            case Type::Double: return "double";
            case Type::Bool: return "bool";
            case Type::String: return "string";
            case Type::StringArray: return "string[]";
            case Type::DoubleArray: return "double[]";
            case Type::IntArray: return "int[]";
            case Type::Unknown: return "unknown";
            default: return "unknown";
        }
    }
    
    std::string typeName() const { return typeName(type_); }
    
    // Comparison
    bool operator==(const ConfigValue& other) const { return type_ == other.type_ && value_ == other.value_; }
    bool operator!=(const ConfigValue& other) const { return !(*this == other); }
};

// Enhanced configuration source with more granular control
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
   SystemDefault
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool isSecret = false; // For sensitive values
    bool isReadOnly = false; // Cannot be changed at runtime
    std::vector<std::string> dependsOn; // Keys that must be set first
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   isSecret(false), isReadOnly(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
               const std::string& desc = "") 
        : key(k), value(v), source(s), description(desc),
          isSecret(false), isReadOnly(false) {}
};

// Enhanced configuration change callback system
using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;

// Enhanced configuration validation system
using ConfigValidator = std::function<bool(const ConfigValue& value, std::string& errorMessage)>;

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigValue::Type expectedType;
    bool required = false;
    ConfigValue defaultValue;
    std::string description;
    ConfigValidator validator;
    
    ConfigSchema() : expectedType(ConfigValue::Type::Unknown), required(false) {}
    ConfigSchema(const std::string& k, ConfigValue::Type type, bool req = false,
                const ConfigValue& def = ConfigValue(),
                const std::string& desc = "", 
                const ConfigValidator& val = nullptr)
        : key(k), expectedType(type), required(req), defaultValue(def),
          description(desc), validator(val) {}
};

// Enhanced configuration class with validation, callbacks, and schema support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON
    std::string toJSON() const;
    
    // Validate configuration against schema
    bool validate(const std::vector<ConfigSchema>& schema, std::string& errorMessage) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-specific get methods
    std::optional<int> getInt(const std::string& key) const;
    std::optional<int64_t> getInt64(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<int>> getIntArray(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleArray(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringArray(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Enhanced set methods for specific types
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
                const std::string& description = "");
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
                 const std::string& description = "");
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by type
    std::vector<std::string> getKeysOfType(ConfigValue::Type type) const;
    
    // Get entries with source information
    std::vector<ConfigEntry> getEntries() const;
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration callbacks
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
    // Validation system
    void addValidator(const std::string& key, const ConfigValidator& validator);
    void removeValidator(const std::string& key);
    
    // Schema management
    void setSchema(const std::vector<ConfigSchema>& schema);
    const std::vector<ConfigSchema>& getSchema() const;
    
    // Batch operations
    void batchSet(const std::map<std::string, ConfigValue>& values);
    
    // File format detection
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static bool isSimpleFormat(const std::string& content);
    
    // Get file modification time for change detection
    static time_t getFileModificationTime(const std::string& filepath);
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void clearError() { lastError_.clear(); }
    
    // Statistics
    size_t entryCount() const { return pImpl->entries.size(); }
    size_t entryCount(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string lastError_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isQuoted(const std::string& str);
    static std::string unquote(const std::string& str);
    static std::string quote(const std::string& str);
    
    // Parse different file formats
    bool parseSimpleFormat(const std::string& content, const std::string& filepath);
    bool parseJSON(const std::string& content, const std::string& filepath);
    bool parseYAML(const std::string& content, const std::string& filepath);
    
    // Configuration change notification
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Validation
    bool validateValue(const std::string& key, const ConfigValue& value, std::string& errorMessage) const;
    
    // Schema validation
    bool validateSchema(const ConfigSchema& schema, const ConfigEntry& entry) const;
};

// Convenience functions for common configuration patterns
namespace ConfigHelpers {
    // Common configuration keys with proper naming
    namespace Keys {
        const std::string RandomSeed = "random_seed";
        const std::string SimulationTimestep = "simulation_timestep";
        const std::string NeuronCount = "neuron_count";
        const std::string RegionCount = "region_count";
        const std::string ConnectionProbability = "connection_probability";
        const std::string STDP_LTP_Weight = "stdp_ltp_weight";
        const std::string STDP_LTD_Weight = "stdp_ltd_weight";
        const std::string STDP_Tau = "stdp_tau";
        const std::string SynaptogenesisRate = "synaptogenesis_rate";
        const std::string PruningRate = "pruning_rate";
        const std::string CuriosityStrength = "curiosity_strength";
        const std::string NoveltyThreshold = "novelty_threshold";
        const std::string RewardScale = "reward_scale";
        const std::string Verbose = "verbose";
        const std::string Quiet = "quiet";
        const std::string Progress = "progress";
    }
    
    // Common configuration schemas
    std::vector<ConfigSchema> getDefaultSchema();
    
    // Common validators
    bool validatePositive(const ConfigValue& value, std::string& errorMessage);
    bool validateNonNegative(const ConfigValue& value, std::string& errorMessage);
    bool validateRange(const ConfigValue& value, double min, double max, std::string& errorMessage);
}

} // namespace nlm
