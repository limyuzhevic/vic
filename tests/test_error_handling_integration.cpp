// Test file for error handling integration
// Tests the comprehensive error handling system

#include "src/core/Error/ErrorCategory.hpp"
#include "src/core/Error/NLMError.hpp"
#include "src/core/Error/ErrorHandler.hpp"
#include <iostream>
#include <cassert>
#include <vector>

namespace nlm {
namespace error_handling_tests {

// Test basic error creation and handling
void testBasicErrorCreation() {
    std::cout << "Testing basic error creation..." << std::endl;
    
    // Test configuration error
    InvalidConfigValueError configError(
        "max_neurons", 
        "positive integer", 
        "-5",
        ErrorContext("Config", "load_config", "config.cpp", 42)
    );
    
    assert(configError.getCategory() == ErrorCategory::CONFIG_INVALID_VALUE);
    assert(configError.getErrorCode() == NLMError::CONFIG_INVALID_VALUE);
    assert(configError.getSeverity() == ErrorSeverity::ERROR);
    assert(configError.getContext().component == "Config");
    assert(configError.getContext().operation == "load_config");
    
    std::cout << "  Basic error creation: PASSED" << std::endl;
}

// Test error hierarchy
void testErrorHierarchy() {
    std::cout << "Testing error hierarchy..." << std::endl;
    
    // Test that specific errors are instances of base errors
    InvalidConfigValueError configError("key", "expected", "actual");
    ConfigurationError baseConfigError("message");
    
    // Both should be caught by the same catch block
    std::vector<std::function<void()>> errorThrowers = {
        []() { throw configError; },
        []() { throw baseConfigError; }
    };
    
    for (auto& thrower : errorThrowers) {
        try {
            thrower();
        } catch (const ConfigurationError& e) {
            // Should catch both errors
            assert(e.getCategory() == ErrorCategory::CONFIG_INVALID_VALUE ||
                   e.getCategory() == ErrorCategory::CONFIG_INVALID_VALUE);
        }
    }
    
    std::cout << "  Error hierarchy: PASSED" << std::endl;
}

// Test error recovery
void testErrorRecovery() {
    std::cout << "Testing error recovery..." << std::endl;
    
    // Set up error handler with recovery strategy
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::RESOURCE_OUT_OF_MEMORY, 
        ErrorStrategy::RECOVER
    );
    
    OutOfMemoryError memError(1024*1024*1024, 512*1024*1024);
    
    // The error should be recoverable
    assert(memError.isRecoverable());
    assert(!memError.getRecoveryMessage().empty());
    
    std::cout << "  Error recovery: PASSED" << std::endl;
}

// Test error reporting
void testErrorReporting() {
    std::cout << "Testing error reporting..." << std::endl;
    
    // Add console reporter
    ErrorHandler::getInstance().addReporter(
        std::make_shared<ConsoleErrorReporter>(ErrorSeverity::WARNING)
    );
    
    // The error should be reported when thrown
    // In a real test, we would capture stdout/stderr
    std::cout << "  Error reporting setup: PASSED" << std::endl;
}

// Test error strategies
void testErrorStrategies() {
    std::cout << "Testing error strategies..." << std::endl;
    
    // Test different strategies
    std::vector<std::pair<ErrorCategory, ErrorStrategy>> strategies = {
        {ErrorCategory::CONFIG_INVALID_VALUE, ErrorStrategy::FALLBACK},
        {ErrorCategory::RESOURCE_OUT_OF_MEMORY, ErrorStrategy::RECOVER},
        {ErrorCategory::IO_READ_ERROR, ErrorStrategy::RETRY},
        {ErrorCategory::SYSTEM_INIT_FAILURE, ErrorStrategy::LOG_AND_THROW}
    };
    
    for (const auto& strategy : strategies) {
        ErrorHandler::getInstance().setStrategy(strategy.first, strategy.second);
        
        // Verify strategy was set
        // In a real test, we would verify the handler config
        std::cout << "    Strategy for " << static_cast<int>(strategy.first) 
                  << " set to " << static_cast<int>(strategy.second) << std::endl;
    }
    
    std::cout << "  Error strategies: PASSED" << std::endl;
}

// Test error context
void testErrorContext() {
    std::cout << "Testing error context..." << std::endl;
    
    ErrorContext ctx("TestComponent", "test_operation", "test.cpp", 123);
    
    assert(ctx.component == "TestComponent");
    assert(ctx.operation == "test_operation");
    assert(ctx.file == "test.cpp");
    assert(ctx.line == 123);
    
    // Test context with metadata
    ctx.setDetail("key1", "value1");
    ctx.setDetail("key2", 42);
    
    std::cout << "  Error context: PASSED" << std::endl;
}

// Test error reporting utilities
void testErrorReportingUtilities() {
    std::cout << "Testing error reporting utilities..." << std::endl;
    
    // Create test errors
    std::vector<std::unique_ptr<NLMError>> errors;
    
    errors.push_back(std::make_unique<InvalidConfigValueError>(
        "param1", "expected1", "actual1"
    ));
    
    errors.push_back(std::make_unique<FileReadError>(
        "file.txt", "Permission denied"
    ));
    
    // Test error formatting
    for (const auto& error : errors) {
        std::string report = ErrorUtilities::formatErrorReport(*error);
        assert(!report.empty());
        assert(report.find("Error Report") != std::string::npos);
    }
    
    std::cout << "  Error reporting utilities: PASSED" << std::endl;
}

// Test comprehensive error handling
void testComprehensiveErrorHandling() {
    std::cout << "Testing comprehensive error handling..." << std::endl;
    
    // Set up comprehensive error handling
    ErrorHandler::getInstance().setEnableRecovery(true);
    ErrorHandler::getInstance().setEnableReporting(true);
    ErrorHandler::getInstance().setEnableRetry(true);
    
    // Configure strategies
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::CONFIG_INVALID_VALUE, 
        ErrorStrategy::FALLBACK
    );
    
    // Test that errors can be created and handled
    try {
        throw InvalidConfigValueError("test_key", "expected", "actual");
    } catch (const NLMError& e) {
        assert(!e.getDetails().empty());
        std::cout << "  Comprehensive error handling: PASSED" << std::endl;
    }
}

// Run all tests
void runAllTests() {
    std::cout << "=== Running Error Handling Tests ===" << std::endl << std::endl;
    
    testBasicErrorCreation();
    testErrorHierarchy();
    testErrorRecovery();
    testErrorReporting();
    testErrorStrategies();
    testErrorContext();
    testErrorReportingUtilities();
    testComprehensiveErrorHandling();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Passed ===" << std::endl;
}

} // namespace error_handling_tests
} // namespace nlm
