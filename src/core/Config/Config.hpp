#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

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

// Batch configuration result
struct BatchLoadResult {
    std::vector<std::string> loadedFiles;
    std::vector<std::string> failedFiles;
    std::vector<std::string> errors;
    size_t totalEntriesLoaded;
    
    BatchLoadResult() : totalEntriesLoaded(0) {}
    
    bool success() const {
        return failedFiles.empty();
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "Batch Load Summary:" << std::endl;
        ss << "  Loaded files: " << loadedFiles.size() << std::endl;
        ss << "  Failed files: " << failedFiles.size() << std::endl;
        ss << "  Total entries: " << totalEntriesLoaded << std::endl;
        return ss.str();
    }
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
    
    // Load from multiple files (batch loading)
    BatchLoadResult loadBatch(const std::vector<std::string>& filepaths);
    
    // Load from directory recursively
    BatchLoadResult loadFromDirectory(const std::string& directoryPath);
    
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
    
    // Batch operations
    void batchSet(const std::vector<std::pair<std::string, ConfigValue>>& settings);
    void batchRemove(const std::vector<std::string>& keys);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Get file list from directory
    static std::vector<std::string> getConfigFilesFromDirectory(const std::string& directoryPath);
    
    // Get configuration file extensions
    static std::vector<std::string> getSupportedConfigExtensions();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isConfigFile(const std::string& filename);
    
    // Validation helpers
    bool validateKey(const std::string& key) const;
    bool validateValue(const ConfigValue& value) const;
};

} // namespace nlm
