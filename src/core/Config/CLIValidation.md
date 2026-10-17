# Command-Line Configuration Validation

## Overview

Command-line configuration validation provides robust validation for configuration values provided via command-line arguments. This system ensures that configuration values are valid before being processed, provides detailed error messages, and helps users quickly identify and correct configuration issues.

## Validation Architecture

### Core Components

1. **CLIValidator** - Main command-line validation class
2. **ValidationRule** - Individual validation rule definition
3. **ValidationResult** - Result of configuration validation
4. **ValidationError** - Validation error structure
5. **ConfigAnalyzer** - Configuration analysis and validation
6. **HelpGenerator** - Help generation for validation errors
7. **RecoverySystem** - Recovery recommendations and suggestions

### Key Features

- **Comprehensive Validation**: Validation for all command-line arguments
- **Detailed Error Messages**: Clear, actionable error messages
- **Required Key Validation**: Ensure all required keys are present
- **Range Validation**: Validate numeric and string ranges
- **Format Validation**: Validate configuration value formats
- **Recovery Recommendations**: Suggest fixes for common issues
- **Interactive Help**: Provide interactive help for validation errors
- **Performance Considerations**: Fast validation with minimal overhead
- **Configurable Validation**: Customizable validation rules
- **Severity Levels**: Different severity levels for validation errors

## Command-Line Validation Implementation

### CLI Validator Class

```cpp
class CLIValidator {
public:
    CLIValidator();
    ~CLIValidator();
    
    // Configuration validation
    ValidationResult validate(int argc, char** argv);
    ValidationResult validate(const std::map<std::string, std::string>& args);
    
    // Individual argument validation
    ValidationResult validateArgument(const std::string& key, const std::string& value);
    bool validateKey(const std::string& key);
    bool validateValue(const std::string& key, const std::string& value);
    
    // Required key validation
    void setRequiredKeys(const std::vector<std::string>& requiredKeys);
    bool areRequiredKeysPresent(const std::map<std::string, std::string>& args) const;
    std::vector<std::string> getMissingRequiredKeys(const std::map<std::string, std::string>& args) const;
    
    // Validation rule management
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    void removeValidationRule(const std::string& key);
    void clearValidationRules();
    
    // Validation configuration
    void setValidationLevel(ValidationLevel level);
    void enableStrictMode(bool enable);
    void setRecoveryEnabled(bool enable);
    void setHelpEnabled(bool enable);
    
    // Error handling
    void setErrorHandler(std::function<void(const ValidationError&)> handler);
    void setWarningHandler(std::function<void(const ValidationError&)> handler);
    void setInfoHandler(std::function<void(const ValidationError&)> handler);
    
    // Configuration validation
    ValidationResult validateConfiguration(const Config& config);
    bool isConfigurationValid(const Config& config) const;
    
    // Help and documentation
    std::string generateValidationHelp() const;
    std::string generateValidationReport(const ValidationResult& result) const;
    std::string getValidationErrorSummary(const ValidationResult& result) const;
    
    // Performance optimization
    void optimizeForSpeed();
    void optimizeForAccuracy();
    size_t getEstimatedValidationTime(const std::map<std::string, std::string>& args) const;
    
    // Statistics
    ValidationStatistics getStatistics() const;
    void resetStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Validation Result Structure

```cpp
struct ValidationResult {
    bool isValid;
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    std::vector<ValidationError> infos;
    std::vector<std::string> recoverySuggestions;
    std::map<std::string, std::string> validatedValues;
    std::chrono::system_clock::time_point validationTime;
    double validationTimeMs;
    
    ValidationResult() {
        isValid = true;
        validationTime = std::chrono::system_clock::now();
        validationTimeMs = 0.0;
    }
    
    bool hasErrors() const;
    bool hasWarnings() const;
    bool hasInfos() const;
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    size_t getInfoCount() const;
    std::string getSummary() const;
    ValidationSeverity getSeverity() const;
    
    void addError(const ValidationError& error);
    void addWarning(const ValidationError& warning);
    void addInfo(const ValidationError& info);
    
    std::string formatError(const ValidationError& error) const;
    std::string formatValidationReport() const;
};
```

### Validation Error Structure

```cpp
struct ValidationError {
    std::string id;
    std::string key;
    std::string value;
    ValidationLevel level;
    ValidationSeverity severity;
    std::string message;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> recoverySuggestions;
    std::map<std::string, std::string> metadata;
    
    ValidationError() : level(ValidationLevel::Warning), severity(ValidationSeverity::Medium), lineNumber(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ValidationError(const std::string& key, const std::string& value, 
                   ValidationLevel level, const std::string& message)
        : key(key), value(value), level(level), severity(mapLevelToSeverity(level)),
          message(message), description("") {
        timestamp = std::chrono::system_clock::now();
        lineNumber = 0;
    }
    
    std::string getFormattedMessage() const;
    std::string getErrorCode() const;
    bool isCritical() const;
    bool isRecoverable() const;
    
    enum class ValidationSeverity {
        Low,
        Medium,
        High,
        Critical
    };
    
    static ValidationSeverity mapLevelToSeverity(ValidationLevel level);
    static std::string severityToString(ValidationSeverity severity);
    
    int lineNumber;
    std::string sourceFile;
    std::string functionName;
};
```

### Validation Rule Structure

```cpp
struct ValidationRule {
    std::string keyPattern;
    std::vector<ValidationType> allowedTypes;
    std::string validationDescription;
    std::string errorMessage;
    ValidationSeverity severity;
    std::string defaultValue;
    bool isRequired;
    bool isReadOnly;
    
