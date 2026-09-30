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

// Configuration value types
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
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(), 
                   createdAt(), updatedAt() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat),
          createdAt(std::chrono::system_clock::now()),
          updatedAt(std::chrono::system_clock::now()) {}
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
class ConfigAdvanced {
public:
    ConfigAdvanced();
    ~ConfigAdvanced();
    
    // Disable copying, enable moving
    ConfigAdvanced(const ConfigAdvanced&) = delete;
    ConfigAdvanced& operator=(const ConfigAdvanced&) = delete;
    ConfigAdvanced(ConfigAdvanced&&) noexcept;
    ConfigAdvanced& operator=(ConfigAdvanced&&) noexcept;
    
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
    
    // Configuration summary
    std::string summary() const;
    
    // Configuration management
    void setEncryptionKey(const std::string& key);
    bool isEncrypted() const;
    
    // Export/Import
    bool exportConfig(const std::string& outputPath, const std::vector<FileFormat>& formats);
    bool importConfig(const std::string& inputPath, FileFormat format);
    
    // Hot-reloading
    bool enableHotReloading(const std::string& filepath, FileFormat format, int interval);
    
    // Backup/Restore
    bool backupConfig(const std::string& name);
    bool restoreConfig(const std::string& backupFile);
    std::vector<std::string> getBackupFiles() const;
    
    // Helper methods
    static std::string getCurrentTimestamp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string toString(const ConfigValue& value) const;
    std::string getTypeString(const ConfigValue& value) const;
    
    // File format detection
    static FileFormat detectFormat(const std::string& content);
};

} // namespace nlm