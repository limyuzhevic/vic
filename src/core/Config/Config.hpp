#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file in JSON/YAML format
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced configuration validation
    bool validateConfig(const std::string& key, const ConfigValue& value) const;
    bool isValidKey(const std::string& key) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with source information
    std::string summary() const;
    
    // Get configuration statistics
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Import/export functionality
    bool merge(const Config& other);
    bool importFromJSON(const std::string& jsonString);
    std::string exportToJSON() const;
    
    // File format support
    enum class FileFormat {
        Auto,    // Auto-detect format
        JSON,    // JSON format
        YAML,    // YAML format
        INI,     // INI format
        CFG      // Simple key=value format
    };
    
    void setFileFormat(FileFormat format);
    FileFormat getFileFormat() const;
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createResearchConfig();
    
    // Configuration validation presets
    static bool validateAllConfigs(const Config& config);
    static std::vector<std::string> getValidationErrors(const Config& config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // JSON/YAML conversion helpers
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& j);
    
    // Error handling
    void setError(const std::string& error);
};

} // namespace nlm

// Note: The nlohmann/json library needs to be added to CMakeLists.txt
// Add the following to CMakeLists.txt:
// find_package(nlohmann_json REQUIRED)
// target_link_libraries(pynlm_core nlohmann_json::nlohmann_json)
