#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "../ErrorHandling/NLMExceptionHierarchy.hpp"
#include "../ErrorHandling/EnhancedErrorHandlingIntegration.hpp"

namespace nlm {

// Forward declarations
class Config;

    // Configuration source enumeration
    enum class ConfigSource {
        Default,    // Default hardcoded values (lowest priority)
        Runtime,    // Programmatically set values
        File,       // Loaded from file
        CommandLine // Command line arguments (highest priority)
    };
    
    // Configuration entry structure
    struct ConfigEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
        std::string description;
        
        ConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source)
            : key(key), value(value), source(source), description("") {}
        
        ConfigEntry(const std::string& key, const ConfigValue& value, ConfigSource source, 
                   const std::string& description)
            : key(key), value(value), source(source), description(description) {}
    };

// =============================================================================
// Config: Configuration Management System
// =============================================================================
// 
// This class provides a flexible, type-safe configuration management system
// for the NLM brain simulation. Configuration values are strongly typed and
// can be loaded from multiple sources including files, command line arguments,
// and runtime modifications.
// 
// Key Features:
// - Support for multiple value types (int, float, string, bool, vectors)
// - Multiple configuration sources with priority order
// - JSON file support for configuration persistence
// - Command line argument integration
// - Runtime configuration modification
// - Type-safe value access with conversion
// - Configuration validation and error handling
// - Thread-safe operations
// 
// Configuration sources (priority order):
// 1. Command Line (highest priority)
// 2. File (from loadFromFile())
// 3. Runtime (programmatically set)
// 4. Default (hardcoded fallback values)
// 
// Usage Example:
// ```
// #include "Config.hpp"
// 
// auto config = std::make_shared<Config>();
// config->set("brain.neuron_count", 1000);
// config->set("plasticity.stdp.enable", true);
// config->loadFromFile("config.json");
// 
// int neuronCount = config->getOr("brain.neuron_count", 1000);
// bool stdpEnabled = config->get<bool>("plasticity.stdp.enable").value_or(true);
// ```
// =============================================================================

class Config {
public:
    // =======================================================================
    // Constructor and Destructor
    // =======================================================================
    
    // Create empty configuration
    // 
    // Initializes an empty configuration with no default values.
    // Default values must be set explicitly using set() or via loadFromFile().
    Config();
    
    // Destructor
    ~Config();
    
    // =======================================================================
    // Copy and Move Semantics
    // =======================================================================
    
    // Disable copying to prevent multiple ownership
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable move operations for efficient resource management
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // =======================================================================
    // Configuration Loading
    // =======================================================================
    
    // Load configuration from file
    // 
    // Loads configuration values from a JSON file. File values have
    // lower priority than command line arguments but higher than defaults.
    // 
    // Parameters:
    //   filepath: Path to JSON configuration file
    // 
    // Returns:
    //   true if file was successfully loaded, false otherwise
    //   (failure is not fatal - defaults will be used)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // 
    // Parses command line arguments in the format:
    //   --key=value
    //   --key value
    //   --flag (boolean flags default to true)
    // 
    // Parameters:
    //   argc: Argument count (typically from main())
    //   argv: Argument vector (typically from main())
    // 
    // Returns:
    //   true if arguments were parsed, false on error
    bool loadFromArgs(int argc, char** argv);
    
    // =======================================================================
    // Configuration Saving
    // =======================================================================
    
    // Save configuration to file
    // 
    // Serializes current configuration values to a JSON file. Only
    // non-default values are saved to keep files compact.
    // 
    // Parameters:
    //   filepath: Path where to save configuration file
    // 
    // Returns:
    //   true if save succeeded, false on error
    bool saveToFile(const std::string& filepath) const;
    
    // =======================================================================
    // Configuration Access
    // =======================================================================
    
    // Get configuration value with type checking
    // 
    // This method provides type-safe access to configuration values.
    // If the value exists but has a different type, returns nullopt.
    // 
    // Template Parameters:
    //   T: Type to convert value to (int, double, string, bool, vector<int>, vector<double>, vector<string>)
    // 
    // Parameters:
    //   key: Configuration key (dot notation supported, e.g., "brain.neuron_count")
    // 
    // Returns:
    //   Optional containing value if key exists and type matches, nullopt otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default
    // 
    // This method provides type-safe access to configuration values
    // with a fallback default if the key doesn't exist or has wrong type.
    // 
    // Template Parameters:
    //   T: Type to convert value to (int, double, string, bool, vector<int>, vector<double>, vector<string>)
    // 
    // Parameters:
    //   key: Configuration key
    //   defaultValue: Default value to return if key doesn't exist or type mismatch
    // 
    // Returns:
    //   Value if key exists and type matches, defaultValue otherwise
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // =======================================================================
    // Configuration Modification
    // =======================================================================
    
    // Set configuration value
    // 
    // Sets a configuration value with specified source. Different sources
    // have different priorities for override behavior.
    // 
    // Template Parameters:
    //   T: Value type (must match one of ConfigValue supported types)
    // 
    // Parameters:
    //   key: Configuration key (dot notation supported)
    //   value: Value to set
    //   source: Source of this configuration (Runtime, File, CommandLine, Default)
    // 
    // Notes:
    //   - Setting from CommandLine or File makes the value override defaults
    //   - Setting from Runtime or Default replaces existing values
    //   - Type must match existing value if key already exists
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Specialized overloads for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // =======================================================================
    // Configuration Management
    // =======================================================================
    
    // Check if configuration key exists
    // 
    // Parameters:
    //   key: Configuration key to check
    // 
    // Returns:
    //   true if key exists (regardless of value type), false otherwise
    bool has(const std::string& key) const;
    
    // Remove configuration key
    // 
    // Parameters:
    //   key: Configuration key to remove
    // 
    // Notes:
    //   - Key is removed regardless of source priority
    //   - Default values remain unchanged
    void remove(const std::string& key);
    
    // Get all configuration keys
    // 
    // Returns:
    //   Vector of all configuration keys (including defaults)
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration values
    // 
    // Removes all runtime and file configuration values, leaving
    // only default values.
    void clear();
    
    // Get configuration summary
    // 
    // Returns:
    //   Human-readable string summarizing all configuration values
    std::string summary() const;
    
protected:
    // Internal implementation details (pImpl pattern)
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal constructor for implementation
    explicit Config(Impl* impl);
    
    // =======================================================================
    // Internal Helpers
    // =======================================================================
    
    // Trim whitespace from string
    // 
    // Parameters:
    //   str: Input string
    // 
    // Returns:
    //   String with leading and trailing whitespace removed
    static std::string trim(const std::string& str);
    
    // Convert string to lowercase
    // 
    // Parameters:
    //   str: Input string
    // 
    // Returns:
    //   Lowercase version of input string
    static std::string toLower(const std::string& str);
    
private:
    // Additional private implementation details
    class Logger* getLogger() const;
};

} // namespace nlm
