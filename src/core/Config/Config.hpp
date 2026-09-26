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
 * @brief Enumeration of configuration sources
 * 
 * Defines where configuration values originate from
 */
enum class ConfigSource {
    Default,      ///< Default configuration values
    File,          ///< Loaded from configuration file
    CommandLine,   ///< Set via command line arguments
    Runtime        ///< Set at runtime during execution
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single key-value pair in the configuration
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Source of this configuration value
    std::string description;   ///< Human-readable description of the configuration option
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration option
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration manager class
 * 
 * This class provides a thread-safe key-value store for configuration values
 * that can be loaded from files (JSON/YAML/safe format), command line arguments,
 * or set programmatically. Values support multiple types including int, int64_t,
 * double, bool, string, and vector of numeric/string types.
 * 
 * The configuration is implemented as a persistent singleton pattern using
 * a pImpl idiom for encapsulation and memory efficiency.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default values
     */
    Config();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up configuration resources
     */
    virtual ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Config objects cannot be copied to prevent slicing and ensure singleton-like behavior
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Config objects cannot be assigned to prevent resource management issues
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (deleted)
     * 
     * Config objects cannot be moved
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (deleted)
     * 
     * Config objects cannot be assigned
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Attempts to load configuration from a file, supporting JSON, YAML, and simple
     * key=value formats. The format is automatically detected based on file extension
     * and content.
     * 
     * @param filepath Path to the configuration file to load
     * @return true if configuration was successfully loaded, false otherwise
     * 
     * @note File format detection:
     *       - .json/.JSON files are parsed as JSON
     *       - .yaml/.yml files are parsed as YAML
     *       - Other files are parsed as simple key=value pairs
     * 
     * @see loadFromJSON()
     * @see loadFromYAML()
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments to extract configuration values in the format
     * --key=value. Supports standard command line argument processing.
     * 
     * @param argc Argument count from main()
     * @param argv Argument vector from main()
     * @return true if configuration was successfully loaded, false otherwise
     * 
     * @note This will process --config arguments. For other arguments, use
     *       loadFromArgsWithoutConfig()
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Load configuration from command line arguments without --config processing
     * 
     * Parses command line arguments to extract configuration values, but does not
     * process --config arguments (use for post-configuration overrides).
     * 
     * @param argc Argument count from main()
     * @param argv Argument vector from main()
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromArgsWithoutConfig(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves the current configuration to a file in simple key=value format with comments.
     * Only configuration values with File source are saved.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if configuration was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration summary
     * 
     * Generates a human-readable summary of all current configuration values
     * with their sources.
     * 
     * @return String containing the configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value for a given key, with automatic type conversion.
     * Returns std::nullopt if the key doesn't exist or the type doesn't match.
     * 
     * @tparam T Type of configuration value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Configuration value if found and type matches, std::nullopt otherwise
     * 
     * @note Supported types: int, int64_t, double, bool, std::string, std::vector<int>, 
     *       std::vector<double>, std::vector<std::string>
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value for a given key, returning a default value
     * if the key doesn't exist or the type doesn't match.
     * 
     * @tparam T Type of configuration value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist or type doesn't match
     * @return T Configuration value if found and type matches, defaultValue otherwise
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with the specified source. If the key already exists,
     * its value and source are updated.
     * 
     * @param key Configuration key
     * @param value Configuration value (any ConfigValue type)
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    
    /**
     * @brief Set a configuration value (string overload)
     * 
     * @param key Configuration key
     * @param value Configuration string value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source);
    
    /**
     * @brief Set a configuration value (integer overload)
     * 
     * @param key Configuration key
     * @param value Configuration integer value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source);
    
    /**
     * @brief Set a configuration value (double overload)
     * 
     * @param key Configuration key
     * @param value Configuration double value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source);
    
    /**
     * @brief Set a configuration value (boolean overload)
     * 
     * @param key Configuration key
     * @param value Configuration boolean value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source);
    
    /**
     * @brief Remove a configuration entry
     * 
     * @param key Configuration key to remove
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
     * @brief Internal method: Load configuration from JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromJSON(const std::string& filepath);
    
    /**
     * @brief Internal method: Load configuration from YAML file
     * 
     * @param filepath Path to the YAML configuration file
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromYAML(const std::string& filepath);
    
    /**
     * @brief Internal helper: Trim whitespace from string
     * 
     * @param str Input string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert string to lowercase
     * 
     * @param str Input string to convert
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * PIMPL idiom implementation details for the Config class
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Pointer to internal implementation
};

// Explicit template instantiations for common types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
