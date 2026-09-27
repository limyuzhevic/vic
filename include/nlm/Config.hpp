#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>

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

// Configuration validation error
struct ValidationError {
    std::string message;
    std::string key;
    std::string expected;
    std::string actual;
    
    ValidationError(const std::string& msg, const std::string& key = "", 
                   const std::string& expected = "", const std::string& actual = "")
        : message(msg), key(key), expected(expected), actual(actual) {}
};

// Configuration validation rule
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual std::string getKeyPattern() const { return "*"; }
    virtual std::string getExpectedValueType() const { return "any"; }
};

// Type validation rule
class TypeValidationRule : public ValidationRule {
public:
    enum class ValueType {
        Int,
        Int64,
        Double,
        Bool,
        String,
        VectorInt,
        VectorDouble,
        VectorString,
        Any
    };
    
    TypeValidationRule(ValueType type, const std::string& keyPattern = "*")
        : type_(type), keyPattern_(keyPattern) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override;
    std::string getErrorMessage() const override;
    std::string getKeyPattern() const override { return keyPattern_; }
    std::string getExpectedValueType() const override;
    
private:
    ValueType type_;
    std::string keyPattern_;
};

// Range validation rule
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(const std::string& keyPattern, double min, double max, 
                       const std::string& type = "number")
        : keyPattern_(keyPattern), min_(min), max_(max), type_(type) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override;
    std::string getErrorMessage() const override;
    std::string getKeyPattern() const override { return keyPattern_; }
    
private:
    std::string keyPattern_;
    double min_;
    double max_;
    std::string type_;
};

// Enum validation rule
class EnumValidationRule : public ValidationRule {
public:
    EnumValidationRule(const std::string& keyPattern, 
                      const std::vector<std::string>& allowedValues,
                      const std::string& type = "string")
        : keyPattern_(keyPattern), allowedValues_(allowedValues), type_(type) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override;
    std::string getErrorMessage() const override;
    std::string getKeyPattern() const override { return keyPattern_; }
    
private:
    std::string keyPattern_;
    std::vector<std::string> allowedValues_;
    std::string type_;
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    void addRule(std::unique_ptr<ValidationRule> rule);
    std::vector<ValidationError> validate(const std::unordered_map<std::string, ConfigValue>& config) const;
    bool hasRuleForKey(const std::string& key) const;
    std::string getSchemaDescription() const;
    
private:
    std::vector<std::unique_ptr<ValidationRule>> rules_;
};

// Parent configuration relationship
class ParentConfig {
public:
    ParentConfig() = default;
    explicit ParentConfig(const std::string& filepath) : parentPath_(filepath) {}
    
    bool loadParentConfig(Config& config) const;
    std::string getParentPath() const { return parentPath_; }
    void setParentPath(const std::string& path) { parentPath_ = path; }
    
private:
    std::string parentPath_;
};

// Configuration template
class ConfigTemplate {
public:
    ConfigTemplate() = default;
    explicit ConfigTemplate(const std::string& name) : name_(name) {}
    
    void addDefaultValue(const std::string& key, const ConfigValue& value, 
                       const std::string& description = "");
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const;
    std::string getTemplateName() const { return name_; }
    std::string getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
private:
    std::string name_;
    std::string description_;
    std::unordered_map<std::string, std::pair<ConfigValue, std::string>> defaults_;
};

// Configuration migration
class ConfigMigration {
public:
    ConfigMigration(int fromVersion, int toVersion) 
        : fromVersion_(fromVersion), toVersion_(toVersion) {}
    
    virtual ~ConfigMigration() = default;
    virtual bool migrate(std::unordered_map<std::string, ConfigValue>& config) = 0;
    int getFromVersion() const { return fromVersion_; }
    int getToVersion() const { return toVersion_; }
    
protected:
    int fromVersion_;
    int toVersion_;
};

// Configuration version migrator
class ConfigMigrator {
public:
    ConfigMigrator() = default;
    ~ConfigMigrator() = default;
    
    void addMigration(std::unique_ptr<ConfigMigration> migration);
    bool migrate(std::unordered_map<std::string, ConfigValue>& config, 
                int fromVersion, int toVersion) const;
    void setCurrentVersion(int version) { currentVersion_ = version; }
    int getCurrentVersion() const { return currentVersion_; }
    
private:
    std::vector<std::unique_ptr<ConfigMigration>> migrations_;
    int currentVersion_ = 1;
};

// Configuration inheritance
class ConfigInheritance {
public:
    ConfigInheritance() = default;
    ~ConfigInheritance() = default;
    
