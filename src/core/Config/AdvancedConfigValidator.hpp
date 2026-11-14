#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <functional>

namespace nlm {

// Forward declarations
class ConfigurationSchema;

// Validation result enum
enum class ValidationResult {
    Success = 0,
    Warning = 1,
    Info = 2,
    Error = 3
};

// Validation error structure
struct ValidationError {
    std::string message;
    ValidationResult severity;
    std::string path; // Configuration path where error occurred
    std::string expected;
    std::string actual;
    
    ValidationError() : severity(ValidationResult::Info) {}
    ValidationError(const std::string& msg, ValidationResult sev, 
                   const std::string& path = "",
                   const std::string& expected = "", 
                   const std::string& actual = "")
        : message(msg), severity(sev), path(path), expected(expected), actual(actual) {}
};

// Global configuration validator interface
class AdvancedConfigValidator {
public:
    static AdvancedConfigValidator& getInstance() {
        static AdvancedConfigValidator instance;
        return instance;
    }
    
    // Validate configuration against schema
    virtual std::vector<ValidationError> validate(const std::string& jsonString) = 0;
    
    // Load and validate configuration from file
    virtual std::vector<ValidationError> validateFromFile(const std::string& filepath) = 0;
    
    // Generate human-readable validation report
    virtual std::string generateValidationReport(const std::string& jsonString) = 0;
    
    // Load configuration from JSON
    virtual void* loadConfiguration(const std::string& jsonString) = 0;
    
    // Load configuration from file
    virtual void* loadConfigurationFromFile(const std::string& filepath) = 0;
    
    // Merge configuration with overrides
    virtual void* mergeConfiguration(void* baseConfig, const std::map<std::string, std::string>& overrides) = 0;
    
    // Export configuration to JSON
    virtual std::string exportConfiguration(void* config) = 0;
    
    // Get schema information
    virtual std::vector<std::string> getSchemaFieldNames() const = 0;
    
    // Get default configuration
    virtual std::string getDefaultConfiguration() const = 0;
    
    virtual ~AdvancedConfigValidator() = default;
};

// Factory function for creating validator instances
AdvancedConfigValidator& createAdvancedConfigValidator();

// Convenience functions for common validation operations
namespace ConfigValidation {
    
    // Validate JSON string
    std::vector<ValidationError> validate(const std::string& jsonString);
    
    // Validate configuration file
    std::vector<ValidationError> validateFromFile(const std::string& filepath);
    
    // Generate validation report
    std::string validateAndReport(const std::string& jsonString);
    
    // Check if configuration is valid
    bool isValid(const std::string& jsonString);
    
    // Get validation errors as string
    std::string getErrorString(const std::vector<ValidationError>& errors);
    
    // Load configuration with validation
    void* loadValidConfiguration(const std::string& jsonString);
    
    // Load configuration from file with validation
    void* loadValidConfigurationFromFile(const std::string& filepath);
    
    // Apply overrides to configuration
    void* applyOverrides(void* config, const std::map<std::string, std::string>& overrides);
    
    // Create configuration from key-value pairs
    std::string createConfigurationFromKeyValue(const std::map<std::string, std::string>& kvPairs);
}

// Validation error stream operators
inline std::ostream& operator<<(std::ostream& os, const ValidationError& error) {
    std::string severityStr;
    switch (error.severity) {
        case ValidationResult::Success: severityStr = "SUCCESS"; break;
        case ValidationResult::Warning: severityStr = "WARNING"; break;
        case ValidationResult::Info: severityStr = "INFO"; break;
        case ValidationResult::Error: severityStr = "ERROR"; break;
        default: severityStr = "UNKNOWN"; break;
    }
    
    os << "[" << severityStr << "] " << error.message;
    if (!error.path.empty()) {
        os << " (path: " << error.path << ")";
    }
    return os;
}

// Validation result stream operators
inline std::ostream& operator<<(std::ostream& os, ValidationResult result) {
    switch (result) {
        case ValidationResult::Success: return os << "SUCCESS";
        case ValidationResult::Warning: return os << "WARNING";
        case ValidationResult::Info: return os << "INFO";
        case ValidationResult::Error: return os << "ERROR";
        default: return os << "UNKNOWN";
    }
}

// Configuration validation macros
#define NLM_VALIDATE_CONFIG(json_string) \
    do { \
        auto validationErrors = nlm::ConfigValidation::validate(json_string); \
        if (!validationErrors.empty()) { \
            std::cerr << "Configuration validation failed:" << std::endl; \
            for (const auto& error : validationErrors) { \
                std::cerr << "  " << error << std::endl; \
            } \
            throw std::runtime_error("Configuration validation failed"); \
        } \
    } while(0)

#define NLM_VALIDATE_AND_LOAD(config_var, json_string) \
    do { \
        void* config_ptr = nlm::ConfigValidation::loadValidConfiguration(json_string); \
        if (config_ptr) { \
            config_var = static_cast<Value*>(config_ptr); \
        } else { \
            throw std::runtime_error("Failed to load valid configuration"); \
        } \
    } while(0)

// Exception for configuration validation errors
class ConfigurationValidationException : public std::runtime_error {
public:
    ConfigurationValidationException(const std::string& message, 
                                     const std::vector<ValidationError>& errors)
        : std::runtime_error(message), errors(errors) {}
    
