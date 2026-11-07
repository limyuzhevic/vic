// Configuration management system for NLM
// Provides type-safe configuration with multiple sources (default, file, command-line, runtime)
// Supports various value types including integers, floats, booleans, strings, and arrays
// Designed for deterministic experiments with reproducible configuration states

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (JSON format)
    // Parses JSON configuration files and merges with existing settings.
    // Returns true if loading succeeds, false otherwise.
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Parses command-line arguments in the format "key=value" or "key value".
    // Returns true if parsing succeeds, false otherwise.
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Serializes current configuration to a JSON file for persistence
    // or distribution. Returns true if saving succeeds, false otherwise.
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined in Config.cpp for template instantiation)
    // Retrieves a configuration value with type checking. Returns optional<T>
    // which is empty if the key doesn't exist or the type is wrong.
template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get value with default (defined in Config.cpp for template instantiation)
    // Retrieves a configuration value, returning the default if not found.
    // Commonly used for optional configuration parameters with sensible defaults.
template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with various types
    // Allows setting configuration values with automatic type conversion
    // and source tracking for determining configuration precedence.
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and management
    // Check if a configuration key exists in the current configuration
    bool has(const std::string& key) const;
    
    // Remove a configuration key
    // Removes a configuration entry, restoring it to default state
    void remove(const std::string& key);
    
    // Get all configuration keys
    // Returns a vector of all currently configured keys for inspection
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration
    // Removes all configuration entries, leaving only defaults
    void clear();
    
    // Get configuration summary
    // Returns a human-readable summary of current configuration
    // including source information for each key-value pair
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers for configuration parsing and validation
    // String utilities for configuration key-value processing
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
