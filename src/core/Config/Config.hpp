#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Configuration management system for NLM
// Provides hierarchical configuration with multiple sources and type safety
//
// Configuration System Architecture:
// =================================
// The Config class implements a hierarchical configuration system that supports
// multiple configuration sources with priority order:
//
// 1. **Command Line**: Highest priority, overrides all other sources
// 2. **File**: Load from JSON/YAML/INI files (application defaults)
// 3. **Runtime**: Set programmatically during execution
// 4. **Default**: Hardcoded defaults, lowest priority
//
// Key Features:
// - **Type-safe**: Variant-based storage with compile-time type checking
// - **Hierarchical**: Multiple levels of precedence
// - **Flexible**: Supports int, float, bool, string, and vector types
// - **Persistent**: Save/load configuration to/from files
// - **Debug-friendly**: Comprehensive error reporting and validation
// - **Thread-safe**: Read operations are thread-safe, writes require synchronization
//
// Configuration Types:
// ------------------
// int, int64_t: Integer parameters (e.g., neuron count, simulation steps)
// double, float: Floating-point parameters (e.g., time constants, weights)
// bool: Boolean flags (e.g., enable debug, enable optimization)
// string: String parameters (e.g., model names, file paths)
// vector<int>: Integer arrays (e.g., neuron populations, region IDs)
// vector<double>: Floating-point arrays (e.g., synaptic weights, thresholds)
// vector<string>: String arrays (e.g., file paths, labels)
//
// Configuration Usage Examples:
// ----------------------------
// Example 1: Basic configuration loading
// std::shared_ptr<Config> config = std::make_shared<Config>();
// config->loadFromFile("nlm_config.json");
// config->loadFromArgs(argc, argv);
//
// Example 2: Type-safe parameter access
// int neuronCount = config->getOr<int>("neuron_count", 1000);
// double learningRate = config->getOr<double>("learning_rate", 0.01);
// bool enableDebug = config->getOr<bool>("debug_enabled", false);
//
// Example 3: Runtime configuration
// config->set("simulation_time", 100.0);
// config->set("enable_plasticity", true);
//
// Error Handling:
// -------------
// - Missing keys return nullopt for get<T>()
// - getOr<T>() provides default values for missing keys
// - Type mismatches cause silent fallback to default
// - Configuration validation can be added for critical parameters
//
// Performance Considerations:
// ---------------------------
// - O(1) access time for most operations
// - Copy-on-write semantics for efficient transfers
// - Minimizes allocations in hot paths
// - Configurable persistence for production use
//
// File Formats:
// -------------
// Currently supports standard JSON format via file loading
// Planned: YAML and INI format support in future versions

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

// Helper functions (inline implementations)
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

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
    
    // Load from file (JSON/YAML/INI format)
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

