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
    Default,    // Default values built into the code
    File,       // Values loaded from configuration file
    CommandLine, // Values from command line arguments
    Runtime     // Values set at runtime
};

// Configuration entry struct for storing configuration values with metadata
struct ConfigEntry {
    std::string key;                // Configuration key (dot-separated for nested values)
    ConfigValue value;             // Configuration value
    ConfigSource source;           // How this value was set (Default/File/CommandLine/Runtime)
    std::string description;       // Human-readable description of the configuration option
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Configuration class for managing NLM system settings
 * 
 * The Config class provides a flexible key-value storage system for NLM configuration
 * parameters. Values can be loaded from files, command line arguments, or set programmatically.
 * The configuration supports various data types including scalars, vectors, and complex nested structures.
 * 
 * Configuration values use a dot-separated key format for nested organization:
 * - brain.neuron_count
 * - plasticity.stdp.learning_rate
 * - neuromodulation.curiosity.exploration_bonus
 * 
 * Configuration sources determine the precedence of values:
 * 1. CommandLine (highest priority)
 * 2. File
 * 3. Runtime
 * 4. Default (lowest priority)
 * 
 * Example usage:
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("brain.neuron_count", 1000);
 * config->set("plasticity.stdp.learning_rate", 0.001f);
 * 
 * if (config->has("brain.neuron_count")) {
 *     int neurons = config->get<int>("brain.neuron_count").value_or(0);
 * }
 * 
 * // Validate configuration
 * std::vector<std::string> errors;
 * if (!config->validate(errors)) {
 *     for (const auto& error : errors) {
 *         std::cerr << "Config error: " << error << std::endl;
 *     }
 * }
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor creates an empty configuration
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    // === Core Configuration Interface ===
    
    /**
     * @brief Disable copying (pimpl idiom)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable assignment (pimpl idiom)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable moving (pimpl idiom)
     */
    Config(Config&&) noexcept = default;
    
    /**
     * @brief Enable move assignment (pimpl idiom)
     */
    Config& operator=(Config&&) noexcept = default;
    
    // === File I/O ===
    
    /**
     * @brief Load configuration from a file
     * 
     * @param filepath Path to the configuration file
     * @return true if file was loaded successfully, false otherwise
     * 
     * The file format supports simple key=value pairs:
     * @code
     * # brain.neuron_count = 1000
     * brain.synapse_density = 0.1
     * # plasticity.stdp.learning_rate = 0.001
     * plasticity.hebbian.enable = true
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if arguments were processed successfully
     * 
     * Supports both --key=value and -key value formats.
     * Command line values override file values (Runtime source).
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * @param filepath Path where configuration will be saved
     * @return true if file was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    // === Value Access ===
    
    /**
     * @brief Get a configuration value of specified type
     * 
     * @tparam T Desired value type
     * @param key Configuration key
     * @return Optional containing the value if key exists, empty otherwise
     * 
     * Supported types: int, int64_t, double, bool, std::string,
     *                   std::vector<int>, std::vector<double>, std::vector<std::string>
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Desired value type
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Value (either from config or default)
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of the value (Runtime, CommandLine, etc.)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // === Query Operations ===
    
    /**
     * @brief Check if configuration contains a key
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * @param key Key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get a human-readable summary of configuration
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
    // === Configuration Validation ===
    
    /**
     * @brief Validate configuration against predefined rules
     * 
     * @param errors Reference to vector where validation errors will be stored
     * @return true if configuration is valid, false otherwise
     * 
     * Validates required keys, type constraints, and value ranges.
     * Common errors include missing required keys and invalid value types.
     */
    bool validate(std::vector<std::string>& errors) const;
    
    /**
     * @brief Get validation errors for current configuration
     * 
     * @return Vector of validation error messages
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * @brief Check if configuration is valid
     * 
     * @return true if configuration passes validation
     */
    bool isValid() const;
    
    /**
     * @brief Get detailed validation report
     * 
     * @return Human-readable validation report with error details
     */
    std::string getValidationReport() const;
    
    // === Configuration Presets ===
    
    /**
     * @brief Apply a predefined configuration preset
     * 
     * @param preset Name of preset to apply ("debug", "production", "research")
     * 
     * Built-in presets:
     * - debug: Low neuron count, high logging, detailed output
     * - production: Medium neuron count, optimized for speed
     * - research: High neuron count, maximum features enabled
     */
    void applyConfigPresets(const std::string& preset);
    
    /**
     * @brief Get list of available configuration presets
     * 
     * @return String describing available presets
     */
    static std::string getPresetList();
    
    // === Private Members ===
    
private:
    /**
     * @brief Internal implementation details (pimpl idiom)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