    void addParent(const ParentConfig& parent);
    bool applyInheritance(Config& config) const;
    bool hasParent() const { return !parents_.empty(); }
    size_t getParentCount() const { return parents_.size(); }
    
private:
    std::vector<ParentConfig> parents_;
};

// Configuration template manager
class ConfigTemplateManager {
public:
    ConfigTemplateManager() = default;
    ~ConfigTemplateManager() = default;
    
    void addTemplate(std::unique_ptr<ConfigTemplate> template);
    std::optional<ConfigTemplate> getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<ConfigTemplate>> templates_;
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
    
    // Configuration validation
    void addValidationRule(std::unique_ptr<ValidationRule> rule);
    std::vector<ValidationError> validate() const;
    bool isValid() const;
    
    // Configuration inheritance
    void addParentConfig(const ParentConfig& parent);
    bool hasInheritance() const;
    bool applyInheritance();
    
    // Configuration schema validation
    void setSchema(const ConfigSchema& schema);
    bool validateSchema() const;
    
    // Configuration templates
    void addTemplate(std::unique_ptr<ConfigTemplate> template);
    bool applyTemplate(const std::string& templateName);
    
    // Configuration migration
    void setMigrator(const ConfigMigrator& migrator);
    bool migrate(int targetVersion);
    
    // Environment variable overrides
    void setEnvironmentVariablePrefix(const std::string& prefix);
    bool applyEnvironmentOverrides();
    
    // Enhanced file format validation
    bool loadFromFileWithValidation(const std::string& filepath);
    bool saveToFileWithValidation(const std::string& filepath) const;
    
    // Load from environment variables
    bool loadFromEnvironment();
    
    // Load with migration support
    bool loadFromFile(const std::string& filepath, int versionHint = 1) override;
    
    // Get configuration version
    int getVersion() const { return version_; }
    void setVersion(int version) { version_ = version; }
    
    // Original methods (extended with new features)
    bool loadFromFile(const std::string& filepath) override;
    bool loadFromArgs(int argc, char** argv) override;
    bool saveToFile(const std::string& filepath) const override;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const override;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const override;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override;
    
    // Check existence
    bool has(const std::string& key) const override;
    
    // Remove key
    void remove(const std::string& key) override;
    
    // Get all keys
    std::vector<std::string> getKeys() const override;
    
    // Clear all
    void clear() override;
    
    // Get configuration summary
    std::string summary() const override;
    
    // Get configuration metadata
    std::string getMetadata() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Version management
    int version_;
    
    // Environment variable prefix
    std::string envVarPrefix_;
    
    // Configuration components
    std::unique_ptr<ConfigValidator> validator_;
    std::unique_ptr<ConfigInheritance> inheritance_;
    std::unique_ptr<ConfigSchema> schema_;
    std::unique_ptr<ConfigTemplateManager> templateManager_;
    std::unique_ptr<ConfigMigrator> migrator_;
    
    // Internal helpers
    std::vector<ValidationError> validateInternal() const;
    bool applyInheritanceInternal(Config& parentConfig) const;
    bool applyEnvironmentOverridesInternal();
    bool applyTemplateDefaultsInternal(const std::string& templateName);
    bool applyMigrationInternal(int fromVersion, int toVersion);
    
    // File format validation helpers
    bool validateFileFormat(const std::string& filepath) const;
    bool validateJsonFormat(const std::string& filepath) const;
    bool validateYamlFormat(const std::string& filepath) const;
    bool validateKeyValueFormat(const std::string& filepath) const;
    
    // Conversion helpers
    std::optional<ConfigValue> convertJsonValue(const nlohmann::json& json_value) const;
    std::optional<ConfigValue> convertYamlValue(const YAML::Node& yaml_value) const;
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    std::string configValueToYaml(const ConfigValue& value) const;
};

// JSON/YAML parsing utilities
namespace config_parser {

// Parse JSON string to ConfigValue
std::optional<ConfigValue> parseJsonValue(const std::string& jsonStr);

// Parse YAML string to ConfigValue  
std::optional<ConfigValue> parseYamlValue(const std::string& yamlStr);

// Convert ConfigValue to JSON string
std::string configValueToJson(const ConfigValue& value);

// Convert ConfigValue to YAML string
std::string configValueToYaml(const ConfigValue& value);

// Detect format from file extension
enum class ConfigFormat {
    Unknown,
    KeyValue,
    Json,
    Yaml
};

ConfigFormat detectFormat(const std::string& filepath);

}} // namespace config_parser

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
    
    // Get configuration metadata
    std::string getMetadata() const;
    
    // Configuration version
    int getVersion() const { return version_; }
    void setVersion(int version) { version_ = version; }
    
    // Environment variable prefix
    void setEnvironmentVariablePrefix(const std::string& prefix);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
