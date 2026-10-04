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
// Represents a single configuration key-value pair with metadata
// Used internally by the Config class to store configuration values
struct ConfigEntry {
    std::string key;              // Configuration key (e.g., "neuron_count")
    ConfigValue value;            // Stored value of any supported type
    ConfigSource source;          // How this value was obtained
    std::string description;      // Human-readable description
    std::string validationPattern; // Optional regex pattern for validation
    bool isRequired;              // Whether this key is required for valid configuration
    
    // Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationPattern(), isRequired(false) {}
    
    // Full constructor with validation support
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& pattern = "", 
                bool required = false)
        : key(k), value(v), source(s), description(desc), 
          validationPattern(pattern), isRequired(required) {}
};

// Main configuration class
// Centralized configuration management for NLM with support for:
// - Multiple configuration sources (defaults, files, command line, runtime)
// - Type-safe value access with automatic type conversion
// - Validation of configuration values
// - File loading with support for JSON, YAML, and simple key=value formats
// - Command line argument parsing
// - Configuration persistence and checkpointing
// 
// The configuration system enables:
// - Reproducible experiments through configuration files and random seeds
// - Flexible deployment through command line options
// - Runtime customization for interactive use
// - Comprehensive validation to catch configuration errors early
// 
// Configuration value types:
// - Scalar types: int, int64_t, double, bool, string
// - Vector types: std::vector<int>, std::vector<double>, std::vector<string>
// - Automatic type inference when setting values
// 
// Configuration sources (priority order):
// 1. CommandLine: Highest priority, overrides file and defaults
// 2. File: Medium priority, overrides defaults
// 3. Default: Built-in defaults
// 4. Runtime: Can be overridden by later runtime calls
// 
// Usage examples:
// 
// Creating and using configuration:
// auto config = std::make_shared<Config>();
// config->set("neuron_count", 1000, ConfigSource::Default);
// config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
// 
// Accessing values with type checking:
// auto neuronCount = config->get<int64_t>("neuron_count");
// if (neuronCount.has_value()) {
//     // Use the value
// }
// 
// Accessing values with defaults:
// int64_t count = config->getOr<int64_t>("neuron_count", 1000);
// 
// Loading from file:
// config->loadFromFile("configs/default.cfg");
// 
// Loading from command line:
// config->loadFromArgs(argc, argv);
// 
// Configuration file formats supported:
// 1. Simple key=value format (Phase 1/2): "key = value"
// 2. JSON format (Phase 3+): "{\"key\": \"value\"}"
// 3. YAML format (Phase 3+): "key: value"
// 
// Example configuration file (simple format):
// # NLM Configuration
// # Simulation parameters
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

// # Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

// # Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

// # Log level
log_level = INFO

// Configuration validation:
// The system can validate configuration values against:
// - Type requirements (e.g., neuron_count must be int64_t)
// - Value ranges (e.g., connection_probability between 0.0 and 1.0)
// - Pattern matching (e.g., string format validation)
// - Required keys (e.g., neuron_count must be set)
// 
// Error handling:
// Configuration errors are logged and reported through:
// - NLM_LOG_ERROR for critical errors
// - NLM_LOG_WARNING for warnings and non-critical issues
// - Graceful fallbacks to default values
// - Detailed error messages with context
// 
// Performance considerations:
// - O(1) access for get() operations
// - O(n) validation during file loading (n = number of config entries)
// - Type erasure with std::variant provides reasonable performance
// - Thread-safe for concurrent access (with external synchronization)
// 
// See related files:
// - src/core/Config/Config.cpp for implementation details
// - docs/ARCHITECTURE.md for configuration system design
// - examples/configuration.cpp for usage examples
// 
// Phase-specific configuration requirements:
// Phase 1: Skeleton implementation
// Phase 2: Real neural computation
// Phase 3: Advanced systems integration
// Phase 4: Cognitive systems
// Phase 5: Performance optimizations
// Phase 6: Final integration and validation
// 
// Configuration keys commonly used:
// - random_seed: Random number generator seed
// - simulation_timestep: Integration timestep
// - neuron_count: Total number of neurons
// - region_count: Number of brain regions
// - connection_probability: Connectivity probability
// - stdp_ltp_weight, stdp_ltd_weight: Plasticity parameters
// - development_synaptogenesis_rate, pruning_rate: Structural plasticity
// - checkpoint_dir: Directory for saved brain states
// - log_level: Logging verbosity
// 
// For scientific reproducibility, always save configuration
// alongside brain checkpoints to enable exact experiment replication.
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
