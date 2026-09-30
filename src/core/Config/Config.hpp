#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <mutex>
#include <filesystem>
#include <any>
#include <type_traits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

// Configuration version
struct ConfigVersion {
    int major;
    int minor;
    int patch;
    
    ConfigVersion() : major(1), minor(0), patch(0) {}
    ConfigVersion(int major, int minor, int patch) : major(major), minor(minor), patch(patch) {}
    
    std::string toString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    
    bool operator==(const ConfigVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
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
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    EnvironmentVariable,
    Inheritance
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    ConfigVersion versionIntroduced;
    std::vector<std::string> tags;
    bool required;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(), 
                   versionIntroduced(), tags(), required(false), createdAt(), updatedAt() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat),
          versionIntroduced(1, 0, 0), tags(), required(false),
          createdAt(std::chrono::system_clock::now()),
          updatedAt(std::chrono::system_clock::now()) {}
};

// Validation rule types
enum class ValidationType {
    None,
    Type,
    Range,
    Regex,
    Dependency,
    CrossParameter,
    Custom
};

struct ValidationRule {
    std::string key;
    ValidationType type;
    std::string message;
    std::any parameters;
    std::function<bool(const ConfigValue&, const Config&)> customValidator;
    
    ValidationRule(const std::string& k, const std::string& msg)
        : key(k), type(ValidationType::None), message(msg), parameters(nullptr) {}
};

// Configuration change record
struct ConfigChangeRecord {
    std::chrono::system_clock::time_point timestamp;
    std::string key;
    std::optional<ConfigValue> oldValue;
    std::optional<ConfigValue> newValue;
    ConfigSource source;
    std::string user;
    std::string notes;
    
    ConfigChangeRecord(const std::string& k, std::optional<ConfigValue> old,
                      std::optional<ConfigValue> newVal, ConfigSource src,
                      const std::string& u = "")
        : timestamp(std::chrono::system_clock::now()), key(k),
          oldValue(old), newValue(newVal), source(src), user(u), notes("") {}
};

// Schema definition for configuration
struct ConfigSchema {
    std::string name;
    ConfigVersion version;
    std::vector<ValidationRule> validationRules;
    std::unordered_map<std::string, ConfigValue> defaultValues;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, std::string> keyDescriptions;
    std::unordered_map<std::string, std::string> keyCategories;
    
    ConfigSchema(const std::string& n) : name(n), version(1, 0, 0) {}
};

// File format enumeration
enum class FileFormat {
    Auto,
    KeyValue,
    JSON,
    YAML,
    XML,
    TOML,
    INI
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration loading with format support
    bool loadFromFile(const std::string& filepath, FileFormat format = FileFormat::Auto);
    bool loadFromString(const std::string& content, FileFormat format = FileFormat::Auto);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, FileFormat format = FileFormat::Auto) const;
    std::string saveToString(FileFormat format = FileFormat::Auto) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "");
    
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "");
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "");
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "");
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Configuration management
    void setSchema(const ConfigSchema& schema);
    void setVersion(const ConfigVersion& version);
    ConfigVersion getVersion() const;
    
    // Error recovery
    bool attemptRecovery();
    bool restoreBackup(const std::string& backupFile = "");
    std::vector<std::string> getBackupFiles() const;
    
    // History and tracking
    void enableHistory(bool enabled = true);
    bool isHistoryEnabled() const;
    void clearHistory();
    std::vector<ConfigChangeRecord> getHistory(size_t maxEntries = 100) const;
    
    // Environment variables
    void enableEnvironmentVariables(bool enabled = true);
    bool areEnvironmentVariablesEnabled() const;
    void setEnvironmentPrefix(const std::string& prefix);
    std::string getEnvironmentPrefix() const;
    
    // Hot-reloading
    void enableHotReloading(bool enabled = true);
    bool isHotReloadingEnabled() const;
    void setReloadInterval(int seconds);
    void startAutoReload(const std::string& filepath, FileFormat format = FileFormat::Auto);
    void stopAutoReload();
    
    // Inheritance
    void addInheritance(const std::string& sourceConfig, ConfigSource source = ConfigSource::Inheritance);
    void clearInheritance();
    std::vector<std::string> getInheritanceSources() const;
    
    // Templates and presets
    static Config createFromTemplate(const std::string& templateName);
    static bool saveAsTemplate(const std::string& name, const Config& config);
    static std::vector<std::string> getAvailableTemplates();
    
    // Comparison
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Documentation
    std::string generateDocumentation() const;
    std::string getAutocompleteSuggestions(const std::string& partialKey) const;
    
    // Configuration summary
    std::string summary() const;
    
    // Encryption
    void setEncryptionKey(const std::string& key);
    bool isEncrypted() const;
    
    // Statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    
    // File format parsers
    static std::string toJSON(const Config& config);
    static std::string toYAML(const Config& config);
    static std::string toXML(const Config& config);
    static std::string toTOML(const Config& config);
    static std::string toINI(const Config& config);
    
    static bool fromJSON(const std::string& content, Config& config);
    static bool fromYAML(const std::string& content, Config& config);
    static bool fromXML(const std::string& content, Config& config);
    static bool fromTOML(const std::string& content, Config& config);
    static bool fromINI(const std::string& content, Config& config);
    
    // Validation helpers
    bool validateValue(const ConfigEntry& entry) const;
    bool validateCrossParameters() const;
    
    // Environment variable helpers
    std::optional<ConfigValue> getFromEnvironment(const std::string& key) const;
    
    // Encryption helpers
    std::string encrypt(const std::string& data) const;
    std::string decrypt(const std::string& data) const;
    
    // File format detection
    static FileFormat detectFormat(const std::string& content);
};

} // namespace nlm