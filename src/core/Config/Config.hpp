#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <typeindex>

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
    size_t position; // For O(1) lookups
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), position(0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), position(0) {}
};

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    ConfigSource source;
    bool required;
    std::string description;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;
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
    
    // Batch load from multiple files
    bool batchLoadFromFiles(const std::vector<std::string>& filepaths);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values by index (O(1) lookup)
    template<typename T>
    std::optional<T> get(size_t index) const;
    
    template<typename T>
    std::optional<int> get(int index) const;
    
    template<typename T>
    std::optional<int64_t> get(int64_t index) const;
    
    template<typename T>
    std::optional<double> get(double index) const;
    
    template<typename T>
    std::optional<bool> get(bool index) const;
    
    template<typename T>
    std::optional<std::string> get(const std::string& key) const;
    
    template<typename T>
    std::optional<std::vector<int>> get(const std::vector<int>& key) const;
    
    template<typename T>
    std::optional<std::vector<double>> get(const std::vector<double>& key) const;
    
    template<typename T>
    std::optional<std::vector<std::string>> get(const std::vector<std::string>& key) const;
    
    // Get or return default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get entry count
    size_t size() const;
    
    // Schema validation
    bool validateSchema(const std::vector<ConfigSchemaEntry>& schema) const;
    std::string getValidationErrors() const;
    
    // Configuration comparison
    std::string compareTo(const Config& other) const;
    
    // Automated configuration optimization
    std::string optimize(const std::string& optimizationType = "performance") const;
    
    // Performance and memory tracking
    double getMemoryUsage() const;
    std::string getPerformanceStats() const;
    
    // Public helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    nlohmann::json toJson(const ConfigValue& value) const;
    ConfigValue fromJson(const nlohmann::json& j) const;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    size_t position; // For O(1) lookups
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), position(0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), position(0) {}
};

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    ConfigSource source;
    bool required;
    std::string description;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;
};