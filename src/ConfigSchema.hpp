#ifndef CONFIG_SCHEMA_HPP
#define CONFIG_SCHEMA_HPP

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

// Include Config.hpp to get ConfigSource enum
#include "core/Config/Config.hpp"

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, 
                         const std::string& value = "")
        : std::runtime_error(message), key_(key), value_(value) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getValue() const { return value_; }
    
private:
    std::string key_;
    std::string value_;
};

// Forward declaration of Config
class Config;

// Configuration parameter metadata
struct ConfigParameter {
    std::string name;
    std::string description;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    std::string typeName;
    bool required;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
    bool hasMinMax;
    
    ConfigParameter(const std::string& name, const std::string& description,
                   const std::variant<int, int64_t, double, bool, std::string>& defaultValue,
                   bool required = false)
        : name(name), description(description), defaultValue(defaultValue), 
          typeName(getTypeName(defaultValue)), required(required),
          allowedValues(), minValue(0.0), maxValue(0.0), hasMinMax(false) {}
    
    ConfigParameter(const std::string& name, const std::string& description,
                   double defaultValue, double minValue, double maxValue)
        : name(name), description(description), defaultValue(defaultValue),
          typeName("double"), required(false),
          allowedValues(), minValue(minValue), maxValue(maxValue), hasMinMax(true) {}
    
    ConfigParameter(const std::string& name, const std::string& description,
                   const std::string& defaultValue,
                   const std::vector<std::string>& allowedValues)
        : name(name), description(description), defaultValue(defaultValue),
          typeName("string"), required(false),
          allowedValues(allowedValues), minValue(0.0), maxValue(0.0), hasMinMax(false) {}
    
    std::string getTypeName(const std::variant<int, int64_t, double, bool, std::string>& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        return "unknown";
    }
    
    template<typename T>
    T getDefaultAs() const {
        return std::get<T>(defaultValue);
    }
    
    std::string getDefaultString() const {
        if (std::holds_alternative<int>(defaultValue)) {
            return std::to_string(std::get<int>(defaultValue));
        } else if (std::holds_alternative<int64_t>(defaultValue)) {
            return std::to_string(std::get<int64_t>(defaultValue));
        } else if (std::holds_alternative<double>(defaultValue)) {
            return std::to_string(std::get<double>(defaultValue));
        } else if (std::holds_alternative<bool>(defaultValue)) {
            return std::get<bool>(defaultValue) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(defaultValue)) {
            return std::get<std::string>(defaultValue);
        }
        return "";
    }
};

// Configuration schema class
class ConfigSchema {
private:
    static std::map<std::string, ConfigParameter> parameters_;
    static bool initialized_;
    
public:
    // Initialize schema with all parameters
    static void initializeSchema();
    
    // Get parameter metadata
    static ConfigParameter getParameter(const std::string& name);
    
    // Validate a configuration value
    static void validate(const std::string& key, const std::string& value,
                        ConfigSource source = ConfigSource::Runtime);
    
    // Get all parameter names
    static std::vector<std::string> getAllParameterNames();
    
    // Check if parameter exists
    static bool hasParameter(const std::string& name);
    
    // Get description for help
    static std::string getDescription(const std::string& name);
    
    // Get default value as string
    static std::string getDefaultString(const std::string& name);
    
    // Apply default values to a config object
    static void applyDefaults(Config& config);
    
    // Get all parameters for schema generation
    static const std::map<std::string, ConfigParameter>& getAllParameters() { return parameters_; }
};

} // namespace nlm

#endif // CONFIG_SCHEMA_HPP
