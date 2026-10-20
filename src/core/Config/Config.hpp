#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <regex>
#include <functional>
#include <set>
#include <stdexcept>
#include <nlohmann/json.hpp>

// Include path for nlohmann/json might need to be added to CMakeLists.txt

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;
class ConfigTransaction;

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

// Error categories
enum class ConfigErrorCategory {
    Validation,
    TypeMismatch,
    MissingRequired,
    ConstraintViolation,
    DependencyFailure,
    Serialization,
    Transaction,
    Schema
};

// Error severity
enum class ConfigErrorSeverity {
    Warning,
    Error,
    Critical
};

// Configuration error
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message, 
                ConfigErrorCategory category,
                ConfigErrorSeverity severity = ConfigErrorSeverity::Error,
                const std::string& key = "",
                const std::string& context = "")
        : std::runtime_error(message), 
          category_(category),
          severity_(severity),
          key_(key),
          context_(context) {}
    
    ConfigErrorCategory getCategory() const { return category_; }
    ConfigErrorSeverity getSeverity() const { return severity_; }
    const std::string& getKey() const { return key_; }
    const std::string& getContext() const { return context_; }
    
    static std::string categoryToString(ConfigErrorCategory category);
    static std::string severityToString(ConfigErrorSeverity severity);
    
private:
    ConfigErrorCategory category_;
    ConfigErrorSeverity severity_;
    std::string key_;
    std::string context_;
};

// Typed configuration entry for type-safe access
template<typename T>
class TypedConfigEntry {
public:
    TypedConfigEntry(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
                     const std::string& description = "")
        : key_(key), value_(value), source_(source), description_(description) {}
    
    const std::string& getKey() const { return key_; }
    T getValue() const { return value_; }
    ConfigSource getSource() const { return source_; }
    const std::string& getDescription() const { return description_; }
    
    void setValue(const T& value) { value_ = value; }
    void setSource(ConfigSource source) { source_ = source; }
    
    bool operator==(const TypedConfigEntry& other) const {
        return key_ == other.key_ && value_ == other.value_;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "TypedConfigEntry[" << key_ << ":";
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << value_ << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (value_ ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < value_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << value_[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < value_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << value_[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < value_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << value_[i] << "\"";
            }
            oss << "]";
        } else {
            oss << value_;
        }
        oss << "(source=" << static_cast<int>(source_) << ")]";
        return oss.str();
    }
    
private:
    std::string key_;
    T value_;
    ConfigSource source_;
    std::string description_;
};

// Schema constraint types
enum class ConstraintType {
    None,
    Min,
    Max,
    Enum,
    Regex,
    Length,
    Custom
};

// Schema constraint
struct SchemaConstraint {
    ConstraintType type;
    std::string value;
    std::string errorMessage;
    
    SchemaConstraint(ConstraintType t, const std::string& v, const std::string& msg = "")
        : type(t), value(v), errorMessage(msg) {}
};

// Configuration schema class
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add validation rule for a key
    ConfigSchema& addValidation(const std::string& key,
                                ConstraintType type,
                                const std::string& constraintValue,
                                const std::string& errorMessage = "");
    
    // Add required key
    ConfigSchema& addRequired(const std::string& key, const std::string& errorMessage = "");
    
    // Add dependency rule (key A requires key B)
    ConfigSchema& addDependency(const std::string& dependentKey,
                                const std::string& requiredKey,
                                const std::string& errorMessage = "");
    
    // Add enum constraint
    ConfigSchema& addEnum(const std::string& key,
                         const std::vector<std::string>& allowedValues,
                         const std::string& errorMessage = "");
    
    // Add range constraint (for numeric values)
    ConfigSchema& addRange(const std::string& key,
                          double minValue,
                          double maxValue,
                          const std::string& errorMessage = "");
    
    // Add regex constraint (for string values)
    ConfigSchema& addRegex(const std::string& key,
                          const std::string& pattern,
                          const std::string& errorMessage = "");
    
    // Validate configuration against schema
    std::vector<ConfigError> validate(const Config& config) const;
    
    // Clear all rules
    void clear() { rules_.clear(); }
    
    // Get all required keys
    std::vector<std::string> getRequiredKeys() const;
    
private:
    struct ValidationRule {
        std::string key;
        ConstraintType type;
        std::string constraintValue;
        std::string errorMessage;
        
        bool isRequired = false;
        std::string dependsOn;
        
        std::string toString() const {
            return "Rule[key=" + key + ",type=" + std::to_string(static_cast<int>(type)) + "]";
        }
    };
    
    std::vector<ValidationRule> rules_;
    
    static std::string constraintTypeToString(ConstraintType type);
    static bool matchesConstraint(const ConfigValue& value, const SchemaConstraint& constraint);
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
    
    // Type-safe get methods
    template<typename T>
    std::optional<TypedConfigEntry<T>> getTyped(const std::string& key) const;
    
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
    
    // Type conversion utilities
    static std::string valueToString(const ConfigValue& value);
    template<typename T>
    static std::optional<T> convertToType(const ConfigValue& value);
    template<typename T>
    static bool canConvertToType(const ConfigValue& value);
    
    // Type introspection
    static ConfigValue typeToValue(int val) { return ConfigValue(val); }
    static ConfigValue typeToValue(int64_t val) { return ConfigValue(val); }
    static ConfigValue typeToValue(double val) { return ConfigValue(val); }
    static ConfigValue typeToValue(bool val) { return ConfigValue(val); }
    static ConfigValue typeToValue(const std::string& val) { return ConfigValue(val); }
    static ConfigValue typeToValue(const std::vector<int>& val) { return ConfigValue(val); }
    static ConfigValue typeToValue(const std::vector<double>& val) { return ConfigValue(val); }
    static ConfigValue typeToValue(const std::vector<std::string>& val) { return ConfigValue(val); }
    
    // Validation
    void setSchema(std::shared_ptr<ConfigSchema> schema) { schema_ = schema; }
    std::shared_ptr<ConfigSchema> getSchema() const { return schema_; }
    
    // Advanced querying
    std::vector<std::string> queryByPattern(const std::string& regex_pattern) const;
    std::vector<std::string> queryBySource(ConfigSource source_filter) const;
    
    // Configuration tree navigation
    std::string configTreeNavigate(const std::string& path_separator = ".") const;
    std::map<std::string, std::vector<std::string>> getConfigHierarchy(const std::vector<std::string>& root_keys) const;
    
    // Find similar configurations
    std::vector<std::string> findSimilarConfigurations(double threshold, const std::vector<std::string>& exclude_keys = {}) const;
    
    // Configuration statistics
    std::string getConfigStatistics(bool include_source_stats = true) const;
    
    // Batch operations
    void batchSet(const std::map<std::string, ConfigValue>& config_dict, ConfigSource source = ConfigSource::Runtime);
    void batchUpdate(const std::vector<std::pair<std::string, ConfigValue>>& config_updates);
    void batchRemove(const std::vector<std::string>& keys);
    bool batchImport(const std::string& json_data, const std::string& merge_strategy = "merge");
    std::string batchExport(bool include_sensitive = false) const;
    
    // Configuration presets
    bool applyConfigurationPreset(const std::string& preset_name);
    
    // Configuration validation
    bool validateConfigurationSet(const std::map<std::string, ConfigValue>& config_dict) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& json);
};

} // namespace nlm
