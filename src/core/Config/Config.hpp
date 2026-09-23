#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <optional>
#include <variant>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <any>
#include <map>
#include <iomanip>
#include "ConfigValidator.hpp"
#include "ConfigSchema.hpp"
#include "ConfigBuilder.hpp"
#include "ConfigFactory.hpp"

namespace nlm {

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
    std::any value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const std::any& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Type-safe configuration class
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
    
    // Initialize with schema
    void initializeWithSchema(const std::unordered_map<std::string, ConfigSchema>& schema);
    
    // Set values with validation (backward compatibility)
    void set(const std::string& key, const std::any& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe getters
    std::optional<int> getInt(const std::string& key) const;
    std::optional<int64_t> getInt64(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<int>> getIntVector(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleVector(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringVector(const std::string& key) const;
    
    // Generic typed getters
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-specific getters with defaults
    int getOrInt(const std::string& key, int defaultValue) const;
    int64_t getOrInt64(const std::string& key, int64_t defaultValue) const;
    double getOrDouble(const std::string& key, double defaultValue) const;
    bool getOrBool(const std::string& key, bool defaultValue) const;
    std::string getOrString(const std::string& key, const std::string& defaultValue) const;
    
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
    
    // JSON schema support
    static std::string generateJSONSchema(const std::unordered_map<std::string, ConfigSchema>& schema);
    bool validateAgainstSchema(const std::unordered_map<std::string, ConfigSchema>& schema) const;
    
    // Fluent builder interface
    static ConfigBuilder createBuilder();
    static ConfigFactory createFactory();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema for validation
    std::unordered_map<std::string, ConfigSchema> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool validateValue(const std::string& key, const std::any& value) const;
    void validateAndSet(const std::string& key, const std::any& value, ConfigSource source);
};

// For backward compatibility with the original ConfigValue type
table ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

} // namespace nlm