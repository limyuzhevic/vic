// Configuration Validator Implementation
#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "ConfigurationSchema.hpp"
#include "ValidationError.hpp"
#include <algorithm>
#include <unordered_set>

namespace nlm {

// Configuration validator that performs validation against a schema
class ConfigurationValidator {
public:
    ConfigurationValidator() = default;
    
    // Set the schema to use for validation
    void setSchema(std::shared_ptr<ConfigurationSchema> schema) {
        currentSchema = schema;
    }
    
    // Get the current schema
    std::shared_ptr<ConfigurationSchema> getSchema() const {
        return currentSchema;
    }
    
    // Validate a complete configuration
    std::vector<ValidationError> validate(const std::unordered_map<std::string, ConfigEntry>& config) {
        std::vector<ValidationError> allErrors;
        
        if (!currentSchema) {
            allErrors.emplace_back(ValidationErrorType::InvalidFormat,
                                 "No schema defined for validation");
            return allErrors;
        }
        
        // Track which parameters have been validated for cross-parameter checks
        std::unordered_set<std::string> validatedParams;
        
        for (const auto& [key, entry] : config) {
            // Validate this parameter against schema
            auto errors = currentSchema->validateParameter(key, entry.value, 0, "config entry");
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
            
            // Track validated parameter for cross-parameter checks
            validatedParams.insert(key);
        }
        
        // Cross-parameter validation
        // This would be implemented based on specific cross-parameter rules
        // For now, we'll just check some common cases
        
        return allErrors;
    }
    
    // Validate a single parameter value
    std::vector<ValidationError> validateParameter(const std::string& key, 
                                                  const ConfigValue& value,
                                                  size_t lineNumber = 0,
                                                  const std::string& context = "") {
        if (!currentSchema) {
            return {ValidationError(ValidationErrorType::InvalidFormat,
                                  "No schema defined for validation", lineNumber, context)};
        }
        
        return currentSchema->validateParameter(key, value, lineNumber, context);
    }
    
    // Validate that all required parameters are present
    std::vector<ValidationError> validateRequiredParameters(
        const std::unordered_map<std::string, ConfigEntry>& config) {
        
        std::vector<ValidationError> errors;
        
        if (!currentSchema) {
            errors.emplace_back(ValidationErrorType::InvalidFormat,
                             "No schema defined for required parameter validation");
            return errors;
        }
        
        // Check each parameter in the schema
        for (const auto& [key, param] : currentSchema->getParameters()) {
            if (param.required) {
                auto configIt = config.find(key);
                if (configIt == config.end()) {
                    errors.emplace_back(ValidationErrorType::RequiredParameterMissing,
                                     "Required parameter missing: '" + key + "'",
                                     0, "schema validation");
                } else {
                    // Parameter exists, validate its value
                    auto valueErrors = currentSchema->validateParameter(key, configIt->second.value, 0, "required parameter");
                    errors.insert(errors.end(), valueErrors.begin(), valueErrors.end());
                }
            }
        }
        
        return errors;
    }
    
    // Validate that no unknown parameters are present
    std::vector<ValidationError> validateNoUnknownParameters(
        const std::unordered_map<std::string, ConfigEntry>& config) {
        
        std::vector<ValidationError> errors;
        
        if (!currentSchema) {
            // If no schema, assume all parameters are unknown
            for (const auto& [key, entry] : config) {
                if (!currentSchema->hasParameter(key)) {
                    errors.emplace_back(ValidationErrorType::InvalidFormat,
                                     "Unknown parameter: '" + key + "'",
                                     0, "config validation");
                }
            }
            return errors;
        }
        
        // Check each parameter in config against schema
        for (const auto& [key, entry] : config) {
            if (!currentSchema->hasParameter(key)) {
                errors.emplace_back(ValidationErrorType::InvalidFormat,
                                 "Unknown parameter: '" + key + "'",
                                 0, "config validation");
            }
        }
        
        return errors;
    }
    
    // Complete validation including type, value, required, and unknown parameters
    std::vector<ValidationError> validateComplete(
        const std::unordered_map<std::string, ConfigEntry>& config,
        bool allowUnknownParameters = false) {
        
        std::vector<ValidationError> allErrors;
        
        // Type validation for all parameters
        for (const auto& [key, entry] : config) {
            auto errors = currentSchema->validateParameter(key, entry.value, 0, "config entry");
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        }
        
        // Required parameter validation
        auto requiredErrors = validateRequiredParameters(config);
        allErrors.insert(allErrors.end(), requiredErrors.begin(), requiredErrors.end());
        
        // Unknown parameter validation (unless explicitly allowed)
        if (!allowUnknownParameters) {
            auto unknownErrors = validateNoUnknownParameters(config);
            allErrors.insert(allErrors.end(), unknownErrors.begin(), unknownErrors.end());
        }
        
        return allErrors;
    }
    
    // Format configuration for error reporting (e.g., for file validation)
    std::string formatConfigForErrorReporting(const std::unordered_map<std::string, ConfigEntry>& config) {
        std::ostringstream oss;
        for (const auto& [key, entry] : config) {
            oss << key << " = " << configValueToString(entry.value) << "\n";
        }
        return oss.str();
    }
    
    // Convert ConfigValue to string for error reporting
    static std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << '"' << std::get<std::string>(value) << '"';
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& arr = std::get<std::vector<int>>(value);
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& arr = std::get<std::vector<double>>(value);
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& arr = std::get<std::vector<std::string>>(value);
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << '"' << arr[i] << '"';
            }
            oss << "]";
        }
        
        return oss.str();
    }
    
    // Get validation suggestions for a specific parameter
    std::string getParameterSuggestions(const std::string& key) {
        if (!currentSchema) {
            return "";
        }
        
        auto paramOpt = currentSchema->getParameter(key);
        if (!paramOpt) {
            return "Parameter not defined in schema";
        }
        
        const auto& param = *paramOpt;
        
        std::ostringstream oss;
        oss << "Parameter: " << param.name << "\n";
        oss << "Type: " << ConfigurationSchema::TypeValidationRule::parameterTypeToString(param.type) << "\n";
        oss << "Description: " << param.description << "\n";
        
        if (param.defaultValue.has_value()) {
            oss << "Default: " << configValueToString(param.defaultValue.value()) << "\n";
        }
        
        if (param.required) {
            oss << "Required: Yes\n";
        }
        
        if (param.deprecated) {
            oss << "Deprecated: Yes\n";
            oss << "Warning: " << param.deprecationMessage << "\n";
        }
        
        return oss.str();
    }
    
    // Check if a parameter value is valid (without collecting errors)
    bool isValid(const std::string& key, const ConfigValue& value) {
        auto errors = validateParameter(key, value);
        return errors.empty();
    }

private:
    std::shared_ptr<ConfigurationSchema> currentSchema;
};

} // namespace nlm

#endif // CONFIG_VALIDATOR_HPP
