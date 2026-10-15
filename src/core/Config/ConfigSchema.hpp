#pragma once

#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <any>
#include <optional>
#include <limits>

namespace nlm {

// Type definitions for configuration parameters
enum class ConfigType {
    INT,
    INT64,
    DOUBLE,
    FLOAT,
    BOOL,
    STRING,
    VECTOR_INT,
    VECTOR_DOUBLE,
    VECTOR_STRING
};

// Validation function type
using ValidationFunc = std::function<bool(const std::any& value, std::string& error)>;

// Range definition for numeric types
struct NumberRange {
    double min = -std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::max();
    bool inclusive = true;
};

// String constraints
struct StringConstraints {
    size_t minLength = 0;
    size_t maxLength = std::numeric_limits<size_t>::max();
    std::string pattern; // regex pattern
    std::vector<std::string> allowedValues;
};

// Vector constraints
struct VectorConstraints {
    size_t minSize = 0;
    size_t maxSize = std::numeric_limits<size_t>::max();
};

// Configuration parameter definition
struct ConfigParameter {
    std::string name;
    ConfigType type;
    std::any defaultValue;
    std::string description;
    bool required = false;
    
    // Type-specific constraints
    NumberRange numberRange;
    StringConstraints stringConstraints;
    VectorConstraints vectorConstraints;
    
    // Custom validation function
    ValidationFunc customValidator;
    
    ConfigParameter() : type(ConfigType::STRING), required(false) {}
    
    ConfigParameter(const std::string& name, ConfigType type, 
                   const std::any& defaultValue, 
                   const std::string& description = "",
                   bool required = false)
        : name(name), type(type), defaultValue(defaultValue), 
          description(description), required(required) {}
};

// Configuration Schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a parameter to the schema
    void addParameter(const ConfigParameter& param);
    
    // Get a parameter by name
    const ConfigParameter* getParameter(const std::string& name) const;
    
    // Check if schema contains a parameter
    bool hasParameter(const std::string& name) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Validate a value against its type and constraints
    bool validate(const std::string& paramName, const std::any& value, 
                  std::string& error) const;
    
    // Convert string to appropriate type
    bool convertString(const std::string& paramName, const std::string& strValue,
                       std::any& outValue, std::string& error) const;
    
    // Get parameter description
    std::string getDescription(const std::string& name) const;
    
    // Check if parameter is required
    bool isRequired(const std::string& name) const;
    
    // Get default value
    std::any getDefaultValue(const std::string& name) const;
    
    // Get type name as string
    static std::string getTypeName(ConfigType type);
    
    // Get all parameter definitions
    const std::map<std::string, ConfigParameter>& getParameters() const { return parameters; }
    
private:
    std::map<std::string, ConfigParameter> parameters;
    
    // Type conversion helpers
    static bool convertToType(ConfigType type, const std::string& str, std::any& outValue);
    static bool validateAgainstRange(const std::any& value, const NumberRange& range, std::string& error);
    static bool validateString(const std::any& value, const StringConstraints& constraints, std::string& error);
    static bool validateVector(const std::any& value, const VectorConstraints& constraints, std::string& error);
};

// Configuration Exception hierarchy
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& paramName, const std::string& message)
        : ConfigException("Config validation error for '" + paramName + "': " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& message) : ConfigException("Config parse error: " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& message) : ConfigException("Config file error: " + message) {}
};

} // namespace nlm