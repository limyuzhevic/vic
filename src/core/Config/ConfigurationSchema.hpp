// Configuration Schema Definition
#ifndef CONFIG_SCHEMA_HPP
#define CONFIG_SCHEMA_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <regex>

namespace nlm {

// Forward declaration
class ValidationError;

// Schema validation error types
enum class ValidationErrorType {
    TypeMismatch,
    ValueOutOfRange,
    RequiredParameterMissing,
    PatternMismatch,
    FilePathInvalid,
    CrossParameterInvalid,
    DeprecatedParameter,
    InvalidFormat
};

// Schema validation error with line number and context
class ValidationError {
public:
    ValidationError(ValidationErrorType type, const std::string& message, 
                   size_t lineNumber = 0, const std::string& context = "")
        : type(type), message(message), lineNumber(lineNumber), context(context) {}
    
    ValidationErrorType getType() const { return type; }
    const std::string& getMessage() const { return message; }
    size_t getLineNumber() const { return lineNumber; }
    const std::string& getContext() const { return context; }
    
    std::string toString() const {
        std::ostringstream oss;
        if (lineNumber > 0) {
            oss << "Line " << lineNumber << ": ";
        }
        oss << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }

private:
    ValidationErrorType type;
    std::string message;
    size_t lineNumber;
    std::string context;
};

// Parameter type definitions
enum class ParameterType {
    Integer,
    Float,
    Boolean,
    String,
    IntArray,
    FloatArray,
    StringArray,
    NestedObject
};

// Validation rule base class
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual bool validate(const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key) const = 0;
    virtual ParameterType getType() const = 0;
};

// Type validation rule
class TypeValidationRule : public ValidationRule {
public:
    TypeValidationRule(ParameterType type) : expectedType(type) {}
    
    bool validate(const ConfigValue& value) const override {
        switch (expectedType) {
            case ParameterType::Integer: return std::holds_alternative<int>(value);
            case ParameterType::Float: return std::holds_alternative<double>(value) || std::holds_alternative<int>(value);
            case ParameterType::Boolean: return std::holds_alternative<bool>(value);
            case ParameterType::String: return std::holds_alternative<std::string>(value);
            case ParameterType::IntArray: return std::holds_alternative<std::vector<int>>(value);
            case ParameterType::FloatArray: return std::holds_alternative<std::vector<double>>(value);
            case ParameterType::StringArray: return std::holds_alternative<std::vector<std::string>>(value);
            case ParameterType::NestedObject: return false; // Nested objects are handled differently
            default: return false;
        }
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': expected type " + parameterTypeToString(expectedType) +
               ", but got different type";
    }
    
    ParameterType getType() const override { return expectedType; }
    
    static std::string parameterTypeToString(ParameterType type) {
        switch (type) {
            case ParameterType::Integer: return "int";
            case ParameterType::Float: return "float/double";
            case ParameterType::Boolean: return "bool";
            case ParameterType::String: return "string";
            case ParameterType::IntArray: return "int[]";
            case ParameterType::FloatArray: return "float[]";
            case ParameterType::StringArray: return "string[]";
            case ParameterType::NestedObject: return "object";
            default: return "unknown";
        }
    }
};

// Range validation rule
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(double min, double max) : min(min), max(max) {}
    
    bool validate(const ConfigValue& value) const override {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        double val = std::holds_alternative<double>(value) ? 
                     std::get<double>(value) : static_cast<double>(std::get<int>(value));
        return val >= min && val <= max;
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': value must be between " + std::to_string(min) + 
               " and " + std::to_string(max);
    }
    
    ParameterType getType() const override { return ParameterType::Float; }

private:
    double min;
    double max;
};

// Pattern validation rule for strings
class PatternValidationRule : public ValidationRule {
public:
    PatternValidationRule(const std::string& pattern) : pattern(pattern) {
        compiledRegex = std::regex(pattern);
    }
    
    bool validate(const ConfigValue& value) const override {
        if (!std::holds_alternative<std::string>(value)) {
            return false;
        }
        return std::regex_match(std::get<std::string>(value), compiledRegex);
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': value must match pattern: " + pattern;
    }
    
    ParameterType getType() const override { return ParameterType::String; }

private:
    std::string pattern;
    std::regex compiledRegex;
};

// File path validation rule
class FilePathValidationRule : public ValidationRule {
public:
    FilePathValidationRule(bool mustExist = true) : mustExist(mustExist) {}
    
    bool validate(const ConfigValue& value) const override {
        if (!std::holds_alternative<std::string>(value)) {
            return false;
        }
        // Basic path validation - check for null bytes and invalid characters
        const std::string& path = std::get<std::string>(value);
        if (path.empty() || path.find('\0') != std::string::npos) {
            return false;
        }
        
        if (mustExist) {
            // In a real implementation, we would check if the file exists
            // For now, we'll just do basic validation
            return true;
        }
        
        return true;
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': file path is invalid";
    }
    
    ParameterType getType() const override { return ParameterType::String; }

private:
    bool mustExist;
};

// Array size validation rule
class ArraySizeValidationRule : public ValidationRule {
public:
    ArraySizeValidationRule(size_t minSize, size_t maxSize = SIZE_MAX) 
        : minSize(minSize), maxSize(maxSize) {}
    
