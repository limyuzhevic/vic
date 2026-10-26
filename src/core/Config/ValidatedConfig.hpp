// Enhanced configuration validation and validation helper functions

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Configuration validation utilities
class ConfigValidator {
public:
    static bool validateKey(const std::string& key);
    static bool validateNeuronCount(int64_t count);
    static bool validateRegionCount(int64_t count);
    static bool validateConnectionProbability(float prob);
    static bool validatePlasticityParams(float ltp, float ltd);
    static bool validateTimestep(double timestep);
    static bool validateMaxSimulationSteps(int64_t steps);
    static bool validateLogLevel(const std::string& level);
    
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
    // Get default value for a key
    static ConfigValue getDefaultValue(const std::string& key);
    
    // Get default description for a key
    static std::string getDefaultDescription(const std::string& key);
};

// Enhanced configuration class with validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format) with validation
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file with validation
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation info
    std::string summary() const;
    
    // Validate entire configuration
    bool validate() const;
    
    // Get all validation errors
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidLogLevel(const std::string& level);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool isSourceHigherPriority(ConfigSource source1, ConfigSource source2) const;
};

// Configuration file format support (enhanced)
class ConfigFileFormat {
public:
    // Parse configuration file with better error handling
    static bool parseFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Write configuration file with better formatting
    static bool writeFile(const std::string& filepath, const std::vector<ConfigEntry>& entries);
    
    // Validate configuration file format
    static bool validateFormat(const std::string& content);
    
    // Format configuration value for file output
    static std::string formatValue(const ConfigValue& value);
};

} // namespace nlm
