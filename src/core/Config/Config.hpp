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
    
    // Advanced configuration commands
    
    // Config file management commands
    std::vector<std::string> listFiles(const std::string& directory = ".") const;
    bool validateFile(const std::string& filepath) const;
    bool convertFile(const std::string& inputPath, const std::string& outputPath) const;
    bool mergeFiles(const std::vector<std::string>& filePaths, const std::string& outputPath) const;
    std::string diffFiles(const std::string& path1, const std::string& path2) const;
    std::string backupFile(const std::string& filepath) const;
    
    // Advanced config operations
    nlm::Config extractKeys(const std::vector<std::string>& keys) const;
    nlm::Config generateTemplate(const std::string& templateType) const;
    std::string getStatistics() const;
    std::vector<std::string> searchKeys(const std::string& pattern) const;
    bool migrateFromVersion(const std::string& oldFormat, const std::string& targetVersion) const;
    
    // Export/import utilities
    bool exportToJSON(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    bool importFromYAML(const std::string& filepath);
    
    // Validation and schema
    bool validateAgainstSchema(const std::string& schemaFile) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::string getFileExtension(const std::string& filepath) const;
    bool isValidFormat(const std::string& filepath) const;
};

} // namespace nlm
