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
    Config();
    ~Config();
    
    Config();
    ~Config();
    
    /**
     * @brief Constructs an empty configuration object.
     * 
     * Creates a new configuration with no entries. All configuration values must be
     * set explicitly before use.
     */
    
    /**
     * @brief Destroys the configuration object.
     * 
     * Cleans up the internal implementation and releases any resources.
     */
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor.
     * 
     * Transfers ownership of the internal implementation from another configuration object.
     * The source object is left in a valid but unspecified state.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator.
     * 
     * Transfers ownership of the internal implementation from another configuration object.
     * The source object is left in a valid but unspecified state.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from a file.
     * 
     * Parses a configuration file in the simple key=value format (one entry per line).
     * Lines starting with # or / are treated as comments and ignored.
     * Values can be quoted with either " or '. Values are stored as strings initially
     * and can be retrieved as their appropriate types.
     * 
     * @param filepath Path to the configuration file to load.
     * @return true if the file was successfully loaded and parsed, false otherwise.
     * 
     * @note Currently uses a simple file parser. Future versions will support JSON/YAML.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command line arguments.
     * 
     * Parses command line arguments in --key=value format. Also supports -key value
     * format for short options. Arguments are stored with CommandLine source.
     * 
     * @param argc Number of command line arguments.
     * @param argv Array of command line argument strings.
     * @return true if the arguments were successfully parsed, false otherwise.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves configuration to a file.
     * 
     * Writes all configuration entries to a file in the simple key=value format.
     * Each entry is preceded by its description comment. Values are written in a
     * format that preserves their original type (strings are quoted, vectors use
     * square bracket notation).
     * 
     * @param filepath Path to the output file.
     * @return true if the file was successfully written, false otherwise.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Retrieves a configuration value with type safety.
     * 
     * Retrieves a configuration value by key and converts it to the requested type.
     * If the key doesn't exist or the value type doesn't match, returns std::nullopt.
     * 
     * @tparam T Type to convert the value to (int, int64_t, double, bool, std::string,
     *         or vector types).
     * @param key Configuration key to retrieve.
     * @return std::optional<T> Value if found and type matches, std::nullopt otherwise.
     * 
     * @note Type conversion is exact - no automatic type coercion occurs.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Retrieves a configuration value with a default fallback.
     * 
     * Retrieves a configuration value by key and converts it to the requested type.
     * If the key doesn't exist or the value type doesn't match, returns the provided
     * default value instead.
     * 
     * @tparam T Type to convert the value to.
     * @param key Configuration key to retrieve.
     * @param defaultValue Default value to return if key doesn't exist or type doesn't match.
     * @return T Value if found and type matches, defaultValue otherwise.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Type-safe getter for boolean values.
     * 
     * Retrieves a boolean configuration value by key. Returns true if successful,
     * and stores the value in the output parameter. Returns false if the key doesn't
     * exist or the value is not a boolean.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved boolean value.
     * @return true if the value was successfully retrieved, false otherwise.
     */
    bool getBool(const std::string& key, bool& value) const;
    
    /**
     * @brief Type-safe getter for integer values.
     * 
     * Retrieves an integer configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not an integer.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved integer value.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    int getInt(const std::string& key, int& value) const;
    
    /**
     * @brief Type-safe getter for 64-bit integer values.
     * 
     * Retrieves a 64-bit integer configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not a 64-bit integer.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved 64-bit integer value.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    int64_t getInt64(const std::string& key, int64_t& value) const;
    
    /**
     * @brief Type-safe getter for double values.
     * 
     * Retrieves a double configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not a double.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved double value.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    double getDouble(const std::string& key, double& value) const;
    
    /**
     * @brief Type-safe getter for string values.
     * 
     * Retrieves a string configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not a string.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved string value.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    std::string getString(const std::string& key, std::string& value) const;
    
    /**
     * @brief Type-safe getter for integer vector values.
     * 
     * Retrieves an integer vector configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not an integer vector.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved integer vector.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    std::vector<int> getIntVector(const std::string& key, std::vector<int>& value) const;
    
    /**
     * @brief Type-safe getter for double vector values.
     * 
     * Retrieves a double vector configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not a double vector.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved double vector.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    std::vector<double> getDoubleVector(const std::string& key, std::vector<double>& value) const;
    
    /**
     * @brief Type-safe getter for string vector values.
     * 
     * Retrieves a string vector configuration value by key. Returns 1 if successful,
     * and stores the value in the output parameter. Returns 0 if the key doesn't
     * exist or the value is not a string vector.
     * 
     * @param key Configuration key to retrieve.
     * @param value Output parameter to store the retrieved string vector.
     * @return 1 if the value was successfully retrieved, 0 otherwise.
     */
    std::vector<std::string> getStringVector(const std::string& key, std::vector<std::string>& value) const;
    
    /**
     * @brief Type-safe setter for boolean values.
     * 
     * Sets a boolean configuration value by key. The value can be set with a specific
     * source (default: Runtime). Type-safe setters ensure that the value is stored
     * as the correct type in the internal configuration system.
     * 
     * @param key Configuration key to set.
     * @param value Boolean value to set.
     * @param source Source of this configuration value (Default, File, CommandLine, or Runtime).
     */
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for integer values.
     * 
     * Sets an integer configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value Integer value to set.
     * @param source Source of this configuration value.
     */
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for 64-bit integer values.
     * 
     * Sets a 64-bit integer configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value 64-bit integer value to set.
     * @param source Source of this configuration value.
     */
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for double values.
     * 
     * Sets a double configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value Double value to set.
     * @param source Source of this configuration value.
     */
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for string values.
     * 
     * Sets a string configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value String value to set.
     * @param source Source of this configuration value.
     */
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for integer vector values.
     * 
     * Sets an integer vector configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value Integer vector value to set.
     * @param source Source of this configuration value.
     */
    void setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for double vector values.
     * 
     * Sets a double vector configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value Double vector value to set.
     * @param source Source of this configuration value.
     */
    void setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Type-safe setter for string vector values.
     * 
     * Sets a string vector configuration value by key. The value can be set with a specific
     * source. Type-safe setters ensure that the value is stored as the correct type.
     * 
     * @param key Configuration key to set.
     * @param value String vector value to set.
     * @param source Source of this configuration value.
     */
    void setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a configuration value with the generic ConfigValue type.
     * 
     * Sets a configuration value using the generic ConfigValue variant type. This allows
     * setting any supported type (int, double, string, bool, or vector types). The value
     * is stored with the specified source.
     * 
     * @param key Configuration key to set.
     * @param value Generic configuration value.
     * @param source Source of this configuration value.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a string configuration value.
     * 
     * Convenience method for setting string values. Equivalent to using the generic
     * set() method with a string value.
     * 
     * @param key Configuration key to set.
     * @param value String value to set.
     * @param source Source of this configuration value.
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets an integer configuration value.
     * 
     * Convenience method for setting integer values. Equivalent to using the generic
     * set() method with an integer value.
     * 
     * @param key Configuration key to set.
     * @param value Integer value to set.
     * @param source Source of this configuration value.
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a double configuration value.
     * 
     * Convenience method for setting double values. Equivalent to using the generic
     * set() method with a double value.
     * 
     * @param key Configuration key to set.
     * @param value Double value to set.
     * @param source Source of this configuration value.
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a boolean configuration value.
     * 
     * Convenience method for setting boolean values. Equivalent to using the generic
     * set() method with a boolean value.
     * 
     * @param key Configuration key to set.
     * @param value Boolean value to set.
     * @param source Source of this configuration value.
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if a configuration key exists.
     * 
     * Checks whether a configuration entry with the specified key exists in the
     * configuration. This is useful for checking if a configuration value has been
     * set before attempting to retrieve it.
     * 
     * @param key Configuration key to check for existence.
     * @return true if the key exists, false otherwise.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key.
     * 
     * Removes the configuration entry with the specified key from the configuration.
     * If the key doesn't exist, this operation has no effect.
     * 
     * @param key Configuration key to remove.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Gets all configuration keys.
     * 
     * Retrieves a list of all configuration keys currently stored in the configuration.
     * The returned vector contains keys in the order they were added (insertion order).
     * 
     * @return Vector of all configuration keys.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Gets configuration keys related to brain parameters.
     * 
     * Retrieves configuration keys that are related to brain parameters (keys starting
     * with "brain."). This is useful for organizing configuration by category.
     * 
     * @return Vector of brain-related configuration keys.
     */
    std::vector<std::string> getBrainKeys() const;
    
    /**
     * @brief Gets configuration keys related to plasticity parameters.
     * 
     * Retrieves configuration keys that are related to plasticity parameters (keys starting
     * with "plasticity."). This helps organize configuration by category and facilitates
     * bulk configuration operations.
     * 
     * @return Vector of plasticity-related configuration keys.
     */
    std::vector<std::string> getPlasticityKeys() const;
    
    /**
     * @brief Gets configuration keys related to neuromodulation parameters.
     * 
     * Retrieves configuration keys that are related to neuromodulation parameters (keys starting
     * with "neuromod."). This helps organize configuration by category and facilitates
     * bulk configuration operations.
     * 
     * @return Vector of neuromodulation-related configuration keys.
     */
    std::vector<std::string> getNeuromodulationKeys() const;
    
    /**
     * @brief Gets configuration keys related to development parameters.
     * 
     * Retrieves configuration keys that are related to development parameters (keys starting
     * with "development."). This helps organize configuration by category and facilitates
     * bulk configuration operations.
     * 
     * @return Vector of development-related configuration keys.
     */
    std::vector<std::string> getDevelopmentKeys() const;
    
    /**
     * @brief Gets configuration keys related to environment parameters.
     * 
     * Retrieves configuration keys that are related to environment parameters (keys starting
     * with "environment."). This helps organize configuration by category and facilitates
     * bulk configuration operations.
     * 
     * @return Vector of environment-related configuration keys.
     */
    std::vector<std::string> getEnvironmentKeys() const;
    
    /**
     * @brief Gets configuration keys related to logging parameters.
     * 
     * Retrieves configuration keys that are related to logging parameters (keys starting
     * with "log."). This helps organize configuration by category and facilitates
     * bulk configuration operations.
     * 
     * @return Vector of logging-related configuration keys.
     */
    std::vector<std::string> getLoggingKeys() const;
    
    /**
     * @brief Gets configuration description for brain parameters.
     * 
     * Generates a human-readable description of all brain-related configuration values.
     * The description includes key names, values, and source information for each brain
     * configuration parameter.
     * 
     * @return String containing brain configuration description.
     */
    std::string getBrainDescription() const;
    
    /**
     * @brief Gets configuration description for plasticity parameters.
     * 
     * Generates a human-readable description of all plasticity-related configuration values.
     * The description includes key names, values, and source information for each plasticity
     * configuration parameter.
     * 
     * @return String containing plasticity configuration description.
     */
    std::string getPlasticityDescription() const;
    
    /**
     * @brief Gets configuration description for neuromodulation parameters.
     * 
     * Generates a human-readable description of all neuromodulation-related configuration values.
     * The description includes key names, values, and source information for each neuromodulation
     * configuration parameter.
     * 
     * @return String containing neuromodulation configuration description.
     */
    std::string getNeuromodulationDescription() const;
    
    /**
     * @brief Gets configuration description for development parameters.
     * 
     * Generates a human-readable description of all development-related configuration values.
     * The description includes key names, values, and source information for each development
     * configuration parameter.
     * 
     * @return String containing development configuration description.
     */
    std::string getDevelopmentDescription() const;
    
    /**
     * @brief Gets configuration description for environment parameters.
     * 
     * Generates a human-readable description of all environment-related configuration values.
     * The description includes key names, values, and source information for each environment
     * configuration parameter.
     * 
     * @return String containing environment configuration description.
     */
    std::string getEnvironmentDescription() const;
    
    /**
     * @brief Gets configuration description for logging parameters.
     * 
     * Generates a human-readable description of all logging-related configuration values.
     * The description includes key names, values, and source information for each logging
     * configuration parameter.
     * 
     * @return String containing logging configuration description.
     */
    std::string getLoggingDescription() const;
    
    /**
     * @brief Applies default brain configuration.
     * 
     * Sets standard default values for all brain-related configuration parameters.
     * This includes neuron count, region count, connection probability, plasticity settings,
     * and neuron dynamics parameters. This provides a good starting point for brain
     * simulations and can be modified as needed.
     */
    void setBrainDefaults();
    
    /**
     * @brief Applies default development configuration.
     * 
     * Sets standard default values for all development-related configuration parameters.
     * This includes synaptogenesis and pruning rates for different developmental stages,
     * providing a biologically plausible development trajectory.
     */
    void setDevelopmentDefaults();
    
    /**
     * @brief Applies default neuromodulation configuration.
     * 
     * Sets standard default values for all neuromodulation-related configuration parameters.
     * This includes dopamine baselines, curiosity scales, and novelty detection parameters
     * that govern neuromodulator effects on brain dynamics.
     */
    void setNeuromodulationDefaults();
    
    /**
     * @brief Applies default environment configuration.
     * 
     * Sets standard default values for all environment-related configuration parameters.
     * This includes world dimensions, sensory processing settings, and environmental
     * characteristics that define the agent's interaction space.
     */
    void setEnvironmentDefaults();
    
    /**
     * @brief Applies default logging configuration.
     * 
     * Sets standard default values for all logging-related configuration parameters.
     * This includes log levels, file output settings, and logging frequency parameters
     * for monitoring and debugging simulations.
     */
    void setLoggingDefaults();
    
    /**
     * @brief Validates the entire configuration.
     * 
     * Performs comprehensive validation of all configuration values against predefined
     * rules and constraints. Returns true if all configuration values are valid, false
     * otherwise. This is useful for ensuring configuration integrity before running
     * simulations.
     * 
     * @return true if configuration is valid, false otherwise.
     */
    bool validate() const;
    
    /**
     * @brief Validates a specific configuration value.
     * 
     * Checks whether a configuration value matches an expected value. Returns true if the
     * key exists and the value matches exactly, false otherwise.
     * 
     * @param key Configuration key to validate.
     * @param expectedValue Expected configuration value.
     * @return true if validation passes, false otherwise.
     */
    bool validate(const std::string& key, const ConfigValue& expectedValue) const;
    
    /**
     * @brief Adds a validation rule to the configuration.
     * 
     * Adds a validation rule that will be checked when validate() is called. Validation rules
     * consist of a key, a validator function, and an error message. The validator function
     * receives the configuration value and returns true if valid, false otherwise.
     * 
     * @param rule Validation rule to add.
     */
    void addValidationRule(const ValidationRule& rule);
    
    /**
     * @brief Compares this configuration with another configuration.
     * 
     * Compares this configuration with another configuration for equality. Returns true
     * if both configurations have the same entries (same keys with same values and sources),
     * false otherwise.
     * 
     * @param other Configuration to compare with.
     * @return true if configurations are equal, false otherwise.
     */
    bool equals(const Config& other) const;
    
    /**
     * @brief Merges another configuration into this one.
     * 
     * Merges another configuration into this one, combining their entries. If override is
     * true, entries from the other configuration will overwrite existing entries in this
     * configuration. If override is false, existing entries will be preserved and only
     * new entries will be added.
     * 
     * @param other Configuration to merge into this one.
     * @param override If true, override existing entries; if false, preserve existing entries.
     */
    void merge(const Config& other, bool override = true);
    
    /**
     * @brief Clears all configuration entries.
     * 
     * Removes all configuration entries from this configuration, leaving it in an empty
     * state. This is useful for resetting the configuration before a new simulation.
     */
    void clear();
    
    /**
     * @brief Gets a human-readable summary of the configuration.
     * 
     * Generates a detailed summary of all configuration entries, including key names,
     * values, and source information. The summary is formatted for human reading and
     * includes statistics about the configuration (number of entries, distribution by source,
     * etc.).
     * 
     * @return String containing configuration summary.
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
