#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <chrono>
#include <any>
#include <stdexcept>

namespace nlm {

// Forward declarations
class ConfigValidator;

// Configuration value types with enhanced support
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

// Configuration source with enhanced tracking
enum class ConfigSource {
    Default,
    File,
    Environment,
    CommandLine,
    Runtime,
    Template
};

// Parameter validation categories
enum class ParameterCategory {
    Neural,
    Plasticity,
    Neuromodulation,
    Development,
    Reward,
    Environment,
    Logging,
    Simulation,
    Visualization,
    Memory,
    Cognition,
    Performance,
    Custom
};

// Parameter type for validation
enum class ParameterType {
    Integer,
    Float,
    Boolean,
    String,
    IntegerArray,
    FloatArray,
    StringArray,
    Object
};

// Validation severity levels
enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Critical
};

// Parameter validation error
struct ValidationError {
    std::string parameter;
    std::string message;
    ValidationSeverity severity;
    
    ValidationError(const std::string& param, const std::string& msg, 
                   ValidationSeverity sev = ValidationSeverity::Error)
        : parameter(param), message(msg), severity(sev) {}
};

// Parameter constraint definition
struct ParameterConstraint {
    ParameterType type;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<size_t> minLength;
    std::optional<size_t> maxLength;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ParameterConstraint(ParameterType t = ParameterType::Float)
        : type(t) {}
};

// Parameter metadata
struct ParameterMetadata {
    std::string key;
    std::string description;
    ParameterCategory category;
    ParameterConstraint constraint;
    bool required;
    ConfigSource defaultSource;
    bool hidden;
    std::string group;
    ParameterMetadata() 
        : category(ParameterCategory::Custom), required(false), 
          defaultSource(ConfigSource::Default), hidden(false) {}
    
    ParameterMetadata(const std::string& k, const std::string& desc, 
                     ParameterCategory cat = ParameterCategory::Custom)
        : key(k), description(desc), category(cat), required(false), 
          defaultSource(ConfigSource::Default), hidden(false) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    ParameterCategory category;
    std::vector<ParameterMetadata> parameters;
    std::unordered_map<std::string, std::string> dependencies; // param -> dependent param
    std::unordered_map<std::string, std::function<bool(const Config&)>> validationRules;
    
    ConfigSchema(const std::string& n, ParameterCategory cat = ParameterCategory::Custom)
        : name(n), category(cat) {}
    
    void addParameter(const ParameterMetadata& param) {
        parameters.push_back(param);
    }
    
    bool hasParameter(const std::string& key) const {
        return std::any_of(parameters.begin(), parameters.end(),
                          [&key](const ParameterMetadata& p) { return p.key == key; });
    }
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ParameterMetadata metadata;
    std::chrono::system_clock::time_point timestamp;
    bool modified;
    
