// Configuration schema and validation for NLM Phase 6
// This provides validation and documentation for all configuration parameters

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <optional>

namespace nlm {

// Configuration parameter validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult(bool v = true, const std::string& error = "", 
                     const std::vector<std::string>& warnings = {})
        : valid(v), errorMessage(error), warnings(warnings) {}
};

// Configuration parameter definition
struct ConfigParameter {
    std::string key;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::string unit;
    std::vector<std::string> validValues;
    double minValue;
    double maxValue;
    bool required;
    std::string category;
    
    ConfigParameter() : description(""), type("string"), defaultValue(""), unit(""), 
                         minValue(std::numeric_limits<double>::lowest()), 
                         maxValue(std::numeric_limits<double>::max()), 
                         required(false), category("General") {}
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Add parameter definition
    void addParameter(const ConfigParameter& param);
    
    // Validate a single configuration value
    ValidationResult validate(const std::string& key, const std::string& value) const;
    
    // Validate all current configuration values
    ValidationResult validateAll(const class Config& config) const;
    
    // Get all parameter keys
    std::vector<std::string> getAllKeys() const;
    
    // Get parameter by key
    std::optional<ConfigParameter> getParameter(const std::string& key) const;
    
    // Get parameters by category
    std::vector<ConfigParameter> getParametersByCategory(const std::string& category) const;
    
    // Generate configuration documentation
    std::string generateDocumentation() const;
    
    // Load standard parameters
    void loadStandardParameters();
    
    // Validate consistency between parameters
    ValidationResult validateConsistency(const class Config& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration help and usage
class ConfigHelp {
public:
    ConfigHelp();
    ~ConfigHelp();
    
    // Generate help text for a specific key
    std::string getHelp(const std::string& key) const;
    
    // Generate full help text
    std::string getFullHelp() const;
    
    // Generate completion suggestions
    std::vector<std::string> getCompletions(const std::string& partialKey) const;
    
    // Get category list
    std::vector<std::string> getCategories() const;
    
    // Find keys by partial match
    std::vector<std::string> findKeys(const std::string& pattern) const;
    
    // Load standard help text
    void loadStandardHelp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration error classes
class ConfigError : public std::runtime_error {
public:
    enum class ErrorCode {
        InvalidValue,
        MissingRequired,
        ValidationError,
        ConsistencyError,
        ParseError
    };
    
    ConfigError(ErrorCode code, const std::string& message);
    ConfigError(ErrorCode code, const std::string& key, const std::string& value);
    
    ErrorCode getErrorCode() const { return errorCode_; }
    const std::string& getKey() const { return key_; }
    const std::string& getValue() const { return value_; }
    
private:
    ErrorCode errorCode_;
    std::string key_;
    std::string value_;
};

// Configuration utils
namespace ConfigUtils {
    // Parse value to appropriate type based on parameter type
    template<typename T>
    std::optional<T> parseValue(const std::string& str, const std::string& type);
    
    // Format value as string for parameter type
    template<typename T>
    std::string formatValue(const T& value, const std::string& type);
    
    // Check if value matches type constraints
    bool matchesType(const std::string& value, const std::string& type);
    
    // Convert string to appropriate type
    std::optional<double> stringToDouble(const std::string& str);
    std::optional<int> stringToInt(const std::string& str);
    std::optional<bool> stringToBool(const std::string& str);
    
    // Get type name from value
    std::string detectType(const std::string& value);
    
    // Trim whitespace
    std::string trim(const std::string& str);
    
    // Convert to lower case
    std::string toLower(const std::string& str);
    
    // Check if string contains only digits
    bool isInteger(const std::string& str);
    
    // Check if string represents a float
    bool isFloat(const std::string& str);
    
    // Check if string is boolean
    bool isBool(const std::string& str);
}

} // namespace nlm