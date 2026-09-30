#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <set>
#include <chrono>
#include <type_traits>
#include <numeric>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration parameter types with validation
enum class ParameterType {
    Integer,
    Float,
    Boolean,
    String,
    VectorInt,
    VectorFloat,
    VectorString,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    ActionType
};

// Parameter validation rules
struct ValidationRules {
    ParameterType type;
    std::string description;
    std::optional<std::string> defaultValue;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<size_t> minSize;
    std::optional<size_t> maxSize;
    bool required = false;
    bool caseSensitive = true;
    std::set<std::string> allowedValues;
    
    // Validation function
    std::function<bool(const ConfigValue&)> validator;
    
    ValidationRules(ParameterType t, const std::string& desc) 
        : type(t), description(desc), required(false) {}
};

// Enhanced configuration entry with validation
struct ValidatedConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ValidationRules validation;
    std::chrono::system_clock::time_point timestamp;
    bool isValidated = false;
    
    ValidatedConfigEntry() : source(ConfigSource::Default), timestamp(std::chrono::system_clock::now()) {}
    ValidatedConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                        const std::string& desc, const ValidationRules& rules)
        : key(k), value(v), source(s), description(desc), validation(rules), 
          timestamp(std::chrono::system_clock::now()), isValidated(false) {}
};

// Configuration parameter schema
struct ParameterSchema {
    std::string key;
    ParameterType type;
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    std::map<std::string, std::string> metadata;
    
    // Type-specific constraints
    union Constraints {
        struct IntRange {
            int64_t min;
            int64_t max;
            double step;
        } intRange;
        
        struct FloatRange {
            double min;
            double max;
            double step;
        } floatRange;
        
        struct StringLength {
            size_t minLength;
            size_t maxLength;
        } stringLength;
        
        struct VectorConstraints {
            size_t minSize;
            size_t maxSize;
            ParameterType elementType;
        } vectorConstraints;
    } constraints;
    
    ConstraintsType constraintType;
};

// Enum for constraint types
enum class ConstraintsType {
    None,
    IntRange,
    FloatRange,
    StringLength,
    VectorConstraints
};

// Enhanced configuration value type
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<NeuronId>,
    std::vector<SynapseId>,
    std::vector<RegionId>,
    std::vector<PopulationId>
>;

// Enhanced configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Generated,
    Inherited
};

// Configuration error
struct ConfigError {
    std::string key;
    std::string message;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigError(const std::string& k, const std::string& msg, ConfigSource s)
        : key(k), message(msg), source(s), timestamp(std::chrono::system_clock::now()) {}
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
    
