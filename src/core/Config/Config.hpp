#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <format>
#include <span>
#include <ranges>
#include <expected>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types with C++20 features
/**
 * @brief Enhanced variant type that can hold any supported configuration value.
 * 
 * The ConfigValue type supports basic scalar types and collections, with
 * enhanced type safety and modern C++20 features.
 */
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

/**
 * @brief Enhanced configuration source enumeration with improved safety.
 * 
 * This enum indicates how a configuration value was set, which is useful
 * for understanding precedence when multiple sources provide the same value.
 */
enum class ConfigSource : uint8_t {
public:
    /** @brief Configuration was set to its default value */
    Default = 0,
    /** @brief Configuration was loaded from a file */
    File = 1,
    /** @brief Configuration was set via command line arguments */
    CommandLine = 2,
    /** @brief Configuration was set at runtime (programmatically) */
    Runtime = 3,
    /** @brief Configuration was set via environment variables */
    Environment = 4
};

/**
 * @brief Enhanced configuration entry structure with C++20 features.
 * 
 * ConfigEntry represents a single configuration parameter with its value,
 * source, and description. This structure enables tracking of where each
 * configuration value originated for precedence resolution.
 */
struct ConfigEntry {
    /** @brief The configuration key/parameter name */
    std::string key;
    
    /** @brief The configuration value (type determined by value variant) */
    ConfigValue value;
    
    /** @brief Source from which this value was set */
    ConfigSource source;
    
    /** @brief Human-readable description of the configuration parameter */
    std::string description;
    
    /** @brief Default constructor creates empty entry */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry with specified parameters.
     * 
     * @param k The configuration key
     * @param v The configuration value
     * @param s The source of this configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Enhanced string representation using std::format (C++20)
    std::string toString() const {
        std::string valueStr;
        std::visit([&valueStr](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + val + "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += "\"" + val[i] + "\"";
                }
                valueStr += "]";
            } else {
                valueStr = std::format("{}", val);
            }
        }, value);
        
        return std::format("{} = {} (source: {})", key, valueStr, static_cast<int>(source));
    }
};

