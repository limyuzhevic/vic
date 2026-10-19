#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>

// JSON library
#include <nlohmann/json.hpp>

// YAML library
#include <yaml-cpp/yaml.h>

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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
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
    std::string type;
    std::string validation;
    std::string defaultValue;
    bool required;
    std::vector<std::string> options;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), 
                   validation(), defaultValue(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(), validation(), 
          defaultValue(), required(false) {}
};

// Advanced configuration features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (auto-detects JSON, YAML, INI, XML formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (supports JSON, YAML, INI, XML formats)
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
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
    
    // Advanced configuration methods
    void setMetadata(const std::string& key, const std::string& description, 
                     const std::string& type = "",
                     const std::string& validation = "",
                     const std::string& defaultValue = "",
                     bool required = false,
                     const std::vector<std::string>& options = {});
    
    void setDescription(const std::string& key, const std::string& description);
    void setType(const std::string& key, const std::string& type);
    void setValidation(const std::string& key, const std::string& validation);
    void setDefaultValue(const std::string& key, const std::string& defaultValue);
    void setRequired(const std::string& key, bool required);
    void setOptions(const std::string& key, const std::vector<std::string>& options);
    
    std::string getDescription(const std::string& key) const;
    std::string getType(const std::string& key) const;
    std::string getValidation(const std::string& key) const;
    std::string getDefaultValue(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    std::vector<std::string> getOptions(const std::string& key) const;
    bool hasOptions(const std::string& key) const;
    
    // Configuration inheritance and merging
    bool loadFromConfig(const Config& other, ConfigSource source = ConfigSource::File);
    bool mergeWith(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Configuration validation
    bool validate(const std::string& key) const;
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Configuration introspection
    std::string summary() const;
    std::string getConfigInfo() const;
    std::string getConfigFormatInfo() const;
    
    // Configuration persistence
    bool backup(const std::string& backupPath = "") const;
    bool restore(const std::string& backupPath = "");
    bool exportConfig(const std::string& filepath, const std::string& format = "json") const;
    bool importConfig(const std::string& filepath, const std::string& format = "auto");
    
    // Configuration version management
    std::string getVersion() const { return "2.0.0"; }
    void setVersion(const std::string& version) { configVersion = version; }
    
    // Performance monitoring
    struct PerformanceMetrics {
        size_t totalOperations = 0;
        size_t successfulOperations = 0;
        std::chrono::system_clock::time_point lastAccess;
        double averageOperationTime = 0.0;
        
        PerformanceMetrics() : lastAccess(std::chrono::system_clock::now()) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return metrics; }
    void resetPerformanceMetrics() { metrics = PerformanceMetrics(); }
    
    // Configuration logging and auditing
    struct ConfigLogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        std::string key;
        std::string oldValue;
        std::string newValue;
        std::string source;
        std::string userId;
        
        ConfigLogEntry(const std::string& op, const std::string& k, 
                      const std::string& oldVal, const std::string& newVal,
                      const std::string& src, const std::string& user = "")
            : timestamp(std::chrono::system_clock::now()), operation(op), key(k),
              oldValue(oldVal), newValue(newVal), source(src), userId(user) {}
    };
    
    std::vector<ConfigLogEntry> getLog() const { return log; }
    void clearLog() { log.clear(); }
    void enableLogging(bool enable) { loggingEnabled = enable; }
    bool isLoggingEnabled() const { return loggingEnabled; }
    
    // Environment variable expansion
    bool expandEnvironmentVariables(std::string& value) const;
    std::string expandEnvironmentVariables(const std::string& value) const;
    
    // Default value management
    bool hasDefaultValue(const std::string& key) const;
    ConfigValue getDefaultValueAsVariant(const std::string& key) const;
    void addDefaultValue(const std::string& key, const ConfigValue& value);
    
    // Configuration schema validation
    struct ConfigSchema {
        std::unordered_map<std::string, std::string> fieldTypes;
        std::unordered_map<std::string, std::string> fieldValidation;
        std::unordered_map<std::string, std::string> fieldDefaults;
        std::unordered_map<std::string, bool> fieldRequired;
        std::unordered_map<std::string, std::vector<std::string>> fieldOptions;
        
        bool validateAgainstSchema() const;
        std::vector<std::string> getSchemaErrors() const;
    };
    
    void setSchema(const ConfigSchema& schema) { configSchema = schema; }
    ConfigSchema getSchema() const { return configSchema; }
    
    // Compression support
    bool compressBackup(const std::string& sourcePath, const std::string& targetPath) const;
    bool decompressBackup(const std::string& sourcePath, const std::string& targetPath) const;
    
    // Configuration diff
    std::string diff(const Config& other) const;
    bool equals(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string getFileFormat(const std::string& filepath);
    
    // File format specific parsers
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromINI(const std::string& filepath);
    bool loadFromXML(const std::string& filepath);
    
    // File format specific writers
    bool saveToJSON(std::ofstream& file) const;
    bool saveToYAML(std::ofstream& file) const;
    bool saveToINI(std::ofstream& file) const;
    bool saveToXML(std::ofstream& file) const;
    
    // Format detection and conversion
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const nlohmann::json& json);
    
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static ConfigValue yamlToConfigValue(const YAML::Node& yaml);
    
    // Validation helpers
    static bool validateValue(const std::string& key, const ConfigValue& value, const std::string& validation) const;
    static std::string valueToString(const ConfigValue& value) const;
    
    // Performance tracking
    mutable PerformanceMetrics metrics;
    bool loggingEnabled = true;
    std::string configVersion = "2.0.0";
    ConfigSchema configSchema;
    mutable std::vector<ConfigLogEntry> log;
};