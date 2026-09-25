// Error Handling Integration Example
// Demonstrates how to integrate the error handling system throughout the NLM codebase

#include "ErrorCategory.hpp"
#include "NLMError.hpp"
#include "ErrorHandler.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {
namespace error_handling_examples {

// Example 1: Basic error handling with configuration errors
void exampleConfigurationErrorHandling() {
    std::cout << "=== Example 1: Configuration Error Handling ===" << std::endl;
    
    try {
        // Simulate a configuration error
        throw InvalidConfigValueError(
            "max_neurons", 
            "positive integer", 
            "-5",
            ErrorContext("Config", "load_config", "config.cpp", 42)
        );
    } catch (const InvalidConfigValueError& e) {
        std::cout << "Caught configuration error: " << e.what() << std::endl;
        std::cout << "Category: " << e.getCategoryString() << std::endl;
        std::cout << "Details: " << e.getDetails() << std::endl;
        if (!e.getRecoveryMessage().empty()) {
            std::cout << "Recovery: " << e.getRecoveryMessage() << std::endl;
        }
    }
    
    std::cout << std::endl;
}

// Example 2: Error handling with neural system errors
void exampleNeuralSystemErrorHandling() {
    std::cout << "=== Example 2: Neural System Error Handling ===" << std::endl;
    
    try {
        // Simulate a neuron not found error
        throw NeuronNotFoundError(
            "neuron_123",
            ErrorContext("Brain", "create_synapse", "brain.cpp", 105)
        );
    } catch (const NeuronNotFoundError& e) {
        std::cout << "Caught neural system error: " << e.what() << std::endl;
        std::cout << "Error code: " << e.getErrorCode() << std::endl;
        std::cout << "Severity: " << e.getSeverityString() << std::endl;
        
        // Check if error is recoverable
        if (e.isRecoverable()) {
            std::cout << "Error is recoverable" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

// Example 3: Error handling with recovery mechanisms
void exampleErrorRecovery() {
    std::cout << "=== Example 3: Error Recovery ===" << std::endl;
    
    // Set up error handler to use recovery strategy
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::RESOURCE_OUT_OF_MEMORY, 
        ErrorStrategy::RECOVER
    );
    
    try {
        // Simulate an out of memory error
        throw OutOfMemoryError(
            1024 * 1024 * 1024, // 1GB requested
            512 * 1024 * 1024, // 512MB available
            ErrorContext("Memory", "allocate", "memory.cpp", 78)
        );
    } catch (const OutOfMemoryError& e) {
        std::cout << "Caught out of memory error: " << e.what() << std::endl;
        
        // The error handler should attempt recovery
        // In this example, we'll catch it and show recovery info
        if (!e.getRecoveryMessage().empty()) {
            std::cout << "Recovery message: " << e.getRecoveryMessage() << std::endl;
        }
    }
    
    std::cout << std::endl;
}

// Example 4: Error handling with retry strategy
void exampleRetryErrorHandling() {
    std::cout << "=== Example 4: Retry Error Handling ===" << std::endl;
    
    // Set up error handler to use retry strategy
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::IO_READ_ERROR, 
        ErrorStrategy::RETRY
    );
    
    try {
        // Simulate a file read error
        throw FileReadError(
            "/important/data.txt",
            "Permission denied",
            ErrorContext("FileSystem", "read", "fileio.cpp", 23)
        );
    } catch (const FileReadError& e) {
        std::cout << "Caught file read error: " << e.what() << std::endl;
        
        // In a real implementation, the error handler would retry
        std::cout << "Error handler would retry the operation" << std::endl;
    }
    
    std::cout << std::endl;
}

// Example 5: Error handling with custom filter
void exampleCustomErrorFilter() {
    std::cout << "=== Example 5: Custom Error Filter ===" << std::endl;
    
    // Set up custom filter to ignore certain errors
    ErrorHandler::getInstance().setCustomFilter(
        [](const NLMError& error) {
            // Ignore validation warnings for demonstration
            return error.getSeverity() != ErrorSeverity::WARNING;
        }
    );
    
    try {
        // Simulate a validation error (would be filtered out)
        throw ValidationError(
            "max_value",
            "<= 100",
            "150",
            ErrorContext("Validation", "validate", "validator.cpp", 67)
        );
    } catch (const ValidationError& e) {
        std::cout << "Caught validation error: " << e.what() << std::endl;
        std::cout << "Note: This error would have been filtered by custom filter" << std::endl;
    }
    
    std::cout << std::endl;
}

// Example 6: Error reporting integration
void exampleErrorReporting() {
    std::cout << "=== Example 6: Error Reporting ===" << std::endl;
    
    // Add a console reporter
    ErrorHandler::getInstance().addReporter(
        std::make_shared<ConsoleErrorReporter>(ErrorSeverity::INFO)
    );
    
    // Add a file reporter
    ErrorHandler::getInstance().addReporter(
        std::make_shared<FileErrorReporter>("/tmp/nlm_errors.log", ErrorSeverity::ERROR)
    );
    
    try {
        // Simulate an error that will be reported
        throw InternalError(
            "Unexpected internal state",
            ErrorContext("System", "process", "system.cpp", 89)
        );
    } catch (const InternalError& e) {
        std::cout << "Caught internal error: " << e.what() << std::endl;
        std::cout << "Error reporter should have logged this to console and file" << std::endl;
    }
    
    std::cout << std::endl;
}

// Example 7: Comprehensive error handling workflow
void exampleComprehensiveErrorHandling() {
    std::cout << "=== Example 7: Comprehensive Error Handling ===" << std::endl;
    
    // Set up comprehensive error handling
    ErrorHandler::getInstance().setEnableRecovery(true);
    ErrorHandler::getInstance().setEnableReporting(true);
    ErrorHandler::getInstance().setEnableRetry(true);
    
    // Configure strategies for different categories
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::CONFIG_INVALID_VALUE, 
        ErrorStrategy::FALLBACK
    );
    
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::RESOURCE_OUT_OF_MEMORY, 
        ErrorStrategy::RECOVER
    );
    
