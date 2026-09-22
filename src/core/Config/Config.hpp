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

/**
 * @enum ConfigSource
 * @brief Source of a configuration value
 * 
 * Each configuration value is tagged with the source that set it.
 * This helps resolve conflicts when multiple sources set the same key.
 */
enum class ConfigSource {
    Default,    ///< Built-in default values
    File,       ///< Loaded from configuration file
    CommandLine,///< Set via command-line arguments
    Runtime     ///< Set programmatically at runtime
};

/**
 * @struct ConfigEntry
 * @brief A single configuration key-value pair with metadata
 * 
 * Each configuration entry stores the key, value, source, and optional
 * description. The description is used when saving configuration to files.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key
    ConfigValue value;        ///< Configuration value
    ConfigSource source;      ///< Source of this value
    std::string description;  ///< Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management system
 * 
 * The Config class provides a key-value configuration system with support for
 * multiple configuration sources (default, file, command-line, runtime). Values
 * can be integers, floats, booleans, strings, or arrays of these types.
 * 
 * The configuration system supports loading from:
 * - Configuration files (simple key=value format, JSON, YAML)
 * - Command-line arguments (--key=value or -k value)
 * - Runtime API calls (set() method)
 * 
 * When multiple sources set the same key, the source with higher priority wins:
 * CommandLine > File > Runtime > Default
 * 
 * Features:
 * - Type-safe value access with get() and getOr() methods
 * - Automatic type conversion for common types
 * - Safe error handling with fallbacks
 * - File I/O support for configuration persistence
 * - Hierarchical file format detection (JSON, YAML, key=value)
 * 
 * @see configs/default.cfg for examples of configuration file format
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Disable copying, enable moving
     * 
     * Configuration objects should not be copied to avoid confusion
     * about which copy's configuration is being used. Moving is allowed.
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Attempts to detect file format (JSON, YAML, or simple key=value) and load
     * accordingly. JSON and YAML parsing requires the nlohmann::json library.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully loaded, false on error
     * 
     * @note Command-line arguments have higher priority than file settings
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Supports two formats:
     * - --key=value (long option)
     * - -k value (short option)
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return true if parsing completed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves configuration in simple key=value format with descriptions.
     * Complex types (arrays) are saved in bracket notation.
     * 
     * @param filepath Path where configuration will be saved
     * @return true if file was successfully written, false on error
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value by key
     * 
     * Retrieves a configuration value with type-safe access. If the key doesn't
     * exist or the value has the wrong type, returns std::nullopt.
     * 
     * @tparam T Type of value to retrieve (int, int64_t, double, bool, std::string,
     *         std::vector<int>, std::vector<double>, or std::vector<std::string>)
     * @param key Configuration key to retrieve
     * @return std::optional<T> The value if found and type matches, otherwise nullopt
     * 
     * @code
     * auto neurons = config.get<int64_t>("brain.neuron_count");
     * if (neurons.has_value()) {
     *     std::cout << "Neuron count: " << neurons.value() << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with fallback default
     * 
     * Retrieves a configuration value or returns the default if not found.
     * This is the preferred method for accessing configuration values as it
     * handles the case where the key might not exist gracefully.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist
     * @return T The configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with an associated source. The source determines
     * the precedence of this value when multiple sources exist for the same key.
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this value (default: Runtime)
     * 
     * @see ConfigSource
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string version)
     * @overload
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (integer version)
     * @overload
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (float version)
     * @overload
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (boolean version)
     * @overload
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries and resets to default state.
     */
    void clear();
    
    /**
     * @brief Get a summary of all configuration entries
     * 
     * @return std::string Formatted summary showing all keys, values, and sources
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from string
     * 
     * @param str Input string
     * @return std::string String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase
     * 
     * @param str Input string
     * @return std::string Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    
    // Internal helper functions for file loading
    bool loadFromKeyValueFile(const std::string& filepath);
    bool loadFromJsonFile(const std::string& filepath);
    bool isJsonFile(const std::string& filepath);
    bool isYamlFile(const std::string& filepath);
};

} // namespace nlm
