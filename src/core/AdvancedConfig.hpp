#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <set>
#include <functional>
#include <any>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class JSONConfigParser;
class YAMLConfigParser;
class ConfigSchema;
class ConfigGroups;

// Enhanced Configuration Value Types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::any>  // For nested arrays
>;

// Enhanced Configuration Source Enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    SchemaDefault
};

// Configuration Entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string unit;
    std::any defaultValue;
    bool required;
    bool readOnly;
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(), unit(), 
                   defaultValue(), required(false), readOnly(false), createdAt(), lastModified() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& cat = "", const std::string& u = "")
        : key(k), value(v), source(s), description(desc), category(cat), unit(u),
          defaultValue(v), required(false), readOnly(false), 
          createdAt(std::chrono::system_clock::now()), lastModified(createdAt) {}
};

// Configuration Group for organized settings
struct ConfigGroup {
    std::string name;
    std::string description;
    int priority;  // Lower number = higher priority
    std::map<std::string, ConfigEntry> entries;
    bool isGlobal;
    std::vector<std::string> parentGroups;
    
    ConfigGroup() : priority(100), isGlobal(false) {}
    ConfigGroup(const std::string& n, const std::string& desc = "", int prio = 100)
        : name(n), description(desc), priority(prio), isGlobal(false) {}
};

// Configuration Schema for validation
class ConfigSchema {
public:
    struct ValidationRule {
        enum class RuleType {
            Range,
            Pattern,
            Length,
            Custom
        };
        
        RuleType type;
        std::string parameter1;
        std::string parameter2;
        std::function<bool(const ConfigValue&)> validator;
        
        ValidationRule(RuleType t, const std::string& p1, const std::string& p2 = "",
                       const std::function<bool(const ConfigValue&)>& val = nullptr)
            : type(t), parameter1(p1), parameter2(p2), validator(val) {}
    };
    
    struct TypeConstraint {
        std::string key;
        std::string typeName;
        std::vector<ValidationRule> validationRules;
        std::any defaultValue;
        bool required;
    };
    
    std::map<std::string, TypeConstraint> constraints;
    
public:
    ConfigSchema() = default;
    
    void addConstraint(const std::string& key, const std::string& typeName,
                      const std::vector<ValidationRule>& rules = {}, 
                      const std::any& defaultValue = std::any(),
                      bool required = false);
    
    bool validate(const ConfigEntry& entry) const;
    bool validate(const Config& config) const;
    
    template<typename T>
    void addTypeConstraint(const std::string& key, T defaultValue, 
                         bool required = false, const std::vector<ValidationRule>& rules = {});
    
    std::string getTypeName(const ConfigValue& value) const;
    bool isTypeCompatible(const std::string& expectedType, const ConfigValue& value) const;
};

// Enhanced Configuration System
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Legacy support for backward compatibility
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced loading methods
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromMultipleFiles(const std::vector<std::string>& filepaths, bool override = true);
    
    // Enhanced serialization
    bool saveToJSONFile(const std::string& filepath, bool pretty = true) const;
    bool saveToYAMLFile(const std::string& filepath, bool pretty = true) const;
    
    // Get values with enhanced type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Advanced get methods for nested access
    template<typename T>
    std::optional<T> get(const std::vector<std::string>& keyPath) const;
    
    // Set values with enhanced features
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Group management
    void addGroup(const ConfigGroup& group);
    void removeGroup(const std::string& name);
    std::vector<std::string> getGroupNames() const;
    ConfigGroup* getGroup(const std::string& name);
    
    // Inheritance management
    void setParentConfig(std::shared_ptr<Config> parent);
    std::shared_ptr<Config> getParentConfig() const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema() const;
    
    // Validation and checking
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    
    // Configuration merging and overrides
    void merge(const Config& other, bool override = true);
    void overrideWith(const Config& other);
    
    // Clear and management
    void clear();
    void clearGroup(const std::string& groupName);
    
    // Advanced queries
    std::vector<ConfigEntry> getEntriesInGroup(const std::string& groupName) const;
    std::map<std::string, std::string> getTagsForKey(const std::string& key) const;
    std::string getUnitForKey(const std::string& key) const;
    
    // Configuration summary
    std::string summary() const;
    std::string schemaSummary() const;
    
    // Configuration templates
    void saveAsTemplate(const std::string& filepath) const;
    bool loadFromTemplate(const std::string& filepath);
    
    // Configuration search
    std::vector<std::string> findKeysByTag(const std::string& tag) const;
    std::vector<std::string> findKeysByPattern(const std::string& pattern) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringMatch(const std::string& pattern, const std::string& str);
    
    // Legacy helper for backward compatibility
    bool legacyLoadFromFile(const std::string& filepath);
};

// JSON Parser Interface
class JSONConfigParser {
public:
    virtual ~JSONConfigParser() = default;
    
    virtual bool parse(const std::string& jsonStr, Config& config) = 0;
    virtual std::string serialize(const Config& config, bool pretty = true) const = 0;
    virtual std::string getFormatName() const = 0;
};

// YAML Parser Interface
class YAMLConfigParser {
public:
    virtual ~YAMLConfigParser() = default;
    
    virtual bool parse(const std::string& yamlStr, Config& config) = 0;
    virtual std::string serialize(const Config& config, bool pretty = true) const = 0;
    virtual std::string getFormatName() const = 0;
};

// Configuration Parser Factory
class ConfigParserFactory {
public:
    static std::unique_ptr<JSONConfigParser> createJSONParser();
    static std::unique_ptr<YAMLConfigParser> createYAMLParser();
};

} // namespace nlm