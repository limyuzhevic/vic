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
    Default,    // Built-in default configuration values
    File,       // Loaded from configuration file
    CommandLine, // Parsed from command line arguments
    Runtime     // Set programmatically at runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;              // Configuration key (must be unique)
    ConfigValue value;            // Configuration value
    ConfigSource source;          // Source of this configuration
    std::string description;      // Human-readable description for documentation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Forward declaration for global configuration access
class Config;

} // namespace nlm

namespace nlm {

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving for performance and safety
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with support for nested structures)
    // @param filepath Path to configuration file
    // @return true if file loaded successfully, false if file not found or invalid JSON
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    // Parses standard NLM command line options and configuration keys
    // @param argc Argument count
    // @param argv Argument vector
    // @return true if parsing succeeds, false otherwise
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    // Writes current configuration to JSON file
    // @param filepath Path to output file
    // @return true if save succeeds, false otherwise
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    // Retrieves configuration value by key with automatic type conversion
    // Supports int, int64_t, double, bool, std::string, and vector types
    // @tparam T Target type
    // @param key Configuration key to retrieve
    // @return Optional containing value if key exists, empty otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values with default fallback
    // Retrieves configuration value or returns default if key doesn't exist
    // @tparam T Target type
    // @param key Configuration key to retrieve
    // @param defaultValue Value to return if key doesn't exist
    // @return Value if key exists, defaultValue otherwise
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety and source tracking
    // Supports all ConfigValue types and automatic source assignment
    // @param key Configuration key to set
    // @param value Value to set (any ConfigValue type)
    // @param source Source of configuration (Default, File, CommandLine, Runtime)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters for common types
    // @param key Configuration key to set
    // @param value String value
    // @param source Source of configuration
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // @param key Configuration key to set
    // @param value Integer value
    // @param source Source of configuration
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    // @param key Configuration key to set
    // @param value 64-bit integer value
    // @param source Source of configuration
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    // @param key Configuration key to set
    // @param value Floating-point value
    // @param source Source of configuration
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    // @param key Configuration key to set
    // @param value Boolean value
    // @param source Source of configuration
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if key exists in configuration
    // @param key Configuration key to check
    // @return true if key exists, false otherwise
    bool has(const std::string& key) const;
    
    // Remove key from configuration
    // @param key Configuration key to remove
    void remove(const std::string& key);
    
    // Get all configuration keys
    // @return Vector of all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    // Resets configuration to default state
    void clear();
    
    // Get configuration summary
    // Provides human-readable summary of current configuration
    // @return Summary string with key configuration values
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Global configuration access
// @return Reference to global configuration instance
Config& getGlobalConfig();

// Set global configuration
// @param config Configuration to set as global
void setGlobalConfig(std::shared_ptr<Config> config);

} // namespace nlm
