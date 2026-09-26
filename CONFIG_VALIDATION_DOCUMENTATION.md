# Comprehensive Configuration Validation and Error Handling Documentation

## Overview

This document provides comprehensive documentation for the enhanced configuration validation and error handling system in the NLM project. The system provides robust validation, error reporting, and automatic repair capabilities for NLM configurations.

## Exception Hierarchy

### Base Exception: NLMException
**Description:** Base exception class for all NLM-specific errors.

```cpp
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message);
    virtual ~NLMException() = default;
    virtual const char* type() const noexcept { return "NLMException"; }
};
```

### Configuration Exceptions

#### ConfigError
**Description:** Base class for all configuration-related errors.

```cpp
class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message);
    virtual const char* type() const noexcept { return "ConfigError"; }
};
```

#### ConfigValidationError
**Description:** Thrown when configuration validation fails.

```cpp
class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& key, const std::string& message);
    virtual const char* type() const noexcept { return "ConfigValidationError"; }
};
```

#### ConfigSchemaError
**Description:** Thrown when schema validation fails.

```cpp
class ConfigSchemaError : public ConfigError {
public:
    explicit ConfigSchemaError(const std::string& message);
    virtual const char* type() const noexcept { return "ConfigSchemaError"; }
};
```

#### ConfigMergeError
**Description:** Thrown when configuration merging fails.

```cpp
class ConfigMergeError : public ConfigError {
public:
    explicit ConfigMergeError(const std::string& message);
    virtual const char* type() const noexcept { return "ConfigMergeError"; }
};
```

### Integration Exceptions

#### IntegrationError
**Description:** Base class for integration-related errors.

```cpp
class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message);
    virtual const char* type() const noexcept { return "IntegrationError"; }
};
```

#### BrainInitializationError
**Description:** Thrown when brain initialization fails.

```cpp
class BrainInitializationError : public IntegrationError {
public:
    explicit BrainInitializationError(const std::string& message);
    virtual const char* type() const noexcept { return "BrainInitializationError"; }
};
```

#### MemorySystemError
**Description:** Thrown when memory system integration fails.

```cpp
class MemorySystemError : public IntegrationError {
public:
    explicit MemorySystemError(const std::string& message);
    virtual const char* type() const noexcept { return "MemorySystemError"; }
};
```

### Memory Exceptions

#### MemoryError
**Description:** Base class for memory-related errors.

```cpp
class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message);
    virtual const char* type() const noexcept { return "MemoryError"; }
};
```

#### PlasticityError
**Description:** Thrown when plasticity rule failures occur.

```cpp
class PlasticityError : public MemoryError {
public:
    explicit PlasticityError(const std::string& message);
    virtual const char* type() const noexcept { return "PlasticityError"; }
};
```

#### SynapticError
**Description:** Thrown when synaptic errors occur.

```cpp
class SynapticError : public MemoryError {
public:
    explicit SynapticError(const std::string& message);
    virtual const char* type() const noexcept { return "SynapticError"; }
};
```

## Validation System

### ValidationResult Structure
**Description:** Contains validation results with pass/fail status and detailed messages.

```cpp
struct ValidationResult {
    bool isValid;
    std::vector<ConfigValidationError> errors;
    std::vector<std::string> warnings;
    std::map<std::string, double> performanceMetrics;
    std::chrono::milliseconds validationTime;
    
    ValidationResult();
    
    void addError(const std::string& key, const std::string& message, ConfigSource source = ConfigSource::Runtime);
    void addWarning(const std::string& warning);
    bool hasErrors() const;
    bool hasWarnings() const;
    std::string getSummary() const;
    void clear();
};
```

### ConfigSchema System
**Description:** Provides schema-based validation for configuration values.

#### ValidationRule Structure
```cpp
struct ValidationRule {
    std::string key;
    std::type_index type;
    std::string validationPattern;
    std::variant<int, double> minValue;
    std::variant<int, double> maxValue;
    bool required;
    std::string defaultValue;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ValidationRule();
    ValidationRule(const std::string& key, const std::type_index& type);
};
```

#### Core Validation Methods

**validate()**
```cpp
ValidationResult Config::validate() const;
```
Validates configuration against basic rules and built-in schema.

**validateAgainstSchema()**
```cpp
ValidationResult Config::validateAgainstSchema(const ConfigSchema& schema) const;
```
Validates configuration against a specific schema.

**getValidationErrors()**
```cpp
std::vector<ConfigValidationError> Config::getValidationErrors() const;
```
Returns all validation errors at once.

**fixCommonIssues()**
```cpp
bool Config::fixCommonIssues();
```
Automatically fixes common configuration problems.

**mergeConfigs()**
```cpp
bool Config::mergeConfigs(const Config& other, ConfigSource source = ConfigSource::Runtime);
```
Merges multiple configuration sources.

**toPythonDict() and fromPythonDict()**
```cpp
std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> Config::toPythonDict() const;
bool Config::fromPythonDict(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>>& dict);
```
Python interoperability methods.

### ConfigSchema API

