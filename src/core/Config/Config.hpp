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
    /**
     * \brief Constructor - initializes configuration with default values
     * \details Creates a new configuration object with empty internal storage.
     *          The configuration is initialized with default values from the Default source.
     *          All internal operations are thread-safe via the pImpl pattern.
     */
    Config();

    /**
     * \brief Destructor - cleans up configuration resources
     * \details Properly cleans up the unique_ptr holding the implementation details.
     *          Ensures proper cleanup of all configuration data and resources.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * \brief Load configuration from a JSON file
     * \details Parses a JSON file and loads all configuration key-value pairs into the configuration.
     *          The file must be in standard JSON format with string keys and various value types
     *          (string, number, boolean, array). Values loaded from files take precedence over
     *          default values and have ConfigSource::File.
     * \param filepath Path to the JSON configuration file
     * \return true if the file was successfully loaded and parsed, false otherwise
     * \note If the file doesn't exist, it will be created if possible. Valid JSON syntax is required.
     * \example
     * \code{.cpp}
     * Config config;
     * if (!config.loadFromFile("config.json")) {
     *     std::cerr << "Failed to load config from file!" << std::endl;
     * }
     * \endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * \brief Load configuration from command line arguments
     * \details Parses command line arguments to extract configuration options in the format
     *          --key=value. Supports both long (--key=value) and short (-k=value) options.
     *          Values loaded from command line have the highest priority (ConfigSource::CommandLine).
     * \param argc Number of command line arguments (typically from main() argc)
     * \param argv Array of command line argument strings (typically from main() argv)
     * \return true if command line arguments were successfully parsed and loaded, false if any errors occurred
     * \note Command line arguments override values from files and defaults.
     *       Supported value types: string, integer, floating-point, boolean, arrays.
     * \example
     * \code{.cpp}
     * int main(int argc, char** argv) {
     *     Config config;
     *     if (!config.loadFromArgs(argc, argv)) {
     *         std::cerr << "Invalid command line arguments!" << std::endl;
     *         return 1;
     *     }
     *     // Configuration now contains values from command line
     * }
     * \endcode
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * \brief Save configuration to a JSON file
     * \details Serializes the current configuration to a JSON file. Only configuration entries
     *          from sources other than ConfigSource::Default are saved to maintain the hierarchy
     *          and precedence of configuration sources.
     * \param filepath Path where the JSON configuration file should be saved
     * \return true if the file was successfully written, false otherwise (e.g., permission denied)
     * \note File permissions and directory structure must exist and be writable. The saved
     *       configuration will only contain runtime and command-line values, preserving their
     *       source information for future reloads.
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("app.port", 8080);
     * config.set("app.debug", true);
     * if (!config.saveToFile("runtime_config.json")) {
     *     std::cerr << "Failed to save configuration!" << std::endl;
     * }
     * \endcode
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * \brief Get a configuration value with type safety
     * \details Retrieves a configuration value associated with the specified key. The template
     *          parameter determines the expected type of the configuration value. This method
     *          provides type-safe access to configuration values and handles type mismatches gracefully.
     * \tparam T Type of the configuration value to retrieve (must match one of ConfigValue types)
     * \param key Configuration key to retrieve
     * \return std::optional<T> The configuration value if found and matches the requested type,
     *         std::nullopt otherwise. Returns nullopt for missing keys or type mismatches.
     * \note Configuration key lookup is case-insensitive. The key is automatically converted
     *       to lowercase for comparison.
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("database.host", "localhost");
     * auto hostOpt = config.get<std::string>("database.host");
     * if (hostOpt) {
     *     std::cout << "Host: " << *hostOpt << std::endl;
     * }
     * \endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * \brief Get a configuration value with default fallback
     * \details Retrieves a configuration value associated with the specified key. If the key does not
     *          exist or the value type doesn't match the requested template type, returns the provided
     *          default value. This method is useful for safe configuration access without null checks.
     * \tparam T Type of the configuration value to retrieve (must match one of ConfigValue types)
     * \param key Configuration key to retrieve
     * \param defaultValue Value to return if key doesn't exist or type mismatch occurs
     * \return T The configuration value if found and matches the requested type, otherwise defaultValue.
     * \note Similar to get() but never returns std::nullopt - always provides a value.
     *       Type mismatch results in returning the default value.
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("app.timeout", 30);
     * int timeout = config.getOr("app.timeout", 10);
     * int port = config.getOr("network.port", 80);
     * std::cout << "Timeout: " << timeout << ", Port: " << port << std::endl;
     * \endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * \brief Set a configuration value with specific source
     * \details Overloads for setting configuration values with different types. The source parameter
     *          determines the priority and origin of the configuration value. By default, newly set
     *          values have ConfigSource::Runtime.
     * \param key Configuration key to set
     * \param value Configuration value as string
     * \param source Origin/source of the configuration value
     * \return void
     * \note Configuration keys can use dot notation for hierarchical organization (e.g., "app.database.host").
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("app.name", "MyApplication");
     * config.set("server.port", 8080, ConfigSource::CommandLine);
     * config.set("feature.enabled", true, ConfigSource::Runtime);
     * \endcode
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * \brief Set a configuration value with specific source
     * \details Overloads for setting configuration values with different types. The source parameter
     *          determines the priority and origin of the configuration value. By default, newly set
     *          values have ConfigSource::Runtime.
     * \param key Configuration key to set
     * \param value Configuration value as string
     * \param source Origin/source of the configuration value
     * \return void
     * \note Configuration keys can use dot notation for hierarchical organization (e.g., "app.database.host").
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * \brief Set a configuration value with specific source
     * \details Overloads for setting configuration values with different types. The source parameter
     *          determines the priority and origin of the configuration value. By default, newly set
     *          values have ConfigSource::Runtime.
     * \param key Configuration key to set
     * \param value Configuration value as int
     * \param source Origin/source of the configuration value
     * \return void
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * \brief Set a configuration value with specific source
     * \details Overloads for setting configuration values with different types. The source parameter
     *          determines the priority and origin of the configuration value. By default, newly set
     *          values have ConfigSource::Runtime.
     * \param key Configuration key to set
     * \param value Configuration value as double
     * \param source Origin/source of the configuration value
     * \return void
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * \brief Set a configuration value with specific source
     * \details Overloads for setting configuration values with different types. The source parameter
     *          determines the priority and origin of the configuration value. By default, newly set
     *          values have ConfigSource::Runtime.
     * \param key Configuration key to set
     * \param value Configuration value as bool
     * \param source Origin/source of the configuration value
     * \return void
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
/**
     * \brief Check if a configuration key exists
     * \details Determines whether a configuration key is currently present in the configuration.
     *          Lookup is case-insensitive and normalizes keys before comparison.
     * \param key Configuration key to check for existence
     * \return true if the key exists in the configuration, false otherwise
     * \note Key lookup is case-insensitive. "Host" and "host" refer to the same key.
     * \example
     * \code{.cpp}
     * Config config;
     * if (config.has("database.host")) {
     *     std::cout << "Database host is configured" << std::endl;
     * } else {
     *     std::cout << "Database host not configured" << std::endl;
     * }
     * \endcode
     */
    bool has(const std::string& key) const;
    
    /**
     * \brief Remove a configuration key
     * \details Deletes the configuration entry associated with the specified key from the configuration.
     *          This operation cannot remove keys with ConfigSource::Default.
     * \param key Configuration key to remove
     * \return void
     * \note This operation cannot remove keys with ConfigSource::Default as they are read-only.
     *       If the key doesn't exist, the operation silently succeeds.
     */
    void remove(const std::string& key);
    
    /**
     * \brief Get all configuration keys
     * \details Retrieves all configuration keys currently present in the configuration.
     *          Returns keys from all sources (Default, File, CommandLine, Runtime) in no specific order.
     * \return std::vector<std::string> List of all configuration keys in the configuration
     * \note The returned list includes all keys regardless of their source priority.
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("app.port", 8080);
     * config.set("app.debug", true);
     * auto keys = config.getKeys();
     * for (const auto& key : keys) {
     *     std::cout << key << std::endl;
     * }
     * \endcode
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * \brief Clear all configuration entries
     * \details Removes all configuration entries except those with ConfigSource::Default.
     *          This resets the configuration to its initial state with only default values.
     * \return void
     * \note Keys with ConfigSource::Default are preserved as they are read-only.
     *       This is different from remove() which only removes specific keys.
     * \example
     * \code{.cpp}
     * Config config;
     * config.set("app.temp", "value");
     * config.clear();
     * // config now only contains default values
     * \endcode
     */
    void clear();
    
    /**
     * \brief Get configuration summary
     * \details Generates a human-readable summary of the configuration state.
     *          The summary includes the total number of configuration entries and lists
     *          configuration values organized by source.
     * \return std::string Formatted configuration summary string
     * \note The summary provides insight into the configuration hierarchy and source priority.
     *       Useful for debugging and configuration validation.
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
