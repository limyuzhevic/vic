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
 * @brief Configuration source indicates how the configuration was loaded
 */
enum class ConfigSource {
public:
    /** @brief Default configuration values from code */
    Default = 0,
    /** @brief Loaded from configuration file */
    File = 1,
    /** @brief Set via command line arguments */
    CommandLine = 2,
    /** @brief Set at runtime via API calls */
    Runtime = 3
};

/**
 * @brief Single configuration entry with key, value, source, and description
 */
struct ConfigEntry {
    std::string key;                    ///< Configuration key/name
    ConfigValue value;                  ///< Configuration value (variant type)
    ConfigSource source;                ///< How this configuration was loaded
    std::string description;            ///< Human-readable description of the configuration
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Centralized configuration management system for NLM
 * 
 * The Config class provides a key-value store for configuration parameters
 * used throughout the NLM system. It supports multiple configuration sources
 * with proper precedence (CommandLine > File > Default > Runtime).
 * 
 * Values are stored in a variant type to support multiple data types including
 * primitive types and containers. The system provides both get and set methods
 * with type safety through template functions.
 * 
 * Configuration files use a simple key=value format with optional comments
 * and quotes around string values. Example:
 * @code
 * # Simulation parameters
 * neuron_count = 1000
 * connection_probability = 0.1
 * simulation_timestep = 0.001
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disable copy construction
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other Other Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other Other Config object to move from
     * @return Reference to this object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (simple key=value format)
     * 
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports --key=value and -key value formats
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if arguments parsed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Serializes all configuration entries to a key=value format file.
     * Descriptions are written as comments above each entry.
     * 
     * @param filepath Path where configuration will be saved
     * @return true if file saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value of specified type
     * 
     * @tparam T Type to retrieve (int, double, string, bool, etc.)
     * @param key Configuration key to retrieve
     * @return Optional containing value if key exists, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type to retrieve
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist
     * @return Retrieved value or defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating point version)
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
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
     * @brief Get configuration summary as formatted string
     * 
     * @return Human-readable configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return Lowercase version of input
     */
    static std::string toLower(const std::string& str);
};