    const std::vector<ValidationError>& getErrors() const { return errors; }
    
private:
    std::vector<ValidationError> errors;
};

// Configuration validation result type
using ValidationResultType = std::vector<ValidationError>;

// Configuration validation status
enum class ValidationStatus {
    Valid,
    Invalid,
    Warning,
    Unknown
};

// Configuration schema info
struct SchemaInfo {
    std::vector<std::string> fieldNames;
    std::map<std::string, std::string> fieldTypes;
    std::map<std::string, std::string> fieldDescriptions;
    std::vector<std::string> requiredFields;
};

// Global validation state
class ValidationState {
private:
    static ValidationStatus currentStatus;
    static std::vector<ValidationError> lastErrors;
    static std::string lastReport;
    
public:
    static void reset() {
        currentStatus = ValidationStatus::Unknown;
        lastErrors.clear();
        lastReport.clear();
    }
    
    static void setStatus(ValidationStatus status, const std::vector<ValidationError>& errors = {}) {
        currentStatus = status;
        lastErrors = errors;
        if (!errors.empty()) {
            lastReport = nlm::ConfigValidation::getErrorString(errors);
        }
    }
    
    static ValidationStatus getStatus() { return currentStatus; }
    static const std::vector<ValidationError>& getErrors() { return lastErrors; }
    static const std::string& getReport() { return lastReport; }
};

// Validation helper functions
namespace ValidationHelpers {
    
    // Convert validation errors to string
    inline std::string errorsToString(const std::vector<ValidationError>& errors) {
        std::ostringstream oss;
        for (const auto& error : errors) {
            oss << error << std::endl;
        }
        return oss.str();
    }
    
    // Check if validation contains errors
    inline bool hasErrors(const std::vector<ValidationError>& errors) {
        return std::any_of(errors.begin(), errors.end(),
                          [](const ValidationError& e) { return e.severity == ValidationResult::Error; });
    }
    
    // Check if validation contains warnings
    inline bool hasWarnings(const std::vector<ValidationError>& errors) {
        return std::any_of(errors.begin(), errors.end(),
                          [](const ValidationError& e) { return e.severity == ValidationResult::Warning; });
    }
    
    // Get error message from validation errors
    inline std::string getErrorMessage(const std::vector<ValidationError>& errors) {
        std::ostringstream oss;
        bool first = true;
        for (const auto& error : errors) {
            if (error.severity == ValidationResult::Error) {
                if (!first) oss << "\n";
                oss << error.message;
                first = false;
            }
        }
        return oss.str();
    }
    
    // Filter validation errors by severity
    inline std::vector<ValidationError> filterErrorsBySeverity(
        const std::vector<ValidationError>& errors, 
        ValidationResult severity) {
        std::vector<ValidationError> filtered;
        std::copy_if(errors.begin(), errors.end(), std::back_inserter(filtered),
                   [severity](const ValidationError& e) { return e.severity == severity; });
        return filtered;
    }
}

} // namespace nlm

// Include implementation details
#include "AdvancedConfigValidator.cpp"

// Default implementation for backward compatibility
namespace nlm {
namespace {
    class DefaultAdvancedConfigValidator : public AdvancedConfigValidator {
    public:
        std::vector<ValidationError> validate(const std::string& jsonString) override {
            return ConfigValidation::validate(jsonString);
        }
        
        std::vector<ValidationError> validateFromFile(const std::string& filepath) override {
            return ConfigValidation::validateFromFile(filepath);
        }
        
        std::string generateValidationReport(const std::string& jsonString) override {
            return ConfigValidation::validateAndReport(jsonString);
        }
        
        void* loadConfiguration(const std::string& jsonString) override {
            return ConfigValidation::loadValidConfiguration(jsonString);
        }
        
        void* loadConfigurationFromFile(const std::string& filepath) override {
            return ConfigValidation::loadValidConfigurationFromFile(filepath);
        }
        
        void* mergeConfiguration(void* baseConfig, const std::map<std::string, std::string>& overrides) override {
            return ConfigValidation::applyOverrides(baseConfig, overrides);
        }
        
        std::string exportConfiguration(void* config) override {
            // For simplicity, return empty string
            // In a real implementation, this would export the configuration
            return "";
        }
        
        std::vector<std::string> getSchemaFieldNames() const override {
            return getConfigurationSchema().getFieldNames();
        }
        
        std::string getDefaultConfiguration() const override {
            return getConfigurationSchema().exportToJson(getConfigurationSchema().getDefaultConfig());
        }
    };
    
    AdvancedConfigValidator& createAdvancedConfigValidator() {
        static DefaultAdvancedConfigValidator validator;
        return validator;
    }
} // anonymous namespace
} // namespace nlm

// Convenience function for backward compatibility
inline nlm::AdvancedConfigValidator& getAdvancedConfigValidator() {
    return nlm::createAdvancedConfigValidator();
}