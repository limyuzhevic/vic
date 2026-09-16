// Test program to verify the enhanced error handling and configuration management
#include "core/ErrorHandling/Error.hpp"
#include "core/EnhancedConfig.hpp"
#include <iostream>

int main() {
    try {
        // Test error handling
        std::cout << "Testing error handling..." << std::endl;
        
        // Test throwing errors
        throwError(ErrorCode::CONFIG_FILE_NOT_FOUND, "Test configuration file not found");
        
    } catch (const NLMError& e) {
        std::cout << "Caught NLMError: " << e.what() << std::endl;
        std::cout << "Error code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Test configuration management
        std::cout << "\nTesting configuration management..." << std::endl;
        
        nlm::EnhancedConfig config;
        
        // Set some values
        config.set("test_int", 42);
        config.set("test_double", 3.14);
        config.set("test_string", "Hello World");
        config.set("test_bool", true);
        
        // Get values with defaults
        int intVal = config.getOr("test_int", 0);
        double doubleVal = config.getOr("test_double", 0.0);
        std::string stringVal = config.getOr("test_string", "");
        bool boolVal = config.getOr("test_bool", false);
        
        std::cout << "test_int: " << intVal << std::endl;
        std::cout << "test_double: " << doubleVal << std::endl;
        std::cout << "test_string: " << stringVal << std::endl;
        std::cout << "test_bool: " << boolVal << std::endl;
        
        // Test validation rules
        nlm::ValidationRule rule1 = {"test_int", "int", "Must be positive", "min=0,max=100", true, "0"};
        config.addValidationRule(rule1);
        
        // Test summary
        std::cout << "\nConfiguration summary:\n" << config.getSummary() << std::endl;
        
    } catch (const NLMError& e) {
        std::cout << "Caught NLMError in config test: " << e.what() << std::endl;
    }
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}
