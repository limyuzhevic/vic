# Test program to verify error handling improvements

#include "src/core/ErrorHandling/ErrorHandling.hpp"
#include "src/core/ErrorHandling/ErrorHandlingUtilities.hpp"
#include "src/experiments/Phase6Demo.cpp"
#include <iostream>

// Simple test function
template<typename T>
nlm::Result<T> safeOperation(T value, bool shouldFail = false) {
    if (shouldFail) {
        return nlm::makeErrorResult<T>(nlm::ErrorCode::GeneralError,
                                      "Test operation failed",
                                      "test.cpp", 1, "safeOperation");
    }
    return nlm::makeSuccessResult(value);
}

// Test error context
void testErrorContext() {
    nlm::ErrorContext context("TestContext");
    context.addParameter("test_param1", 42);
    context.addParameter("test_param2", 3.14f);
    context.addParameter("test_param3", std::string("hello"));
    
    std::cout << "Error Context: " << context.getFormattedContext() << std::endl;
}

// Test error formatter
void testErrorFormatter() {
    try {
        nlm::NLMException error(nlm::ErrorCode::BrainInitializationFailed,
                               "Test error message",
                               "test.cpp", 42, "testErrorFormatter");
        
        std::cout << "Formatted Error:\n" << nlm::ErrorFormatter::formatError(error);
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

// Test result type
void testResultType() {
    // Test successful result
    auto successResult = safeOperation(42, false);
    if (successResult.success()) {
        std::cout << "Success result: " << successResult.value() << std::endl;
    }
    
    // Test failed result
    auto failedResult = safeOperation(0, true);
    if (!failedResult.success()) {
        std::cout << "Failed result: " << failedResult.getFullError() << std::endl;
    }
}

int main() {
    std::cout << "=== Error Handling Improvements Test ===" << std::endl << std::endl;
    
    std::cout << "Testing error context..." << std::endl;
    testErrorContext();
    std::cout << std::endl;
    
    std::cout << "Testing error formatter..." << std::endl;
    testErrorFormatter();
    std::cout << std::endl;
    
    std::cout << "Testing result type..." << std::endl;
    testResultType();
    std::cout << std::endl;
    
    std::cout << "=== Test completed ===" << std::endl;
    return 0;
}
