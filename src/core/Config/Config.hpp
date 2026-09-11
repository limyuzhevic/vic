#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <regex>
#include <stdexcept>

// nlohmann/json for serialization
#include <nlohmann/json.hpp>

// For YAML support
#ifdef YAML_AVAILABLE
#include <yaml-cpp/yaml.h>
#endif

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
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message)
        : std::runtime_error(message) {}
};

// Validation rules
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    // Validate a key-value pair
    virtual void validate(const std::string& key, const ConfigValue& value) = 0;
    
    // Returns the validator type name
    virtual const char* name() const = 0;
};

// Integer range validator
class IntegerRangeValidator : public ConfigValidator {
private:
    int64_t min;
    int64_t max;
    bool inclusive;
public:
    IntegerRangeValidator(int64_t min, int64_t max, bool inclusive = true)
        : min(min), max(max), inclusive(inclusive) {}
    
    void validate(const std::string& key, const ConfigValue& value) override {
        if (const int64_t* val = std::get_if<int64_t>(&value)) {
            if (inclusive) {
                if (*val < min || *val > max) {
                    throw ConfigValidationError(
                        "Key \"" + key + "\" has value " + std::to_string(*val) +
                        " which is outside the range [" + std::to_string(min) + ", " + std::to_string(max) + "]"
                    );
                }
            } else {
                if (*val <= min || *val >= max) {
                    throw ConfigValidationError(
                        "Key \"" + key + "\" has value " + std::to_string(*val) +
                        " which is outside the range (" + std::to_string(min) + ", " + std::to_string(max) + ")"
                    );
                }
            }
        }
    }
    
    const char* name() const override { return "IntegerRangeValidator"; }
};

// Double range validator
class DoubleRangeValidator : public ConfigValidator {
private:
    double min;
    double max;
    bool inclusive;
public:
    DoubleRangeValidator(double min, double max, bool inclusive = true)
        : min(min), max(max), inclusive(inclusive) {}
    
    void validate(const std::string& key, const ConfigValue& value) override {
        if (const double* val = std::get_if<double>(&value)) {
            if (inclusive) {
                if (*val < min || *val > max) {
                    throw ConfigValidationError(
                        "Key \"" + key + "\" has value " + std::to_string(*val) +
                        " which is outside the range [" + std::to_string(min) + ", " + std::to_string(max) + "]"
                    );
                }
            } else {
                if (*val <= min || *val >= max) {
                    throw ConfigValidationError(
                        "Key \"" + key + "\" has value " + std::to_string(*val) +
                        " which is outside the range (" + std::to_string(min) + ", " + std::to_string(max) + ")"
                    );
                }
            }
        }
    }
    
    const char* name() const override { return "DoubleRangeValidator"; }
};

// String pattern validator
class StringPatternValidator : public ConfigValidator {
private:
    std::regex pattern;
    std::string errorMessage;
public:
    StringPatternValidator(const std::string& pattern, const std::string& errorMessage = "")
        : pattern(pattern), errorMessage(errorMessage) {}
    
    void validate(const std::string& key, const ConfigValue& value) override {
        if (const std::string* val = std::get_if<std::string>(&value)) {
            if (!std::regex_match(*val, pattern)) {
                if (errorMessage.empty()) {
                    throw ConfigValidationError(
                        "Key \"" + key + "\" has value \"" + *val + "\" which does not match pattern"
                    );
                } else {
                    throw ConfigValidationError(
                        "Key \"" + key + "\": " + errorMessage
                    );
                }
            }
        }
    }
    
    const char* name() const override { return "StringPatternValidator"; }
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
    
    // Load from file (JSON or YAML format)
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
    void addValidator(const std::string& key, std::unique_ptr<ConfigValidator> validator);
    void removeValidator(const std::string& key);
    void clearValidators(const std::string& key);
    void clearAllValidators();
    
    // Get validators for a key
    const std::vector<std::pair<std::string, std::unique_ptr<ConfigValidator>>>& 
        getValidators(const std::string& key) const;
    
    // Validation for a key-value pair
    void validateKey(const std::string& key, const ConfigValue& value) const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper for YAML parsing
#ifdef YAML_AVAILABLE
    bool parseYAML(const std::string& content);
#endif
};

// Explicit template instantiations
// This needs to be outside the class to be properly instantiated
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