    bool validate(const ConfigValue& value) const override {
        if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& arr = std::get<std::vector<int>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& arr = std::get<std::vector<double>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& arr = std::get<std::vector<std::string>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        }
        return false;
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': array size must be between " + 
               std::to_string(minSize) + " and " + std::to_string(maxSize);
    }
    
    ParameterType getType() const override { return ParameterType::IntArray; }

private:
    size_t minSize;
    size_t maxSize;
};

// Cross-parameter validation rule
class CrossParameterValidationRule : public ValidationRule {
public:
    CrossParameterValidationRule(const std::string& dependentKey, 
                                const std::function<bool(const ConfigValue&, const ConfigValue&)>& validator)
        : dependentKey(dependentKey), validator(validator) {}
    
    bool validate(const ConfigValue& /*value*/) const override {
        // Cross-parameter validation requires access to multiple values,
        // so this is handled separately in the SchemaValidator
        return true;
    }
    
    std::string getErrorMessage(const std::string& key) const override {
        return "Parameter '" + key + "': cross-parameter validation with '" + dependentKey + "' failed";
    }
    
    ParameterType getType() const override { return ParameterType::String; }

private:
    std::string dependentKey;
    std::function<bool(const ConfigValue&, const ConfigValue&)> validator;
};

// Parameter definition in schema
struct ParameterDefinition {
    std::string name;
    ParameterType type;
    std::optional<ConfigValue> defaultValue;
    std::vector<std::shared_ptr<ValidationRule>> validationRules;
    std::string description;
    bool required;
    bool deprecated;
    std::string deprecationMessage;
    
    ParameterDefinition(const std::string& n, ParameterType t, const std::string& desc = "")
        : name(n), type(t), validationRules(), description(desc), required(false), 
          deprecated(false), deprecationMessage("Parameter is deprecated") {}
    
    ParameterDefinition(const std::string& n, ParameterType t, const ConfigValue& defaultVal, 
                       const std::string& desc = "")
        : name(n), type(t), defaultValue(defaultVal), validationRules(), description(desc), 
          required(true), deprecated(false), deprecationMessage("Parameter is deprecated") {}
};

// Schema definition for configuration
class ConfigurationSchema {
public:
    ConfigurationSchema() = default;
    
    // Add a parameter to the schema
    void addParameter(const ParameterDefinition& param) {
        parameters[param.name] = param;
    }
    
    // Check if a parameter exists in the schema
    bool hasParameter(const std::string& name) const {
        return parameters.find(name) == parameters.end();
    }
    
    // Get parameter definition
    std::optional<ParameterDefinition> getParameter(const std::string& name) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameters.size());
        for (const auto& pair : parameters) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get all parameter definitions
    const std::unordered_map<std::string, ParameterDefinition>& getParameters() const {
        return parameters;
    }
    
    // Validate a parameter value against its schema definition
    std::vector<ValidationError> validateParameter(const std::string& key, 
                                                  const ConfigValue& value,
                                                  size_t lineNumber = 0,
                                                  const std::string& context = "") const {
        std::vector<ValidationError> errors;
        
        // Check if parameter exists
        if (!hasParameter(key)) {
            errors.emplace_back(ValidationErrorType::InvalidFormat,
                              "Unknown parameter: '" + key + "'",
                              lineNumber, context);
            return errors;
        }
        
        const auto& param = parameters.at(key);
        
        // Check deprecation
        if (param.deprecated) {
            errors.emplace_back(ValidationErrorType::DeprecatedParameter,
                              param.deprecationMessage + " ('" + key + "')",
                              lineNumber, context);
        }
        
        // Type validation
        auto typeRule = std::make_shared<TypeValidationRule>(param.type);
        if (!typeRule->validate(value)) {
            errors.emplace_back(ValidationErrorType::TypeMismatch,
                              typeRule->getErrorMessage(key),
                              lineNumber, context);
        }
        
        // Additional validation rules
        for (const auto& rule : param.validationRules) {
            if (!rule->validate(value)) {
                ValidationErrorType errorType = ValidationErrorType::InvalidFormat;
                if (dynamic_cast<TypeValidationRule*>(rule.get())) {
                    errorType = ValidationErrorType::TypeMismatch;
                } else if (dynamic_cast<RangeValidationRule*>(rule.get())) {
                    errorType = ValidationErrorType::ValueOutOfRange;
                } else if (dynamic_cast<PatternValidationRule*>(rule.get())) {
                    errorType = ValidationErrorType::PatternMismatch;
                }
                
                std::string errorMsg = rule->getErrorMessage(key);
                if (!errorMsg.empty()) {
                    errors.emplace_back(errorType, errorMsg, lineNumber, context);
                }
            }
        }
        
        return errors;
    }
    
    // Get default value for a parameter
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second.defaultValue;
        }
        return std::nullopt;
    }
    
    // Check if parameter is required
    bool isParameterRequired(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second.required;
        }
        return false;
    }

private:
    std::unordered_map<std::string, ParameterDefinition> parameters;
};

} // namespace nlm

#endif // CONFIG_SCHEMA_HPP
