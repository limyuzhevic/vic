#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "constants.h"

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

// Configuration system constants
namespace ConfigSystemConstants {
    constexpr const char* DEFAULT_FILE_EXTENSION = ".cfg";           // Default config file extension
    constexpr const char* DEFAULT_BACKUP_EXTENSION = ".bak";       // Default backup extension
    constexpr size_t MAX_BACKUP_FILES = 10;                        // Maximum backup files to keep
    constexpr const char* AUTO_SAVE_DIR = "./auto_save";          // Auto-save directory
    constexpr size_t AUTO_SAVE_INTERVAL = 60;                     // Auto-save interval (seconds)
    constexpr const char* EXPORT_FORMAT = "json";                 // Default export format
    constexpr size_t MAX_CONFIG_SIZE = 10 * 1024 * 1024;           // Maximum config file size (10MB)
    constexpr const char* CONFIG_COMMENT_PREFIX = "#";             // Comment prefix
    constexpr const char* KEY_VALUE_SEPARATOR = "=";               // Key-value separator
    constexpr const char* SECTION_START = "[";                     // Section start marker
    constexpr const char* SECTION_END = "]";                      // Section end marker
}

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
    
    // Set values with constants fallback
    template<typename T>
    void setWithConstantFallback(const std::string& key, const std::string& constantName, const T& defaultValue);
    
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
    
    // Get configuration metadata
    std::string getMetadata() const;
    
    // File format support
    enum class FileFormat {
        KeyValue,
        JSON,
        YAML
    };
    
    void setFileFormat(FileFormat format);
    FileFormat getFileFormat() const;
    
    // Backup and restore
    bool createBackup(const std::string& originalFile, const std::string& backupFile = "") const;
    bool restoreFromBackup(const std::string& backupFile, const std::string& targetFile = "") const;
    
    // Auto-save functionality
    void enableAutoSave(size_t intervalSeconds = ConfigSystemConstants::AUTO_SAVE_INTERVAL);
    void disableAutoSave();
    bool isAutoSaveEnabled() const;
    std::string getAutoSavePath() const;
    
    // Configuration validation
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration export
    bool exportToFile(const std::string& filepath, const std::string& format = ConfigSystemConstants::EXPORT_FORMAT) const;
    bool exportToStream(std::ostream& output, const std::string& format = ConfigSystemConstants::EXPORT_FORMAT) const;
    
    // Configuration merge
    bool mergeWith(const Config& otherConfig, ConfigSource source = ConfigSource::File);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    std::string getStatistics() const;
    
    // Constant mapping for backward compatibility
    static std::string getConstantName(const std::string& key);
    static bool isConstantKey(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    
    // Configuration file parsing
    bool loadFromFileInternal(const std::string& filepath, FileFormat format);
    bool loadJsonFromFile(const std::string& filepath);
    bool loadYamlFromFile(const std::string& filepath);
    bool loadKeyValueFromFile(const std::string& filepath);
    
    // Configuration file writing
    bool saveToFileInternal(const std::string& filepath, FileFormat format) const;
    bool saveAsJson(const std::string& filepath) const;
    bool saveAsYaml(const std::string& filepath) const;
    bool saveAsKeyValue(const std::string& filepath) const;
    
    // Configuration conversion
    std::string configValueToString(const ConfigValue& value) const;
    bool stringToConfigValue(const std::string& str, ConfigValue& value) const;
    
    // Constant resolution
    bool resolveConstant(const std::string& key, ConfigValue& value) const;
    bool isBuiltInConstant(const std::string& key) const;
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Convenience functions
inline bool configHasKey(const Config& config, const std::string& key) {
    return config.has(key);
}

inline std::string configGetString(const Config& config, const std::string& key, const std::string& defaultValue = "") {
    auto value = config.get<std::string>(key);
    return value.has_value() ? value.value() : defaultValue;
}

inline double configGetDouble(const Config& config, const std::string& key, double defaultValue = 0.0) {
    auto value = config.get<double>(key);
    return value.has_value() ? value.value() : defaultValue;
}

inline int configGetInt(const Config& config, const std::string& key, int defaultValue = 0) {
    auto value = config.get<int>(key);
    return value.has_value() ? value.value() : defaultValue;
}

inline bool configGetBool(const Config& config, const std::string& key, bool defaultValue = false) {
    auto value = config.get<bool>(key);
    return value.has_value() ? value.value() : defaultValue;
}

inline void configSet(const Config& config, const std::string& key, const std::string& value) {
    const_cast<Config&>(config).set(key, value, ConfigSource::Runtime);
}

inline void configSet(const Config& config, const std::string& key, double value) {
    const_cast<Config&>(config).set(key, value, ConfigSource::Runtime);
}

inline void configSet(const Config& config, const std::string& key, int value) {
    const_cast<Config&>(config).set(key, value, ConfigSource::Runtime);
}

inline void configSet(const Config& config, const std::string& key, bool value) {
    const_cast<Config&>(config).set(key, value, ConfigSource::Runtime);
}

} // namespace nlm