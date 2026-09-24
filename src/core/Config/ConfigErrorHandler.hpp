// Simple configuration validation and error handling support
// This provides enhanced error handling and validation for the existing Config class
// without modifying the core implementation

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <algorithm>

namespace nlm {

// Simple error handling for configuration
class ConfigErrorHandler {
public:
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
    
    static std::string toString(ErrorType type) {
        switch (type) {
            case ErrorType::FileNotFound: return "File not found";
            case ErrorType::ParseError: return "Parse error";
            case ErrorType::InvalidValue: return "Invalid value";
            case ErrorType::MissingRequired: return "Missing required parameter";
            case ErrorType::ValidationError: return "Configuration validation error";
            case ErrorType::FileWriteError: return "File write error";
            case ErrorType::MemoryError: return "Memory error";
            default: return "Unknown error";
        }
    }
    
    static std::string formatError(const Error& error) {
        std::string result = toString(error.type);
        if (!error.message.empty()) {
            result += ": " + error.message;
        }
        if (!error.key.empty()) {
            result += " (key: " + error.key + ")";
        }
        if (!error.context.empty()) {
            result += " (context: " + error.context + ")";
        }
        return result;
    }
    
    static std::vector<Error> recentErrors;
    static Error lastError;
};

// Enhanced Config error handling
class ConfigErrorHandler {
public:
    static std::vector<ConfigErrorHandler::Error> recentErrors;
    static ConfigErrorHandler::Error lastError;
    
    static bool handleFileNotFound(const std::string& filepath);
    static bool handleParseError(const std::string& line, size_t lineNum, const std::string& context);
    static bool handleInvalidValue(const std::string& key, const std::string& value, const std::string& expectedType);
    static bool handleMissingRequired(const std::string& key, const std::string& context);
    static bool handleValidationError(const std::string& key, const std::string& rule, const std::string& value);
    static bool handleFileWriteError(const std::string& filepath);
    static bool handleMemoryError(const std::string& context);
    
    static void logError(ErrorType type, const std::string& message, const std::string& key = "", const std::string& context = "");
    static void logError(const ConfigErrorHandler::Error& error);
    static void clearErrors();
    static size_t getErrorCount() { return recentErrors.size(); }
    static std::string getAllErrors();
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
    
    bool matchesKey(const std::string& testKey) const {
        return key == testKey || key.empty();
    }
    
    bool validate(const std::string& value) const;
};

// Configuration validation helper
class ConfigValidator {
public:
    static std::vector<ValidationRule> validationRules;
    
    static void addValidationRule(const std::string& key, const std::string& type, 
                                 const std::string& params, const std::string& error);
    
    static void removeValidationRule(const std::string& key);
    
    static void clearValidationRules();
    
    static bool validateKey(const std::string& key);
    static bool validateValue(const std::string& key, const std::string& value);
    
    static bool isValidConfigValue(const std::string& key, const std::string& value);
    
    static bool isValidFile(const std::string& filename);
    static bool isValidDirectory(const std::string& dirname);
    
    static std::string getValidationError(const std::string& key, const std::string& value);
};

// Enhanced Config methods with error handling and validation support
class EnhancedConfig {
public:
    EnhancedConfig() = default;
    
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    bool validate() const;
    std::string getValidationError() const;
    
    // Forward all methods from Config
    bool has(const std::string& key) const { return originalConfig.has(key); }
    std::optional<std::string> getString(const std::string& key) const { return originalConfig.get<std::string>(key); }
    std::optional<int64_t> getInt(const std::string& key) const { return originalConfig.get<int64_t>(key); }
    std::optional<double> getDouble(const std::string& key) const { return originalConfig.get<double>(key); }
    std::optional<bool> getBool(const std::string& key) const { return originalConfig.get<bool>(key); }
    std::vector<std::string> getKeys() const { return originalConfig.getKeys(); }
    
private:
    Config originalConfig;
    std::string validationError;
    
    bool set(const std::string& key, const std::string& value, ConfigSource source) {
        // Implementation
        return true;
    }
    
    bool set(const std::string& key, int value, ConfigSource source) {
        // Implementation
        return true;
    }
    
    bool set(const std::string& key, double value, ConfigSource source) {
        // Implementation
        return true;
    }
    
