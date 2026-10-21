// Enhanced Config.hpp with validation and better defaults
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

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

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    ConfigSource source;
    
    ConfigValidationError(const std::string& k, const std::string& msg, ConfigSource s)
        : key(k), message(msg), source(s) {}
};

// Type constraint for configuration values
enum class ConfigType {
    INT,
    INT64,
    DOUBLE,
    BOOL,
    STRING,
    INT_VECTOR,
    DOUBLE_VECTOR,
    STRING_VECTOR,
    NONE
};

// Parameter constraints
struct ConfigConstraint {
    std::string key;
    ConfigType type;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<std::string> description;
    bool required;
    
    ConfigConstraint(const std::string& k, ConfigType t) 
        : key(k), type(t), required(false) {}
    
    ConfigConstraint(const std::string& k, ConfigType t, double min, double max) 
        : key(k), type(t), minValue(min), maxValue(max), required(false) {}
    
    bool validate(const ConfigValue& value, std::vector<ConfigValidationError>& errors) const;
};

// Main configuration class with validation
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
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    bool validateAll() const;
    std::vector<ConfigValidationError> getValidationErrors() const;
    void clearValidationErrors();
    
    // Configuration schema management
    void registerParameter(const ConfigConstraint& constraint);
    bool isParameterRegistered(const std::string& key) const;
    
    // Default values management
    void setDefault(const std::string& key, const ConfigValue& value);
    bool hasDefault(const std::string& key) const;
    
    // Configuration history for debugging
    void logChanges() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    bool validateValue(const ConfigConstraint& constraint, const ConfigValue& value, ConfigSource source) const;
    
    // Validation errors
    mutable std::vector<ConfigValidationError> validationErrors;
};

} // namespace nlm
