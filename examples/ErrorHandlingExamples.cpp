#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"
#include "Validation.hpp"
#include "ErrorRecovery.hpp"
#include "Logger.hpp"
#include <iostream>
#include <vector>
#include <string>

// Example 1: Basic error usage
void exampleBasicErrorUsage() {
    std::cout << "=== Example 1: Basic Error Usage ===" << std::endl;
    
    try {
        // Throw a specific error type
        throw ValidationError("Invalid input value", "param1", __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::report(e);
        std::cout << "Caught error: " << e.formatMessage() << std::endl;
    }
    
    try {
        // Throw memory error
        throw MemoryError("Out of memory when allocating 1MB", 1048576, __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::report(e);
        std::cout << "Caught memory error: " << e.formatMessage() << std::endl;
    }
    
    try {
        // Throw configuration error
        throw ConfigurationError("Configuration file not found", "config.json", __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::report(e);
        std::cout << "Caught config error: " << e.formatMessage() << std::endl;
    }
}

// Example 2: Parameter validation framework
void exampleParameterValidation() {
    std::cout << "\n=== Example 2: Parameter Validation Framework ===" << std::endl;
    
    nlm::ParameterValidator validator;
    
    // Register validation functions for different types
    validator.registerValidator<int>("int_type", [](int value, const std::string& context) {
        nlm::ValidationResult result;
        if (value < 0) {
            result.passed = false;
            nlm::ValidationErrorDetail error("Value cannot be negative", "input_value");
            error.severity = nlm::ValidationSeverity::Error;
            error.suggestedFix = "Provide a non-negative value";
            result.errors.push_back(error);
        }
        return result;
    });
    
    validator.registerValidator<std::string>("string_type", [](const std::string& value, const std::string& context) {
        nlm::ValidationResult result;
        if (value.empty()) {
            result.passed = false;
            nlm::ValidationErrorDetail error("Value cannot be empty", "input_value");
            error.severity = nlm::ValidationSeverity::Warning;
            error.suggestedFix = "Provide a non-empty string";
            result.warnings.push_back(error);
        }
        return result;
    });
    
    // Validate parameters
    std::map<std::string, std::any> params;
    params["count"] = 5;
    params["name"] = std::string("");
    
    auto results = validator.validateAll(params, "test_operation");
    for (const auto& result : results) {
        if (result.hasErrors()) {
            std::cout << "Validation ERROR: " << result.toString() << std::endl;
        }
        if (result.hasWarnings()) {
            std::cout << "Validation WARNING: " << result.toString() << std::endl;
        }
    }
}

// Example 3: Configuration validation
void exampleConfigurationValidation() {
    std::cout << "\n=== Example 3: Configuration Validation ===" << std::endl;
    
    nlm::ConfigValidator validator;
    
    // Add validation rules
    nlm::ConfigValidator::ConfigValidationRule intRule;
    intRule.key = "learning_rate";
    intRule.description = "Learning rate for neural network training";
    intRule.type = "float";
    intRule.required = true;
    intRule.minValue = "0.0";
    intRule.maxValue = "1.0";
    validator.addRule(intRule);
    
    nlm::ConfigValidator::ConfigValidationRule stringRule;
    stringRule.key = "model_name";
    stringRule.description = "Name of the neural network model";
    stringRule.type = "string";
    stringRule.required = true;
    stringRule.allowedValues = "simple,complex,advanced";
    validator.addRule(stringRule);
    
    // Test configuration
    std::map<std::string, std::string> config;
    config["learning_rate"] = "0.05"; // Valid
    config["model_name"] = "simple";  // Valid
    
    auto result = validator.validate(config);
    std::cout << "Config validation result: " << result.toString() << std::endl;
    
    // Test with invalid configuration
    std::map<std::string, std::string> invalidConfig;
    invalidConfig["learning_rate"] = "2.0"; // Invalid - exceeds max
    invalidConfig["model_name"] = "unknown"; // Invalid - not in allowed list
    
    auto invalidResult = validator.validate(invalidConfig);
    std::cout << "Invalid config validation result: " << invalidResult.toString() << std::endl;
}

// Example 4: Error recovery mechanisms
void exampleErrorRecovery() {
    std::cout << "\n=== Example 4: Error Recovery Mechanisms ===" << std::endl;
    
    nlm::RetryConfig retryConfig;
    retryConfig.maxRetries = 3;
    retryConfig.initialDelay = std::chrono::milliseconds(100);
    
    // Operation that might fail
    auto operation = []() {
        static int attempts = 0;
        attempts++;
        if (attempts < 3) {
            throw nlm::ValidationError("Temporary failure", "test");
        }
        std::cout << "Operation succeeded!" << std::endl;
    };
    
    // Try with retry mechanism
    auto result = nlm::ErrorRecovery::executeWithRetry(operation, retryConfig, "Test Operation");
    std::cout << "Retry result: " << nlm::ErrorFormatter::formatRecoveryResult(result) << std::endl;
    
    // Operation with fallback
    auto mainOperation = []() {
        throw nlm::PerformanceError("Performance threshold exceeded", 100.0, "cpu_usage");
    };
    
    auto fallback1 = []() { std::cout << "Fallback 1 executed" << std::endl; };
    auto fallback2 = []() { std::cout << "Fallback 2 executed" << std::endl; };
    
    auto degradationResult = nlm::ErrorRecovery::executeWithDegradation(
        mainOperation, {fallback1, fallback2}, "Performance Operation");
    
    std::cout << "Degradation result: " << nlm::ErrorFormatter::formatRecoveryResult(degradationResult) << std::endl;
}

// Example 5: Integration with existing logging
void exampleLoggingIntegration() {
    std::cout << "\n=== Example 5: Integration with Existing Logging ===" << std::endl;
    
    // Configure logger
    auto& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Debug);
    
    // Create a console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
    logger.addLogger(consoleLogger);
    
    // Demonstrate different error severities
    std::cout << "Throwing different severity errors:" << std::endl;
    
    try {
        throw nlm::ValidationError("This is a validation error", "param", __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::logException(e, nlm::ErrorSeverity::Error);
    }
    
    try {
        throw nlm::MemoryError("Out of memory", 1024, __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::logException(e, nlm::ErrorSeverity::Critical);
    }
    
    try {
        throw nlm::ConfigurationError("Invalid configuration", "key", __FILE__, __LINE__, __func__);
    } catch (const NLMException& e) {
        nlm::Exception::logException(e, nlm::ErrorSeverity::Warning);
    }
    
    std::cout << "\nDemonstrating structured error reporting:" << std::endl;
    
    // Create multiple errors and report them in batch
    std::vector<std::unique_ptr<nlm::NLMException>> errors;
    errors.push_back(std::make_unique<nlm::ValidationError>("Validation failed", "input1"));
    errors.push_back(std::make_unique<nlm::MemoryError>("Memory allocation failed", 512));
    errors.push_back(std::make_unique<nlm::ConfigurationError>("Config file missing", "config.json"));
    
    nlm::ErrorUtilities::reportBatch(errors);
}

// Example 6: User-friendly error reporting
void exampleUserFriendlyReporting() {
    std::cout << "\n=== Example 6: User-Friendly Error Reporting ===" << std::endl;
    
    try {
        // Simulate various errors
        throw nlm::ValidationError("Invalid value for parameter 'learning_rate'", "learning_rate");
    } catch (const nlm::NLMException& e) {
        std::cout << "\nHuman-readable error report:" << std::endl;
        std::cout << nlm::ErrorFormatter::format(e, true) << std::endl;
    }
    
    try {
        throw nlm::MemoryError("Out of memory when allocating neural network weights", 10485760);
    } catch (const nlm::NLMException& e) {
        std::cout << "\nMemory error report:" << std::endl;
        std::cout << nlm::ErrorFormatter::format(e) << std::endl;
    }
    
    try {
        throw nlm::SimulationError("Network disconnected during training", "network_42");
    } catch (const nlm::NLMException& e) {
        std::cout << "\nSimulation error report:" << std::endl;
        std::cout << nlm::ErrorFormatter::format(e) << std::endl;
    }
}

// Example 7: Safe error handling utilities
void exampleSafeErrorHandling() {
    std::cout << "\n=== Example 7: Safe Error Handling Utilities ===" << std::endl;
    
    // Safe exception throwing
    nlm::ErrorUtilities::throwIf([](int x) { return x < 0; }, 
                                 nlm::ValidationError, "Value cannot be negative", "value");
    
    std::cout << "Safe check passed (value = 5)" << std::endl;
    
    try {
        nlm::ErrorUtilities::throwIf([](int x) { return x < 0; },
                                      nlm::ValidationError, "Value cannot be negative", "value");
    } catch (const nlm::NLMException& e) {
        std::cout << "Caught expected error: " << e.getMessage() << std::endl;
    }
    
    // Try execute with exception handling
    auto riskyOperation = []() -> int {
        throw std::runtime_error("Random failure");
    };
    
    auto result = nlm::ErrorUtilities::tryExecute(riskyOperation, "Risky Operation");
    if (!result) {
        std::cout << "Operation failed as expected" << std::endl;
    }
    
    // Wrap standard exception
    try {
        throw std::invalid_argument("Invalid argument provided");
    } catch (const std::exception& e) {
        auto nlmException = nlm::ErrorUtilities::wrapException(e);
        nlm::Exception::report(*nlmException);
    }
}

int main() {
    // Initialize logging
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    std::cout << "NLM Error Handling System Examples" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    exampleBasicErrorUsage();
    exampleParameterValidation();
    exampleConfigurationValidation();
    exampleErrorRecovery();
    exampleLoggingIntegration();
    exampleUserFriendlyReporting();
    exampleSafeErrorHandling();
    
    std::cout << "\n=== All Examples Completed ===" << std::endl;
    
    return 0;
}
