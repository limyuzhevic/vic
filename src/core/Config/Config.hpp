// Enhanced Configuration System with JSON/YAML Support
// Comprehensive configuration management for NLM

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <regex>
#include <iostream>

namespace nlm {

// Configuration source enumeration
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic,
    Inherited
};

// Configuration value variant supporting all primitive types
using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    std::string configType;
    bool required;
    std::string defaultValue;
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string"
    std::string description;
    bool required;
    std::string defaultValue;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
};

// Configuration history entry for audit trails
struct ConfigHistoryEntry {
    std::string timestamp;
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    std::string action; // "set", "remove", "import", "export", "clear"
};

// Main Configuration class with enhanced functionality
class Config {
public:
    Config();
    ~Config();
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;

    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromJSON(const std::string& jsonStr);
    bool loadFromYAML(const std::string& yamlStr);
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    std::string exportConfig(const std::string& format = "legacy") const;
    
    // Configuration setting
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Programmatic);
    
    // Configuration getting with type-safe accessors
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int> getInt(const std::string& key) const;
    std::optional<int64_t> getInt64(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    
    // Default value accessors
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const;
    int getIntOr(const std::string& key, int defaultValue) const;
    int64_t getInt64Or(const std::string& key, int64_t defaultValue) const;
    double getDoubleOr(const std::string& key, double defaultValue) const;
    bool getBoolOr(const std::string& key, bool defaultValue) const;
    
    // Configuration metadata
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    void remove(const std::string& key);
    void clear();
    std::string summary() const;
    
    // Schema and validation
    bool addSchema(const ConfigSchema& schema);
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    bool isValueValid(const std::string& key, const ConfigValue& value) const;
    
    // Configuration inheritance
    void setInheritance(const std::string& key, const std::string& baseKey);
    void mergeWithInheritedConfig();
    
    // Configuration history and audit
    std::vector<ConfigHistoryEntry> getHistory() const;
    std::vector<ConfigHistoryEntry> getHistory(const std::string& key) const;
    size_t getHistorySize() const { return pImpl->history.size(); }
    void clearHistory() { pImpl->history.clear(); }
    
    // Configuration backup and restore
    bool enableAutoSave(bool enable) { pImpl->autoSave = enable; return true; }
    bool backup(const std::string& filepath = "");
    bool restore(const std::string& filepath);
    
    // Configuration templates
    std::vector<std::string> getConfigTemplates() const;
    bool applyTemplate(const std::string& templateName);
    
    // Configuration change callbacks
    using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;
    void setChangeCallback(ConfigChangeCallback callback);
    
    // Import/Export enhanced functionality
    bool importConfig(const std::string& configData);
    bool importConfigFromJSON(const std::string& jsonStr);
    bool importConfigFromYAML(const std::string& yamlStr);
    
    // Configuration comparison and diff
    std::string diff(const Config& other) const;
    bool equals(const Config& other) const;
    
    // Configuration management
    void setBaseDir(const std::string& dir) { pImpl->baseDir = dir; }
    std::string getBaseDir() const { return pImpl->baseDir; }
    void setBackupDir(const std::string& dir) { pImpl->backupDir = dir; }
    std::string getBackupDir() const { return pImpl->backupDir; }
    
    // Validation control
    void enableValidation(bool enable) { pImpl->enableValidation = enable; }
    bool isValidationEnabled() const { return pImpl->enableValidation; }
    
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static std::string getCurrentTimestamp();

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
    const ConfigEntry* findEntry(const std::string& key) const;
    bool updateEntry(const std::string& key, const ConfigValue& value, ConfigSource source);
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    std::string formatValue(const ConfigValue& value) const;
    
    // Helper functions
    static bool parseJSON(const std::string& jsonStr, std::vector<ConfigEntry>& entries, ConfigSource source);
    static bool parseYAML(const std::string& yamlStr, std::vector<ConfigEntry>& entries, ConfigSource source);
    static bool validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema);
};

} // namespace nlm