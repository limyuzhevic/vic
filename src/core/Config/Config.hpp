#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <set>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigSchema;
class ConfigTemplate;

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
    Template,
    Inherited
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string error_type;
    std::string message;
    std::string severity;
    std::string suggestion;
    
    ValidationError(const std::string& k, const std::string& et, const std::string& msg, const std::string& sev = "error", const std::string& sug = "")
        : key(k), error_type(et), message(msg), severity(sev), suggestion(sug) {}
};

// Configuration constraint
struct ValidationConstraint {
    std::string constraint_type;
    std::string key;
    std::string secondary_key;
    std::string operation;
    double value;
    std::string message;
    std::string suggestion;
    
    ValidationConstraint(const std::string& ct, const std::string& k, const std::string& sk, 
                         const std::string& op, double val, const std::string& msg, const std::string& sug = "")
        : constraint_type(ct), key(k), secondary_key(sk), operation(op), value(val), message(msg), suggestion(sug) {}
};

// Configuration schema validation result
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    std::vector<std::string> optimization_suggestions;
    std::map<std::string, std::string> metadata;
    
    ValidationResult() : valid(true) {}
    ValidationResult(const std::vector<ValidationError>& err, const std::vector<ValidationError>& warn,
                     const std::vector<std::string>& sug = std::vector<std::string>())
        : valid(err.empty()), errors(err), warnings(warn), optimization_suggestions(sug) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::map<std::string, std::string> type_definitions;
    std::map<std::string, std::string> required_keys;
    std::map<std::string, std::string> default_values;
    std::vector<ValidationConstraint> constraints;
    std::map<std::string, std::string> descriptions;
    std::string version;
    bool validate_on_set = true;
    bool allow_unknown_keys = false;
    
    ConfigSchema() : version("1.0") {}
    ConfigSchema(const std::string& ver) : version(ver) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::map<std::string, std::string> values;
    std::map<std::string, std::string> overrides;
    std::vector<std::string> base_templates;
    std::string version;
    bool is_preset = false;
    
    ConfigTemplate() : version("1.0") {}
    ConfigTemplate(const std::string& n) : name(n), version("1.0") {}
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string key;
    ConfigValue old_value;
    ConfigValue new_value;
    ConfigSource source;
    std::string operation;
    std::string user;
    std::string comment;
    
    ConfigHistoryEntry(const std::string& k, const ConfigValue& old_val, const ConfigValue& new_val,
                       ConfigSource src, const std::string& op, const std::string& user = "", const std::string& comment = "")
        : timestamp(std::chrono::system_clock::now()), key(k), old_value(old_val), new_value(new_val),
          source(src), operation(op), user(user), comment(comment) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    
    // Schema validation and management
    bool setSchema(const ConfigSchema& schema);
    bool validate(const std::vector<std::string>& keys = {}) const;
    std::string getSchemaVersion() const;
    
    // Cross-parameter validation
    bool validateCrossParameters() const;
    
    // Configuration inheritance and merging
    bool mergeFrom(const Config& other, ConfigSource source = ConfigSource::Inherited);
    bool inheritFrom(const Config& template_config);
    
    // Configuration templates and presets
    bool createTemplate(const ConfigTemplate& template_config);
    bool useTemplate(const std::string& template_name);
    bool applyPreset(const std::string& preset_name);
    std::vector<std::string> getAvailableTemplates() const;
    std::vector<std::string> getAvailablePresets() const;
    
    // Validation history and rollback
    void enableValidationHistory(bool enable);
    bool isValidationHistoryEnabled() const;
    std::vector<ConfigHistoryEntry> getValidationHistory() const;
    bool rollback(const std::string& key, const std::chrono::system_clock::time_point& timestamp = std::chrono::system_clock::time_point());
    std::string getValidationHistorySummary() const;
    
    // Configuration comparison and diffing
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diff(const Config& other) const;
    std::string diffToString(const Config& other) const;
    
    // Configuration optimization
    std::vector<std::string> analyzeOptimizations() const;
    std::string getOptimizationReport() const;
    
    // Configuration encryption and security
    bool encrypt(const std::string& key_file);
    bool decrypt(const std::string& key_file);
    bool isEncrypted() const;
    
    // Configuration versioning
    bool version(const std::string& version);
    std::string getVersion() const;
    bool saveVersion(const std::string& version_file) const;
    bool loadVersion(const std::string& version_file);
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set with validation
    ValidationResult setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get validation errors
    std::vector<ValidationError> getLastValidationErrors() const;
    
    // Get validation warnings
    std::vector<ValidationError> getLastValidationWarnings() const;
    
    // Configuration monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::map<std::string, std::string> getMonitoringData() const;
    
    // Configuration integration
    bool integrateWithCI(const std::string& pipeline_config);
    bool integrateWithInfrastructureAsCode(const std::string& iac_config);
    
    // Export/Import configuration
    std::string exportToString() const;
    bool importFromString(const std::string& config_string);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const nlohmann::json& value);
    static nlohmann::json mapToJSON(const std::map<std::string, std::string>& map);
    static std::map<std::string, std::string> jsonToMap(const nlohmann::json& json);
    
    // Configuration parsing helpers
    static std::map<std::string, ConfigValue> parseKeyValueFile(const std::string& content);
    static std::map<std::string, ConfigValue> parseJSONFile(const std::string& filepath);
    static std::map<std::string, ConfigValue> parseYAMLFile(const std::string& filepath);
    static void validateConstraint(const ValidationConstraint& constraint, const std::map<std::string, ConfigValue>& config) const;
};

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add schema
    bool addSchema(const std::string& name, const ConfigSchema& schema);
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config, const std::string& schema_name = "default") const;
    
    // Add constraint
    bool addConstraint(const ValidationConstraint& constraint);
    
    // Set validation callback
    void setValidationCallback(std::function<ValidationResult(const Config&)> callback);
    
    // Get validation errors
    std::vector<ValidationError> getLastErrors() const;
    std::vector<ValidationError> getLastWarnings() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration management utilities
namespace ConfigUtils {
    // Configuration comparison
    bool compareValues(const ConfigValue& a, const ConfigValue& b);
    
    // Configuration optimization
    std::vector<std::string> generateOptimizationSuggestions(const Config& config);
    
    // Configuration diffing
    std::string diffToString(const std::map<std::string, std::pair<ConfigValue, ConfigValue>>& diff);
    
    // Configuration validation helpers
    std::vector<ValidationError> validateType(const std::string& key, const ConfigValue& value, const std::string& expected_type);
    std::vector<ValidationError> validateRange(const std::string& key, double value, double min, double max);
    std::vector<ValidationError> validateCrossParameter(const ValidationConstraint& constraint, const std::map<std::string, ConfigValue>& config);
    
    // Configuration history helpers
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    
    // Configuration serialization helpers
    std::string toString(const ConfigValue& value);
    std::string configToString(const Config& config);
}

} // namespace nlm

// Enable JSON serialization for ConfigValue
namespace nlohmann {
    void to_json(json& j, const nlm::ConfigValue& value);
    void from_json(const json& j, nlm::ConfigValue& value);
}
