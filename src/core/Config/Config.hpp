#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

namespace nlm {

// Configuration system for NLM
// Provides type-safe configuration management with multiple input sources
// Supports serialization to/from files and command-line arguments
//
// Key features:
// - Type-safe configuration values (int, double, string, bool, vectors)
// - Multiple configuration sources (file, command line, runtime)
// - Priority-based source resolution
// - Validation and error handling
// - JSON/YAML support (planned for Phase 2)
// - Serialization to persistent storage
// - Runtime configuration modification

// Forward declarations
class Config;

// Configuration value types
// Using variant to support multiple value types with type safety
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
// Determines priority and persistence of configuration values
enum class ConfigSource {
    Default,      // Initial/default values
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments
    Runtime       // Set at runtime via API
};

// Configuration entry
// Stores a single configuration value with metadata
struct ConfigEntry {
    std::string key;                    // Configuration key
    ConfigValue value;                   // Configuration value
    ConfigSource source;                 // Source of this configuration value
    std::string description;              // Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying for performance
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable moving (for efficient transfers)
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file
    // Supports simple key=value format (Phase 1) and JSON/YAML (Phase 2)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports --key=value and -k value formats
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Converts internal representation to file format
    bool saveToFile(const std::string& filepath) const;
    
    // Get typed configuration value
    // Returns nullopt if key doesn't exist or type mismatch
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default
    // Returns default if key doesn't exist or type mismatch
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    // Validates key format and value type
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience overloads for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if key exists in configuration
    bool has(const std::string& key) const;
    
    // Remove configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // Get configuration summary for logging
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static void validateValue(const ConfigValue& value, const std::string& key);
};

// Helper function to check if string ends with extension
inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace nlm
