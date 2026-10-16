# Configuration System Validation Features

## Overview

This document describes the enhanced configuration management system with validation capabilities added to the NLM (Neural Learning Machine) project.

## Background

The original configuration system in `src/core/Config/` provided basic configuration loading from files and command line arguments, but lacked validation capabilities. The configuration values could be loaded and retrieved, but there was no mechanism to ensure they met specific requirements or constraints.

## New Features

### 1. Configuration Schema Validation

#### ConfigSchema Class

The `ConfigSchema` class in `src/core/Config/ConfigSchema.hpp` allows defining configuration validation rules:

**Validation Types:**
- `None`: No validation
- `Range`: Value must be within a specified range (min/max)
- `Enum`: Value must be one of allowed enumerated values
- `Format`: Value must match a format pattern (e.g., wildcard patterns)
- `Required`: Field must be present in the configuration
- `Dependency`: Field presence depends on other fields
- `Custom`: Custom validation function

**Validation Severities:**
- `Warning`: Non-blocking, just a warning
- `Error`: Blocking, prevents configuration from being valid
- `Critical`: Critical error, prevents system from functioning

**Usage Example:**
```cpp
nlm::ConfigSchema schema;
schema.addRangeRule("temperature", -50.0, 150.0, 
                   nlm::ValidationSeverity::Error,
                   "Temperature must be between -50 and 150°C");
schema.addEnumRule("environment_type", {"indoor", "outdoor", "marine", "aerospace"},
                  nlm::ValidationSeverity::Warning, "Recommended environment types");
```

### 2. Enhanced Config Class

The `Config` class now includes validation capabilities while maintaining full backward compatibility:

**New Methods:**
- `validate(const ConfigSchema& schema, bool isStrict = true) const`: Validate configuration against a schema
- `validateKey(const std::string& key, const ConfigSchema& schema, bool isStrict = true) const`: Validate individual key
- `isValid(const ConfigSchema& schema, bool isStrict = true) const`: Quick validation check
- `getValidationErrors(const ConfigSchema& schema, bool isStrict = true) const`: Get validation error details
- `clearValidationCache()`: Clear validation cache
- `invalidateValidationCache()`: Invalidate validation cache

**New Features:**
- Configuration merging and inheritance (`merge()`)
- Configuration sanitization and normalization (`sanitize()`)
- Schema management (`setSchema()`, `getSchema()`)
- Validation modes (`ValidationMode::Strict`, `ValidationMode::Lenient`, `ValidationMode::Warnings`)

**Usage Example:**
```cpp
nlm::Config config;
config.loadFromFile("configs/default.cfg");

nlm::ConfigSchema schema;
schema.addRangeRule("steps", 0, 1000000);
schema.addEnumRule("experiment_type", {"learning", "plasticity", "stdp"});

if (config.validate(schema)) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    auto errors = config.getValidationErrors(schema);
    for (const auto& error : errors) {
        std::cout << "Error in " << error.fieldName << ": " << error.message << std::endl;
    }
}
```

### 3. Validation Engine

The `ConfigValidator` class provides the core validation logic:

**Key Features:**
- **Range validation**: Validates numeric values against min/max constraints
- **Enum validation**: Validates string values against allowed lists
- **Format validation**: Validates string values against pattern matching
- **Dependency validation**: Validates field dependencies and relationships
- **Custom validation**: Supports user-defined validation functions
- **Performance**: Validation caching for improved performance

**Validation Severity Handling:**
- Critical errors prevent system operation
- Error violations block configuration from being considered valid
- Warning violations are reported but don't block operation

### 4. Integration with Existing System

The validation system integrates seamlessly with the existing NLM architecture:

**Backward Compatibility:**
- All existing Config API methods remain unchanged
- Existing code using configuration loading continues to work
- New validation features are opt-in through method calls

**ConfigManager Integration:**
- Configuration validation can be applied to system-wide settings
- Validation can be performed during system initialization
- Configuration errors can be reported during option processing

## Validation Examples

### Example 1: Temperature Range Validation

