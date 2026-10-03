#include <iostream>
#include <cassert>
#include "src/core/Error/NLMErrors.hpp"

void testErrorHandlingFramework() {
    std::cout << "Testing Error Handling Framework..." << std::endl;
    
    nlm::ErrorHandler errorHandler;  // Create local instance instead of global
    errorHandler.setThrowOnError(false);  // Don't throw, just collect errors
    
    // Test 1: Create a validation error
    errorHandler.addError(std::make_shared<nlm::ValidationError>("Test validation", "Test"));
    std::cout << "✓ Validation error creation works" << std::endl;
    
    // Test 2: Test error handler functionality
    auto err1 = std::make_shared<nlm::ValidationError>("Error 1", "Test");
    auto err2 = std::make_shared<nlm::ResourceError>("Error 2", "Test");
    
    errorHandler.addError(err1);
    errorHandler.addError(err2);
    
    std::cout << "Error handler has " << errorHandler.getErrors().size() << " errors" << std::endl;
    
    assert(errorHandler.hasErrors());
    assert(errorHandler.getErrors().size() == 2);
    std::cout << "✓ Error handler collection works: " << errorHandler.getErrors().size() << " errors" << std::endl;
    
    // Test 3: Test error message formatting
    std::string msg = err1->getFullMessage();
    assert(!msg.empty());
    assert(msg.find("Error 1") != std::string::npos);
    std::cout << "✓ Error message formatting works" << std::endl;
    
    // Test 4: Test error recovery
    errorHandler.clear();
    assert(!errorHandler.hasErrors());
    std::cout << "✓ Error clearing works" << std::endl;
    
    std::cout << "Error Handling Framework tests completed successfully!" << std::endl;
}

void testErrorTypes() {
    std::cout << "\nTesting Error Type Hierarchy..." << std::endl;
    
    nlm::ErrorHandler errorHandler;
    errorHandler.setThrowOnError(false);
    
    // Test different error types
    auto valError = std::make_shared<nlm::ValidationError>("Validation failed", "ValidationTest");
    auto resError = std::make_shared<nlm::ResourceError>("Resource failed", "ResourceTest");
    auto ioError = std::make_shared<nlm::IOError>("IO failed", "IOTest");
    auto runtimeError = std::make_shared<nlm::RuntimeError>("Runtime failed", "RuntimeTest");
    auto configError = std::make_shared<nlm::ConfigurationError>("Config failed", "ConfigTest");
    
    errorHandler.addError(valError);
    errorHandler.addError(resError);
    errorHandler.addError(ioError);
    errorHandler.addError(runtimeError);
    errorHandler.addError(configError);
    
    assert(errorHandler.getErrors().size() == 5);
    
    // Verify error types
    assert(std::dynamic_pointer_cast<nlm::ValidationError>(errorHandler.getErrors()[0]) != nullptr);
    assert(std::dynamic_pointer_cast<nlm::ResourceError>(errorHandler.getErrors()[1]) != nullptr);
    assert(std::dynamic_pointer_cast<nlm::IOError>(errorHandler.getErrors()[2]) != nullptr);
    assert(std::dynamic_pointer_cast<nlm::RuntimeError>(errorHandler.getErrors()[3]) != nullptr);
    assert(std::dynamic_pointer_cast<nlm::ConfigurationError>(errorHandler.getErrors()[4]) != nullptr);
    
    std::cout << "✓ All error types work correctly" << std::endl;
    std::cout << "✓ Error type hierarchy validation works" << std::endl;
    
    std::cout << "Error Type tests completed successfully!" << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling Framework Test Suite ===" << std::endl;
    
    try {
        testErrorHandlingFramework();
        testErrorTypes();
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        std::cout << "Error handling framework implementation is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** Test failed with exception: " << e.what() << " ***" << std::endl;
        return 1;
    }
    
    return 0;
}

