#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <any>
#include <stdexcept>
#include <regex>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <cassert>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigSchema;

// Configuration error hierarchy
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& message) : ConfigError("File error: " + message) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigError("Parse error: " + message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Validation error: " + message) {}
};

class ConfigTypeError : public ConfigError {
public:
    explicit ConfigTypeError(const std::string& message) : ConfigError("Type error: " + message) {}
};

class ConfigSchemaError : public ConfigError {
public:
    explicit ConfigSchemaError(const std::string& message) : ConfigError("Schema error: " + message) {}
};

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, std::any> // For nested objects
>;

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string type;
    std::any default_value;
    std::optional<std::pair<double, double>> range;
    bool required = false;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    ConfigMetadata() : description(), type(), required(false) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    Environment,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigMetadata metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const ConfigMetadata& meta = ConfigMetadata())
        : key(k), value(v), source(s), description(desc), metadata(meta) {}
};

// Hierarchical configuration node
class ConfigNode {
public:
    ConfigNode() = default;
    explicit ConfigNode(const std::string& name) : name(name) {}
    
    std::string name;
    std::any value;
    std::map<std::string, std::unique_ptr<ConfigNode>> children;
    std::map<std::string, ConfigMetadata> metadata;
    
    ConfigNode* getChild(const std::string& name);
    const ConfigNode* getChild(const std::string& name) const;
    ConfigNode* addChild(const std::string& name);
    void setValue(const std::any& val);
    
    template<typename T>
    T getValue() const {
        return std::any_cast<T>(value);
    }
    
    bool hasValue() const { return value.has_value(); }
    bool hasChild(const std::string& name) const { return children.find(name) != children.end(); }
};

// Configuration schema for validation
class ConfigSchema {
public:
    using ValidationFunction = std::function<bool(const ConfigValue&)>;
    using TransformationFunction = std::function<ConfigValue(const ConfigValue&)>;n    
    struct FieldSchema {
        std::string name;
        std::string type; // "int", "double", "string", "bool", "array", "object"
        std::any default_value;
        std::optional<std::pair<double, double>> range;
        bool required = false;
        std::vector<std::string> tags;
        ValidationFunction validator;
        TransformationFunction transformer;
        std::string description;
    };
    
    ConfigSchema() = default;
    explicit ConfigSchema(const std::string& name) : name(name) {}
    
    std::string name;
    std::vector<FieldSchema> fields;
    std::map<std::string, std::unique_ptr<ConfigSchema>> sub_schemas;
    
    void addField(const FieldSchema& field);
    void addSubSchema(std::unique_ptr<ConfigSchema> schema);
    bool validate(const Config& config) const;
    ConfigValue transform(const ConfigValue& value, const std::string& field) const;
    bool hasField(const std::string& field_name) const;
};

// Configuration validator
class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&)>;
    using ValidationMessage = std::function<std::string(const ConfigValue&)>;
    
    struct ValidationRuleEntry {
        std::string field_name;
        ValidationRule rule;
        ValidationMessage error_message;
        bool is_range_check = false;
    };
    
    ConfigValidator() = default;
    explicit ConfigValidator(const std::string& name) : name(name) {}
    
    std::string name;
    std::vector<ValidationRuleEntry> rules;
    
    void addRule(const std::string& field_name, 
                 const ValidationRule& rule,
                 const ValidationMessage& error_msg = nullptr,
                 bool is_range_check = false);
    bool validate(const Config& config) const;
    std::vector<std::string> getValidationErrors(const Config& config) const;
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
    
    // Environment variable integration
    static void setEnvironmentPrefix(const std::string& prefix);
    static std::string getEnvironmentPrefix();
    
    // Load from different sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromTOML(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment();
    bool loadFromDefaults();
    
    // Save to different formats
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    bool saveToYAML(const std::string& filepath, bool pretty = false) const;
    bool saveToJSON(const std::string& filepath, bool pretty = false) const;
    bool saveToTOML(const std::string& filepath, bool pretty = false) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Hierarchical access with dot notation
    template<typename T>
    std::optional<T> getHierarchical(const std::string& dot_key) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime,
             const ConfigMetadata& metadata = ConfigMetadata());
    
    // Hierarchical set
    void setHierarchical(const std::string& dot_key, const ConfigValue& value,
                        ConfigSource source = ConfigSource::Runtime,
                        const ConfigMetadata& metadata = ConfigMetadata());
    
    // Configuration metadata
    const ConfigMetadata& getMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const ConfigMetadata& metadata);
    
    // Validation
    bool validate(const ConfigSchema& schema) const;
    bool validate(const ConfigValidator& validator) const;
    std::vector<std::string> getValidationErrors(const ConfigSchema& schema) const;
    std::vector<std::string> getValidationErrors(const ConfigValidator& validator) const;
    
    // Check existence
    bool has(const std::string& key) const;
    bool hasHierarchical(const std::string& dot_key) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeHierarchical(const std::string& dot_key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getHierarchicalKeys() const;
    
    // Configuration hierarchy management
    ConfigNode* getRootNode() { return root_node.get(); }
    const ConfigNode* getRootNode() const { return root_node.get(); }
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryHierarchical() const;
    
    // Configuration management utilities
    void merge(const Config& other, bool override_source = true);
    Config diff(const Config& other) const;
    bool isSubset(const Config& other) const;
    
    // Schema and template management
    void addSchema(const std::string& name, std::unique_ptr<ConfigSchema> schema);
    ConfigSchema* getSchema(const std::string& name);
    const ConfigSchema* getSchema(const std::string& name) const;
    
    void addValidator(const std::string& name, std::unique_ptr<ConfigValidator> validator);
    ConfigValidator* getValidator(const std::string& name);
    const ConfigValidator* getValidator(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigNode> root_node;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string snakeToCamel(const std::string& s);
    static std::string camelToSnake(const std::string& s);
    static std::string normalizeKey(const std::string& key);
    
    // File parsing helpers
    static bool parseYAML(const std::string& content, Config& config);
    static bool parseJSON(const std::string& content, Config& config);
    static bool parseTOML(const std::string& content, Config& config);
    
    // Environment variable helpers
    static std::string getEnvVarName(const std::string& key);
    static std::string convertEnvVarName(const std::string& key);
    
    // Hierarchical helpers
    static void splitKey(const std::string& dot_key, std::vector<std::string>& parts);
    static std::string joinKey(const std::vector<std::string>& parts);
    
    // Validation helpers
    static bool validateType(const ConfigValue& value, const std::string& type_name) const;
    static bool validateRange(const ConfigValue& value, 
                             const std::optional<std::pair<double, double>>& range) const;
};