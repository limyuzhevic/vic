#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <chrono>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>

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
    AutoBackup
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    bool valid;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(), valid(true) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()), valid(true) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "vec_int", "vec_double", "vec_string"
    std::string description;
    std::string defaultValue;
    bool required;
    std::string validationRegex;
    double minValue;
    double maxValue;
    
    ConfigSchema() : required(false), minValue(std::numeric_limits<double>::lowest()), 
                     maxValue(std::numeric_limits<double>::max()) {}
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    void addError(const std::string& error) { errors.push_back(error); valid = false; }
    void addWarning(const std::string& warning) { warnings.push_back(warning); }
};

// Configuration backup info
struct BackupInfo {
    std::string filepath;
    std::chrono::system_clock::time_point created;
    size_t checksum;
    
    BackupInfo(const std::string& path) : filepath(path), created(std::chrono::system_clock::now()), checksum(0) {}
};

// Exception classes
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& message) : ConfigException(message) {}
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
    
    // Load from file (JSON/YAML format with fallback to key=value)
    bool loadFromFile(const std::string& filepath, bool autoBackup = true);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with backup creation
    bool saveToFile(const std::string& filepath, bool createBackup = true) const;
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Get configuration summary with details
    std::string summary() const;
    
    // Configuration schema management
    void addSchema(const ConfigSchema& schema);
    bool validateSchema() const;
    ValidationResult validate() const;
    
    // Backup and recovery
    std::vector<BackupInfo> listBackups(const std::string& pattern = "*");
    bool restoreFromBackup(const std::string& backupPath);
    bool autoBackup(const std::string& filepath);
    
    // File operation error handling
    static bool safeWrite(const std::string& filepath, const std::string& content);
    static bool safeRead(const std::string& filepath, std::string& content);
    static bool validateFilePath(const std::string& filepath);
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    
    // Configuration migration
    bool migrateFromLegacy(const std::string& legacyPath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool validateEntry(const ConfigEntry& entry) const;
    bool validateValueType(const ConfigEntry& entry) const;
    bool validateValueRange(const ConfigEntry& entry) const;
    size_t calculateChecksum(const std::string& content) const;
    std::string entryToString(const ConfigEntry& entry) const;
    std::string currentTimestamp() const;
};

// Template method implementations
inline std::optional<int> Config::get<int>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->valid) {
        return std::nullopt;
    }
    
    try {
        return std::get<int>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

inline std::optional<int64_t> Config::get<int64_t>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->valid) {
        return std::nullopt;
    }
    
    try {
        return std::get<int64_t>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

inline std::optional<double> Config::get<double>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->valid) {
        return std::nullopt;
    }
    
    try {
        return std::get<double>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

inline std::optional<bool> Config::get<bool>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->valid) {
        return std::nullopt;
    }
    
    try {
        return std::get<bool>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

inline std::optional<std::string> Config::get<std::string>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->valid) {
        return std::nullopt;
    }
    
    try {
        return std::get<std::string>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

inline int Config::getOr<int>(const std::string& key, const int& defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const {
    auto val = get<int64_t>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline double Config::getOr<double>(const std::string& key, const double& defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