**Schema Creation and Configuration:**
```cpp
void ConfigSchema::addRule(const ValidationRule& rule);
void ConfigSchema::addRule(const std::string& key, const std::type_index& type, bool required = false,
                           const std::variant<int, double>& minValue = nullptr, 
                           const std::variant<int, double>& maxValue = nullptr,
                           const std::string& defaultValue = "");
void ConfigSchema::addStringRule(const std::string& key, size_t minLength = 0, size_t maxLength = 0,
                                 const std::string& pattern = "", bool required = false);
void ConfigSchema::addNumericRule(const std::string& key, const std::type_index& type, 
                                   const std::variant<int, double>& minValue = nullptr,
                                   const std::variant<int, double>& maxValue = nullptr,
                                   bool required = false);
void ConfigSchema::addRangeRule(const std::string& key, double minValue, double maxValue,
                                bool required = false);
void ConfigSchema::addListRule(const std::string& key, const std::type_index& elementType,
                               size_t minItems = 0, size_t maxItems = 0, bool required = false);
```

**Schema Validation:**
```cpp
ValidationResult ConfigSchema::validate(const Config& config) const;
bool ConfigSchema::hasRule(const std::string& key) const;
std::vector<ValidationRule> ConfigSchema::getRules() const;
void ConfigSchema::injectDefaults(Config& config) const;
```

**Default Schema:**
```cpp
static ConfigSchema ConfigSchema::getDefaultSchema();
```
Returns a schema with common configuration rules.

## Configuration Health Monitoring

### ConfigHealthMonitor Class
**Description:** Monitors configuration health and provides metrics.

```cpp
class ConfigHealthMonitor {
public:
    struct HealthMetric {
        std::string metricName;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        bool isHealthy;
    };
    
    ConfigHealthMonitor();
    ~ConfigHealthMonitor();
    
    void recordMetric(const std::string& name, double value, const std::string& unit = "", bool isHealthy = true);
    ValidationResult checkHealth() const;
    void addHealthCheck(std::function<ValidationResult()> check);
    
    std::vector<HealthMetric> getMetrics() const;
    double getMetricAverage(const std::string& name) const;
};
```

### Health Check API
```cpp
void Config::enableHealthMonitoring();
void Config::disableHealthMonitoring();
bool Config::isHealthMonitoringEnabled() const;
ValidationResult Config::validateHealth() const;
```

## Brain Integration

### Brain Validation Methods
```cpp
bool Brain::initializeWithValidation();
bool Brain::validateConfiguration() const;
ValidationResult Brain::getConfigurationValidation() const;
bool Brain::isConfigurationHealthy() const;
ValidationResult Brain::getConfigurationHealth() const;
bool Brain::repairConfiguration();
ValidationResult Brain::validateIntegration() const;
ValidationResult Brain::crossValidateConfiguration() const;
std::map<std::string, double> Brain::getValidationStatistics() const;
std::map<std::string, std::chrono::milliseconds> Brain::getValidationPerformanceMetrics() const;
void Brain::enableHealthMonitoring(bool enable = true);
bool Brain::isHealthMonitoringEnabled() const;
```

### Brain Initialization with Validation
The `Brain::initializeWithValidation()` method provides comprehensive validation before brain initialization:

1. **Configuration Validation:** Validates configuration against schema and basic rules
2. **Automatic Repair:** Attempts to fix common issues if validation fails
3. **Re-validation:** Re-validates after repair attempts
4. **Integration Validation:** Validates compatibility between brain systems
5. **Standard Initialization:** Proceeds with original initialization only if all validation passes

## Error Recovery Mechanisms

### Automatic Configuration Repair
The system provides automatic repair for common configuration issues:

1. **Empty Keys:** Automatically generates unique keys for empty configuration keys
2. **Value Range Fixing:** Clamps threshold, rate, and level values to valid ranges
3. **Default Injection:** Injects default values from schema for missing required keys
4. **Type Correction:** Attempts to convert values to appropriate types

### Graceful Degradation
When configurations are invalid, the system:

1. **Reports All Errors:** Provides comprehensive error reporting
2. **Allows Partial Recovery:** Continues with valid portions of configuration
3. **Provides Warnings:** Issues warnings for potentially problematic configurations
4. **Logs Validation Results:** Records validation performance metrics for analysis

## Performance Tracking

### Validation Statistics
The validation system tracks and reports:

```cpp
struct ValidationStatistics {
    std::map<std::string, double> metrics;
    std::map<std::string, std::chrono::milliseconds> performanceMetrics;
    std::chrono::milliseconds totalValidationTime;
    size_t totalValidations;
    size_t failedValidations;
    double successRate;
};
```

### Performance Metrics
- **Validation Time:** Time taken for each validation operation
- **Error Count:** Number of validation errors detected
- **Warning Count:** Number of warnings issued
- **Repair Success Rate:** Percentage of successful automatic repairs
- **Schema Validation Performance:** Time and success rate for schema-based validation

## Error Documentation and Reporting

### Error Message Format
All error messages follow a consistent format:

```
<ConfigError Type>: <Descriptive Message>
Additional context: <Key-specific information>
Possible solutions: <Suggested fixes>
```

### Example Error Messages

