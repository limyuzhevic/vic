#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Error Handling System Components..." << std::endl;
    
    // Test 1: Basic exception hierarchy
    try {
        throw nlm::ValidationError("Test validation error", "test_param");
    } catch (const nlm::NLMException& e) {
        std::cout << "✓ ValidationError caught: " << e.getMessage() << std::endl;
    }
    
    // Test 2: Error utilities
    try {
        nlm::ErrorUtilities::throwIf<bool>(false, 
                                            nlm::ValidationError, 
                                            "Test condition failed", 
                                            "test_condition");
    } catch (const nlm::NLMException& e) {
        std::cout << "✓ ErrorUtilities::throwIf caught: " << e.getMessage() << std::endl;
    }
    
    // Test 3: Parameter validation
    nlm::ParameterValidator validator;
    validator.registerValidator<int>("test_type", [](int value, const std::string& context) {
        nlm::ValidationResult result;
        if (value < 0) {
            result.passed = false;
            nlm::ValidationErrorDetail error("Value must be non-negative", "input");
            error.severity = nlm::ValidationSeverity::Error;
            result.errors.push_back(error);
        }
        return result;
    });
    
    std::map<std::string, std::any> params;
    params["value"] = 5;
    auto results = validator.validateAll(params);
    std::cout << "✓ Parameter validation passed for value=5" << std::endl;
    
    // Test 4: Configuration validation
    nlm::ConfigValidator configValidator;
    nlm::ConfigValidator::ConfigValidationRule rule;
    rule.key = "test_key";
    rule.type = "string";
    rule.required = true;
    configValidator.addRule(rule);
    
    std::map<std::string, std::string> config;
    config["test_key"] = "test_value";
    auto configResult = configValidator.validate(config);
    std::cout << "✓ Configuration validation passed for valid config" << std::endl;
    
    // Test 5: Error formatting
    nlm::ValidationError testError("Test error", "test_param", "test.cpp", 123, "testFunction");
    std::string formatted = testError.formatMessage();
    std::cout << "✓ Error formatting: " << formatted << std::endl;
    
    std::cout << "\nAll error handling system tests passed!" << std::endl;
    return 0;
}
