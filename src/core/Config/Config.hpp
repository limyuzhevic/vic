#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "ErrorHandling/Error.hpp"

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "ErrorHandling/Error.hpp"

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

class Config {
public:
    Config();
    ~Config() noexcept;
    
    // Disable copying, enable moving with proper noexcept
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&& other) noexcept;
    Config& operator=(Config&& other) noexcept;
    
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
    
    // Memory management optimizations
    static constexpr bool isSmallObject() noexcept { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validate configuration key
    static void validateKey(const std::string& key);
    
    // Initialize implementation
    void initializeImpl();
    
    // Cleanup implementation
    void cleanupImpl() noexcept;
};

} // namespace nlm

// Define exception macros for use in other files
#define NLM_CONFIG_THROW(msg) throw nlm::ConfigurationException(msg)
#define NLM_CONFIG_THROW_CODE(msg, code) throw nlm::ConfigurationException(msg, code)
