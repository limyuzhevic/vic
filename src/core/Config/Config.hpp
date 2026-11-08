#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Configuration entry
struct ConfigEntry {
    /**
     * @brief A single configuration entry containing a key-value pair
     * 
     * This structure represents a single configuration parameter with its
     * associated value, source, and optional description. Configuration entries
     * are stored in the Config class and can be retrieved using key-based access.
     */
    std::string key;                    ///< Configuration parameter name
    ConfigValue value;                  ///< Configuration value (variant type)
    ConfigSource source;                ///< Source of this configuration (Default, File, CommandLine, Runtime)
    std::string description;            ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a configuration entry with the specified key, value, source, and description.
     * 
     * @param k Configuration key/parameter name
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM simulation parameters
 * 
 * This class manages configuration parameters for the NLM (熙然) neural learning
 * machine project. It provides a unified interface for setting, getting, and
 * validating configuration values from multiple sources (default, file, command
 * line, runtime). Configuration values can be of various types (int, double,
 * string, bool, vectors of primitive types) and are validated for correctness.
 * 
 * The Config class supports:
 * - Loading configuration from files (key=value format)
 * - Parsing command line arguments (--key=value or -key value)
 * - Setting configuration values at runtime
 * - Validating configuration parameters
 * - Saving configuration to files
 * - Checking parameter existence and retrieving values
 * 
 * @note Configuration parameters are case-sensitive and use dot notation
 *       for nested values (e.g., "brain.neuron_count", "plasticity.stdp_ltp_weight").
 */
class Config {
public:
    /**
     * @brief Constructs an empty configuration object
     * 
     * Creates a new Config instance with no entries. All configuration parameters
     * must be set using the set() methods or by loading from files/command line
     * arguments. Default values can be set using the setDefault() method.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up all configuration entries and resources.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from a file
     * 
     * This method loads configuration parameters from a text file using the
     * key=value format. Empty lines and comments (lines starting with #, ;, or //)
     * are skipped. Values can be quoted strings. If the file cannot be opened,
     * the method returns false and no changes are made to the current configuration.
     * 
     * @param filepath Path to the configuration file to load
     * @return bool true if the configuration was loaded successfully, false otherwise
     * 
     * @note File loading errors are reported to stderr via NLM_LOG_ERROR().
     * @note Lines with invalid format are reported via NLM_LOG_WARN().
     * 
     * @see loadFromArgs()
     * @see saveToFile()
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command line arguments
     * 
     * This method parses command line arguments in the following formats:
     * - --key=value (for long options)
     * - -k value (for short options)
     * 
     * Command line arguments override configuration loaded from files. The method
     * processes all arguments from index 1 onwards (skipping argv[0] which is typically
     * the program name).
     * 
     * @param argc The number of command line arguments (typically from main())
     * @param argv The array of command line argument strings (typically from main())
     * @return bool true if configuration was parsed successfully, false otherwise
     * 
     * @note Command line parsing errors are reported to stderr via NLM_LOG_ERROR().
     * 
     * @see loadFromFile()
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves the current configuration to a file
     * 
     * This method saves the current configuration to a text file in key=value format.
     * Each configuration entry is preceded by its description as a comment. The
     * format is suitable for reloading via loadFromFile().
     * 
     * @param filepath Path to the configuration file to save
     * @return bool true if the configuration was saved successfully, false otherwise
     * 
     * @note File saving errors are reported to stderr via NLM_LOG_ERROR().
     * 
     * @see loadFromFile()
     */
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