    ConfigEntry() : source(ConfigSource::Default), modified(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), modified(false) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const ParameterMetadata& meta)
        : key(k), value(v), source(s), description(meta.description), 
          metadata(meta), modified(false) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Enhanced configuration class with validation, inheritance, and dynamic features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set validation mode
    void setValidationEnabled(bool enabled) { validationEnabled_ = enabled; }
    bool isValidationEnabled() const { return validationEnabled_; }
    
    // Parameter registration with schema
    void registerParameter(const ParameterMetadata& metadata);
    void registerSchema(const ConfigSchema& schema);
    void setParameterGroup(const std::string& group, const ConfigSchema& schema);
    
    // Enhanced set methods with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& group = "");
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& group = "");
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& group = "");
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& group = "");
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& group = "");
    
    // Enhanced get methods
    template<typename T>
    std::optional<T> get(const std::string& key, const std::string& group = "") const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, 
            const std::string& group = "") const;
    
    // Check existence and get metadata
    bool has(const std::string& key, const std::string& group = "") const;
    std::optional<ParameterMetadata> getMetadata(const std::string& key, 
                                                 const std::string& group = "") const;
    
    // Remove and inheritance
    void remove(const std::string& key, const std::string& group = "");
    std::string getInheritedValue(const std::string& key, 
                                  const std::string& group = "") const;
    
    // Parameter groups and categories
    std::vector<std::string> getGroups() const;
    std::vector<ParameterMetadata> getParameters(const std::string& group = "") const;
    std::vector<ParameterCategory> getCategories() const;
    
    // Advanced operations
    void clear();
    void clear(const std::string& group);
    std::vector<std::string> getKeys(const std::string& group = "") const;
    std::vector<ConfigEntry> getEntries(const std::string& group = "") const;
    
    // Schema management
    bool hasSchema(const std::string& schemaName) const;
    std::optional<ConfigSchema> getSchema(const std::string& schemaName) const;
    std::vector<std::string> getSchemaNames() const;
    
    // Environment variable support
    void setEnvironmentVariablePrefix(const std::string& prefix) {
        envVarPrefix_ = prefix;
    }
    std::string getEnvironmentVariablePrefix() const { return envVarPrefix_; }
    bool loadFromEnvironment();
    
    // Dynamic configuration features
    void enableHotSwapping(bool enabled) { hotSwappingEnabled_ = enabled; }
    bool isHotSwappingEnabled() const { return hotSwappingEnabled_; }
    bool hasChangedSince(const std::chrono::system_clock::time_point& time) const;
    std::chrono::system_clock::time_point getLastModifiedTime() const;
    
    // File I/O with multiple formats
    bool loadFromFile(const std::string& filepath, const std::string& group = "");
    bool saveToFile(const std::string& filepath, const std::string& group = "") const;
    bool loadFromJSON(const std::string& filepath, const std::string& group = "");
    bool saveToJSON(const std::string& filepath, const std::string& group = "") const;
    bool loadFromYAML(const std::string& filepath, const std::string& group = "");
    bool saveToYAML(const std::string& filepath, const std::string& group = "") const;
    
    // Configuration templates
    bool loadTemplate(const std::string& templateName);
    std::vector<std::string> getTemplateNames() const;
    bool saveTemplate(const std::string& templateName) const;
    
    // Validation and error reporting
    std::vector<ValidationError> validate() const;
    std::vector<ValidationError> validate(const std::string& group) const;
    bool validateParameter(const std::string& key, const std::string& group = "") const;
    void setValidator(std::shared_ptr<ConfigValidator> validator) {
        validator_ = validator;
    }
    
    // Configuration comparison and merging
    bool equals(const Config& other) const;
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    void override(const Config& other);
    
    // Runtime monitoring and change detection
    void enableMonitoring(bool enabled) { monitoringEnabled_ = enabled; }
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)> 
    getChangeCallback() const { return changeCallback_; }
    void setChangeCallback(std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)> callback) {
        changeCallback_ = callback;
    }
    
    // Thread safety
    void lock() { configMutex_.lock(); }
    void unlock() { configMutex_.unlock(); }
    bool tryLock() { return configMutex_.try_lock(); }
    
    // Summary and debugging
    std::string summary(const std::string& group = "") const;
    std::string schemaSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void emitChangeEvent(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static ConfigValue parseValue(const std::string& str, const std::string& group = "");
    
    // State management
    bool validationEnabled_;
    bool monitoringEnabled_;
    bool hotSwappingEnabled_;
    std::string envVarPrefix_;
    std::shared_ptr<ConfigValidator> validator_;
    std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)> changeCallback_;
    std::chrono::system_clock::time_point lastModifiedTime_;
    mutable std::mutex configMutex_;
    
    // Schema and metadata management
    std::unordered_map<std::string, ConfigSchema> schemas_;
    std::unordered_map<std::string, std::string> groupToSchema_;
    std::unordered_map<std::string, ParameterMetadata> parameterRegistry_;
    std::vector<std::string> templateNames_;
};

// Global configuration management
class GlobalConfig {
public:
    static GlobalConfig& getInstance() {
        static GlobalConfig instance;
        return instance;
    }
    
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Convenience methods
    template<typename T>
    std::optional<T> get(const std::string& key) {
        return config_ ? config_->get<T>(key) : std::nullopt;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) {
        return config_ ? config_->getOr<T>(key, defaultValue) : defaultValue;
    }
    
private:
    GlobalConfig() : config_(std::make_shared<Config>()) {}
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    
    std::shared_ptr<Config> config_;
};

} // namespace nlm