    ErrorHandler::getInstance().setStrategy(
        ErrorCategory::IO_READ_ERROR, 
        ErrorStrategy::RETRY
    );
    
    // Add recovery handlers
    ErrorHandler::getInstance().addRecoveryHandler(
        ErrorCategory::RESOURCE_OUT_OF_MEMORY,
        std::make_shared<MemoryAllocationRecoveryHandler>()
    );
    
    ErrorHandler::getInstance().addRecoveryHandler(
        ErrorCategory::CONFIG_INVALID_VALUE,
        std::make_shared<ConfigValidationRecoveryHandler>()
    );
    
    try {
        // Simulate different types of errors
        std::vector<std::function<void()>> errorExamples = {
            []() { throw InvalidConfigValueError("param1", "1-100", "999"); },
            []() { throw OutOfMemoryError(1024*1024*1024, 512*1024*1024); },
            []() { throw FileReadError("data.txt", "File not found"); }
        };
        
        for (int i = 0; i < errorExamples.size(); ++i) {
            std::cout << "\nProcessing error example " << (i + 1) << ":" << std::endl;
            try {
                errorExamples[i]();
            } catch (const NLMError& e) {
                std::cout << "Error handled: " << e.getCategoryString() << std::endl;
                std::cout << "Message: " << e.what() << std::endl;
                
                if (!e.getRecoveryMessage().empty()) {
                    std::cout << "Recovery suggestion: " << e.getRecoveryMessage() << std::endl;
                }
            }
        }
    } catch (...) {
        std::cout << "Unexpected error occurred" << std::endl;
    }
    
    std::cout << std::endl;
}

// Test all examples
void runAllExamples() {
    std::cout << "=== Running All Error Handling Examples ===" << std::endl << std::endl;
    
    exampleConfigurationErrorHandling();
    exampleNeuralSystemErrorHandling();
    exampleErrorRecovery();
    exampleRetryErrorHandling();
    exampleCustomErrorFilter();
    exampleErrorReporting();
    exampleComprehensiveErrorHandling();
    
    std::cout << "=== All Examples Completed Successfully ===" << std::endl;
}

} // namespace error_handling_examples
} // namespace nlm
