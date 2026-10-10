// Enhanced configuration error handling system
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Enhanced configuration exception hierarchy
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message, ConfigErrorType type = ConfigErrorType::None,
                    const std::string& key = "", const std::string& suggestion = "")
        : std::runtime_error(message), errorType(type), key(key), suggestion(suggestion) {}
    
    ConfigErrorType getErrorType() const { return errorType; }
    const std::string& getKey() const { return key; }
    const std::string& getSuggestion() const { return suggestion; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "ConfigException: ";
        ss << "[" << static_cast<int>(errorType) << "]";
        if (!key.empty()) ss << " Key: '" << key << "'";
        ss << " - " << what();
        if (!suggestion.empty()) ss << "\n  Suggestion: " << suggestion;
        return ss.str();
    }
    
protected:
    ConfigErrorType errorType;
    std::string key;
    std::string suggestion;
};

class ConfigValidationError : public ConfigException {
public:
    ConfigValidationError(const std::string& message, ConfigErrorType type,
                          const std::string& key = "", const std::string& suggestion = "")
        : ConfigException(message, type, key, suggestion) {}
};

class ConfigParseError : public ConfigException {
public:
    ConfigParseError(const std::string& message, const std::string& key = "",
                     const std::string& suggestion = "")
        : ConfigException(message, ConfigErrorType::ParseError, key, suggestion) {}
};

class ConfigFileError : public ConfigException {
public:
    ConfigFileError(const std::string& message, const std::string& filepath = "",
                    const std::string& suggestion = "")
        : ConfigException(message, ConfigErrorType::FileNotFound, filepath, suggestion) {}
};

// Configuration recovery suggestions
class ConfigRecoveryHelper {
public:
    static std::string getRecoverySuggestion(const ConfigError& error) {
        switch (error.type) {
            case ConfigErrorType::InvalidType:
                return "Try using a value of the correct type for this parameter.";
            case ConfigErrorType::OutOfRange:
                return "Try a value within the valid range.";
            case ConfigErrorType::MissingDependency:
                return "Ensure all dependencies are set before using this parameter.";
            case ConfigErrorType::ConflictingValues:
                return "Remove one of the conflicting parameters.";
            case ConfigErrorType::InvalidValue:
                return "Use one of the valid values listed in the parameter description.";
            case ConfigErrorType::FileNotFound:
                return "Check that the file exists and is readable.";
            case ConfigErrorType::ParseError:
                return "Check the syntax of the configuration file.";
            case ConfigErrorType::SchemaError:
                return "Check that the configuration matches the expected schema.";
            default:
                return "Try adjusting the configuration value.";
        }
    }
    
    static std::string getParameterSuggestions(const std::string& key, const ConfigSchema& schema) {
        const auto& param = schema.getParameter(key);
        if (param.key.empty()) return "Unknown parameter.";
        
        std::stringstream ss;
        ss << "Suggestions for '" << key << "':\n";
        
        if (!param.validValues.empty()) {
            ss << "  Try one of these valid values: ";
            for (size_t i = 0; i < param.validValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << '"' << param.validValues[i] << '"';
            }
            ss << "\n";
        }
        
        if (param.floatRange.has_value()) {
            ss << "  Try a value between " << param.floatRange->first << " and " << param.floatRange->second << "\n";
        } else if (param.int64Range.has_value()) {
            ss << "  Try a value between " << param.int64Range->first << " and " << param.int64Range->second << "\n";
        } else if (param.intRange.has_value()) {
            ss << "  Try a value between " << param.intRange->first << " and " << param.intRange->second << "\n";
        }
        
        if (param.positive) {
            ss << "  Try a positive value\n";
        }
        
        if (param.nonNegative) {
            ss << "  Try a non-negative value\n";
        }
        
        if (!param.dependencies.empty()) {
            ss << "  Dependencies: ";
            for (size_t i = 0; i < param.dependencies.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << param.dependencies[i];
            }
            ss << "\n";
        }
        
        return ss.str();
    }
};