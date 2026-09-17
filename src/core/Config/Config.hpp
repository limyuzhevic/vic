#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>
#include <any>
#include <optional>
#include <functional>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Forward declaration for nlohmann::json
namespace nlohmann {
    class json;
}

namespace nlm {

class Config;

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration value validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::any& value) const = 0;
    virtual std::string getErrorMessage() const = 0;
};

// Concrete validators
class FloatValidator : public ConfigValidator {
    float minValue;
    float maxValue;
    
public:
    FloatValidator(float min = -std::numeric_limits<float>::infinity(),
                   float max = std::numeric_limits<float>::infinity())
        : minValue(min), maxValue(max) {}
    
    bool validate(const std::any& value) const override {
        if (!value.has_value()) return false;
        float val = std::any_cast<float>(value);
        return val >= minValue && val <= maxValue;
    }
    
    std::string getErrorMessage() const override {
        return "Value must be between " + std::to_string(minValue) + 
               " and " + std::to_string(maxValue);
    }
};

class Int64Validator : public ConfigValidator {
    int64_t minValue;
    int64_t maxValue;
    
public:
    Int64Validator(int64_t min = -std::numeric_limits<int64_t>::infinity(),
                   int64_t max = std::numeric_limits<int64_t>::infinity())
        : minValue(min), maxValue(max) {}
    
    bool validate(const std::any& value) const override {
        if (!value.has_value()) return false;
        int64_t val = std::any_cast<int64_t>(value);
        return val >= minValue && val <= maxValue;
    }
    
    std::string getErrorMessage() const override {
        return "Value must be between " + std::to_string(minValue) + 
               " and " + std::to_string(maxValue);
    }
};

class DoubleValidator : public ConfigValidator {
    double minValue;
    double maxValue;
    
public:
    DoubleValidator(double min = -std::numeric_limits<double>::infinity(),
                   double max = std::numeric_limits<double>::infinity())
        : minValue(min), maxValue(max) {}
    
    bool validate(const std::any& value) const override {
        if (!value.has_value()) return false;
        double val = std::any_cast<double>(value);
        return val >= minValue && val <= maxValue;
    }
    
    std::string getErrorMessage() const override {
        return "Value must be between " + std::to_string(minValue) + 
               " and " + std::to_string(maxValue);
    }
};

class StringValidator : public ConfigValidator {
    std::string pattern;
    
public:
    StringValidator(const std::string& regexPattern = "") : pattern(regexPattern) {}
    
    bool validate(const std::any& value) const override {
        if (!value.has_value()) return false;
        std::string val = std::any_cast<std::string>(value);
        // Basic validation - pattern matching could be implemented
        if (!pattern.empty()) {
            // Simple pattern matching
            return !pattern.empty() && val.length() >= pattern.length() / 2;
        }
        return true;
    }
    
    std::string getErrorMessage() const override {
        return "Value does not match required pattern";
    }
};

class ConfigValue {
public:
    std::any value;
    ConfigSource source;
    std::vector<std::shared_ptr<ConfigValidator>> validators;
    
    ConfigValue(const std::any& val, ConfigSource src = ConfigSource::Default) 
        : value(val), source(src) {}
    
    bool isValid() const {
        for (const auto& validator : validators) {
            if (!validator->validate(value)) {
                return false;
            }
        }
        return true;
    }
    
    std::string getValidationError() const {
        for (const auto& validator : validators) {
            if (!validator->validate(value)) {
                return validator->getErrorMessage();
            }
        }
        return "";
    }
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration management
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Programmatic);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration validation
    bool validate(const std::string& key) const;
    std::string getValidationError(const std::string& key) const;
    
    // Configuration modification
    void remove(const std::string& key);
    void clear();
    bool exists(const std::string& key) const;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Command line argument parsing
    bool loadFromArgs(int argc, char** argv);
    void printHelp() const;
    
    // Configuration inspection
    std::vector<std::string> getAllKeys() const;
    ConfigSource getSource(const std::string& key) const;
    
    // JSON/YAML support (if libraries available)
    bool loadFromJson(const nlohmann::json& jsonData);
    nlohmann::json toJson() const;
    
    // Performance optimization
    void optimizeMemoryUsage();
    size_t getMemoryUsage() const;
    
    // Debugging and diagnostics
    void printConfig() const;
    void validateAll() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper methods
    static bool parseLine(const std::string& line, std::string& key, std::string& value);
    static std::any parseValue(const std::string& str);
    static std::string toString(const std::any& value);
};

} // namespace nlm