    std::map<std::string, std::string> additionalParams;
    std::vector<std::string> allowedValues;
    std::pair<double, double> numericRange;
    std::pair<size_t, size_t> stringLengthRange;
    std::string patternRegex;
    std::string formatTemplate;
    
    bool isValid(const std::string& key, const std::string& value) const;
    std::string getErrorMessage(const std::string& key, const std::string& value) const;
    bool isValueAllowed(const std::string& value) const;
    bool isInNumericRange(double value) const;
    bool matchesPattern(const std::string& value) const;
    bool isStringInRange(const std::string& value) const;
    
    enum class ValidationType {
        Integer,
        Integer64,
        Double,
        Boolean,
        String,
        VectorInt,
        VectorDouble,
        VectorString,
        VectorBool,
        Any
    };
};
```

### Validation Level

```cpp
enum class ValidationLevel {
    Debug,      // Detailed validation information
    Info,       // General validation information
    Warning,    // Potential issues
    Error,      // Configuration errors
    Critical,   // Critical configuration errors
    Fatal       // Unrecoverable configuration errors
};
```

### Validation Statistics

```cpp
struct ValidationStatistics {
    size_t totalValidations;
    size_t successfulValidations;
    size_t failedValidations;
    double averageValidationTime;
    size_t errorCountsByLevel[5];  // Debug, Info, Warning, Error, Critical, Fatal
    size_t errorCountsBySeverity[4];  // Low, Medium, High, Critical
    std::map<std::string, size_t> errorCountsByKey;
    std::map<std::string, size_t> validationCountsByType;
    
    size_t getTotalErrors() const;
    size_t getTotalWarnings() const;
    double getSuccessRate() const;
    std::string getFormattedStats() const;
    void reset();
};
```

## Implementation Details

### CLI Validator Implementation

```cpp
class CLIValidator::Impl {
public:
    Impl() {
        setupDefaultValidationRules();
        setupErrorHandlers();
        initializeStatistics();
        setupValidationCache();
    }
    
    ValidationResult validate(int argc, char** argv) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::map<std::string, std::string> args;
        if (!parseCommandLineArgs(argc, argv, args)) {
            ValidationResult result;
            result.isValid = false;
            result.validationTimeMs = getElapsedTime(startTime);
            return result;
        }
        
        ValidationResult result = validateArguments(args);
        
        result.validationTimeMs = getElapsedTime(startTime);
        updateStatistics(result);
        
        return result;
    }
    
    ValidationResult validate(const std::map<std::string, std::string>& args) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        ValidationResult result = validateArguments(args);
        
        result.validationTimeMs = getElapsedTime(startTime);
        updateStatistics(result);
        
        return result;
    }
    
    ValidationResult validateArguments(const std::map<std::string, std::string>& args) {
        ValidationResult result;
        
        // Check for unknown keys
        checkUnknownKeys(args, result);
        
        // Validate each argument
        for (const auto& pair : args) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            if (!validateKey(key, result)) {
                continue;
            }
            
            if (!validateValue(key, value, result)) {
                continue;
            }
            
            result.validatedValues[key] = value;
        }
        
        // Check required keys
        checkRequiredKeys(args, result);
        
        // Validate configuration
        validateConfiguration(result);
        
        // Generate recovery suggestions
        generateRecoverySuggestions(result);
        
        // Determine overall validity
        result.isValid = result.errors.empty() && 
                        (severity == ValidationLevel::Debug || result.warnings.empty());
        
        return result;
    }
    
private:
    // Data members
    mutable std::mutex mutex;
    ValidationLevel severity;
    bool strictMode;
    bool recoveryEnabled;
    bool helpEnabled;
    std::vector<ValidationRule> validationRules;
    std::vector<std::string> requiredKeys;
    ValidationStatistics statistics;
    std::map<std::string, ValidationRule> validationRuleCache;
    std::map<std::string, bool> keyValidationCache;
    std::map<std::pair<std::string, std::string>, bool> valueValidationCache;
    
    // Error handlers
    std::function<void(const ValidationError&)> errorHandler;
    std::function<void(const ValidationError&)> warningHandler;
    std::function<void(const ValidationError&)> infoHandler;
    
    // Helper methods
    void setupDefaultValidationRules();
    void setupErrorHandlers();
    void initializeStatistics();
    void setupValidationCache();
    
    bool parseCommandLineArgs(int argc, char** argv, 
                             std::map<std::string, std::string>& args) const;
    ValidationResult validateArguments(const std::map<std::string, std::string>& args);
    void checkUnknownKeys(const std::map<std::string, std::string>& args, 
                         ValidationResult& result) const;
    bool validateKey(const std::string& key, ValidationResult& result) const;
    bool validateValue(const std::string& key, const std::string& value, 
                      ValidationResult& result) const;
    void checkRequiredKeys(const std::map<std::string, std::string>& args, 
                          ValidationResult& result) const;
    void validateConfiguration(ValidationResult& result) const;
    void generateRecoverySuggestions(ValidationResult& result) const;
    void updateStatistics(const ValidationResult& result);
    double getElapsedTime(std::chrono::high_resolution_clock::time_point startTime) const;
    
    ValidationRule getValidationRule(const std::string& key) const;
    bool isKnownKey(const std::string& key) const;
    bool isKeyInRange(const std::string& key, const std::string& value) const;
    bool isValueInAllowedTypes(const std::string& key, const std::string& value) const;
    bool isValueInAllowedValues(const std::string& value, 
                              const std::vector<std::string>& allowedValues) const;
    bool isValueInNumericRange(const std::string& key, const std::string& value) const;
    bool matchesPattern(const std::string& value, const std::string& pattern) const;
    bool isStringInLengthRange(const std::string& value, size_t minLength, size_t maxLength) const;
    
    void handleValidationError(const ValidationError& error);
    void handleValidationWarning(const ValidationError& warning);
    void handleValidationInfo(const ValidationError& info);
    
    std::string getKeyTypeName(const std::string& key) const;
    ValidationLevel getRecommendedValidationLevel() const;
    
    struct ValidationCache {
        std::chrono::system_clock::time_point timestamp;
        bool isValid;
        ValidationResult result;
        
        ValidationCache() {
            timestamp = std::chrono::system_clock::now();
            isValid = false;
        }
    };
};
```

### Command-Line Argument Parser

```cpp
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Argument parsing
    std::map<std::string, std::string> parse(int argc, char** argv);
    std::map<std::string, std::string> parse(const std::vector<std::string>& args);
    
    // Argument analysis
    std::vector<std::string> getAllKeys(const std::map<std::string, std::string>& args) const;
    std::vector<std::string> getUnknownKeys(const std::map<std::string, std::string>& args) const;
    std::vector<std::pair<std::string, std::string>> getArgumentPairs(const std::map<std::string, std::string>& args) const;
    
    // Configuration detection
    bool detectsConfiguration(const std::map<std::string, std::string>& args) const;
    std::string detectConfigurationFile(const std::map<std::string, std::string>& args) const;
    bool detectsExperimentMode(const std::map<std::string, std::string>& args) const;
    std::string detectExperimentMode(const std::map<std::string, std::string>& args) const;
    
    // Argument validation
    bool validateArgumentFormat(const std::string& arg) const;
    std::string normalizeKey(const std::string& key) const;
    std::string normalizeValue(const std::string& value) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Validation Helper Functions

