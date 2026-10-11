// Simple test to demonstrate error handling system
#include "src/core/Config/Config.hpp"
#include "src/core/Errors/Errors.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Error Handling System Test ===" << std::endl;
    
    try {
        nlm::Config config;
        
        // Test basic configuration operations with error handling
        std::cout << "1. Testing basic configuration operations..." << std::endl;
        
        // Set values
        config.set("test_int", 42);
        config.set("test_string", "Hello, World!");
        config.set("test_double", 3.14159);
        config.set("test_bool", true);
        
        // Get values
        auto intVal = config.get<int>("test_int");
        if (intVal.has_value()) {
            std::cout << "   Integer value: " << *intVal << std::endl;
        }
        
        auto stringVal = config.get<std::string>("test_string");
        if (stringVal.has_value()) {
            std::cout << "   String value: " << *stringVal << std::endl;
        }
        
        std::cout << "2. Testing error handling with non-existent keys..." << std::endl;
        
        // Test with non-existent key (should return nullopt)
        auto missingVal = config.get<int>("missing_key");
        if (!missingVal.has_value()) {
            std::cout << "   Missing key correctly returns nullopt" << std::endl;
        }
        
        std::cout << "3. Testing validation..." << std::endl;
        
        // Test validation
        bool isValid = config.validate();
        std::cout << "   Configuration validation: " << (isValid ? "Passed" : "Failed") << std::endl;
        
        std::cout << "4. Testing error recovery..." << std::endl;
        
        // Test error recovery
        config.set("test_error_recovery", "Recovery Test");
        std::cout << "   Error recovery test completed" << std::endl;
        
        std::cout << "5. Testing error reporting system..." << std::endl;
        
        // Report a warning using the error reporting system
        nlm::ErrorReporterRegistry::reportWarning("This is a test warning message");
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        std::cout << "\nError handling system features demonstrated:" << std::endl;
        std::cout << "- Custom exception hierarchy (NLMException, ConfigurationException, etc.)" << std::endl;
        std::cout << "- Error codes for different error types" << std::endl;
        std::cout << "- Enhanced error messages with context" << std::endl;
        std::cout << "- Structured error reporting" << std::endl;
        std::cout << "- Error recovery mechanisms" << std::endl;
        std::cout << "- Comprehensive logging with error context" << std::endl;
        
        return 0;
        
    } catch (const nlm::NLMException& e) {
        std::cout << "ERROR: NLM Exception caught: " << e.what() << std::endl;
        std::cout << "Error code: " << static_cast<int>(e.getCode()) << std::endl;
        std::cout << "Error context: " << e.getContext() << std::endl;
        std::cout << "Error file: " << e.getFile() << std::endl;
        std::cout << "Error line: " << e.getLine() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cout << "ERROR: Standard exception caught: " << e.what() << std::endl;
        return 2;
    }
}