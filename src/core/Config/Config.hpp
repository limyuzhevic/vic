#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;
class Logger;

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

// Validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, const ConfigValue& value)
        : std::runtime_error(message), key_(key), value_(value) {}
    
    const std::string& getKey() const { return key_; }
    const ConfigValue& getValue() const { return value_; }
    
private:
    std::string key_;
    ConfigValue value_;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationFunction> validators;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    bool validate() const {
        for (const auto& validator : validators) {
            if (!validator(value)) {
                return false;
            }
        }
        return true;
    }
};

// Main configuration class
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
    
    // Configuration validation
    bool addValidation(const std::string& key, const ValidationFunction& validator);
    bool removeValidation(const std::string& key);
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Parameter range validation
    bool addRangeValidation(const std::string& key, double min, double max, const std::string& description = "");
    bool addIntegerRangeValidation(const std::string& key, int64_t min, int64_t max, const std::string& description = "");
    bool addPositiveValidation(const std::string& key, const std::string& description = "");
    bool addNonNegativeValidation(const std::string& key, const std::string& description = "");
    
    // Neural operation validation
    bool addNeuronCountValidation(size_t maxNeurons = 1000000);
    bool addConnectionProbabilityValidation(double min = 0.0, double max = 1.0);
    
    // Memory validation
    bool addMemoryCapacityValidation(size_t maxWorkingMemory = 10000);
    bool addTimeStepValidation(double min = 0.0, double max = 1.0);
    
    // Integration validation
    bool addRegionValidation(size_t maxRegions = 100);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all validation keys
    std::vector<std::string> getValidationKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation summary
    std::string validationSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static ConfigValue stringToConfigValue(const std::string& str);
};

} // namespace nlm

} // namespace nlm
