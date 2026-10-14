#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    
    // Advanced configuration management
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from JSON file (Phase 2)
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file (Phase 3)
    bool loadFromYAML(const std::string& filepath);
    
    // Validate configuration against schema
    bool validate() const;
    
    // Get configuration groups
    std::vector<std::string> getGroups() const;
    
    // Add configuration group
    void addGroup(const std::string& group, const std::string& description = "");
    
    // Move configuration entry to group
    void moveToGroup(const std::string& key, const std::string& group);
    
    // Get configuration value with type checking
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue) const;
    
    // Set configuration with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration commands
    bool executeCommand(const std::string& command);
    std::vector<std::string> getAvailableCommands() const;
    
    // Configuration templates
    bool saveTemplate(const std::string& name, const std::string& description = "");
    bool loadTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration analysis
    std::string analyze() const;
    std::map<std::string, double> getStatistics() const;
    
    // Configuration migration
    bool migrate(const std::string& fromVersion, const std::string& toVersion);
    std::string getVersion() const;
    void setVersion(const std::string& version);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Configuration backup/restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // Configuration search
    std::vector<std::string> search(const std::string& pattern) const;
    
    // Configuration export/import
    bool exportToJSON(const std::string& filepath, bool pretty = true) const;
    bool exportToINI(const std::string& filepath) const;
    
    // Configuration history
    void recordChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    std::vector<std::pair<std::string, ConfigValue>> getHistory() const;
    void clearHistory();
    
    // Configuration optimization
    void optimize();
    double getMemoryUsage() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getGroupCount() const;
    double getAverageValueLength() const;
    
    // Configuration cleanup
    void removeUnused();
    void removeOldEntries(ConfigSource source);
    
    // Configuration batch operations
    void batchSet(const std::map<std::string, ConfigValue>& settings);
    void batchRemove(const std::vector<std::string>& keys);
    
    // Configuration merge
    void merge(const Config& other, bool overwrite = true);
    
    // Configuration diff
    struct DiffEntry {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        std::string timestamp;
    };
    
    std::vector<DiffEntry> computeDiff(const Config& other) const;
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    void addValidationRule(const ValidationRule& rule);
    void removeValidationRule(const std::string& key);
    bool hasValidationRule(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