```cpp
nlm::ConfigSchema sensorSchema;
sensorSchema.addRangeRule("temperature", -40.0, 60.0, 
                        nlm::ValidationSeverity::Error,
                        "Temperature out of safe operating range");

nlm::Config config;
config.set("temperature", 75.0, nlm::ConfigSource::Runtime);

if (!config.validate(sensorSchema)) {
    auto errors = config.getValidationErrors(sensorSchema);
    for (const auto& error : errors) {
        std::cout << "Sensor error: " << error.message << std::endl;
    }
}
```

### Example 2: Enum Validation for Device Types

```cpp
nlm::ConfigSchema deviceSchema;
deviceSchema.addEnumRule("device_type", {"sensor", "controller", "actuator"},
                       nlm::ValidationSeverity::Warning, "Invalid device type");

nlm::Config config;
config.set("device_type", std::string("unknown"), nlm::ConfigSource::Runtime);

if (!config.validate(deviceSchema)) {
    std::cout << "Device validation failed!" << std::endl;
}
```

### Example 3: Dependency Validation

```cpp
nlm::ConfigSchema dependencySchema;
dependencySchema.addDependencyRule("password", "username", 
                                  true, // required when username is present
                                  nlm::ValidationSeverity::Error,
                                  "Password required when username is provided");

nlm::Config config;
config.set("username", std::string("admin"), nlm::ConfigSource::Runtime);

if (!config.validate(dependencySchema)) {
    std::cout << "Dependency validation failed!" << std::endl;
}
```

## Configuration Validation Demo

A demonstration program `demo_validation.cpp` shows all the validation features in action:

**Features Demonstrated:**
- Loading configuration from file
- Creating and applying validation schemas
- Range validation
- Enum validation
- Required field validation
- Validation caching
- Different validation modes (Strict, Lenient, Warnings)
- Schema merging

Run with: `nlm demo_validation` (after compilation)

## Testing

The validation features are thoroughly tested in `tests/test_config.cpp`:

**Test Cases:**
- Configuration creation and basic operations
- Value setting and retrieval
- Range validation
- Enum validation
- Required field validation
- Validation caching
- Different validation modes

Run tests with: `make test` or `ctest`

## Performance Considerations

**Validation Caching:**
- Validations are cached to improve performance
- Cache is invalidated when configuration changes
- Cache can be cleared manually if needed

**Strict vs Lenient Modes:**
- Strict mode: All violations are errors
- Lenient mode: Only critical violations are errors
- Warnings mode: Only warnings, no blocking

## Error Handling

**ValidationError Structure:**
- `fieldName`: Name of the field being validated
- `path`: Path in nested configuration (for future use)
- `message`: Human-readable error message
- `severity`: Error severity level
- `timestamp`: When the error occurred
- `code`: Error code for programmatic handling

**Error Reporting:**
- Errors are sorted by severity (Critical > Error > Warning)
- Detailed error messages include field names and validation rules
- Error codes can be used for programmatic error handling

## Configuration Best Practices

**When to Use Validation:**
- Validate critical system parameters (range, type, required fields)
- Validate user-provided configuration values
- Validate API configuration parameters

**Schema Design Tips:**
- Use descriptive field names
- Provide clear error messages
- Consider validation severity levels
- Use dependency validation for complex configurations

**Performance Tips:**
- Cache frequently used schemas
- Use lenient mode for non-critical validations
- Clear validation cache when configuration changes significantly

## Future Enhancements

**Planned Features:**
- JSON/YAML schema support
- Nested configuration validation
- Schema versioning
- Validation plugins
- Real-time configuration monitoring

## Summary

The enhanced configuration system provides:

1. **Comprehensive Validation**: Multiple validation types with detailed error reporting
2. **Backward Compatibility**: All existing functionality remains unchanged
3. **Performance**: Validation caching and efficient validation engine
4. **Flexibility**: Support for custom validation functions
5. **Integration**: Seamless integration with existing NLM architecture
6. **Testing**: Thorough test coverage for all validation features

The configuration validation system makes NLM more robust and easier to configure, with better error reporting and validation of system parameters.