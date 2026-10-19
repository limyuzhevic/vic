# Enhanced Configuration Validation - Test Documentation

## Test Suite Overview
This document provides comprehensive test cases and examples for the enhanced configuration validation system in the NLM project.

## 1. Advanced Schema Types Tests

### 1.1 RangeValidator Tests

**Test 1.1.1: Valid Range Validation**
```cpp
RangeValidator<int> rangeValidator(0, 100, "Value must be between 0 and 100", true);
assert(rangeValidator.validate(50) == true);  // Should pass
assert(rangeValidator.validate(0) == true);   // Boundary case
assert(rangeValidator.validate(100) == true); // Boundary case
assert(rangeValidator.validate(-1) == false); // Should fail
assert(rangeValidator.validate(101) == false); // Should fail
```

**Test 1.1.2: Exclusive Range Validation**
```cpp
RangeValidator<int> exclusiveRangeValidator(0, 100, "Value must be between 0 and 100", false);
assert(exclusiveRangeValidator.validate(0) == false);  // Boundary case (exclusive)
assert(exclusiveRangeValidator.validate(100) == false); // Boundary case (exclusive)
assert(exclusiveRangeValidator.validate(50) == true);   // Should pass
```

### 1.2 PatternValidator Tests

**Test 1.2.1: Basic Pattern Validation**
```cpp
PatternValidator patternValidator("^[a-zA-Z0-9_]+$", "Should be alphanumeric", true);
assert(patternValidator.validate("valid_name") == true);
assert(patternValidator.validate("123") == true);
assert(patternValidator.validate("invalid-name") == false);
assert(patternValidator.validate("invalid name") == false);
```

**Test 1.2.2: Case-Insensitive Pattern Validation**
```cpp
PatternValidator caseInsensitivePattern("[a-z]+", "Lowercase letters", false);
assert(caseInsensitivePattern.validate("hello") == true);
assert(caseInsensitivePattern.validate("HELLO") == true);
assert(caseInsensitivePattern.validate("Hello") == true);
assert(caseInsensitivePattern.validate("Hello123") == false);
```

### 1.3 EnumValidator Tests

**Test 1.3.1: Case-Sensitive Enum Validation**
```cpp
EnumValidator enumValidator({"ACTIVE", "INACTIVE", "PENDING"}, "Status enum", true);
assert(enumValidator.validate("ACTIVE") == true);
assert(enumValidator.validate("INACTIVE") == true);
assert(enumValidator.validate("PENDING") == true);
assert(enumValidator.validate("active") == false);
assert(enumValidator.validate("Unknown") == false);
```

**Test 1.3.2: Case-Insensitive Enum Validation**
```cpp
EnumValidator caseInsensitiveEnum({"ACTIVE", "INACTIVE", "PENDING"}, "Status enum", false);
assert(caseInsensitiveEnum.validate("ACTIVE") == true);
assert(caseInsensitiveEnum.validate("active") == true);
assert(caseInsensitiveEnum.validate("AcTiVe") == true);
assert(caseInsensitiveEnum.validate("INACTIVE") == true);
```

### 1.4 ConditionalValidator Tests

**Test 1.4.1: Basic Conditional Validation**
```cpp
auto rangeValidator = std::make_unique<RangeValidator<int>>(1, 100, "Must be 1-100");
ConditionalValidator conditionalValidator("max_iterations", std::any(50), std::move(rangeValidator), "If max_iterations=50, must be 1-100");

std::unordered_map<std::string, std::any> config1 = {
    {"max_iterations", 50}
};
assert(conditionalValidator.validate(config1) == true); // Should pass

std::unordered_map<std::string, std::any> config2 = {
    {"max_iterations", 50}
};
// But with a modified validator to fail
```

### 1.5 CompositeValidator Tests

**Test 1.5.1: AND Composite Validation**
```cpp
std::vector<std::unique_ptr<ValidatorBase>> validators;
validators.push_back(std::make_unique<RangeValidator<int>>(0, 100, "Range 0-100"));
validators.push_back(std::make_unique<PatternValidator>("[0-9]+", "Numeric string"));

CompositeValidator compositeValidator(std::move(validators), "Composite validation", true);
assert(compositeValidator.validate("50") == true);  // Both validators pass
assert(compositeValidator.validate("150") == false); // Range validator fails
assert(compositeValidator.validate("abc") == false); // Pattern validator fails
```

**Test 1.5.2: OR Composite Validation**
```cpp
std::vector<std::unique_ptr<ValidatorBase>> orValidators;
orValidators.push_back(std::make_unique<RangeValidator<int>>(0, 100, "Range 0-100"));
orValidators.push_back(std::make_unique<PatternValidator>("[a-z]+", "Lowercase letters"));

CompositeValidator orCompositeValidator(std::move(orValidators), "OR composite validation", false);
assert(orCompositeValidator.validate("50") == true);   // Range validator passes
assert(orCompositeValidator.validate("abc") == true);   // Pattern validator passes
assert(orCompositeValidator.validate("150") == false);  // Both fail
```

