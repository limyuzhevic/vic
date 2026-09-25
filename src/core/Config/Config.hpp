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
    
    /**
     * Load configuration from a file (JSON format)
     * 
     * @param filepath Path to the configuration file
     * @return true if file was loaded successfully, false otherwise
     * 
     * The file should contain key-value pairs in JSON format.
     * Examples:
     *   {"neuron_count": 1000, "region_count": 1}
     *   {"simulation_timestep": 0.001, "random_seed": 42}
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line arguments array
     * @return true if arguments were parsed successfully, false otherwise
     * 
     * Supported formats:
     *   --key=value (for string, int, double, bool values)
     *   --key value (for string values without '=')
     *   Examples: --neuron_count=1000 --simulation_timestep=0.001 --random_seed=42
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * Save configuration to a file (JSON format)
     * 
     * @param filepath Path to save the configuration file
     * @return true if file was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Get a configuration value by key
     * 
     * @tparam T Type of the value to retrieve (int, double, bool, string, etc.)
     * @param key Configuration key
     * @return std::optional<T> The value if key exists, std::nullopt otherwise
     * 
     * Examples:
     *   config.get<int>("neuron_count") -> 1000
     *   config.get<double>("simulation_timestep") -> 0.001
     *   config.get<bool>("enable_checkpointing") -> true
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Get a configuration value with default fallback
     * 
     * @tparam T Type of the value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return T The value if key exists, otherwise defaultValue
     * 
     * Examples:
     *   config.getOr("neuron_count", 500) -> 1000 (if set) or 500 (if not set)
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set a configuration value with specific source
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of the configuration (Default, File, CommandLine, Runtime)
     * 
     * ConfigSource enum:
     *   - Default: Initial default configuration
     *   - File: Loaded from configuration file
     *   - CommandLine: Parsed from command line arguments
     *   - Runtime: Set programmatically at runtime
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Validate the configuration for consistency and correctness
     * 
     * @return true if configuration is valid, false otherwise
     * @throws std::runtime_error if validation fails with detailed error message
     * 
     * Validates:
     *   - Required parameters are set
     *   - Values are within acceptable ranges
     *   - No conflicting configuration parameters
     *   - Dependencies between parameters are satisfied
     */
    bool validate() const;
    
    /**
     * Get detailed validation error messages
     * 
     * @return Vector of validation error messages
     * 
     * @see validate()
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * Check existence of a configuration key
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove a configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Clear all configuration entries
     */
    void clear();
    
    /**
     * Get a human-readable summary of the configuration
     * 
     * @return String containing configuration summary
     * 
     * Useful for logging and debugging purposes
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // PIMPL implementation details
};

} // namespace nlm
