#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>

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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Program,
    Runtime
};

// Configuration entry with enhanced functionality
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Comparison operators for easier searching
    bool operator==(const ConfigEntry& other) const { return key == other.key; }
    bool operator<(const ConfigEntry& other) const { return key < other.key; }
};

class Config {
public:
    Config();
    ~Config() = default;
    
    // Disable copying, enable moving with noexcept
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file (JSON format)
    [[nodiscard]] bool loadFromFile(const std::string& filepath) noexcept;
    
    // Load from command line arguments
    [[nodiscard]] bool loadFromArgs(int argc, char** argv) noexcept;
    
    // Save to file
    [[nodiscard]] bool saveToFile(const std::string& filepath) const noexcept;
    
    // Get values with enhanced error handling
    template<typename T>
    [[nodiscard]] std::optional<T> get(const std::string& key) const noexcept;
    
    template<typename T>
    [[nodiscard]] T getOr(const std::string& key, const T& defaultValue) const noexcept;
    
    // Set values with overload resolution
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) noexcept;
    
    // Enhanced configuration management
    [[nodiscard]] bool has(const std::string& key) const noexcept;
    void remove(const std::string& key) noexcept;
    [[nodiscard]] std::vector<std::string> getKeys() const noexcept;
    void clear() noexcept;
    
    // Configuration query with performance optimizations
    [[nodiscard]] std::string summary() const;
    
    // Enhanced configuration operations
    [[nodiscard]] std::optional<ConfigSource> getSource(const std::string& key) const noexcept;
    void update(const std::string& key, const std::string& value) noexcept;
    void merge(const Config& other) noexcept;
    
    // Validation and type checking
    [[nodiscard]] bool validate() const noexcept;
    [[nodiscard]] std::string getValidationErrors() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Optimized helper functions
    static std::string trim(const std::string& str) noexcept;
    static std::string toLower(const std::string& str) noexcept;
    
    // Optimized value conversion utilities
    static bool tryConvertToString(const ConfigValue& value, std::string& result) noexcept;
    static bool tryConvertFromString(const std::string& str, ConfigValue& value) noexcept;
    
    // Thread-safe operations
    void lock() const noexcept {}
    void unlock() const noexcept {}
};

// Explicit template instantiations for common types
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