#### ConfigValidationError
```
ConfigValidationError: ConfigValidationError for key 'learning_rate': Value must be between 0.0 and 1.0
Additional context: Threshold parameter 'learning_rate' has invalid value 1.5
Possible solutions: Set learning_rate to a value between 0.0 and 1.0, or remove the parameter to use default
```

#### ConfigSchemaError
```
ConfigSchemaError: ConfigSchemaError: Required configuration key 'neuron_count' is missing
Additional context: Neural system requires neuron count specification
Possible solutions: Add 'neuron_count' parameter to configuration, or ensure configuration is complete
```

#### BrainInitializationError
```
BrainInitializationError: BrainInitializationError: Configuration validation failed during brain initialization
Additional context: 2 validation errors found
Possible solutions: Fix configuration errors and retry initialization, or use automatic repair
```

## Configuration Examples

### Basic Validation Usage
```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    // Load configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile("config.cfg");
    
    // Validate configuration
    if (!config->validate()) {
        auto errors = config->getValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error.errorMessage << std::endl;
        }
        // Attempt automatic repair
        config->fixCommonIssues();
        // Re-validate
        if (!config->validate()) {
            std::cerr << "Configuration repair failed" << std::endl;
            return 1;
        }
    }
    
    // Create brain with validation
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initializeWithValidation()) {
        std::cerr << "Brain initialization failed" << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Schema-Based Validation
```cpp
// Create custom schema
ConfigSchema schema;
schema.addRangeRule("learning_rate", 0.0, 1.0, false);
schema.addRangeRule("threshold", 0.0, 1.0, false);
schema.addNumericRule("neuron_count", typeid(size_t), 1, 1000000, true);
schema.addStringRule("model_name", 1, 100, "^[a-zA-Z0-9_-]+$", false);

// Validate against schema
ValidationResult result = config->validateAgainstSchema(schema);
if (!result.isValid) {
    for (const auto& error : result.errors) {
        std::cerr << "Schema Error: " << error.key << ": " << error.errorMessage << std::endl;
    }
}
```

### Health Monitoring
```cpp
// Enable health monitoring
brain->enableHealthMonitoring(true);

// Check configuration health
if (!brain->isConfigurationHealthy()) {
    ValidationResult health = brain->getConfigurationHealth();
    std::cerr << "Configuration health issues:" << std::endl;
    for (const auto& error : health.errors) {
        std::cerr << "  " << error.errorMessage << std::endl;
    }
    for (const auto& warning : health.warnings) {
        std::cerr << "  Warning: " << warning << std::endl;
    }
}
```

## Migration Guide

### From Legacy Configuration System
If migrating from the legacy configuration system:

1. **Update Configuration Format:** Move to key=value format or JSON format
2. **Add Schema Rules:** Define schema rules for your specific configuration
3. **Enable Validation:** Call `validate()` on configuration objects
4. **Gradual Adoption:** Use `fixCommonIssues()` for gradual migration

### Common Issues and Solutions

| Issue | Symptoms | Solution |
|-------|----------|----------|
| Empty Configuration | Validation fails with "Empty key" errors | Use `fixCommonIssues()` to auto-fix |
| Invalid Values | Range validation errors | Set values within valid ranges |
| Missing Keys | Schema validation fails | Add required keys or use default schema |
| Type Mismatches | Conversion errors | Ensure correct value types |
| Integration Failures | Brain initialization fails | Validate integration before initialization |

## Troubleshooting

### Error Code Reference

| Exception Type | Error Code | Description |
|---------------|------------|-------------|
| ConfigValidationError | CV001 | Invalid configuration value |
| ConfigSchemaError | CS001 | Schema validation failed |
| ConfigMergeError | CM001 | Configuration merge failed |
| BrainInitializationError | BI001 | Brain initialization failed |
| MemorySystemError | MS001 | Memory system integration failed |
| PlasticityError | PL001 | Plasticity rule failure |
| SynapticError | SY001 | Synaptic computation error |

### Common Troubleshooting Steps

1. **Check Configuration Files:** Ensure all configuration files are valid and accessible
2. **Validate Configuration:** Run validation before initialization
3. **Review Error Messages:** Examine detailed error messages for specific issues
4. **Use Automatic Repair:** Let the system attempt automatic repair of common issues
5. **Check System Resources:** Ensure sufficient memory and CPU for brain initialization
6. **Review Configuration Schema:** Verify schema rules match your requirements

## Conclusion

The enhanced configuration validation and error handling system provides:

- **Comprehensive Validation:** Multiple validation levels from basic to schema-based
- **Automatic Repair:** Attempts to fix common configuration issues automatically
- **Detailed Error Reporting:** Provides clear, actionable error messages
- **Graceful Degradation:** Continues with valid configurations when possible
- **Performance Tracking:** Monitors and reports validation performance
- **Brain Integration:** Seamlessly integrates with brain initialization and operation
- **Health Monitoring:** Provides ongoing configuration health monitoring

This system ensures robust, reliable, and user-friendly configuration management for the NLM project, making it easier to develop, test, and deploy neural network simulations with confidence in configuration validity.