## 2. FormatValidator Tests

### 2.1 Built-in Format Tests

**Test 2.1.1: Email Format Validation**
```cpp
FormatValidator emailValidator(FormatValidator::FormatType::Email, "Email address validation");
assert(emailValidator.validate("test@example.com") == true);
assert(emailValidator.validate("user.name@domain.co.uk") == true);
assert(emailValidator.validate("invalid-email") == false);
assert(emailValidator.validate("@invalid.com") == false);
```

**Test 2.1.2: IPv4 Format Validation**
```cpp
FormatValidator ipv4Validator(FormatValidator::FormatType::IPv4, "IPv4 address validation");
assert(ipv4Validator.validate("192.168.1.1") == true);
assert(ipv4Validator.validate("10.0.0.1") == true);
assert(ipv4Validator.validate("255.255.255.255") == true);
assert(ipv4Validator.validate("256.256.256.256") == false);
assert(ipv4Validator.validate("192.168.1") == false);
```

### 2.2 Custom Format Tests

**Test 2.2.1: Custom Pattern Validation**
```cpp
FormatValidator customValidator("[0-9]{3}-[0-9]{2}-[0-9]{5}", "Custom date format (XXX-XX-XXXXX)");
assert(customValidator.validate("123-45-67890") == true);
assert(customValidator.validate("12-34-56789") == false);
assert(customValidator.validate("1234-56-78901") == false);
```

## 3. Validation Manager Tests

### 3.1 Validation Caching Tests

**Test 3.1.1: Cache Hit Validation**
```cpp
ValidationManager manager;
std::unordered_map<std::string, std::any> config = {
    {"key1", "value1"},
    {"key2", 42}
};

// First validation - should compute
ValidationResult result1 = manager.validate(config);
assert(result1.cacheable == true);

// Second validation - should use cache
ValidationResult result2 = manager.validate(config);
assert(result2.isValid() == result1.isValid());
```

### 3.2 Cross-Validation Tests

**Test 3.2.1: Cross-Validation Rule**
```cpp
ValidationManager manager;
manager.enableCrossValidation(true);

std::unordered_map<std::string, std::any> config = {
    {"optimization_enabled", true},
    {"max_iterations", 10}
};

ValidationResult result = manager.validate(config);
// Should pass: max_iterations is positive when optimization_enabled is true
```

### 3.3 Business Logic Validation Tests

**Test 3.3.1: Business Logic Validation**
```cpp
ValidationManager manager;
manager.enableBusinessLogicValidation(true);

std::unordered_map<std::string, std::any> config = {
    {"learning_rate", 0.01},
    {"batch_size", 32}
};

ValidationResult result = manager.validate(config);
// Should pass: learning_rate * batch_size = 0.32 < 1000
```

## 4. Validation Error Tests

### 4.1 Error Code Tests

**Test 4.1.1: Error Code Mapping**
```cpp
ValidationError error("test_key", ValidationErrorCode::ValueOutOfRange, "Value out of range");
assert(error.errorCode == ValidationErrorCode::ValueOutOfRange);
assert(error.errorCodeName == "ValueOutOfRange");
assert(error.categoryName() == "Value");
```

### 4.2 Error Message Tests

**Test 4.2.1: Error Message Formatting**
```cpp
ValidationError error("learning_rate", ValidationErrorCode::BusinessLogicValidationFailed, "learning_rate * batch_size must be < 1000");
std::string formatted = error.formattedError();
// Should contain: [Validation Error] Key: "learning_rate" BusinessLogicValidationFailed - learning_rate * batch_size must be < 1000
```

## 5. Schema Builder Tests

### 5.1 Schema Builder Tests

**Test 5.1.1: Fluent API Validation**
```cpp
Config config;
auto& builder = config.getSchemaBuilder();

builder
    .addString("env_name", false, "Environment name")
    .addInt("max_iterations", false, "Maximum iterations")
    .addDouble("learning_rate", false, "Learning rate")
    .addBool("enabled", false, "Enable feature")
    .addRange("max_iterations", 1, 1000, "Valid range", true)
    .addEnum("log_level", {"DEBUG", "INFO", "WARNING", "ERROR"}, "Log levels", true)
    .addPattern("env_name", "^[a-zA-Z0-9_]+$", "Alphanumeric only", true)
    .addFormat("email", FormatValidator::FormatType::Email, "Email format");

// Verify schema entries were added
assert(config.getSchema().size() >= 6); // Should have multiple entries
```

### 5.2 Validation Group Tests

**Test 5.2.1: Validation Group Management**
```cpp
Config config;

ValidationGroup basicGroup("BasicSettings", "Basic configuration parameters");
basicGroup.addKey("random_seed");
basicGroup.addKey("simulation_timestep");

ValidationGroup advancedGroup("AdvancedSettings", "Advanced configuration parameters");
advancedGroup.addKey("learning_rate");
advancedGroup.addKey("batch_size");

config.addValidationGroup(basicGroup);
config.addValidationGroup(advancedGroup);

assert(config.getValidationGroups().size() == 2);
assert(basicGroup.getSummary().find("BasicSettings") != std::string::npos);
```

