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

// Main configuration class for the NLM (熙然) neural simulation framework
// Manages neural simulation parameters and settings through a flexible key-value system
// 
// Configuration serves as the primary interface for:
//   - Setting neural network parameters (neuron_count, connection_probability, etc.)
//   - Configuring simulation behavior (timestep, max_steps, etc.)
//   - Managing learning parameters (learning_rate, regularization, etc.)
//   - Controlling debugging and output options
// 
// Configuration values are stored with their source (default, file, command-line, runtime)
// allowing override hierarchy: CommandLine > File > Runtime > Default
// This enables flexible configuration where command-line arguments override file settings,
// which override runtime settings, which override default values.
// 
// The configuration system supports multiple data types:
//   - Basic types: int, int64_t, double, bool, std::string
//   - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
// 
// Thread safety: Config objects are not thread-safe for concurrent modification.
// Use a single Config instance per thread or coordinate access externally.
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (Phase 2: Currently uses simple key=value format, planned JSON/YAML support)
// The file should contain configuration lines in the format:
//   key = value
//   key = "quoted value with spaces"
//   key = [value1, value2, ...]  // Array format
// Comments start with # or //
// 
// Returns true on success, false on file I/O errors
// The file format is designed for human readability while maintaining flexibility
// for complex configuration needs
    
    // Load from command line arguments
// Supports two formats:
//   --key=value    // Long option format
//   -key value     // Short option format
// Command line arguments have highest priority in configuration precedence
// 
// Returns true on success, false if there are parsing errors
// Invalid arguments are logged but don't prevent successful completion
    
    // Save configuration to file
// The file is saved in human-readable key=value format
// Configuration values are sorted by key for consistent output
// Each entry includes a comment with its source (default, file, command-line, runtime)
// 
// Returns true on success, false on file I/O errors
// The saved file can be reloaded using loadFromFile()
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
