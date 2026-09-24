// ConfigValidator.hpp - Configuration validation support

#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nlm {

// Forward declarations
class Config;
class ConfigError;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string error;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& err) : valid(v), error(err) {}
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string ruleType;  // "range", "enum", "positive", "nonnegative", "pattern", etc.
    std::string ruleParams;
    std::string errorMessage;
    
    ValidationRule() {}
    ValidationRule(const std::string& k, const std::string& type, 
                  const std::string& params, const std::string& error)
        : key(k), ruleType(type), ruleParams(params), errorMessage(error) {}
};

// Configuration error handling
namespace ConfigError {
    enum class ErrorType {
        FileNotFound,
        ParseError,
        InvalidValue,
        MissingRequired,
        ValidationError,
        FileWriteError,
        MemoryError
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        std::string key;
        std::string context;
        
        Error(ErrorType t, const std::string& msg) : type(t), message(msg) {}
        Error(ErrorType t, const std::string& msg, const std::string& key) 
            : type(t), message(msg), key(key) {}
    };
    
    std::string toString(ErrorType type);
    std::string formatError(const Error& error);
}

// Configuration validation class
class ConfigValidator {
public:
    static ValidationResult validate(const Config& config);
    static ValidationResult validateKey(const Config& config, const std::string& key);
    static void addValidationRule(const std::string& key, const std::string& type, 
                                 const std::string& params, const std::string& error);
    static void clearValidationRules();
    
    static const std::vector<ValidationRule>& getValidationRules() { return validationRules_; }
    
private:
    static std::vector<ValidationRule> validationRules_;
    
    static bool validateRange(const ConfigValue& value, const std::string& params);
    static bool validateEnum(const ConfigValue& value, const std::string& params);
    static bool validatePattern(const ConfigValue& value, const std::string& params);
    static bool validatePositive(const ConfigValue& value);
    static bool validateNonNegative(const ConfigValue& value);
    static bool validateStringLength(const ConfigValue& value, const std::string& params);
};

// Global error handling
static std::vector<ConfigError::Error> recentErrors;
static ConfigError::Error lastError;

// Enhanced Config with validation support
class ValidatedConfig {
public:
    ValidatedConfig() = default;
    
    bool set(const std::string& key, const std::string& value) {
        ValidationResult result = ConfigValidator::validateKey(*this, key);
        if (!result.valid) {
            // Log validation error
            return false;
        }
        return set(key, value);
    }
    
    bool set(const std::string& key, int value) {
        // Convert to string for validation
        std::string valueStr = std::to_string(value);
        ValidationResult result = ConfigValidator::validateKey(*this, key);
        if (!result.valid) {
            // Log validation error
            return false;
        }
        return set(key, value);
    }
    
    bool set(const std::string& key, double value) {
        // Convert to string for validation
        std::string valueStr = std::to_string(value);
        ValidationResult result = ConfigValidator::validateKey(*this, key);
        if (!result.valid) {
            // Log validation error
            return false;
        }
        return set(key, value);
    }
    
    // Forward all other methods from Config
    bool has(const std::string& key) const { return originalConfig.has(key); }
    std::optional<std::string> getString(const std::string& key) const { return originalConfig.get<std::string>(key); }
    std::optional<int64_t> getInt(const std::string& key) const { return originalConfig.get<int64_t>(key); }
    std::optional<double> getDouble(const std::string& key) const { return originalConfig.get<double>(key); }
    std::optional<bool> getBool(const std::string& key) const { return originalConfig.get<bool>(key); }
    std::vector<std::string> getKeys() const { return originalConfig.getKeys(); }
    
private:
    Config originalConfig;
    
    bool set(const std::string& key, const std::string& value) {
        // Implementation
        return true;
    }
    
    bool set(const std::string& key, int value) {
        // Implementation
        return true;
    }
    
    bool set(const std::string& key, double value) {
        // Implementation
        return true;
    }
    
    // Other private methods...
};

// Convenience functions
inline ValidationResult validateConfig(const Config& config) {
    return ConfigValidator::validate(config);
}

inline bool isValidConfigFile(const std::string& filename) {
    return !filename.empty() && (filename.find(".cfg") != std::string::npos || 
                                filename.find(".txt") != std::string::npos ||
                                filename.find(".json") != std::string::npos);
}

inline bool isValidCheckpointDir(const std::string& dirname) {
    return !dirname.empty() && dirname.find('/') != std::string::npos;
}

} // namespace nlm

// Template for using validated configuration
#define VALIDATE_CONFIG config_validation::ConfigValidator::validate