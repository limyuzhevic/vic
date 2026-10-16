// **NLM Configuration Validation Documentation**
// 
// This document provides comprehensive examples for using the NLM configuration validation system.
// 
// 
// ## Overview
// 
// The NLM configuration validation system provides robust validation for configuration values
// with support for type checking, range validation, enum validation, format validation, and
cross-field validation. It integrates seamlessly with the existing Config class while
maintaining backward compatibility.
// 
// 
// ## Quick Start
// 
// ### 1. Basic Validation Setup
// 
// ```cpp
// #include "core/Config/Config.hpp"
// #include "core/Config/ConfigValidator.hpp"
// 
// void setupValidation(Config& config) {
//     // Create a validator with strict mode
//     auto validator = std::make_unique<NLMConfigValidator>(ValidationMode::Strict);
//     
//     // Set the validator on the config object
//     config.setValidator(std::move(validator));
// }
// ```
// 
// ### 2. Basic Validation Usage
// 
// ```cpp
// void configureSystem(Config& config) {
//     // This will work - within valid ranges
//     config.set("neuron_count", static_cast<int64_t>(1000));
//     config.set("simulation_timestep", 0.001);
//     
//     try {
//         // This will throw ValidationException
//         config.set("neuron_count", static_cast<int64_t>(0)); // Invalid: below minimum
//     } catch (const ValidationException& e) {
//         std::cerr << "Validation error: " << e.what() << std::endl;
//     }
// }
// ```
// 
// 
// ## Features
// 
// ### 1. Type Checking
// Validates that configuration values are of the correct type.
// 
// **Supported types:**
// - `int64_t` for integer values
// - `double` for floating point values  
// - `bool` for boolean values
// - `std::string` for text values
// 
// **Example:**
// ```cpp
// config.set("learning_rate", 0.01); // Valid double
// // config.set("learning_rate", "0.01"); // Would be string type, not double
// ```
// 
// 
// ### 2. Range Validation
// Enforces minimum and maximum values for numeric parameters.
// 
// **Built-in ranges:**
// - `neuron_count`: [1, 1,000,000]
// - `simulation_timestep`: [1e-6, 1.0]
// - `region_count`: [1, 100]
// - `connection_probability`: [0.0, 1.0]
// 
// **Example:**
// ```cpp
// // Valid range
// config.set("connection_probability", 0.15);
// 
// try {
//     // Invalid range - will throw
//     config.set("connection_probability", 1.5);
// } catch (const ValidationException& e) {
//     std::cerr << "Error: " << e.what() << std::endl;
// }
// ```
// 
// 
// ### 3. Enum Validation
// Restricts values to a predefined set of allowed values.
// 
// **Example:**
// ```cpp
// // Create custom validator with enum constraints
// class CustomValidator : public ConfigValidator {
// public:
//     ConfigSchema getDefaultSchema() const override {
ConfigSchema schema;
//         // Add enum constraint for a field
//         ConfigFieldSchema field("mode", "Processing mode");
//         field.addConstraint(std::make_shared<EnumConstraint>(
//             {"fast", "accurate", "balanced"}
//         ));
//         schema.addField(field);
//         return schema;
//     }
// };
// 
// auto customValidator = std::make_unique<CustomValidator>(ValidationMode::Strict);
// config.setValidator(std::move(customValidator));
// 
// config.set("mode", "accurate"); // Valid
// // config.set("mode", "unknown"); // Invalid - not in enum
// ```
// 
// 
// ### 4. Format Validation
// Validates string values against regular expressions.
// 
// **Example:**
// ```cpp
// // Validate email format
// ConfigFieldSchema emailField("email", "Email address");
// emailField.addConstraint(std::make_shared<FormatConstraint>(
//     R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})$" // Email regex
// ));
// 
// // Create custom validator
// class EmailValidator : public ConfigValidator {
// public:
//     ConfigSchema getDefaultSchema() const override {
//         ConfigSchema schema;
//         schema.addField(emailField);
//         return schema;
//     }
// };
// ```
// 
// 
// ### 5. Required Field Validation
// Ensures that critical fields are present.
// 
// **Example:**
// ```cpp
// // Field is required by default
// ConfigFieldSchema requiredField("api_key", "API authentication key");
// requiredField.addConstraint(std::make_shared<RequiredConstraint>());
// 
// // Optional field
// ConfigFieldSchema optionalField("backup_path", "Path for backup files", true);
// optionalField.addConstraint(std::make_shared<RequiredConstraint>(true));
// ```
// 
// 
// ## Validation Modes
// 
// The validation system supports three modes:
// 
// ### 1. Strict Mode (Default)
// - Throws `ValidationException` on any validation error
// - Suitable for production environments
// - Ensures configuration integrity
// 
// ```cpp
// auto validator = std::make_unique<NLMConfigValidator>(ValidationMode::Strict);
// config.setValidator(std::move(validator));
// ```
// 
// ### 2. Warning Mode
// - Logs warnings for validation errors
// - Allows invalid configuration to be set
// - Useful for development and testing
// 
// ```cpp
// auto validator = std::make_unique<NLMConfigValidator>(ValidationMode::Warning);
// config.setValidator(std::move(validator));
// 
// // Warnings will be logged, but configuration is still set
// config.set("neuron_count", 0); // Logs warning, but sets the value
// ```
// 
// ### 3. Off Mode
// - No validation performed
// - Maximum flexibility
// - Suitable when validation is not needed
// 
// ```cpp
// auto validator = std::make_unique<ConfigValidator>(ValidationMode::Off);
// config.setValidator(std::move(validator));
// ```
// 
// 
// ## Integration Examples
// 
// ### 1. Integrating with Existing Config Code
// 
// ```cpp
// void initializeSystem(Config& config) {
//     // Setup validation
//     auto validator = std::make_unique<NLMConfigValidator>(ValidationMode::Strict);
//     config.setValidator(std::move(validator));
//     
//     // Load from file (with validation)
//     config.loadFromFile("config.cfg");
//     
//     // Override with command line (with validation)
//     config.loadFromArgs(argc, argv);
//     
//     // Set additional configuration
//     config.set("learning_rate", 0.01);
//     
//     // Validate all configuration
//     ValidationResult result = config.validate();
//     if (!result.isValid()) {
//         std::cerr << "Configuration validation failed:" << std::endl;
//         std::cerr << result.toString();
//         return 1;
//     }
//     
//     // Check for warnings
//     if (!result.warnings.empty()) {
//         for (const auto& warning : result.warnings) {
//             std::cout << "Warning: " << warning << std::endl;
//         }
//     }
// }
// ```
// 
// 
// ### 2. Custom Validation Schema
// 
// ```cpp
// class CustomNLMValidator : public NLMConfigValidator {
// public:
//     CustomNLMValidator(ValidationMode mode = ValidationMode::Strict) 
//         : NLMConfigValidator(mode) {
//         // Extend or modify the default schema
//         extendSchema();
//     }
//     
// private:
//     void extendSchema() {
//         // Add custom fields to the schema
//         ConfigFieldSchema field("custom_param", "Custom parameter");
//         field.addConstraint(std::make_shared<RangeConstraint>(0, 100, "Must be 0-100"));
//         
//         // Add cross-field validation
//         field.addConstraint(std::make_shared<CustomConstraint>(
//             [](const ConfigValue& value, ValidationResult& result) {
//                 if (value.has_value<double>() && value.value<double>() > 50) {
//                     // Check related field
//                     // (would need access to other fields in a real implementation)
//                     if (someCondition) {
//                         result.addWarning("High value may affect system stability");
//                     }
//                 }
//                 return true;
//             }
//         ));
//         
//         // Add to schema (this would need to override the schema modification method)
//     }
// };
// ```
// 
// 
// ## Error Handling
// 
// The validation system provides detailed error messages for debugging.
// 
// ### Exception Types
// - `ValidationException`: Base exception type
// - `RequiredFieldError`: For missing required fields
// - `TypeMismatchError`: For type mismatches
// - `RangeValidationError`: For out-of-range values
// - `EnumValidationError`: For invalid enum values
// - `FormatValidationError`: For format validation failures
// 
// ### Error Message Examples
// ```
// Configuration validation failed:
//   - Field "neuron_count" has type "int64_t", expected "double"
//   - Field "simulation_timestep" value 0.0001 violates constraint: Range [1e-06, 1.0]
//   - Field "connection_probability" value "high" is not in allowed values: fast, accurate, balanced
// ```
// 
// 
// ## Validation Result API
// 
// The `ValidationResult` structure provides comprehensive validation feedback.
// 
// ```cpp
// ValidationResult result = config.validate();
// 
// if (!result.isValid()) {
//     for (const auto& error : result.errors) {
//         std::cerr << "ERROR: " << error->what() << std::endl;
//     }
// }
// 
// for (const auto& warning : result.warnings) {
//     std::cout << "WARNING: " << warning << std::endl;
// }
// ```
// 
// 
// ## Performance Considerations
// 
// - Validation is performed at configuration modification time
// - The validation schema is cached for efficient lookup
// - In Off mode, there is minimal overhead
// - In Strict mode, validation adds some overhead but ensures integrity
// 
// 
// ## Migration Guide
// 
// ### From Legacy Config Usage to Validated Config
// 
// **Legacy code:**
// ```cpp
// Config config;
// config.set("neuron_count", 100);
// ```
// 
// **Upgraded with validation:**
// ```cpp
// Config config;
// auto validator = std::make_unique<NLMConfigValidator>(ValidationMode::Strict);
// config.setValidator(std::move(validator));
// 
// config.set("neuron_count", 100); // Now validated!
// ```
// 
// The upgrade is backward compatible - existing code continues to work
// but with additional validation safety.
