#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true), errorMessage(), warnings() {}
    ValidationResult(bool v, const std::string& error) : valid(v), errorMessage(error), warnings() {}
};

// Configuration constraint type
using ValidationConstraint = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration template
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, ValidationConstraint> constraints;
    bool enabled;
    
    ConfigurationTemplate() : enabled(true) {}
    ConfigurationTemplate(const std::string& n, const std::string& desc) 
        : name(n), description(desc), enabled(true) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), lastModified() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          lastModified(std::chrono::system_clock::now()) {}
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
    
    // Runtime modification methods
    void modifyRuntime(const std::string& key, const ConfigValue& value);
    bool overrideConfig(const std::string& key, const ConfigValue& value);
    
    // Configuration templates
    bool addTemplate(const ConfigurationTemplate& template);
    bool removeTemplate(const std::string& name);
    std::vector<std::string> getTemplateNames() const;
    bool applyTemplate(const std::string& templateName);
    ConfigurationTemplate* getTemplate(const std::string& name);
    const ConfigurationTemplate* getTemplate(const std::string& name) const;
    
    // Configuration validation
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    void addValidationConstraint(const std::string& key, const ValidationConstraint& constraint);
    void clearValidationConstraints(const std::string& key);
    
    // Configuration export/import
    bool exportConfig(const std::string& filepath, bool includeRuntime = false) const;
    bool importConfig(const std::string& filepath);
    bool importConfigFromJSON(const std::string& jsonData);
    std::string exportConfigToJSON(bool includeRuntime = false) const;
    
    // Documentation generation
    std::string generateDocumentation() const;
    std::string generateTemplateDocumentation(const std::string& templateName) const;
    
    // Configuration comparison and history
    bool hasChangesSince(const std::chrono::system_clock::time_point& since) const;
    std::string getChangeSummary(const std::chrono::system_clock::time_point& since) const;
    
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
    
    // Get modification timestamp for key
    std::chrono::system_clock::time_point getModificationTime(const std::string& key) const;
    
    // Get configuration entry (internal use)
    ConfigEntry* getEntry(const std::string& key);
    const ConfigEntry* getEntry(const std::string& key) const;
    
    // Get all entries
    const std::vector<ConfigEntry>& getAllEntries() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string generateID();
};

} // namespace nlm
