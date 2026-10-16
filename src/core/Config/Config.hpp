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

// Configuration validation types
struct ConfigValidationRule {
    std::string key;
    std::string type; // "int", "int64_t", "double", "bool", "string"
    std::string validation; // "positive", "non-negative", "range:min-max", "enum:value1,value2"
    std::string description;
    
    ConfigValidationRule(const std::string& k, const std::string& t, 
                        const std::string& v = "", const std::string& d = "")
        : key(k), type(t), validation(v), description(d) {}
};

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    ConfigSource source;
    
    ConfigValidationError(const std::string& k, const std::string& msg, ConfigSource src)
        : key(k), message(msg), source(src) {}
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
    
    // Configuration validation
    void validate() const;
    void addValidationRule(const ConfigValidationRule& rule);
    const std::vector<ConfigValidationRule>& getValidationRules() const;
    
    // Set validation enabled/disabled
    void setValidationEnabled(bool enabled) { validationEnabled = enabled; }
    bool isValidationEnabled() const { return validationEnabled; }
    
    // Get validation errors
    const std::vector<ConfigValidationError>& getValidationErrors() const { return validationErrors; }
    
    // Clear validation errors
    void clearValidationErrors() { validationErrors.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Configuration validation helpers
    bool validateValue(const ConfigEntry& entry) const;
    bool validateType(const std::string& key, const ConfigValue& value) const;
    bool validateRange(const std::string& key, const ConfigValue& value) const;
    
    // Validation system
    std::vector<ConfigValidationRule> validationRules;
    std::vector<ConfigValidationError> validationErrors;
    bool validationEnabled{true};
    
    // Predefined validation rules
    void initializeValidationRules();
};

} // namespace nlm

} // namespace nlm
