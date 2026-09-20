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
    // Get configuration summary
    std::string summary() const;
    
    // Advanced configuration features
    
    // Batch operations
    bool setBatch(const std::vector<std::pair<std::string, ConfigValue>>& updates,
                 ConfigSource source = ConfigSource::Runtime);
    std::vector<std::pair<std::string, bool>> setMultipleFromJson(
        const std::string& filepath);
    
    // Validation
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Configuration templates
    static bool loadTemplate(const std::string& name,
                             const std::string& filepath);
    bool saveTemplate(const std::string& name) const;
    
    // Configuration comparison
    bool hasChanges(const Config& other) const;
    double calculateSimilarity(const Config& other) const;
    
    // Configuration import/export
    bool exportToJson(const std::string& filepath, bool pretty = true) const;
    bool importFromJson(const std::string& filepath);
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::string getStats() const;
    
    // Configuration keys with specific properties
    std::vector<std::string> getIntegerKeys() const;
    std::vector<std::string> getDoubleKeys() const;
    std::vector<std::string> getStringKeys() const;
    std::vector<std::string> getBoolKeys() const;
    std::vector<std::string> getVectorKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper for advanced operations
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    ConfigValue convertToTargetType(const ConfigValue& value,
                                    const std::string& targetType) const;
}

} // namespace nlm
