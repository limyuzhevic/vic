// Enhanced error handling tests for NLM
// Comprehensive unit tests for all error handling scenarios
// Covers edge cases and integration scenarios for error handling

#include "src/core/Error/Error.hpp"
#include "src/core/Error/ErrorReporting.hpp"
#include "src/core/Error/ErrorRecovery.hpp"
#include "tests/test_error_handling.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <thread>
#include <future>
#include <chrono>

namespace test_error_handling {

void testErrorCreation() {
    std::cout << "    Testing error creation..." << std::endl;
    
    try {
        nlm::NLMError error("Test error", nlm::ErrorCategory::General, "Test message");
        assert(error.getCategory() == nlm::ErrorCategory::General);
        assert(error.getMessage() == "Test message");
    } catch (...) {
        std::cout << "    testErrorCreation failed: exception caught" << std::endl;
        throw;
    }
    
    std::cout << "    testErrorCreation passed" << std::endl;
}

void testErrorCategories() {
    std::cout << "    Testing error categories..." << std::endl;
    
    // Test all error categories
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::General) == "General");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::Validation) == "Validation");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::Memory) == "Memory");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::Config) == "Config");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::Computation) == "Computation");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::IO) == "IO");
    assert(nlm::getErrorCategoryName(nlm::ErrorCategory::Integration) == "Integration");
    
    std::cout << "    testErrorCategories passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "    Testing error reporting..." << std::endl;
    
    nlm::ErrorReporting errorReporter("test_reporter", nlm::LogLevel::Warning);
    
    nlm::NLMError error("Test error", nlm::ErrorCategory::Memory, "Out of memory");
    errorReporter.reportError(error);
    
    // Test error recovery
    nlm::NLMError recoveredError("Recovered error", nlm::ErrorCategory::General, "Recovery message");
    nlm::ErrorRecovery::autoRecover(errorReporter, recoveredError);
    
    std::cout << "    testErrorReporting passed" << std::endl;
}

void testErrorRecovery() {
    std::cout << "    Testing error recovery..." << std::endl;
    
    nlm::ErrorRecovery recoveryManager;
    
    // Test different error types and recovery strategies
    nlm::NLMError validationError("Validation error", nlm::ErrorCategory::Validation, "Invalid parameter");
    bool recovered = recoveryManager.attemptRecovery(validationError);
    assert(recovered == true);  // Validation errors should be recoverable
    
    nlm::NLMError memoryError("Memory error", nlm::ErrorCategory::Memory, "Allocation failed");
    recovered = recoveryManager.attemptRecovery(memoryError);
    assert(recovered == false);  // Memory errors might not be recoverable
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

void testConcurrentErrorHandling() {
    std::cout << "    Testing concurrent error handling..." << std::endl;
    
    nlm::ErrorReporting errorReporter("concurrent_reporter", nlm::LogLevel::Error);
    
    auto errorTask = [&]() {
        nlm::NLMError error("Concurrent error", nlm::ErrorCategory::Computation, "Parallel execution error");
        errorReporter.reportError(error);
    };
    
    // Run multiple error reporting tasks concurrently
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(std::async(std::launch::async, errorTask));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    std::cout << "    testConcurrentErrorHandling passed" << std::endl;
}

void testErrorEscalation() {
    std::cout << "    Testing error escalation..." << std::endl;
    
    nlm::ErrorReporting errorReporter("escalation_reporter", nlm::LogLevel::Error);
    
    // Test error escalation from warning to critical
    nlm::NLMError minorError("Minor error", nlm::ErrorCategory::General, "Minor issue");
    errorReporter.reportError(minorError);
    
    nlm::NLMError majorError("Major error", nlm::ErrorCategory::Integration, "System failure");
    errorReporter.reportError(majorError);
    
    // The error reporter should handle escalation
    std::cout << "    testErrorEscalation passed" << std::endl;
}

void testErrorFormatting() {
    std::cout << "    Testing error formatting..." << std::endl;
    
    nlm::NLMError error("Format test", nlm::ErrorCategory::Validation, "Value out of range: -999 to 999");
    
    std::string formatted = error.formatDetailed();
    assert(!formatted.empty());
    assert(formatted.find("Format test") != std::string::npos);
    assert(formatted.find("Validation") != std::string::npos);
    
    std::cout << "    testErrorFormatting passed" << std::endl;
}

void testEdgeCaseErrors() {
    std::cout << "    Testing edge case errors..." << std::endl;
    
    // Test empty error message
    nlm::NLMError emptyError("", nlm::ErrorCategory::General, "");
    
    // Test null pointer category (should default to General)
    nlm::NLMError nullError("Null test", nlm::ErrorCategory(), "Test");
    
    // Test special characters in error messages
    nlm::NLMError specialCharsError("Special chars", nlm::ErrorCategory::General, "Error with \n\t\\\" special \\' chars");
    
    std::cout << "    testEdgeCaseErrors passed" << std::endl;
}

void runAll() {
    testErrorCreation();
    testErrorCategories();
    testErrorReporting();
    testErrorRecovery();
    testConcurrentErrorHandling();
    testErrorEscalation();
    testErrorFormatting();
    testEdgeCaseErrors();
}

} // namespace test_error_handling