```cpp
namespace ValidationHelpers {
    // Key validation
    bool isValidKey(const std::string& key);
    std::vector<std::string> getValidKeyPatterns();
    bool matchesKeyPattern(const std::string& key, const std::string& pattern);
    
    // Value validation
    bool isValidValue(const std::string& value);
    bool isNumericValue(const std::string& value);
    bool isBooleanValue(const std::string& value);
    bool isVectorValue(const std::string& value);
    
    // Type conversion
    std::string getValueType(const std::string& value);
    bool tryConvertToInt(const std::string& value, int& result);
    bool tryConvertToInt64(const std::string& value, int64_t& result);
    bool tryConvertToDouble(const std::string& value, double& result);
    bool tryConvertToBool(const std::string& value, bool& result);
    
    // Format validation
    bool isValidFormat(const std::string& format);
    std::string getFormatError(const std::string& format);
    
    // Range validation
    bool isInRange(double value, double min, double max);
    bool isInRange(const std::string& value, size_t minLength, size_t maxLength);
    
    // Pattern validation
    bool matchesRegex(const std::string& value, const std::string& pattern);
    std::vector<std::string> getCommonPatterns();
    
    // Error message generation
    std::string generateKeyError(const std::string& key);
    std::string generateValueError(const std::string& key, const std::string& value);
    std::string generateFormatError(const std::string& key, const std::string& value);
    std::string generateRangeError(const std::string& key, const std::string& value);
    
    // Recovery suggestions
    std::vector<std::string> getKeyRecoverySuggestions(const std::string& key);
    std::vector<std::string> getValueRecoverySuggestions(const std::string& key, 
                                                       const std::string& value);
    std::vector<std::string> getFormatRecoverySuggestions(const std::string& key, 
                                                         const std::string& value);
    
    // Validation statistics
    std::string getValidationSummary(const ValidationResult& result);
    std::string getErrorBreakdown(const ValidationResult& result);
    std::string getWarningSummary(const ValidationResult& result);
    
    // Performance optimization
    size_t estimateValidationTime(const std::map<std::string, std::string>& args);
    bool shouldSkipValidation(const std::string& key, const std::string& value);
    void optimizeValidationOrder(std::map<std::string, std::string>& args);
    
    // Validation rules
    std::vector<ValidationRule> getDefaultValidationRules();
    ValidationRule getValidationRuleForKey(const std::string& key);
    bool isKeyValidated(const std::string& key);
    
    // Error handling
    std::string getErrorCode(const ValidationError& error);
    ValidationSeverity mapErrorToSeverity(ValidationLevel level);
    bool isRecoverableError(const ValidationError& error);
    
    // Performance monitoring
    struct ValidationTimer {
        std::chrono::high_resolution_clock::time_point startTime;
        std::string operation;
        
        ValidationTimer(const std::string& op) : operation(op) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(endTime - startTime).count();
        }
    };
};
```

## Integration with Configuration System

### Configuration Validation Integration

```cpp
// Integration with Config class
class Config {
public:
    // Validation integration
    ValidationResult validateFromArgs(int argc, char** argv);
    bool validateCommandLineArguments(int argc, char** argv);
    ValidationResult validateConfiguration(const std::map<std::string, std::string>& args);
    
    // Command-line argument processing
    void processCommandLineArguments(int argc, char** argv);
    void setValidationEnabled(bool enable);
    bool isValidationEnabled() const;
    
    // Validation statistics
    ValidationStatistics getValidationStatistics() const;
    void resetValidationStatistics();
    
    // Validation help
    std::string getValidationHelp() const;
    std::string getValidationErrorHelp(const ValidationError& error) const;
    std::string getValidationRecoveryHelp(const ValidationError& error) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### CLI Integration with Validation

```cpp
// Enhanced CLI integration
class EnhancedCLI {
public:
    EnhancedCLI();
    ~EnhancedCLI();
    