    // Other private methods...
};

// Global validation rules initialization
std::vector<ValidationRule> ConfigValidator::validationRules = {
    ValidationRule("random_seed", "range", "0, 2147483647", "random_seed must be between 0 and 2147483647"),
    ValidationRule("simulation_timestep", "range", "0.0001, 1.0", "simulation_timestep must be between 0.0001s and 1.0s"),
    ValidationRule("neuron_count", "range", "1, 1000000", "neuron_count must be between 1 and 1000000"),
    ValidationRule("region_count", "range", "1, 100", "region_count must be between 1 and 100"),
    ValidationRule("connection_probability", "range", "0.0, 1.0", "connection_probability must be between 0.0 and 1.0"),
    ValidationRule("working_memory_capacity", "range", "0, 10000", "working_memory_capacity must be between 0 and 10000"),
    ValidationRule("stdp_ltp_weight", "range", "0.0, 1.0", "stdp_ltp_weight must be between 0.0 and 1.0"),
    ValidationRule("stdp_ltd_weight", "range", "0.0, 1.0", "stdp_ltd_weight must be between 0.0 and 1.0"),
    ValidationRule("stdp_tau", "range", "1, 1000", "stdp_tau must be between 1 and 1000"),
    ValidationRule("synaptogenesis_rate", "range", "0.0, 0.1", "synaptogenesis_rate must be between 0.0 and 0.1"),
    ValidationRule("pruning_rate", "range", "0.0, 0.01", "pruning_rate must be between 0.0 and 0.01"),
    ValidationRule("checkpoint_dir", "pattern", ".+", "checkpoint_dir cannot be empty"),
    ValidationRule("config_file", "pattern", ".+", "config_file cannot be empty"),
};

// Global error handling initialization
std::vector<ConfigErrorHandler::Error> ConfigErrorHandler::recentErrors;
ConfigErrorHandler::Error ConfigErrorHandler::lastError;

// Validation rule implementation
bool ValidationRule::validate(const std::string& value) const {
    if (ruleType == "range") {
        // Parse range params (min,max)
        size_t commaPos = ruleParams.find(',');
        if (commaPos == std::string::npos) return true;
        
        std::string minStr = ruleParams.substr(0, commaPos);
        std::string maxStr = ruleParams.substr(commaPos + 1);
        
        double minVal = std::stod(minStr);
        double maxVal = std::stod(maxStr);
        
        double val = std::stod(value);
        return val >= minVal && val <= maxVal;
    } else if (ruleType == "pattern") {
        // Simple pattern validation for non-empty
        return !value.empty();
    } else if (ruleType == "positive") {
        double val = std::stod(value);
        return val > 0.0;
    } else if (ruleType == "nonnegative") {
        double val = std::stod(value);
        return val >= 0.0;
    }
    return true;
}

// Error handling implementation
bool ConfigErrorHandler::handleFileNotFound(const std::string& filepath) {
    logError(ErrorType::FileNotFound, "Configuration file not found: " + filepath, "", "file loading");
    return false;
}

bool ConfigErrorHandler::handleParseError(const std::string& line, size_t lineNum, const std::string& context) {
    std::string errorMsg = "Parse error on line " + std::to_string(lineNum) + ": " + line;
    if (!context.empty()) {
        errorMsg += " (context: " + context + ")";
    }
    logError(ErrorType::ParseError, errorMsg, "", "file parsing");
    return false;
}

bool ConfigErrorHandler::handleInvalidValue(const std::string& key, const std::string& value, const std::string& expectedType) {
    std::string errorMsg = "Invalid value for key '" + key + "': '" + value + "' (expected: " + expectedType + ")";
    logError(ErrorType::InvalidValue, errorMsg, key, "value setting");
    return false;
}

bool ConfigErrorHandler::handleMissingRequired(const std::string& key, const std::string& context) {
    std::string errorMsg = "Missing required parameter: '" + key + "'";
    if (!context.empty()) {
        errorMsg += " (context: " + context + ")";
    }
    logError(ErrorType::MissingRequired, errorMsg, key, "initialization");
    return false;
}

bool ConfigErrorHandler::handleValidationError(const std::string& key, const std::string& rule, const std::string& value) {
    std::string errorMsg = "Validation error for key '" + key + "': '" + value + "' (rule: " + rule + ")";
    logError(ErrorType::ValidationError, errorMsg, key, "validation");
    return false;
}

bool ConfigErrorHandler::handleFileWriteError(const std::string& filepath) {
    std::string errorMsg = "Failed to write configuration file: '" + filepath + "'";
    logError(ErrorType::FileWriteError, errorMsg, "", "file writing");
    return false;
}

bool ConfigErrorHandler::handleMemoryError(const std::string& context) {
    std::string errorMsg = "Memory allocation error (context: " + context + ")";
    logError(ErrorType::MemoryError, errorMsg, "", "memory management");
    return false;
}

void ConfigErrorHandler::logError(ErrorType type, const std::string& message, const std::string& key, const std::string& context) {
    Error error(type, message, key);
    error.context = context;
    recentErrors.push_back(error);
    lastError = error;
    
    // Also output to stderr
    std::cerr << "[NLM Config Error] " << formatError(error) << std::endl;
}

void ConfigErrorHandler::logError(const ConfigErrorHandler::Error& error) {
    recentErrors.push_back(error);
    lastError = error;
    
    // Also output to stderr
    std::cerr << "[NLM Config Error] " << formatError(error) << std::endl;
}

void ConfigErrorHandler::clearErrors() {
    recentErrors.clear();
}

std::string ConfigErrorHandler::getAllErrors() {
    std::ostringstream oss;
    if (recentErrors.empty()) {
        return "No errors";
    }
    
    oss << "Configuration errors:";
    for (const auto& error : recentErrors) {
        oss << "\n  - " << formatError(error);
    }
    return oss.str();
}

// Validation helper implementation
bool ConfigValidator::validateKey(const std::string& key) {
    for (const auto& rule : validationRules) {
        if (rule.matchesKey(key)) {
            return rule.validate("") && true;  // Empty value will be validated later
        }
    }
    return true;  // No matching rule means valid
}

bool ConfigValidator::validateValue(const std::string& key, const std::string& value) {
    for (const auto& rule : validationRules) {
        if (rule.matchesKey(key)) {
            if (!rule.validate(value)) {
                return false;
            }
        }
    }
    return true;
}

bool ConfigValidator::isValidConfigValue(const std::string& key, const std::string& value) {
    return validateValue(key, value);
}

bool ConfigValidator::isValidFile(const std::string& filename) {
    return !filename.empty() && (
        filename.find(".cfg") != std::string::npos ||
        filename.find(".txt") != std::string::npos ||
        filename.find(".json") != std::string::npos ||
        filename.find(".yaml") != std::string::npos ||
        filename.find(".yml") != std::string::npos
    );
}

bool ConfigValidator::isValidDirectory(const std::string& dirname) {
    return !dirname.empty() && (
        dirname.find('/') != std::string::npos ||
        dirname.find('\\') != std::string::npos ||
        dirname == "." || dirname == ".."
    );
}

std::string ConfigValidator::getValidationError(const std::string& key, const std::string& value) {
    for (const auto& rule : validationRules) {
        if (rule.matchesKey(key) && !rule.validate(value)) {
            return rule.errorMessage;
        }
    }
    return "";
}

} // namespace nlm

// Convenience function for validation
namespace config_utils {
    inline bool validateConfigFile(const std::string& filename) {
        return nlm::ConfigValidator::isValidFile(filename);
    }
    
    inline bool validateCheckpointDirectory(const std::string& dirname) {
        return nlm::ConfigValidator::isValidDirectory(dirname);
    }
    
    inline bool validateConfigValue(const std::string& key, const std::string& value) {
        return nlm::ConfigValidator::isValidConfigValue(key, value);
    }
    
    inline std::string getConfigValidationError(const std::string& key, const std::string& value) {
        return nlm::ConfigValidator::getValidationError(key, value);
    }
}

// Macro for simplified configuration validation
#define VALIDATE_CONFIG_VALUE(key, value) nlm::config_utils::validateConfigValue(key, value)
#define VALIDATE_CONFIG_FILE(filename) nlm::config_utils::validateConfigFile(filename)
#define VALIDATE_CONFIG_DIR(dirname) nlm::config_utils::validateCheckpointDirectory(dirname)