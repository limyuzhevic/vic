// Main demonstration program for error handling system
// Shows the comprehensive error handling system in action

#include "src/core/Error/ErrorCategory.hpp"
#include "src/core/Error/NLMError.hpp"
#include "src/core/Error/ErrorHandler.hpp"
#include <iostream>

namespace nlm {
namespace error_demo {

void demonstrateErrorHandlingSystem() {
    std::cout << "=== NLM Error Handling System Demonstration ===" << std::endl << std::endl;
    
    // Initialize error handling
    ErrorHandler::getInstance(); // Ensure instance is created
    std::cout << "Error handling system initialized" << std::endl << std::endl;
    
    // Configure error handling strategies
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
    
    // Add reporters
    ErrorHandler::getInstance().addReporter(
        std::make_shared<ConsoleErrorReporter>(ErrorSeverity::INFO)
    );
    
    std::cout << "Error handling system configured with:" << std::endl;
    std::cout << "  - Fallback strategy for configuration errors" << std::endl;
    std::cout << "  - Recovery strategy for memory errors" << std::endl;
    std::cout << "  - Retry strategy for IO errors" << std::endl;
    std::cout << "  - Memory allocation recovery handler" << std::endl;
    std::cout << "  - Configuration validation recovery handler" << std::endl;
    std::cout << "  - Console error reporter" << std::endl << std::endl;
    
    // Demonstrate different error types
    std::cout << "Demonstrating different error types:" << std::endl << std::endl;
    
    // 1. Configuration error
    std::cout << "1. Configuration Error:" << std::endl;
    try {
        throw InvalidConfigValueError(
            "max_neurons",
            "positive integer between 1 and 100",
            "-5",
            ErrorContext("Config", "load_config", "config.cpp", 42)
        );
    } catch (const InvalidConfigValueError& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   Category: " << e.getCategoryString() << std::endl;
        std::cout << "   Severity: " << e.getSeverityString() << std::endl;
        std::cout << "   Recovery: " << e.getRecoveryMessage() << std::endl;
    }
    
    std::cout << std::endl;
    
    // 2. Neural system error
    std::cout << "2. Neural System Error:" << std::endl;
    try {
        throw NeuronNotFoundError(
            "neuron_123",
            ErrorContext("Brain", "create_synapse", "brain.cpp", 105)
        );
    } catch (const NeuronNotFoundError& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   Error Code: " << e.getErrorCode() << std::endl;
        std::cout << "   Context: " << e.getContext().component << 
                  "::" << e.getContext().operation << std::endl;
    }
    
    std::cout << std::endl;
    
    // 3. Resource error
    std::cout << "3. Resource Error (Out of Memory):" << std::endl;
    try {
        throw OutOfMemoryError(
            1024 * 1024 * 1024, // 1GB requested
            512 * 1024 * 1024, // 512MB available
            ErrorContext("Memory", "allocate", "memory.cpp", 78)
        );
    } catch (const OutOfMemoryError& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   Recoverable: " << (e.isRecoverable() ? "Yes" : "No") << std::endl;
        std::cout << "   Recovery: " << e.getRecoveryMessage() << std::endl;
    }
    
    std::cout << std::endl;
    
    // 4. IO error
    std::cout << "4. IO Error:" << std::endl;
    try {
        throw FileReadError(
            "/important/data.txt",
            "Permission denied",
            ErrorContext("FileSystem", "read", "fileio.cpp", 23)
        );
    } catch (const FileReadError& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   Category: " << e.getCategoryString() << std::endl;
    }
    
    std::cout << std::endl;
    
    // 5. System error
    std::cout << "5. System Error:" << std::endl;
    try {
        throw InitializationError(
            "Brain",
            "Failed to initialize neural networks",
            ErrorContext("System", "initialize", "system.cpp", 89)
        );
    } catch (const InitializationError& e) {
        std::cout << "   Error: " << e.what() << std::endl;
        std::cout << "   Component: " << e.getContext().component << std::endl;
    }
    
    std::cout << std::endl;
    
    // Show error report formatting
    std::cout << "Error Report Formatting Example:" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    try {
        throw ValidationError(
            "learning_rate",
            "between 0.0 and 1.0",
            "2.5",
            ErrorContext("Training", "validate", "training.cpp", 156)
        );
    } catch (const ValidationError& e) {
        std::cout << ErrorUtilities::formatErrorReport(e);
    }
    
    std::cout << "=" << std::string(50, '=') << std::endl << std::endl;
    
    // Summary
    std::cout << "=== Error Handling System Summary ===" << std::endl;
    std::cout << "The NLM error handling system provides:" << std::endl;
    std::cout << "✓ Comprehensive error classification (5 main categories)" << std::endl;
    std::cout << "✓ Type-safe error hierarchy with specific error subclasses" << std::endl;
    std::cout << "✓ Configurable error handling strategies (8 strategies)" << std::endl;
    std::cout << "✓ Recovery mechanisms for common error types" << std::endl;
    std::cout << "✓ Detailed error reporting with context" << std::endl;
    std::cout << "✓ Integration throughout the NLM codebase" << std::endl;
    std::cout << "✓ Human-readable and structured error information" << std::endl;
    std::cout << std::endl;
    std::cout << "The system is designed to be:" << std::endl;
    std::cout << "• Type-safe with proper exception hierarchy" << std::endl;
    std::cout << "• Configurable through error handling strategies" << std::endl;
    std::cout << "• Recoverable where possible" << std::endl;
    std::cout << "• Reportable with detailed context information" << std::endl;
    std::cout << "• Integratable across the entire NLM architecture" << std::endl;
}

} // namespace error_demo
} // namespace nlm

int main() {
    nlm::error_demo::demonstrateErrorHandlingSystem();
    return 0;
}
