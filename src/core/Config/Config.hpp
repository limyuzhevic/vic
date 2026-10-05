/**
 * NLM Configuration System
 * 
 * This file contains the comprehensive configuration system for NLM (Neural Learning Machine).
 * The configuration system supports multiple formats:
 *   - JSON (.json files)
 *   - YAML (.yaml/.yml files)  
 *   - Configuration file (.cfg files) - original format for backward compatibility
 * 
 * Features:
 *   - Multi-format support for maximum flexibility
 *   - Type-safe configuration value storage
 *   - Source tracking (file, command line, runtime)
 *   - Configuration validation and error handling
 *   - Documentation strings for each configuration parameter
 *   - Automatic type conversion for arrays and scalars
 *   - Hierarchical configuration structure
 * 
 * Configuration hierarchy:
 *   - Core parameters (simulation, random, logging)
 *   - Brain parameters (neurons, regions, connectivity)
 *   - Neuromodulation parameters (dopamine, curiosity, novelty)
 *   - Reward processing parameters
 *   - Environment parameters
 *   - Visualization parameters
 *   - Experimental parameters
 * 
 * Usage example:
 *   Config config;
 *   config.loadFromFile("config.json");
 *   float learningRate = config.getOr("brain.plasticity.learning_rate", 0.01f);
 *   config.set("simulation.max_steps", 5000, ConfigSource::Runtime);
 *   
 * @author NLM Development Team
 * @version 2.0
 * @date 2026
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

// Forward declarations for JSON/YAML libraries
namespace nlohmann {
    template<typename> class basic_json;
    using json = basic_json<std::string, std::vector, std::map, std::set, std::string, float, int64_t, bool>;
}

namespace YAML {
    class Node;
    class Emitter;
}

namespace nlm {

/**
 * @brief Enumeration of supported configuration file formats.
 * 
 * This enum defines the different file formats that the configuration system supports.
 * The format detection is based on file extensions and content analysis.
 */
enum class FileFormat {
    JSON,      ///< JSON format (.json files)
    YAML,      ///< YAML format (.yaml or .yml files)
    CFG,       ///< Configuration file format (.cfg files) - original simple format
    UNKNOWN    ///< Unknown or unsupported format
};

/**
 * @brief Enumeration of configuration sources for tracking parameter origins.
 * 
 * This enum tracks where configuration parameters come from, which is useful for
 * debugging and understanding parameter precedence (file > command line > runtime).
 */
enum class ConfigSource {
    Default,       ///< Default value from configuration file
    File,          ///< Loaded from configuration file
    CommandLine,   ///< Set via command line arguments
    Runtime        ///< Set at runtime via API
};

/**
 * @brief Configuration entry structure storing a single configuration parameter.
 * 
 * This structure holds a configuration parameter value, its source, and documentation.
 * The use of std::variant allows storing different value types (scalars, arrays).
 * 
 * @tparam T Supported value types: int, int64_t, double, bool, std::string,
 *           std::vector<int>, std::vector<double>, std::vector<std::string>
 */
struct ConfigEntry {
    std::string key;               ///< Configuration parameter key
    ConfigValue value;             ///< Configuration value (type-safe variant)
    ConfigSource source;           ///< Origin of this configuration value
    std::string description;       ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all parameters.
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM.
 * 
 * This class provides a comprehensive configuration system for NLM with support
 * for multiple file formats, type safety, source tracking, and validation.
 * 
 * The configuration system allows users to:
 *   - Load configuration from JSON, YAML, or simple .cfg files
 *   - Save configuration in various formats
 *   - Set and get configuration parameters with type safety
 *   - Track configuration sources for debugging
 *   - Override configurations at runtime
 *   - Get configuration summaries and validation
 * 
 * Thread safety: Methods are not thread-safe. External synchronization is required
 * for concurrent access.
 */
class Config {
public:
    /**
     * @brief Constructor creates empty configuration.
     */
    Config();
    
    /**
     * @brief Destructor.
     */
    ~Config();
    
    /**
     * @brief Disable copying for safety.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable assignment for safety.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file.
     * 
     * Automatically detects file format (JSON, YAML, or .cfg) based on file
     * extension and content. Falls back to .cfg format if detection fails.
     * 
     * @param filepath Path to configuration file
     * @return true if configuration loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from JSON file.
     * 
     * Parses JSON configuration file using nlohmann::json. The JSON structure
     * should match the hierarchical organization used throughout NLM.
     * 
     * @param filepath Path to JSON configuration file
     * @return true if configuration loaded successfully, false otherwise
     */
    bool loadFromJSON(const std::string& filepath);
    
