#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <sstream>
#include <ctime>

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
    Runtime,
    Template,
    Encrypted
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationError;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationError() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationError() {}
};

// Schema validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::vector<std::string>& errs, const std::vector<std::string>& warns = {})
        : isValid(valid), errors(errs), warnings(warns) {}
};

// Configuration change record
struct ConfigChangeRecord {
    std::chrono::system_clock::time_point timestamp;
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string description;
    
    ConfigChangeRecord(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal, 
                      ConfigSource src, const std::string& desc = "")
        : key(k), oldValue(oldVal), newValue(newVal), source(src), description(desc) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Configuration monitor interface
class ConfigMonitor {
public:
    virtual ~ConfigMonitor() = default;
    virtual void onConfigurationChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source) = 0;
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
    
    // ========== ADVANCED CONFIGURATION MANAGEMENT ==========
    
    // Schema validation
    bool hasValidSchema() const;
    std::string getSchema() const;
    ValidationResult validateConfiguration() const;
    void setValidationEnabled(bool enabled);
    bool isValidationEnabled() const;
    std::vector<std::string> getValidationErrors() const;
    void clearValidationErrors();
    
    // Configuration templates
    size_t getTemplateCount() const;
    std::vector<std::string> getTemplateNames() const;
    std::shared_ptr<Config> getTemplate(const std::string& templateName) const;
    bool addTemplate(const std::string& templateName, const Config& templateConfig);
    bool removeTemplate(const std::string& templateName);
    bool loadTemplate(const std::string& templateName, const std::string& filepath);
    
    // Configuration monitoring
    bool hasMonitor() const;
    std::shared_ptr<ConfigMonitor> getMonitor() const;
    void setMonitor(std::shared_ptr<ConfigMonitor> monitor);
    void onConfigurationChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source);
    std::vector<ConfigChangeRecord> getConfigurationHistory() const;
    std::vector<ConfigChangeRecord> getRecentChanges(size_t maxChanges = 10) const;
    bool rollback(size_t changeIndex);
    std::string getDifferences(const Config& otherConfig) const;
    
    // Configuration comparison
    static std::string compareConfigurations(const Config& config1, const Config& config2);
    static std::string getConfigurationDifferences(const Config& config1, const Config& config2);
    static void mergeConfigurations(const Config& config1, const Config& config2, Config& result);
    
    // Configuration encryption/security
    std::string encryptConfiguration(const std::string& key) const;
    bool decryptConfiguration(const std::string& encryptedData, const std::string& key);
    bool validateSignature(const std::string& signature, const std::string& data) const;
    void setEncryptionKey(const std::string& key);
    std::string getEncryptionStatus() const;
    
    // Configuration export/import
    bool exportConfiguration(const std::string& filepath, const std::string& format = "json") const;
    bool importConfiguration(const std::string& filepath, const std::string& format = "json");
    std::vector<std::string> getExportFormats() const;
    void setExportPath(const std::string& path);
    std::string getExportPath() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
