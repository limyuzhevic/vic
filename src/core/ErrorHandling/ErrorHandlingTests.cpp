// Test the error handling framework
#include "ErrorHandling.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace nlm {

void testErrorHandling() {
    std::cout << "=== Testing Error Handling Framework ===" << std::endl;
    
    // Test 1: Basic error handling
    std::cout << "Test 1: Basic error handling..." << std::endl;
    try {
        NLM_THROW_ERROR("Test error message", ErrorSeverity::Error);
    } catch (const NLMException& e) {
        std::cout << "✓ Caught exception with severity: " << static_cast<int>(e.getSeverity()) << std::endl;
        std::cout << "  Message: " << e.what() << std::endl;
    }
    
    // Test 2: Warning handling
    std::cout << "\nTest 2: Warning handling..." << std::endl;
    try {
        NLM_THROW_ERROR("Test warning message", ErrorSeverity::Warning);
    } catch (const NLMException& e) {
        std::cout << "✓ Caught warning with severity: " << static_cast<int>(e.getSeverity()) << std::endl;
    }
    
    // Test 3: Safe operation
    std::cout << "\nTest 3: Safe operation..." << endl;
    try {
        auto result = ErrorHandler::getInstance()->safeOperation("test safe", []() -> int {
            return 42;
        });
        std::cout << "✓ Safe operation returned: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Safe operation failed: " << e.what() << std::endl;
    }
    
    // Test 4: Null safety
    std::cout << "\nTest 4: Null safety..." << endl;
    std::shared_ptr<int> nullPtr;
    auto safeValue = ErrorHandler::getInstance()->safeAccess(nullPtr, "null access test");
    std::cout << "✓ Safe null access returned: " << safeValue << std::endl;
    
    // Test 5: Exception types
    std::cout << "\nTest 5: Exception types..." << endl;
    try {
        throw ConfigurationError("Test config error");
    } catch (const ConfigurationError& e) {
        std::cout << "✓ Caught ConfigurationError: " << e.what() << std::endl;
    }
    
    try {
        throw InitializationError("Test initialization error");
    } catch (const InitializationError& e) {
        std::cout << "✓ Caught InitializationError: " << e.what() << std::endl;
    }
    
    try {
        throw InvalidParameterError("Test invalid parameter error");
    } catch (const InvalidParameterError& e) {
        std::cout << "✓ Caught InvalidParameterError: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== Error Handling Tests Complete ===" << std::endl;
}

void initializeSystem() {
    initializeErrorHandling();
}

} // namespace nlm