/**
 * @brief Main configuration class for NLM (Neural Learning Machine).
 * 
 * The Config class provides a type-safe, thread-safe configuration management
 * system for the NLM framework. It supports loading configuration from files,
 * command line arguments, and environment variables, storing configuration
 * values, and saving them back. Enhanced with C++20 features for improved
 * performance and type safety.
 * 
 * Key features:
 * - Type-safe value storage using std::variant
 * - Multiple configuration sources with precedence rules
 * - Pimpl idiom for efficient copying
 * - Comprehensive error handling using std::expected
 * - Simple key=value file format for Phase 1
 * - Enhanced string formatting using std::format
 * - Range-based algorithms for better performance
 * 
 * Usage example:
 * @code
 * Config config;
 * 
 * // Load from file
 * config.loadFromFile("config.cfg");
 * 
 * // Load from command line
 * config.loadFromArgs(argc, argv);
 * 
 * // Load from environment
 * config.loadFromEnvironment();
 * 
 * // Set values
 * config.set("learning_rate", 0.01f);
 * 
 * // Get values with type safety
 * auto learningRateOpt = config.get<double>("learning_rate");
 * if (learningRateOpt) {
 *     double lr = *learningRateOpt;
 * }
 * 
 * // Save configuration
 * config.saveToFile("output.cfg");
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor creates an empty configuration.
     * 
     * The configuration starts with no entries and all values will use defaults
     * when accessed via getOr().
     */
    Config();
    
    /**
     * @brief Destructor cleans up implementation details.
     */
    ~Config();
    
    /**
     * @brief Copy constructor is deleted (config objects are not copyable).
     * 
     * This enforces the Pimpl idiom and prevents accidental copies that could
     * lead to unexpected behavior.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted (config objects are not copyable).
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor enables efficient transfer of configuration data.
     * 
     * This allows config objects to be moved without deep copying the actual
     * configuration data.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator enables efficient transfer of configuration data.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file.
     * 
     * This function parses a configuration file in a simple key=value format.
     * Each line should contain a key-value pair. Values can be quoted if they
     * contain spaces or special characters. Lines starting with # or / are
     * treated as comments and ignored.
     * 
     * @param filepath Path to the configuration file to load.
     * 
     * @return std::expected<bool, std::string> true if the file was successfully 
     *         loaded and parsed, false otherwise with error message.
     * 
     * @note The file format is a simplified key=value format for Phase 1.
     *       A full JSON/YAML parser will be implemented in a future phase.
     * 
     * @see loadFromArgs
     * @see loadFromEnvironment
     * @see saveToFile
     */
    std::expected<bool, std::string> loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments.
     * 
     * This function parses command line arguments for configuration values.
     * It supports multiple formats:
     * - --key=value (long format)
     * - -k value (short format)
     * - --key value (alternative format)
     * 
     * @param argc Number of command line arguments (as passed to main).
     * @param argv Array of command line argument strings (as passed to main).
     * 
     * @return std::expected<bool, std::string> true if arguments were successfully 
     *         parsed, false otherwise with error message.
     * 
     * @note Command line arguments have the highest precedence, followed by
     *       environment variables, then config files, then defaults.
     * 
     * @see loadFromFile
     * @see loadFromEnvironment
     * @see set
     */
    std::expected<bool, std::string> loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Load configuration from environment variables.
     * 
     * This function parses environment variables for configuration values.
     * Each environment variable should be in the format NLM_KEY=value, where
     * the prefix NLM_ is stripped from the key.
     * 
     * @return std::expected<bool, std::string> true if environment variables were 
     *         successfully processed, false otherwise with error message.
     * 
     * @note Environment variables have lower precedence than command line
     *       arguments but higher than config files.
     * 
     * @see loadFromArgs
     * @see loadFromFile
     */
    std::expected<bool, std::string> loadFromEnvironment();
    
    /**
     * @brief Save configuration to a file.
     * 
     * This function writes all configuration entries to a file in a simple
     * key=value format that can be read back by loadFromFile(). Each entry is
     * preceded by a comment describing the parameter.
     * 
     * @param filepath Path to the configuration file to save.
     * 
     * @return std::expected<bool, std::string> true if the file was successfully 
     *         written, false otherwise with error message.
     * 
     * @note String values are written with double quotes, numeric arrays with
     *       brackets. The format is compatible with loadFromFile().
     * 
     * @see loadFromFile
     */
    std::expected<bool, std::string> saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with enhanced type safety.
     * 
     * This template method retrieves a configuration value of the specified
     * type. If the value exists and is of the requested type, it returns an
     * expected containing the value. Otherwise, returns unexpected with error
     * message.
     * 
     * @tparam T The type of value to retrieve (must be one of the supported
     *           variant types: int, int64_t, double, bool, std::string,
     *           std::vector<int>, std::vector<double>, or std::vector<std::string>)
     * @param key The configuration key to retrieve.
     * 
     * @return std::expected<T, std::string> The value if found and of the correct type,
     *         unexpected otherwise with descriptive error message.
     * 
     * @note This method provides better error reporting than std::optional.
     * 
     * @see getOr
     * @see set
     */
    template<typename T>
    std::expected<T, std::string> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with a default fallback and type safety.
     * 
     * This template method retrieves a configuration value, returning a default
     * value if the key is not found or if the value is of the wrong type.
     * This is more convenient than get() when a default is always acceptable.
     * 
     * @tparam T The type of value to retrieve.
     * @param key The configuration key to retrieve.
     * @param defaultValue The value to return if the key is not found.
     * 
     * @return T The configuration value if found and of correct type,
     *         otherwise the default value.
     * 
     * @note This method never throws exceptions.
     * 
     * @see get
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value programmatically.
     * 
     * This method sets a configuration value with the specified source. The
     * source helps track where configuration values come from and resolve
     * conflicts when the same key is set from multiple sources.
     * 
     * @tparam T Type of the value to set (must be one of the supported types).
     * @param key The configuration key.
     * @param value The configuration value.
     * @param source The source of this configuration value (default: Runtime).
     * 
     * @see ConfigSource
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists.
     * 
     * This method checks whether a configuration key has been set in the
     * configuration, regardless of its value.
     * 
     * @param key The configuration key to check.
     * 
     * @return true if the key exists in the configuration, false otherwise.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key.
     * 
     * This method removes a configuration key and its value from the
     * configuration if it exists. If the key doesn't exist, this is a no-op.
     * 
     * @param key The configuration key to remove.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys.
     * 
     * This method returns a list of all configuration keys that have been set.
     * The returned vector contains unique keys in the order they were inserted.
     * 
     * @return std::vector<std::string> Vector containing all configuration keys.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values.
     * 
     * This method removes all configuration entries, leaving the configuration
     * in a clean state as if it were just constructed.
     */
    void clear();
    
    /**
     * @brief Get a string summary of the configuration.
     * 
     * This method generates a human-readable summary of all configuration
     * entries, showing keys, values, and their sources using modern formatting.
     * 
     * @return std::string Human-readable configuration summary.
     */
    std::string summary() const;
    
    // Enhanced C++20 helper methods
    
    /**
     * @brief Validate configuration value with type checking.
     * 
     * @tparam T Expected type
     * @param key Configuration key to validate
     * @return std::expected<bool, std::string> Validation result
     */
    template<typename T>
    std::expected<bool, std::string> validate(const std::string& key) const {
        auto valueOpt = get<T>(key);
        if (!valueOpt) {
            return std::unexpected(std::format("Configuration key '{}' not found or wrong type", key));
        }
        return true;
    }
    
    /**
     * @brief Get configuration as span for efficient processing.
     * 
     * @return std::span<const ConfigEntry> Span of configuration entries
     */
    std::span<const ConfigEntry> getEntries() const;
    
    /**
     * @brief Filter configuration entries by source.
     * 
     * @param source Configuration source to filter by
     * @return std::vector<ConfigEntry> Filtered entries
     */
    std::vector<ConfigEntry> filterBySource(ConfigSource source) const;
    
    /**
     * @brief Get configuration statistics using range algorithms.
     * 
     * @return std::string Statistics string
     */
    std::string getStatistics() const;
    
    // Structured bindings helper for easier access
    auto getEntryByKey(const std::string& key) const {
        for (const auto& entry : getEntries()) {
            if (entry.key == key) {
                return std::tie(entry.key, entry.value, entry.source, entry.description);
            }
        }
        return std::tie(std::string(), ConfigValue(), ConfigSource::Default, std::string());
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Enhanced private helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isComment(const std::string& line);
    static std::pair<std::string, std::string> parseKeyValue(const std::string& line);
};

} // namespace nlm