    // Load from file (enhanced JSON/YAML support)
    bool loadFromFile(const std::string& filepath, bool validateOnLoad = true);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv, bool validateOnLoad = true);
    
    // Save to file with optional formatting
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Enhanced get methods with type safety
    template<typename T>
    std::optional<T> get(const std::string& key, bool validate = true) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, bool validate) const;
    
    // Set methods with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Vector parameter setters
    void set(const std::string& key, const std::vector<int>& values, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& values, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced validation and checking
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    ParameterType getParameterType(const std::string& key) const;
    
    // Vector getters
    std::vector<int> getVectorInt(const std::string& key, bool validate = true) const;
    std::vector<double> getVectorFloat(const std::string& key, bool validate = true) const;
    std::vector<std::string> getVectorString(const std::string& key, bool validate = true) const;
    
    // Enhanced parameter management
    void remove(const std::string& key);
    void remove(const std::string& key, bool validate);
    
    // Get all keys with optional filtering
    std::vector<std::string> getKeys(bool includeDefaults = false) const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Enhanced configuration summary with statistics
    std::string summary() const;
    std::string detailedSummary() const;
    
    // Validation methods
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    bool validate(const std::string& key) const;
    std::vector<std::string> validate() const;
    
    // Schema management
    void addParameterSchema(const ParameterSchema& schema);
    bool hasParameterSchema(const std::string& key) const;
    ParameterSchema getParameterSchema(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    bool similar(const Config& other, double tolerance = 0.01) const;
    
    // Configuration modification
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void replace(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Statistical methods
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    double getMemoryUsage() const;
    std::chrono::system_clock::time_point getLastModified() const;
    
    // File format support
    bool canLoadFormat(const std::string& format) const;
    std::vector<std::string> getSupportedFormats() const;
    
    // Configuration export/import
    std::string exportToString(bool pretty = false) const;
    bool importFromString(const std::string& configString);
    
    // Configuration health
    bool isHealthy() const;
    std::string getHealthReport() const;
    
    // Configuration version control
    void setVersion(const std::string& version);
    std::string getVersion() const;
    bool needsMigration(const std::string& targetVersion) const;
    
    // Configuration backups
    bool createBackup(const std::string& backupPath) const;
    bool restoreBackup(const std::string& backupPath);
    
    // Configuration monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::map<std::string, std::string> getMonitoringData() const;
    
    // Configuration validation rules
    void addValidationRule(const std::string& key, 
                          const std::function<bool(const ConfigValue&)>& validator,
                          const std::string& errorMessage);
    void removeValidationRule(const std::string& key);
    
    // Configuration parameter constraints
    void addParameterConstraint(const std::string& key,
                               ParameterType expectedType,
                               const std::string& constraintDescription,
                               const std::map<std::string, std::string>& constraints);
    void removeParameterConstraint(const std::string& key);
    std::string getParameterConstraints(const std::string& key) const;
    
    // Configuration batch operations
    void batchSet(const std::map<std::string, ConfigValue>& keyValues,
                 ConfigSource source = ConfigSource::Runtime);
    void batchRemove(const std::vector<std::string>& keys);
    std::map<std::string, ConfigValue> batchGet(const std::vector<std::string>& keys) const;
    
    // Configuration export/import with schema
    bool exportWithSchema(const std::string& filepath) const;
    bool importWithSchema(const std::string& filepath);
    
    // Configuration statistics
    std::string getStatistics() const;
    std::map<std::string, double> getPerformanceMetrics() const;
    
    // Configuration snapshot
    std::string createSnapshot() const;
    bool restoreSnapshot(const std::string& snapshotId);
    
    // Configuration analytics
    std::string getUsagePatterns() const;
    std::map<std::string, double> getParameterUsageStats(const std::string& key) const;
    
    // Configuration change tracking
    std::vector<std::string> getRecentChanges(size_t limit = 10) const;
    bool hasChangedSince(const std::string& snapshotId) const;
    
    // Configuration validation helpers
    static bool isValidInteger(const ConfigValue& value, int64_t min = INT64_MIN, int64_t max = INT64_MAX);
    static bool isValidFloat(const ConfigValue& value, double min = -std::numeric_limits<double>::max(), 
                           double max = std::numeric_limits<double>::max());
    static bool isValidString(const ConfigValue& value, size_t minLength = 0, size_t maxLength = SIZE_MAX);
    static bool isValidVector(const ConfigValue& value, size_t minSize = 0, size_t maxSize = SIZE_MAX);
    
    // Configuration type conversion
    static std::string parameterTypeToString(ParameterType type);
    static ParameterType stringToParameterType(const std::string& typeStr);
    static std::string configValueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToConfigValue(const std::string& value, ParameterType type);
    
    // Configuration error handling
    std::vector<ConfigError> getRecentErrors() const;
    void clearErrors();
    
    // Configuration templates
    static std::shared_ptr<Config> createTemplate(const std::string& name);
    static bool validateTemplate(const std::shared_ptr<Config>& config, 
                                const std::string& templateName);
    
    // Configuration validation presets
    static ValidationRules getBrainNeuronCountValidation();
    static ValidationRules getConnectionProbabilityValidation();
    static ValidationRules getLearningRateValidation();
    static ValidationRules getSimulationTimestepValidation();
    static ValidationRules getDefaultValidation(ParameterType type, const std::string& description);
    
    // Configuration preset collections
    static std::map<std::string, std::shared_ptr<Config>> getCommonConfigurations();
    static std::shared_ptr<Config> getConfiguration(const std::string& presetName);
    
    // Configuration comparison operators
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Configuration stream operators
    friend std::ostream& operator<<(std::ostream& os, const Config& config);
    friend std::istream& operator>>(std::istream& is, Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateType(const ConfigValue& value, ParameterType expectedType);
    static bool validateConstraints(const ConfigValue& value, const ParameterSchema& schema);
    static std::string getTypeString(const ConfigValue& value);
    static bool isValueInAllowedSet(const ConfigValue& value, const std::set<std::string>& allowedValues);
    
    // Error handling
    void logError(const std::string& key, const std::string& message, ConfigSource source);
    void validateEntry(const ValidatedConfigEntry& entry) const;
    
    // Schema validation
    bool validateAgainstSchema(const std::string& key, const ConfigValue& value) const;
    
    // Monitoring data
    mutable std::map<std::string, std::string> monitoringData;
    bool monitoringEnabled = false;
    
    // Error tracking
    mutable std::vector<ConfigError> recentErrors;
    
    // Parameter schemas
    std::map<std::string, ParameterSchema> parameterSchemas;
    
    // Validation rules
    std::map<std::string, std::pair<std::function<bool(const ConfigValue&)>, std::string>> validationRules;
    
    // Parameter constraints
    std::map<std::string, std::map<std::string, std::string>> parameterConstraints;
    
    // Version control
    std::string configVersion;
    std::map<std::string, std::string> versionHistory;
    
    // Usage statistics
    std::map<std::string, std::map<std::string, double>> usageStats;
    
    // Snapshot management
    std::map<std::string, std::string> snapshots;
    
    // Change tracking
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> recentChanges;
};
