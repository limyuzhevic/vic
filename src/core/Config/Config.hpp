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

// Main configuration class
// 
// Centralized configuration management for the NLM brain.
// The Config class provides:
// - Loading/saving configuration from/to files (JSON format)
// - Command-line argument parsing
// - Type-safe value retrieval and setting
// - Configuration validation
// - Support for multiple configuration sources with precedence
// - Configuration validation and schema checking
//
// Configuration values are stored in a type-safe variant system that
// supports basic types (int, double, bool, string) and containers
// (vectors of basic types). This allows for both simple parameters
// and complex configuration structures.
//
// Configuration sources have precedence (higher priority overwrites lower):
// 1. Runtime (set via code)
// 2. Command-line arguments
// 3. File configuration
// 4. Default configuration
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    // 
    // Loads configuration from a JSON file.
    // 
    // Args:
    //   filepath: Path to the JSON configuration file
    // 
    // Returns:
    //   true if loading succeeds, false if file not found or invalid
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    // 
    // Parses command line arguments in the format:
    //   --key=value
    //   --key value
    // Supports both short (--key) and long (--key-name) options.
    // 
    // Args:
    //   argc: Argument count
    //   argv: Argument array
    // 
    // Returns:
    //   true if parsing succeeds, false on error
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    // 
    // Saves configuration to a JSON file.
    // 
    // Args:
    //   filepath: Path to the output file
    // 
    // Returns:
    //   true if saving succeeds, false on error
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    // 
    // Retrieves a configuration value with type safety.
    // Returns nullopt if the key doesn't exist.
    // 
    // Args:
    //   key: Configuration key to retrieve
    // 
    // Template Args:
    //   T: Type to retrieve (must be one of the supported ConfigValue types)
    // 
    // Returns:
    //   Optional containing the value if found, nullopt otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get value with default
    // 
    // Retrieves a configuration value or returns a default if not found.
    // 
    // Args:
    //   key: Configuration key to retrieve
    //   defaultValue: Default value to return if key not found
    // 
    // Template Args:
    //   T: Type to retrieve (must be one of the supported ConfigValue types)
    // 
    // Returns:
    //   Value from configuration or defaultValue if not found
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    // 
    // Sets a configuration value. Supports type-safe setting of all
    // supported types.
    // 
    // Args:
    //   key: Configuration key to set
    //   value: Value to set
    //   source: Source of this configuration value (defaults to Runtime)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    // 
    // Checks if a configuration key exists.
    // 
    // Args:
    //   key: Configuration key to check
    // 
    // Returns:
    //   true if key exists, false otherwise
    bool has(const std::string& key) const;
    
    // Remove key
    // 
    // Removes a configuration key.
    // 
    // Args:
    //   key: Configuration key to remove
    void remove(const std::string& key);
    
    // Get all keys
    // 
    // Gets all configuration keys.
    // 
    // Returns:
    //   Vector of all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    // 
    // Clears all configuration entries, leaving only defaults.
    void clear();
    
    // Get configuration summary
    // 
    // Returns a formatted summary of all configuration entries.
    // 
    // Returns:
    //   Formatted string containing configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