    // Main CLI processing
    int run(int argc, char** argv);
    
    // Validation integration
    void setupValidation(const Config& config);
    ValidationResult validateArguments(int argc, char** argv);
    bool handleValidationErrors(const ValidationResult& result);
    
    // Interactive validation
    void showValidationError(const ValidationError& error);
    void showValidationSummary(const ValidationResult& result);
    bool askForRecoveryOptions(const ValidationResult& result);
    
    // Validation help
    void showValidationHelp();
    void showSpecificValidationHelp(const std::string& key);
    void showRecoveryHelp(const ValidationError& error);
    
    // Configuration loading with validation
    bool loadConfigurationWithValidation(const std::string& configFile, 
                                        const std::map<std::string, std::string>& args);
    bool saveConfigurationWithValidation(const std::string& configFile, 
                                        const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Validation Configuration

### CLI Validation Configuration

```bash
# Enable command-line validation
./nlm --validate-args

# Set validation level
./nlm --validation-level Warning
./nlm --validation-level Error

# Enable strict mode
./nlm --strict-validation

# Disable recovery suggestions
./nlm --no-recovery-suggestions

# Enable detailed validation output
./nlm --detailed-validation

# Set validation timeout
./nlm --validation-timeout 1000

# Configure validation rules
./nlm --validation-rules "file_rules,cli_rules,config_rules"

# Load validation configuration
./nlm --validation-config validation_config.json
```

### Validation Configuration File

```json
{
    "validation": {
        "enabled": true,
        "level": "Warning",
        "strict_mode": false,
        "recovery_enabled": true,
        "help_enabled": true,
        "timeout": 1000,
        
        "rules": {
            "file_validation": {
                "enabled": true,
                "required_keys": ["neuron_count", "region_count"],
                "allowed_keys": ["neuron_count", "region_count", "connection_probability", 
                               "stdp_ltp_weight", "stdp_ltd_weight", "simulation_timestep"],
                "numeric_ranges": {
                    "neuron_count": [1, 10000],
                    "region_count": [1, 10],
                    "connection_probability": [0, 1],
                    "stdp_ltp_weight": [0.001, 0.1],
                    "stdp_ltd_weight": [0.001, 0.1],
                    "simulation_timestep": [0.0001, 1.0]
                }
            },
            
            "cli_validation": {
                "enabled": true,
                "key_prefixes": ["--", "-"],
                "value_separators": ["=", " "],
                "required_keys": ["random_seed", "experiment_mode"],
                "allowed_keys": ["random_seed", "experiment_mode", "enable_advanced", 
                               "validation_level", "strict_validation"],
                "boolean_values": ["true", "false", "yes", "no", "1", "0"],
                "allowed_patterns": ["^[a-zA-Z0-9_]+$", "^[0-9]+$", "^[0-9]+\.[0-9]+$"]
            },
            
            "config_validation": {
                "enabled": true,
                "schema_validation": true,
                "type_validation": true,
                "range_validation": true,
                "format_validation": true
            }
        },
        
        "error_handling": {
            "fail_on_error": false,
            "show_help_on_error": true,
            "suggest_recovery": true,
            "log_errors": true,
            "log_warnings": true,
            "log_infos": false
        },
        
        "performance": {
            "enable_caching": true,
            "cache_size": 1000,
            "optimize_for_speed": false,
            "optimize_for_accuracy": true
        }
    }
}
```

## Validation Error Types

### Common Validation Errors

```cpp
// Error types
enum class ValidationErrorType {
    UnknownKey,           // Unknown or unsupported key
    MissingRequiredKey,    // Missing required configuration key
    InvalidKeyFormat,     // Key format is invalid
    InvalidKeyType,       // Key type is invalid
    InvalidValueType,     // Value type is invalid
    InvalidValueFormat,   // Value format is invalid
    InvalidValueRange,    // Value is outside valid range
    InvalidValuePattern,  // Value doesn't match required pattern
    InvalidValueLength,   // Value length is invalid
    ValueConversionError, // Value cannot be converted
    ConfigurationError,   // Configuration is invalid
    SchemaError,          // Schema validation failed
    FileError,            // File-related error
    PermissionError,      // Permission error
    SystemError,          // System error
    TimeoutError,         // Validation timeout
    MemoryError,          // Memory allocation error
    NetworkError,         // Network error
    UnknownError         // Unknown error
};
```

### Error Codes

```cpp
// Error codes for validation errors
static const std::string ERROR_CODES[] = {
    "ERR_UNKNOWN_KEY",           // Unknown key
    "ERR_MISSING_REQUIRED_KEY",   // Missing required key
    "ERR_INVALID_KEY_FORMAT",     // Invalid key format
    "ERR_INVALID_KEY_TYPE",       // Invalid key type
    "ERR_INVALID_VALUE_TYPE",     // Invalid value type
    "ERR_INVALID_VALUE_FORMAT",   // Invalid value format
    "ERR_INVALID_VALUE_RANGE",    // Invalid value range
    "ERR_INVALID_VALUE_PATTERN", // Invalid value pattern
    "ERR_INVALID_VALUE_LENGTH",   // Invalid value length
    "ERR_VALUE_CONVERSION",       // Value conversion error
    "ERR_CONFIGURATION",          // Configuration error
    "ERR_SCHEMA",                 // Schema error
    "ERR_FILE",                   // File error
    "ERR_PERMISSION",             // Permission error
    "ERR_SYSTEM",                 // System error
    "ERR_TIMEOUT",               // Timeout error
    "ERR_MEMORY",                 // Memory error
    "ERR_NETWORK",                // Network error
    "ERR_UNKNOWN"                 // Unknown error
};
```

### Error Messages

```cpp
// Common error messages
static const std::string ERROR_MESSAGES[] = {
    "Unknown configuration key: %s. Available keys: %s",
    "Missing required configuration key: %s. Required keys: %s",
    "Invalid key format: %s. Keys must contain only alphanumeric characters and underscores",
    "Invalid key type: %s. Expected type: %s",
    "Invalid value type for key: %s. Expected type: %s, got: %s",
    "Invalid value format for key: %s. Expected format: %s",
    "Value out of range for key: %s. Expected range: [%f, %f], got: %f",
    "Value does not match required pattern for key: %s. Pattern: %s",
    "Value length invalid for key: %s. Expected length range: [%zu, %zu], got: %zu",
    "Cannot convert value for key: %s. Value: %s, Expected type: %s",
    "Configuration validation failed for key: %s. Error: %s",
    "Schema validation failed for configuration. Error: %s",
    "Configuration file error: %s",
    "Permission denied: %s",
    "System error: %s",
    "Validation timeout: %s",
    "Memory allocation error: %s",
    "Network error: %s",
    "Unknown validation error: %s"
};
```

## Validation Error Handling

### Error Recovery System

```cpp
class RecoverySystem {
public:
    RecoverySystem();
    ~RecoverySystem();
    
    // Recovery suggestions
    std::vector<std::string> getRecoverySuggestions(const ValidationError& error) const;
    std::vector<std::string> getKeyRecoverySuggestions(const std::string& key) const;
    std::vector<std::string> getValueRecoverySuggestions(const std::string& key, 
                                                        const std::string& value) const;
    
    // Interactive recovery
    bool suggestRecovery(const ValidationError& error);
    std::string getAutoRecoverySuggestion(const ValidationError& error);
    bool applyAutoRecovery(const ValidationError& error);
    
    // Recovery rules
    void addRecoveryRule(const RecoveryRule& rule);
    void removeRecoveryRule(const std::string& errorId);
    void clearRecoveryRules();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

struct RecoveryRule {
    std::string errorId;
    std::string errorType;
    std::string recoveryAction;
    std::string recoveryCommand;
    double successProbability;
    std::vector<std::string> conditions;
    std::vector<std::string> requiredKeys;
    
    bool canApply(const ValidationError& error) const;
    std::string getRecoveryCommand(const std::map<std::string, std::string>& args) const;
    std::string getFormattedCommand(const std::map<std::string, std::string>& args) const;
};
```

### Interactive Validation

```cpp
// Interactive validation interface
class InteractiveValidator {
public:
    InteractiveValidator();
    ~InteractiveValidator();
    
    // Interactive error handling
    void showValidationError(const ValidationError& error);
    bool askForErrorRecovery(const ValidationError& error);
    bool askForSkipValidation(const ValidationError& error);
    bool askForContinueWithErrors(const ValidationError& error);
    
    // Interactive help
    void showValidationHelp();
    void showSpecificHelp(const std::string& key);
    void showRecoveryOptions(const ValidationError& error);
    void showAvailableCommands();
    
    // Interactive configuration
    std::string getInteractiveInput(const std::string& prompt, 
                                   const std::string& defaultValue = "",
                                   bool isSecret = false);
    std::vector<std::string> getInteractiveList(const std::string& prompt,
                                              const std::vector<std::string>& options,
                                              bool multiple = false);
    bool getInteractiveConfirmation(const std::string& prompt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Validation Performance Optimization

### Performance Optimization Strategies

```cpp
// Performance optimization strategies
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    // Optimization strategies
    void optimizeForSpeed();
    void optimizeForAccuracy();
    void optimizeForMemory();
    void optimizeForParallel();
    
    // Validation caching
    void enableValidationCaching(bool enable);
    void setCacheSize(size_t cacheSize);
    size_t getCacheHits() const;
    size_t getCacheMisses() const;
    double getCacheHitRate() const;
    
    // Validation ordering
    std::vector<std::string> optimizeValidationOrder(const std::map<std::string, std::string>& args);
    size_t estimateValidationTime(const std::map<std::string, std::string>& args);
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    bool isMonitoring() const;
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Resource management
    void setMemoryLimit(size_t memoryLimit);
    void setTimeLimit(size_t timeLimit);
    bool isWithinResourceLimits() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Validation Performance Monitoring

```cpp
// Validation performance monitoring
class ValidationPerformanceMonitor {
public:
    ValidationPerformanceMonitor();
    ~ValidationPerformanceMonitor();
    
    // Performance tracking
    void startValidation(const std::string& operation);
    void stopValidation(const std::string& operation);
    double getValidationTime(const std::string& operation) const;
    std::map<std::string, double> getAllValidationTimes() const;
    void resetPerformanceMetrics();
    
    // Performance analysis
    double getAverageValidationTime() const;
    double getValidationTimeStandardDeviation() const;
    std::string getPerformanceReport() const;
    
    // Performance optimization
    void analyzePerformanceBottlenecks();
    void suggestPerformanceImprovements();
    void applyPerformanceOptimizations();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Validation Integration Examples

### Example 1: Basic Validation Integration

```cpp
#include "core/Config/Config.hpp"
#include "core/CLIValidator.hpp"

int main(int argc, char** argv) {
    try {
        // Create configuration
        Config config;
        
        // Setup validation
        config.setValidationEnabled(true);
        config.setValidationLevel(ValidationLevel::Warning);
        
        // Validate command line arguments
        ValidationResult validation = config.validateFromArgs(argc, argv);
        
        if (!validation.isValid) {
            std::cout << "Configuration validation failed:" << std::endl;
            for (const auto& error : validation.errors) {
                std::cout << "  ERROR: " << error.getFormattedMessage() << std::endl;
            }
            
            if (config.isHelpEnabled()) {
                std::cout << "\nTry --help for more information or --fix-errors to auto-fix common issues." << std::endl;
            }
            
            return 1;
        }
        
        // Process validated arguments
        config.processCommandLineArguments(argc, argv);
        
        // Continue with normal execution
        std::cout << "Configuration validation passed." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 2: Advanced Validation Integration

```cpp
#include "core/Config/Config.hpp"
#include "core/CLIValidator.hpp"
#include "core/InteractiveValidator.hpp"

int main(int argc, char** argv) {
    try {
        // Create configuration with validation
        Config config;
        
        // Setup advanced validation
        config.setValidationEnabled(true);
        config.setValidationLevel(ValidationLevel::Error);
        config.setStrictMode(true);
        config.setRecoveryEnabled(true);
        config.setHelpEnabled(true);
        
        // Validate command line arguments
        InteractiveValidator interactiveValidator;
        ValidationResult validation = config.validateFromArgs(argc, argv);
        
        if (!validation.isValid) {
            std::cout << "Configuration validation found " << validation.errors.size() 
                      << " error(s):" << std::endl;
            
            // Show validation errors interactively
            interactiveValidator.showValidationError(validation.errors[0]);
            
            // Ask for recovery options
            if (config.isRecoveryEnabled()) {
                std::cout << "\nWould you like to try auto-recovery? (y/n): ";
                char response;
                std::cin >> response;
                
                if (response == 'y' || response == 'Y') {
                    // Apply auto-recovery
                    bool recovered = interactiveValidator.applyAutoRecovery(validation.errors[0]);
                    if (recovered) {
                        std::cout << "Auto-recovery applied successfully." << std::endl;
                        
                        // Retry validation
                        ValidationResult recoveryResult = config.validateFromArgs(argc, argv);
                        if (recoveryResult.isValid) {
                            std::cout << "Recovery successful! Configuration is now valid." << std::endl;
                            return 0;
                        } else {
                            std::cout << "Recovery failed. Please check the errors." << std::endl;
                        }
                    } else {
                        std::cout << "Auto-recovery failed. Please check the errors manually." << std::endl;
                    }
                }
            }
            
            // Show validation summary
            interactiveValidator.showValidationSummary(validation);
            
            return 1;
        }
        
        // Show validation success
        std::cout << "Configuration validation passed successfully!" << std::endl;
        
        // Process validated arguments
        config.processCommandLineArguments(argc, argv);
        
        // Continue with normal execution
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Example 3: Performance Monitoring Integration

```cpp
#include "core/Config/Config.hpp"
#include "core/CLIValidator.hpp"
#include "core/ValidationPerformanceMonitor.hpp"

int main(int argc, char** argv) {
    try {
        // Create configuration with performance monitoring
        Config config;
        
        // Setup validation with performance monitoring
        config.setValidationEnabled(true);
        config.setValidationLevel(ValidationLevel::Info);
        
        // Start performance monitoring
        ValidationPerformanceMonitor performanceMonitor;
        performanceMonitor.startPerformanceMonitoring();
        
        // Validate command line arguments
        ValidationResult validation = config.validateFromArgs(argc, argv);
        
        // Stop performance monitoring
        performanceMonitor.stopPerformanceMonitoring();
        
        // Show performance metrics
        PerformanceMetrics metrics = performanceMonitor.getPerformanceMetrics();
        std::cout << "=== Validation Performance Metrics ===" << std::endl;
        std::cout << "Total validations: " << metrics.totalValidations << std::endl;
        std::cout << "Successful validations: " << metrics.successfulValidations << std::endl;
        std::cout << "Failed validations: " << metrics.failedValidations << std::endl;
        std::cout << "Average validation time: " << metrics.averageValidationTime << " ms" << std::endl;
        std::cout << "Cache hits: " << metrics.getCacheHits() << std::endl;
        std::cout << "Cache misses: " << metrics.getCacheMisses() << std::endl;
        std::cout << "Cache hit rate: " << (metrics.getCacheHitRate() * 100) << "%" << std::endl;
        
        if (!validation.isValid) {
            std::cout << "\nConfiguration validation failed:" << std::endl;
            for (const auto& error : validation.errors) {
                std::cout << "  ERROR: " << error.getFormattedMessage() << std::endl;
            }
            return 1;
        }
        
        // Process validated arguments
        config.processCommandLineArguments(argc, argv);
        
        std::cout << "Configuration validation passed successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Validation Configuration File

### CLI Validation Configuration

```bash
# Enable command-line validation
./nlm --validate-args

# Set validation level
./nlm --validation-level Warning
./nlm --validation-level Error

# Enable strict mode
./nlm --strict-validation

# Disable recovery suggestions
./nlm --no-recovery-suggestions

# Enable detailed validation output
./nlm --detailed-validation

# Set validation timeout
./nlm --validation-timeout 1000

# Configure validation rules
./nlm --validation-rules "file_rules,cli_rules,config_rules"

# Load validation configuration
./nlm --validation-config validation_config.json
```

### Validation Configuration File Example

```ini
# NLM Command-Line Validation Configuration
# Generated by: Your Tool
# Generated on: 2026-10-17

[validation]
enable = true
level = "Warning"
strict_mode = false
recovery_enabled = true
help_enabled = true
timeout = 1000

[rules]
file_validation = {
    enabled = true,
    required_keys = ["neuron_count", "region_count"],
    allowed_keys = ["neuron_count", "region_count", "connection_probability", 
                   "stdp_ltp_weight", "stdp_ltd_weight", "simulation_timestep"],
    numeric_ranges = {
        "neuron_count" = [1, 10000],
        "region_count" = [1, 10],
        "connection_probability" = [0, 1],
        "stdp_ltp_weight" = [0.001, 0.1],
        "stdp_ltd_weight" = [0.001, 0.1],
        "simulation_timestep" = [0.0001, 1.0]
    }
}

cli_validation = {
    enabled = true,
    key_prefixes = ["--", "-"],
    value_separators = ["=", " "],
    required_keys = ["random_seed", "experiment_mode"],
    allowed_keys = ["random_seed", "experiment_mode", "enable_advanced", 
                   "validation_level", "strict_validation"],
    boolean_values = ["true", "false", "yes", "no", "1", "0"],
    allowed_patterns = ["^[a-zA-Z0-9_]+$", "^[0-9]+$", "^[0-9]+\.[0-9]+$"]
}

[error_handling]
fail_on_error = false
show_help_on_error = true
suggest_recovery = true
log_errors = true
log_warnings = true
log_infos = false

[performance]
enable_caching = true
cache_size = 1000
optimize_for_speed = false
optimize_for_accuracy = true

[help]
show_detailed_help = true
show_recovery_help = true
show_example_usage = true
```

## Validation Error Codes

### Common Validation Error Codes

```cpp
// Error codes for validation
static const std::string VALIDATION_ERROR_CODES[] = {
    "VAL_UNKNOWN_KEY",           // Unknown configuration key
    "VAL_MISSING_REQUIRED",      // Missing required configuration key
    "VAL_INVALID_KEY_FORMAT",    // Invalid key format
    "VAL_INVALID_KEY_TYPE",      // Invalid key type
    "VAL_INVALID_VALUE_TYPE",    // Invalid value type
    "VAL_INVALID_VALUE_FORMAT",  // Invalid value format
    "VAL_INVALID_VALUE_RANGE",   // Invalid value range
    "VAL_INVALID_VALUE_PATTERN", // Invalid value pattern
    "VAL_INVALID_VALUE_LENGTH",  // Invalid value length
    "VAL_VALUE_CONVERSION",      // Value conversion error
    "VAL_CONFIGURATION",         // Configuration error
    "VAL_SCHEMA",                // Schema validation error
    "VAL_FILE",                  // File error
    "VAL_PERMISSION",            // Permission error
    "VAL_SYSTEM",                // System error
    "VAL_TIMEOUT",               // Timeout error
    "VAL_MEMORY",                // Memory error
    "VAL_NETWORK",               // Network error
    "VAL_UNKNOWN"                // Unknown error
};
```

### Error Code Usage

```cpp
// Error code usage example
void handleValidationError(const ValidationError& error) {
    std::string errorCode = error.getErrorCode();
    
    if (errorCode == "VAL_UNKNOWN_KEY") {
        std::cout << "Unknown configuration key. Use --help to see available keys." << std::endl;
    } else if (errorCode == "VAL_MISSING_REQUIRED") {
        std::cout << "Missing required configuration key. Use --help to see required keys." << std::endl;
    } else if (errorCode == "VAL_INVALID_VALUE_RANGE") {
        std::cout << "Value is out of valid range. Please check the help for valid ranges." << std::endl;
    } else {
        std::cout << "Validation error: " << errorCode << std::endl;
    }
}
```

## Validation Testing

### Validation Test Framework

```cpp
// Validation test framework
class ValidationTestFramework {
public:
    ValidationTestFramework();
    ~ValidationTestFramework();
    
    // Test setup
    void addValidationTest(const ValidationTest& test);
    void removeValidationTest(const std::string& testName);
    void clearValidationTests();
    
    // Test execution
    ValidationResult runValidationTests(const std::map<std::string, std::string>& args);
    ValidationResult runSingleTest(const std::string& testName, 
                                  const std::map<std::string, std::string>& args);
    
    // Test reporting
    std::string getTestResults() const;
    size_t getTotalTests() const;
    size_t getPassedTests() const;
    size_t getFailedTests() const;
    double getTestSuccessRate() const;
    
    // Test management
    void saveTestResults(const std::string& filepath) const;
    bool loadTestResults(const std::string& filepath);
    void resetTestResults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

struct ValidationTest {
    std::string name;
    std::string description;
    std::map<std::string, std::string> testArguments;
    ValidationResult expectedResult;
    std::vector<std::string> expectedErrors;
    std::vector<std::string> expectedWarnings;
    
    bool runTest() const;
    std::string getTestResult() const;
    bool isTestPassed() const;
};
```

## Performance Monitoring Integration

### Real-time Validation Monitoring

```cpp
// Real-time validation monitoring
class RealTimeValidationMonitor {
public:
    RealTimeValidationMonitor();
    ~RealTimeValidationMonitor();
    
    // Monitoring setup
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Real-time metrics
    ValidationMetrics getCurrentMetrics() const;
    std::vector<ValidationError> getCurrentErrors() const;
    std::vector<ValidationError> getCurrentWarnings() const;
    
    // Callbacks
    void setMetricsUpdateCallback(std::function<void(const ValidationMetrics&)> callback);
    void setErrorCallback(std::function<void(const ValidationError&)> callback);
    void setWarningCallback(std::function<void(const ValidationError&)> callback);
    
    // Real-time display
    void displayMetrics() const;
    void displayErrors() const;
    void displayWarnings() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Validation Integration Summary

### Key Integration Features

1. **Comprehensive Validation**: Validate all command-line arguments against defined rules
2. **Detailed Error Messages**: Clear, actionable error messages with recovery suggestions
3. **Required Key Validation**: Ensure all required configuration keys are present
4. **Interactive Help**: Interactive help system for validation errors
5. **Recovery System**: Auto-recovery suggestions and interactive recovery options
6. **Performance Monitoring**: Real-time validation performance monitoring
7. **Configurable Validation**: Customizable validation rules and thresholds
8. **Error Code System**: Standardized error codes for error handling
9. **Integration with Configuration**: Seamless integration with configuration system

### Integration Benefits

1. **Improved User Experience**: Clear error messages and recovery suggestions
2. **Enhanced Reliability**: Comprehensive validation prevents configuration errors
3. **Better Performance**: Optimized validation with caching and performance monitoring
4. **Maintainability**: Configurable validation rules and error handling
5. **Debugging Support**: Detailed error messages and recovery suggestions
6. **Production Readiness**: Robust validation for production environments
7. **Development Support**: Comprehensive validation during development
8. **Documentation**: Integrated help and documentation system

### Integration Requirements

1. **Type Safety**: Type-safe validation of configuration values
2. **Performance**: Fast validation with minimal overhead
3. **Extensibility**: Extensible validation rule system
4. **Usability**: User-friendly error messages and recovery options
5. **Reliability**: Robust error handling and recovery mechanisms
6. **Compatibility**: Compatibility with existing configuration system
7. **Monitoring**: Comprehensive performance monitoring and metrics
8. **Documentation**: Complete documentation and examples

### Integration Examples

```cpp
// Basic integration
#include "core/Config/Config.hpp"

void setupCLIValidation() {
    Config config;
    config.setValidationEnabled(true);
    config.setValidationLevel(ValidationLevel::Warning);
    
    // Validate command line arguments
    ValidationResult result = config.validateFromArgs(argc, argv);
    
    if (!result.isValid) {
        // Handle validation errors
        return 1;
    }
    
    // Process validated arguments
    config.processCommandLineArguments(argc, argv);
    return 0;
}

// Advanced integration with recovery
#include "core/Config/Config.hpp"
#include "core/InteractiveValidator.hpp"

int main(int argc, char** argv) {
    Config config;
    InteractiveValidator validator;
    
    // Setup validation with recovery
    config.setValidationEnabled(true);
    config.setValidationLevel(ValidationLevel::Error);
    config.setRecoveryEnabled(true);
    
    // Validate arguments
    ValidationResult result = config.validateFromArgs(argc, argv);
    
    if (!result.isValid) {
        // Show interactive error handling
        validator.showValidationError(result.errors[0]);
        
        if (config.isRecoveryEnabled()) {
            bool recovered = validator.applyAutoRecovery(result.errors[0]);
            if (recovered) {
                // Retry validation
                ValidationResult recoveryResult = config.validateFromArgs(argc, argv);
                if (recoveryResult.isValid) {
                    return 0;  // Success
                }
            }
        }
        
        // Show validation summary
        validator.showValidationSummary(result);
        return 1;
    }
    
    return 0;
}
```

## Conclusion

Command-line configuration validation provides comprehensive validation for configuration values provided via command-line arguments. This system ensures:

1. **Type Safety**: Type-safe validation of configuration values
2. **Comprehensive Validation**: Validation for all command-line arguments
3. **Detailed Error Messages**: Clear, actionable error messages
4. **Recovery System**: Auto-recovery suggestions and interactive recovery options
5. **Performance Optimization**: Optimized validation with caching and performance monitoring
6. **User Experience**: Interactive help and recovery options
7. **Integration**: Seamless integration with configuration system
8. **Extensibility**: Configurable validation rules and error handling

The command-line configuration validation system enhances the user experience, improves reliability, and provides comprehensive error handling for command-line configuration in the NLM neuromorphic neural network simulation.

## References

- Command-Line Interface Design
- Configuration Management Best Practices
- Validation System Architecture
- Error Handling and Recovery
- Performance Optimization
- User Experience Design
- Interactive Systems
- Configuration Testing

## See Also

- [`Config.hpp`](/app/src/core/Config/Config.hpp) - Configuration header
- [`Config.cpp`](/app/src/core/Config/Config.cpp) - Configuration implementation
- [`Logger.hpp`](/app/src/core/Logger/Logger.hpp) - Enhanced logging header
- [`Brain.cpp`](/app/src/brain/Brain.cpp) - Enhanced brain implementation
- [test_config.cpp](/app/tests/test_config.cpp) - Configuration test suite
- [`CLIValidator.hpp`](/app/src/core/CLIValidator.hpp) - Command-line validation header
- [`ValidationHelp.hpp`](/app/src/core/ValidationHelp.hpp) - Validation help header
- [`PerformanceMonitor.hpp`](/app/src/core/PerformanceMonitor.hpp) - Performance monitoring header
- [`RecoverySystem.hpp`](/app/src/core/RecoverySystem.hpp) - Recovery system header
- [`InteractiveValidator.hpp`](/app/src/core/InteractiveValidator.hpp) - Interactive validation header