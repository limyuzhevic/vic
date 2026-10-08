#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <variant>
#include <optional>
#include <any>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

namespace nlm {

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
    std::vector<bool>
>;

// Configuration validation rule
struct ValidationRule {
    std::string type;  // "range", "enum", "pattern", "custom"
    std::string name;
    std::string description;
    std::map<std::string, std::any> parameters;
    std::function<bool(const ConfigValue&)> validator;
    
    ValidationRule(const std::string& name, const std::string& desc)
        : name(name), description(desc) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Python
};

// Configuration group for organization
struct ConfigGroup {
    std::string name;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    ConfigGroup(const std::string& name, const std::string& desc = "")
        : name(name), description(desc) {}
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<std::string> tags;
    std::map<std::string, std::any> metadata;
    std::vector<ValidationRule> validators;
    
    // Convenience constructors
    ConfigEntry() = default;
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc, const std::vector<std::string>& tg)
        : key(k), value(v), source(s), description(desc), tags(tg) {}
    
    // Validation methods
    bool validate() const {
        for (const auto& validator : validators) {
            if (!validator.validator(value)) {
                return false;
            }
        }
        return true;
    }
    
    std::string getValidationError() const {
        for (const auto& validator : validators) {
            if (!validator.validator(value)) {
                return "Validation failed for key: " + key + " (" + validator.name + ")";
            }
        }
        return "";
    }
};

// Enhanced configuration manager with JSON/YAML support
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
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content, ConfigSource source = ConfigSource::File);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, ConfigSource source = ConfigSource::CommandLine);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars, 
                            const std::string& prefix = "NLM_",
                            ConfigSource source = ConfigSource::Environment);
    
    // Load from Python (if available)
    bool loadFromPython(const std::map<std::string, std::string>& pythonVars,
                       ConfigSource source = ConfigSource::Python);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with automatic type conversion
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::vector<std::string>& tags = {});
    
    // Type-specific setters
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "") {
        set(key, ConfigValue(value), source, description);
    }
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const std::type_info& type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& tag) const;
    
    // Get keys by group
    std::vector<std::string> getKeys(const std::string& group) const;
    
    // Configuration management
    void clear();
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Group management
    void createGroup(const std::string& groupName, const std::string& description = "");
    bool hasGroup(const std::string& groupName) const;
    void addToGroup(const std::string& key, const std::string& groupName);
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    
    // Validation
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Setters for validation rules
    void addValidator(const std::string& key, const ValidationRule& validator);
    void setRangeValidator(const std::string& key, double min, double max, const std::string& desc = "");
    void setEnumValidator(const std::string& key, const std::vector<std::string>& values, const std::string& desc = "");
    void setPatternValidator(const std::string& key, const std::string& pattern, const std::string& desc = "");
    
    // Configuration inheritance
    void setInheritance(const std::string& parentKey, const Config& parentConfig);
    
    // Hot-reload support
    bool needsReload() const { return reloadRequired_; }
    void markForReload() { reloadRequired_ = true; }
    void reload();
    
    // Get configuration summary with groups
    std::string summary() const;
    std::string summary(const std::string& group) const;
    
    // Configuration statistics
    size_t getEntryCount() const { return pImpl->entries.size(); }
    size_t getGroupCount() const { return pImpl->groups.size(); }
    std::vector<std::string> getSources() const;
    
    // File format detection
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLError(const std::string& filepath);
    
    // Utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool reloadRequired_;
    
    // Internal helpers
    static ConfigValue yamlToConfigValue(const YAML::Node& node);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const nlohmann::json& json);
    
    // File loading helpers
    bool loadFromJSON(const std::string& content, ConfigSource source);
    bool loadFromYAML(const std::string& content, ConfigSource source);
    
    // Validation helpers
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateEnum(const ConfigValue& value, const std::vector<std::string>& allowedValues);
    static bool validatePattern(const ConfigValue& value, const std::string& pattern);
};

// Template implementations
#include "Config.tpp"

// Validation rule factory functions
inline ValidationRule rangeValidator(const std::string& name, double min, double max, 
                                     const std::string& desc = "") {
    ValidationRule rule(name, desc);
    rule.type = "range";
    rule.validator = [min, max](const ConfigValue& value) {
        return validateRange(value, min, max);
    };
    rule.parameters["min"] = min;
    rule.parameters["max"] = max;
    return rule;
}

inline ValidationRule enumValidator(const std::string& name, const std::vector<std::string>& values,
                                    const std::string& desc = "") {
    ValidationRule rule(name, desc);
    rule.type = "enum";
    rule.validator = [values](const ConfigValue& value) {
        return validateEnum(value, values);
    };
    rule.parameters["values"] = values;
    return rule;
}

inline ValidationRule patternValidator(const std::string& name, const std::string& pattern,
                                       const std::string& desc = "") {
    ValidationRule rule(name, desc);
    rule.type = "pattern";
    rule.validator = [pattern](const ConfigValue& value) {
        return validatePattern(value, pattern);
    };
    rule.parameters["pattern"] = pattern;
    return rule;
}

} // namespace nlm