    /**
     * @brief Load configuration from YAML file.
     * 
     * Parses YAML configuration file using yaml-cpp. The YAML structure
     * should match the hierarchical organization used throughout NLM.
     * 
     * @param filepath Path to YAML configuration file
     * @return true if configuration loaded successfully, false otherwise
     */
    bool loadFromYAML(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments.
     * 
     * Supports both --key=value and -key value formats for command line
     * configuration overrides. Command line settings take precedence over
     * file settings.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if configuration loaded successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file.
     * 
     * Automatically detects file format based on file extension and saves
     * in the appropriate format (JSON, YAML, or .cfg).
     * 
     * @param filepath Path to save configuration file
     * @return true if configuration saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to JSON file.
     * 
     * Saves configuration in JSON format with proper indentation for readability.
     * 
     * @param filepath Path to save JSON configuration file
     * @return true if configuration saved successfully, false otherwise
     */
    bool saveToJSON(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to YAML file.
     * 
     * Saves configuration in YAML format with proper indentation for readability.
     * 
     * @param filepath Path to save YAML configuration file
     * @return true if configuration saved successfully, false otherwise
     */
    bool saveToYAML(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type checking.
     * 
     * Retrieves configuration value for given key with type-safe casting.
     * Returns std::nullopt if key not found or type mismatch.
     * 
     * @tparam T Expected type (int, double, bool, std::string, or std::vector)
     * @param key Configuration key
     * @return Configuration value if found and type matches, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback.
     * 
     * Retrieves configuration value for given key, returning default value
     * if key not found or type mismatch.
     * 
     * @tparam T Expected type (int, double, bool, std::string, or std::vector)
     * @param key Configuration key
     * @param defaultValue Default value to return if key not found
     * @return Configuration value if found and type matches, defaultValue otherwise
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value.
     * 
     * Sets configuration value for given key with specified source. The source
     * helps track where configuration values come from for debugging purposes.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from string.
     * 
     * Convenience method for setting string configuration values.
     * 
     * @param key Configuration key
     * @param value String configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from integer.
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key Configuration key
     * @param value Integer configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from double.
     * 
     * Convenience method for setting floating-point configuration values.
     * 
     * @param key Configuration key
     * @param value Double configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from boolean.
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key Configuration key
     * @param value Boolean configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists.
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key.
     * 
     * Removes configuration parameter for given key from the configuration.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys.
     * 
     * @return Vector of all configuration keys in the current configuration
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries.
     * 
     * Removes all configuration parameters from the configuration.
     */
    void clear();
    
    /**
     * @brief Get configuration summary.
     * 
     * Generates a human-readable summary of all configuration parameters
     * including their values and sources.
     * 
     * @return Formatted configuration summary string
     */
    std::string summary() const;
    
    /**
     * @brief Detect file format from file extension and content.
     * 
     * Automatically detects whether a file contains JSON, YAML, or .cfg format
     * based on both file extension and content analysis.
     * 
     * @param filepath Path to configuration file
     * @return Detected file format
     */
    static FileFormat detectFileFormat(const std::string& filepath);
    
    /**
     * @brief Convert string to lowercase for case-insensitive operations.
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
private:
    /**
     * @brief Internal configuration implementation.
     * 
     * This class uses the Pimpl idiom to hide implementation details and
     * improve compilation times.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Convert JSON data to configuration entries.
     * 
     * Recursively converts JSON data to configuration entries with hierarchical
     * key names (e.g., "brain.plasticity.learning_rate").
     * 
     * @param jsonData JSON data to convert
     * @param prefix Key prefix for hierarchical keys
     */
    void convertJSONToConfig(const nlohmann::json& jsonData, const std::string& prefix);
    
    /**
     * @brief Convert YAML data to configuration entries.
     * 
     * Recursively converts YAML data to configuration entries with hierarchical
     * key names (e.g., "brain.plasticity.learning_rate").
     * 
     * @param yamlData YAML data to convert
     * @param prefix Key prefix for hierarchical keys
     */
    void convertYAMLToConfig(const YAML::Node& yamlData, const std::string& prefix);
    
    /**
     * @brief Convert configuration entries to JSON format.
     * 
     * Recursively converts configuration entries to JSON format.
     * 
     * @param jsonData JSON object to populate
     * @param prefix Key prefix for hierarchical keys
     */
    void convertConfigToJSON(nlohmann::json& jsonData, const std::string& prefix) const;
    
    /**
     * @brief Convert configuration entries to YAML format.
     * 
     * Recursively converts configuration entries to YAML format.
     * 
     * @param emitter YAML emitter to write to
     * @param prefix Key prefix for hierarchical keys
     */
    void convertConfigToYAML(YAML::Emitter& emitter, const std::string& prefix) const;
    
    /**
     * @brief Get configuration value as generic ConfigValue.
     * 
     * @param value Configuration value to return as generic type
     * @return Generic configuration value
     */
    static ConfigValue getConfigValue(const ConfigValue& value);
    
    // Internal helper functions
    /**
     * @brief Trim whitespace from string.
     * 
     * @param str Input string
     * @return String with whitespace trimmed from both ends
     */
    static std::string trim(const std::string& str);
};

// Template implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const;

} // namespace nlm