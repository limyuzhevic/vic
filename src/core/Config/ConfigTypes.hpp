// Additional types needed for unified demo system
// Contains validation rules and configuration types

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <map>

using namespace std;

// Configuration validation rule
struct ValidationRule {
    string field;
    function<bool(const string&)> validator;
    string errorMessage;
    bool isRequired = false;
    string helpMessage;
};

// Configuration validation error
struct ConfigError {
    string field;
    string message;
    enum class Type { VALUE_ERROR, MISSING_REQUIRED, TYPE_ERROR, RANGE_ERROR, FILE_ERROR, FORMAT_ERROR } errorType;
    bool isCritical;
};

// Configuration validation namespace
namespace ConfigValidation {
    // Common validation functions
    bool validateInt(const string& str, int min = INT_MIN, int max = INT_MAX);
    bool validateDouble(const string& str, double min = -DBL_MAX, double max = DBL_MAX);
    bool validateString(const string& str, size_t minLength = 0, size_t maxLength = 1000);
    bool validateBool(const string& str);
    bool validateFileExists(const string& str);
    bool validateDirectoryExists(const string& str);
    
    // Get default validation rules for NLM configuration
    vector<ValidationRule> getDefaultValidationRules();
    
    // Custom validation rules with parameters
    function<bool(const string&)> createRangeValidator(double min, double max);
    function<bool(const string&)> createMinValidator(double min);
    function<bool(const string&)> createMaxValidator(double max);
    function<bool(const string&)> createFileExistsValidator();
    function<bool(const string&)> createDirectoryExistsValidator();
    
    // Validation error formatters
    string formatValidationError(const ConfigError& error);
    vector<string> formatValidationErrors(const vector<ConfigError>& errors);
}

// Configuration source enum (extended from core/Config.hpp)
enum class ConfigSource {
    Default = 0,
    File = 1,
    Environment = 2,
    CommandLine = 3,
    Override = 4
};

// Configuration value types
enum class ConfigValueType {
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    VECTOR_FLOAT,
    UNKNOWN
};

// Configuration value
struct ConfigValue {
    ConfigValueType type;
    union {
        int64_t intValue;
        double doubleValue;
        string stringValue;
        bool boolValue;
        vector<float> vectorFloatValue;
    };
    
    ConfigValue() : type(ConfigValueType::UNKNOWN) {}
    ~ConfigValue() {}
};

// Configuration source helper
inline string configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::Environment: return "Environment";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Override: return "Override";
        default: return "Unknown";
    }
}