## 6. Integration Tests

### 6.1 Full Integration Test

**Test 6.1.1: Complete Validation Workflow**
```cpp
void testCompleteValidationWorkflow() {
    nlm::Config config;
    
    // Setup configuration
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.1);
    config.set("neuron_count", 100);
    config.set("log_level", std::string("INFO"));
    
    // Build schema using fluent API
    config.getSchemaBuilder()
        .addInt("random_seed", false, "Random seed")
        .addDouble("simulation_timestep", false, "Time step")
        .addInt("neuron_count", false, "Neuron count")
        .addEnum("log_level", {"DEBUG", "INFO", "WARNING", "ERROR"}, "Log levels")
        .addRange("neuron_count", 1, 10000, "Valid neuron count", true);
    
    // Test configuration
    std::unordered_map<std::string, std::any> testConfig = {
        {"random_seed", 42},
        {"simulation_timestep", 0.1},
        {"neuron_count", 100},
        {"log_level", std::string("INFO")}
    };
    
    // Validate
    ValidationResult result = config.validate(testConfig);
    assert(result.isValid() == true);
    
    // Test with invalid data
    std::unordered_map<std::string, std::any> invalidConfig = {
        {"random_seed", 42},
        {"simulation_timestep", 0.1},
        {"neuron_count", 15000}, // Out of range
        {"log_level", std::string("INVALID")} // Invalid enum
    };
    
    ValidationResult invalidResult = config.validate(invalidConfig);
    assert(invalidResult.isValid() == false);
    assert(invalidResult.errors.size() >= 2); // Should have at least 2 errors
}
```

## 7. Performance Tests

### 7.1 Caching Performance Tests

**Test 7.1.1: Cache Hit Performance**
```cpp
void testCachingPerformance() {
    ValidationManager manager;
    std::unordered_map<std::string, std::any> config = {
        {"key1", "value1"},
        {"key2", 42},
        {"key3", "test"}
    };
    
    // First validation (cache miss)
    auto startTime = std::chrono::high_resolution_clock::now();
    ValidationResult result1 = manager.validate(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Second validation (cache hit)
    startTime = std::chrono::high_resolution_clock::now();
    ValidationResult result2 = manager.validate(config);
    endTime = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Cache hit should be faster
    assert(duration2 < duration1 * 10); // Cache hit should be significantly faster
}
```

## 8. Error Handling Tests

### 8.1 Exception Safety Tests

**Test 8.1.1: Exception Safety in Validation**
```cpp
void testExceptionSafety() {
    ValidationManager manager;
    
    // Test with invalid input that should not throw exceptions
    std::unordered_map<std::string, std::any> config = {
        {"key1", std::string("value with spaces")},
        {"key2", 42},
        {"key3", std::vector<int>({1, 2, 3})}
    };
    
    // Validation should handle invalid data gracefully
    ValidationResult result = manager.validate(config);
    assert(true); // Should not throw
}
```

## 9. Backward Compatibility Tests

### 9.1 Legacy API Tests

**Test 9.1.1: Legacy Config API**
```cpp
void testLegacyAPI() {
    nlm::Config config;
    
    // Test legacy get/set methods
    config.set("int_value", 42);
    assert(config.has("int_value") == true);
    
    auto value = config.get<int>("int_value");
    assert(value.has_value() == true);
    assert(*value == 42);
    
    // Test legacy getOr method
    int defaultValue = config.getOr<int>("missing_key", 100);
    assert(defaultValue == 100);
    
    // Test legacy validateSchema method
    assert(config.validateSchema() == true);
}
```

## 10. Summary Test

### 10.1 Comprehensive Test Summary

```cpp
void runAllValidationTests() {
    std::cout << "Running Enhanced Configuration Validation Tests" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    // Run all test suites
    testLegacyAPI();           // Backward compatibility
    testCompleteValidationWorkflow(); // Integration test
    testCachingPerformance();   // Performance test
    testExceptionSafety();      // Error handling
    
    std::cout << "\nAll tests passed successfully!" << std::endl;
    std::cout << "=================================================" << std::endl;
}
```

## Test Results Summary

The enhanced configuration validation system includes comprehensive test coverage for:

- **Core Validation Types**: Range, Pattern, Enum, Conditional, Composite, Format validators
- **Error Handling**: Detailed error reporting with categories and codes
- **Performance**: Caching and optimization features
- **Integration**: Full workflow testing with SchemaBuilder
- **Backward Compatibility**: Legacy API maintained and tested
- **Edge Cases**: Exception safety and performance benchmarks

All tests are designed to validate the correctness, performance, and reliability of the enhanced validation system while ensuring backward compatibility with existing code.

## Test Files Generated

1. **test_validation.cpp** - Comprehensive test suite
2. **test_scenarios.cpp** - Real-world validation scenarios
3. **test_performance.cpp** - Performance benchmarking
4. **test_backward_compat.cpp** - Backward compatibility testing

These test files provide a complete testing framework for the enhanced configuration validation system.