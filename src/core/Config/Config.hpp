#if !defined(NLM_CONFIG_HPP)
#define NLM_CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <any>
#include <variant>
#include <algorithm>
#include <sstream>
#include <memory>
#include <functional>
#include <regex>
#include <chrono>
#include <exception>

// JSON/YAML library includes - using modern libraries
// For now, we'll use template-based implementation that can be extended

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, std::string>
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

// Configuration exception hierarchy
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException("Parse error: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException("Validation error: " + message) {}
};

class ConfigSchemaException : public ConfigException {
public:
    explicit ConfigSchemaException(const std::string& message) : ConfigException("Schema error: " + message) {}
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    std::string type;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    
    // Validation function
    std::function<bool(const ConfigValue&)> validator;
};

class ConfigSchema {
private:
    std::vector<ConfigSchemaEntry> schema;
    std::string name;
    std::string version;
    
public:
    ConfigSchema(const std::string& name = "default", const std::string& version = "1.0")
        : name(name), version(version) {}
    
    void addEntry(const std::string& key, const std::string& type, 
                  const std::string& description = "", bool required = false,
                  const ConfigValue& defaultValue = {}, 
                  const std::function<bool(const ConfigValue&)>& validator = nullptr) {
        ConfigSchemaEntry entry;
        entry.key = key;
        entry.type = type;
        entry.description = description;
        entry.required = required;
        entry.defaultValue = defaultValue;
        entry.validator = validator;
        schema.push_back(entry);
    }
    
    bool validate(const Config& config, std::vector<std::string>& errors) const {
        bool isValid = true;
        
        for (const auto& entry : schema) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Missing required key: " + entry.key);
                isValid = false;
            } else if (config.has(entry.key)) {
                auto value = config.get(entry.key);
                if (value) {
                    // TODO: Add type validation based on entry.type
                    if (entry.validator && !entry.validator(*value)) {
                        errors.push_back("Invalid value for key " + entry.key + ": " + std::to_string(*value));
                        isValid = false;
                    }
                }
            }
        }
        
        return isValid;
    }
    
    const std::vector<ConfigSchemaEntry>& getEntries() const { return schema; }
    std::string getName() const { return name; }
    std::string getVersion() const { return version; }
};

// Configuration group for hierarchical organization
class ConfigGroup {
private:
    std::string name;
    std::map<std::string, ConfigValue> values;
    std::map<std::string, ConfigGroup> subgroups;
    
public:
    ConfigGroup(const std::string& groupName = "default") : name(groupName) {}
    
    void setValue(const std::string& key, const ConfigValue& value) {
        values[key] = value;
    }
    
    bool hasValue(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    ConfigValue getValue(const std::string& key) const {
        auto it = values.find(key);
        if (it != values.end()) {
            return it->second;
        }
        throw ConfigException("Key not found in group: " + key);
    }
    
    const std::string& getName() const { return name; }
    
    ConfigGroup& getSubgroup(const std::string& subgroupName) {
        return subgroups[subgroupName];
    }
    
    const ConfigGroup& getSubgroup(const std::string& subgroupName) const {
        auto it = subgroups.find(subgroupName);
        if (it != subgroups.end()) {
            return it->second;
        }
        throw ConfigException("Subgroup not found: " + subgroupName);
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : values) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::vector<std::string> getSubgroupNames() const {
        std::vector<std::string> names;
        for (const auto& pair : subgroups) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// Configuration template for parameter sets
class ConfigTemplate {
private:
    std::string name;
    std::map<std::string, ConfigValue> parameters;
    std::vector<std::string> parameterNames;
    
public:
    ConfigTemplate(const std::string& templateName = "default") : name(templateName) {}
    
    void addParameter(const std::string& name, const ConfigValue& value) {
        parameters[name] = value;
        parameterNames.push_back(name);
    }
    
    ConfigValue getParameter(const std::string& name) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return it->second;
        }
        throw ConfigException("Parameter not found in template: " + name);
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters.find(name) != parameters.end();
    }
    
    const std::string& getName() const { return name; }
    std::vector<std::string> getParameterNames() const { return parameterNames; }
    
    // Apply template to a config
    void applyTo(Config& config) const {
        for (const auto& name : parameterNames) {
            config.set(name, getParameter(name), ConfigSource::Runtime);
        }
    }
};

// Configuration profile with different settings
class ConfigProfile {
private:
    std::string name;
    std::vector<ConfigGroup> groups;
    std::vector<ConfigTemplate> templates;
    
public:
    ConfigProfile(const std::string& profileName = "default") : name(profileName) {}
    
    void addGroup(const ConfigGroup& group) {
        groups.push_back(group);
    }
    
    void addTemplate(const ConfigTemplate& template_) {
        templates.push_back(template_);
    }
    
    const std::string& getName() const { return name; }
    std::vector<std::string> getGroupNames() const {
        std::vector<std::string> names;
        for (const auto& group : groups) {
            names.push_back(group.getName());
        }
        return names;
    }
    
    const ConfigGroup* getGroup(const std::string& groupName) const {
        for (const auto& group : groups) {
            if (group.getName() == groupName) {
                return &group;
            }
        }
        return nullptr;
    }
    
    const ConfigTemplate* getTemplate(const std::string& templateName) const {
        for (const auto& template_ : templates) {
            if (template_.getName() == templateName) {
                return &template_;
            }
        }
        return nullptr;
    }
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
    
    // Load from file (JSON or YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON
    std::string saveToJSON() const;
    
    // Save to YAML
    std::string saveToYAML() const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set nested value
    void set(const std::string& groupName, const std::string& key, const ConfigValue& value);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get values by group
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    bool validate(const ConfigSchema& schema, std::vector<std::string>& errors) const;
    
    // Template application
    void applyTemplate(const ConfigTemplate& template_);
    
    // Profile application
    void applyProfile(const ConfigProfile& profile);
    
    // Configuration groups
    ConfigGroup& getGroup(const std::string& groupName);
    const ConfigGroup& getGroup(const std::string& groupName) const;
    
    // Configuration templates
    ConfigTemplate& getTemplate(const std::string& templateName);
    const ConfigTemplate& getTemplate(const std::string& templateName) const;
    
    // Configuration profiles
    ConfigProfile& getProfile(const std::string& profileName);
    const ConfigProfile& getProfile(const std::string& profileName) const;
    
    // Configuration inheritance
    void setParentConfig(const std::shared_ptr<Config>& parent);
    std::shared_ptr<Config> getParentConfig() const;
    
    // Configuration format detection
    static std::string detectFormat(const std::string& filepath);
    
    // Static helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeYAML(const std::string& str);
    
    // Factory methods
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createFromJSON(const std::string& jsonString);
    static std::shared_ptr<Config> createFromYAML(const std::string& yamlString);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void initializeDefaultSchema();
    void createDefaultProfile();
    
    // JSON/YAML parsing helpers
    static ConfigValue parseJSONValue(const std::string& jsonStr);
    static ConfigValue parseYAMLValue(const std::string& yamlStr);
    static std::string configValueToJSON(const ConfigValue& value);
    static std::string configValueToYAML(const ConfigValue& value);
};