#pragma once

#include <string>
#include <unordered_map>
#include <optional>

// Forward declaration
namespace nlm {
    class Config;
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
}

namespace nlm {

// Configuration parameter types
enum class ConfigParameterType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    VectorInt,
    VectorDouble,
    VectorString
};

// Configuration parameter definition
struct ConfigParameterDefinition {
    std::string name;
    ConfigParameterType type;
    std::optional<std::string> defaultValue;
    std::string description;
    bool required;
    std::string validationRegex;
    std::string minValue;
    std::string maxValue;
    
    ConfigParameterDefinition() 
        : name(), type(ConfigParameterType::String), required(false), 
          validationRegex(), minValue(), maxValue() {}
    
    ConfigParameterDefinition(const std::string& n, ConfigParameterType t, 
                             const std::string& desc = "",
                             const std::optional<std::string>& def = std::nullopt,
                             bool req = false)
        : name(n), type(t), defaultValue(def), description(desc), required(req) {}
};

// Configuration error types
enum class ConfigErrorType {
    FileNotFound,
    ParseError,
    InvalidType,
    ValidationError,
    MissingRequiredParameter,
    DuplicateParameter,
    FileSaveError
};

class ConfigError {
public:
    ConfigError(ConfigErrorType type, const std::string& message, 
                const std::string& paramName = "", 
                const std::string& filePath = "")
        : type(type), message(message), parameterName(paramName), filePath(filePath) {}
    
    ConfigErrorType type;
    std::string message;
    std::string parameterName;
    std::string filePath;
    
    std::string what() const {
        std::string result = "Config Error: ";
        result += message;
        if (!parameterName.empty()) {
            result += " (parameter: " + parameterName + ")";
        }
        if (!filePath.empty()) {
            result += " (file: " + filePath + ")";
        }
        return result;
    }
};

// Configuration schema management
class ConfigSchema {
private:
    static ConfigSchema* instance;
    std::unordered_map<std::string, ConfigParameterDefinition> parameters;
    
    ConfigSchema() {}
    
public:
    static ConfigSchema& getInstance() {
        if (!instance) {
            instance = new ConfigSchema();
        }
        return *instance;
    }
    
    // Register a parameter with its definition
    void registerParameter(const ConfigParameterDefinition& param);
    
    // Get parameter definition by name
    ConfigParameterDefinition getParameter(const std::string& name) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
    
    // Get all registered parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Clear all parameters
    void clear();
    
    // Get the complete parameter map
    const std::unordered_map<std::string, ConfigParameterDefinition>& getAllParameters() const {
        return parameters;
    }
    
    // Validate parameter value against its definition
    bool validateParameter(const std::string& name, const std::string& value, 
                          ConfigParameterType expectedType) const;
    
    // Convert string value to appropriate type based on parameter definition
    bool convertValue(const std::string& valueStr, ConfigParameterType type,
                     ConfigValue& outValue) const;
    
    // Get default value for a parameter
    std::optional<ConfigValue> getDefaultValue(const std::string& name) const;
};

// Convenience function to get the schema instance
inline ConfigSchema& schema() {
    return ConfigSchema::getInstance();
}

} // namespace nlm
