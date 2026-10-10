// Configuration System for NLM
// ==============================
// 
// This file defines the configuration system used by NLM (Neural Learning Machine)
// to manage simulation parameters, settings, and runtime configuration.
// 
// The configuration system provides:
// 1. Typed configuration values using std::variant for flexibility
// 2. Multiple configuration sources (default, file, command line, runtime)
// 3. Safe value retrieval with type conversion and error handling
// 4. Configuration persistence to file and from command line arguments
// 
// Configuration values support these types:
// - Basic types: int, int64_t, double, bool, std::string
// - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
// - Composite types: PlasticityFlags, SpikeEvent, etc.
// 
// Configuration sources:
// - Default: Built-in defaults loaded at startup
// - File: Loaded from JSON/toml/text config files
// - CommandLine: Parsed from --key=value or -k value arguments
// - Runtime: Set programmatically during execution
// 
// Usage example:
//   auto config = std::make_shared<Config>();
//   config->loadFromFile("config.cfg");
//   int neurons = config->getOr("neuron_count", 1000);
//   bool plasticity = config->getOr("enable_plasticity", true);

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

// Configuration source enumeration
enum class ConfigSource {
    Default,      // Built-in defaults (Phase 1 skeleton)
    File,         // Loaded from configuration file
    CommandLine,  // Parsed from command line arguments
    Runtime       // Set programmatically during execution
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;        // Configuration key
    ConfigValue value;       // Configuration value (untyped storage)
    ConfigSource source;     // How this value was loaded
    std::string description; // Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying for safety (pImpl pattern)
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable moving for efficiency
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // === File I/O Operations ===
    
    // Load configuration from file (JSON/toml/text format)
    // Currently supports simple key=value format with quotes
    // TODO PHASE 2: Implement proper JSON/YAML parser
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports --key=value and -k value formats
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // TODO PHASE 2: Implement proper formatting with value serialization
    bool saveToFile(const std::string& filepath) const;
    
    // === Value Access Operations ===
    
    // Get typed configuration value
    // Returns std::nullopt if key doesn't exist or type mismatch
    // Automatic string-to-type conversion for numeric and boolean values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    // More convenient than checking std::optional directly
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // === Value Setting Operations ===
    
    // Set configuration value with explicit source
    // Allows programmatic configuration during runtime
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience overloads for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // === Configuration Management ===
    
    // Check if configuration key exists
    bool has(const std::string& key) const;
    
    // Remove configuration key (useful for runtime overrides)
    void remove(const std::string& key);
    
    // Get all configuration keys (useful for introspection)
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // === Utility Operations ===
    
    // Get human-readable configuration summary
    std::string summary() const;
    
private:
    // Pimpl implementation for ABI stability and forward compatibility
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
