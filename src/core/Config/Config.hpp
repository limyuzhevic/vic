#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>

namespace nlm {

// Configuration file formats
enum class ConfigFileFormat {
    /** JSON format - standard JavaScript Object Notation */
    KEY_VALUE,
    /** JSON format - standard JavaScript Object Notation */
    JSON,
    /** YAML format - human-readable data serialization */
    YAML
};

/**
 * @struct ValidationResult
 * @brief Result of configuration validation operations
 * 
 * Contains the outcome of validation checks, indicating whether a configuration
 * value meets specified requirements and providing detailed error messages.
 * 
 * @note Used by validators to report validation failures with specific error messages
 * @see Config::setValidator()
 * @see Config::setValidated()
 * @ingroup core
 */
struct ValidationResult {
    bool isValid;
    std::string message;
    
    /**
     * @brief Default constructor creates valid result with empty message
     * 
     * @param valid Initial validity state (default: true)
     * @param msg Descriptive message (default: "")
     */
    ValidationResult(bool valid = true, const std::string& msg = "") 
        : isValid(valid), message(msg) {}
};

/**
 * @brief Type aliases for all supported configuration value types
 * 
 * The configuration system supports multiple data types for flexibility:
 * - Integer types (int, int64_t)
 * - Floating-point types (double)
 * - Boolean type
 * - String types (std::string)
 * - Collection types (vectors of primitives)
 * 
 * @note Variant type allows dynamic storage while maintaining type safety
 * @see Config::get()
 * @see Config::set()
 * @ingroup core
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

// Configuration source
enum class ConfigSource {
    /** Default configuration value provided by code */
    Default,
    /** Configuration loaded from external file (JSON/YAML/key=value) */
    File,
    /** Configuration from command-line arguments */
    CommandLine,
    /** Configuration set at runtime via API */
    Runtime
};

/**
 * @struct ConfigEntry
 * @brief Represents a single configuration key-value pair
 * 
 * Contains all metadata about a configuration entry including its value,
 * source, and descriptive information. Used for introspection and documentation.
 * 
 * @note Each configuration entry tracks its origin to support validation and
 * locking mechanisms
 * @see ConfigValue
 * @see ConfigSource
 * @see Config::getSectionEntries()
 * @ingroup core
 */
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    /**
     * @brief Default constructor creates empty entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct configuration entry with all metadata
     * 
     * @param k Configuration key (must be unique within section)
     * @param v Configuration value
     * @param s Source of configuration (affects validation/locking)
     * @param desc Human-readable description for documentation
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @class Config
 * @brief Complete configuration management system for NLM brain
 * 
 * The Config class provides comprehensive configuration management for the NLM
 * brain project, supporting multiple file formats, validation, locking, and
 * environment variable substitution. It serves as the central configuration
 * interface for all brain systems.
 * 
 * Core Features:
 * - Multi-format support: JSON, YAML, and key=value formats
 * - Type-safe value storage with automatic type conversion
 * - Source tracking for validation and persistence
 * - Section-based organization for large configurations
 * - Validation framework for runtime configuration checks
 * - Environment variable substitution for dynamic values
 * - Auto-save capabilities for persistent configuration
 * - Deep merge operations for configuration combination
 * 
 * Configuration Categories:
 * - brain: Neural system parameters (regions, neuron types, connectivity)
 * - memory: Memory system settings (working, episodic, associative)
 * - plasticity: Learning algorithm parameters (STDP, Hebbian, structural)
 * - neuromodulation: Neuromodulator behaviors and responses
 * - prediction: Prediction system configuration
 * - cognition: Cognitive mechanism settings
 * - development: Developmental stage parameters
 * - simulation: Simulation control (timestep, duration, etc.)
 * - logging: Logging configuration and levels
 * 
 * Usage Examples:
 * @code
 * // Create and configure brain section
 * config->set("brain.regions", 4);
 * config->set("brain.region.0.size", 100);
 * config->set("brain.region.0.neuronType", "Excitatory");
 * 
 * // Configure memory systems
 * config->set("memory.working.capacity", 1000);
 * config->set("memory.episodic.encodingRate", 0.5);
 * 
 * // Configure plasticity rules
 * config->set("plasticity.stdp.learningRate", 0.01);
 * config->set("plasticity.stdp.timeWindow", 20);
 * @endcode
 * 
 * Performance Characteristics:
 * - Fast lookups: O(1) average case for key-based access
 * - Memory efficient: Uses compact storage for configuration values
 * - Thread-safe: Supports concurrent read operations
 * - Serialization: Efficient save/load for persistence
 * 
 * @note Configuration is immutable after locking and serves as the primary
 * interface for brain system configuration
 * @see ConfigFileFormat
 * @see ConfigSource
 * @see ValidationResult
 * @see ConfigEntry
 * @ingroup core
 */

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/key=value format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath, ConfigFileFormat format);
    
    // Load from JSON/YAML strings
    bool loadFromJSON(const std::string& jsonStr);
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    bool saveToFile(const std::string& filepath, ConfigFileFormat format) const;
    
    // Save to string
    std::string saveToString(ConfigFileFormat format) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by section
    std::vector<std::string> getKeys(const std::string& section) const;
    
    // Get sections
    std::vector<std::string> getSections() const;
    
    // Check if key is locked
    bool isLocked(const std::string& key) const;
    
    // Set validation
    bool setValidator(const std::function<ValidationResult(const std::string&, const ConfigValue&)>& validator);
    
    // Set validated value
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source, 
                     const std::function<ValidationResult(const std::string&, const ConfigValue&)>& validator);
    
    // Merge with another config
    void merge(const Config& other);
    
    // Locking
    void lock(const std::string& key, bool locked);
    
    // Auto-save configuration
    void setAutoSave(bool enable, const std::string& format = "json");
    bool isAutoSaveEnabled() const;
    std::string getSaveFormat() const;
    
    // Section management
    bool hasSection(const std::string& section) const;
    ConfigValue getSectionValue(const std::string& section, const std::string& key) const;
    void removeSection(const std::string& section);
    std::vector<ConfigEntry> getSectionEntries(const std::string& section) const;
    std::string getSectionDescription(const std::string& section) const;
    void setSectionDescription(const std::string& section, const std::string& description);
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Environment variable substitution
    static std::string substituteVariables(const std::string& str);
    
    // Global logger access
    static Logger& getGlobalLogger();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Internal save/load methods
    void saveToJSONFile(std::ofstream& file) const;
    void saveToJSONStream(std::ostream& stream) const;
    void saveToYAMLFile(std::ofstream& file) const;
    void saveToYAMLStream(std::ostream& stream) const;
    void saveToKeyValueFile(std::ofstream& file) const;
    void saveToKeyValueStream(std::ostream& stream) const;
    bool loadFromJSONStream(std::istream& stream);
    bool loadFromYAMLStream(std::istream& stream);
    bool loadFromKeyValueStream(std::istream& stream);
};

} // namespace nlm