// Simple demonstration of the NLM error handling system

#include "src/core/ErrorHandling/ErrorHandlingSystem.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Error Handling System Demo ===" << std::endl << std::endl;
    
    // Initialize the error handling system
    std::cout << "1. Initializing error handling system..." << std::endl;
    if (!initializeErrorHandling()) {
        std::cerr << "ERROR: Failed to initialize error handling system!" << std::endl;
        return 1;
    }
    std::cout << "   ✓ Error handling system initialized successfully." << std::endl;
    
    try {
        // Example: Report a configuration error
        std::cout << "\n2. Reporting configuration error..." << std::endl;
        reportError("Configuration file not found", nlm::ErrorCode::ConfigValidationFailed, nlm::ErrorCategory::Configuration);
        std::cout << "   ✓ Configuration error reported successfully." << std::endl;
        
        // Example: Report a validation error
        std::cout << "\n3. Reporting validation error..." << std::endl;
        reportError("Invalid parameter value", nlm::ErrorCode::ParameterInvalid, nlm::ErrorCategory::Validation);
        std::cout << "   ✓ Validation error reported successfully." << std::endl;
        
        // Example: Report a neural computation error
        std::cout << "\n4. Reporting neural computation error..." << std::endl;
        reportError("Neuron not properly initialized", nlm::ErrorCode::NeuronInvalidState, nlm::ErrorCategory::NeuralComputation);
        std::cout << "   ✓ Neural computation error reported successfully." << std::endl;
        
        // Example: Report an I/O error
        std::cout << "\n5. Reporting I/O error..." << std::endl;
        reportError("File not found: config.json", nlm::ErrorCode::FileNotFound, nlm::ErrorCategory::IO);
        std::cout << "   ✓ I/O error reported successfully." << std::endl;
        
        // Example: Report a resource error
        std::cout << "\n6. Reporting resource error..." << std::endl;
        reportError("Memory allocation failed", nlm::ErrorCode::MemoryAllocationFailed, nlm::ErrorCategory::Memory);
        std::cout << "   ✓ Resource error reported successfully." << std::endl;
        
        // Example: Get error statistics
        std::cout << "\n7. Getting error statistics..." << std::endl;
        nlm::ErrorStats stats = getErrorStats();
        std::cout << "   Total errors: " << stats.totalErrors << std::endl;
        std::cout << "   Error statistics retrieved successfully." << std::endl;
        
        // Example: Enable graceful degradation
        std::cout << "\n8. Enabling graceful degradation..." << std::endl;
        enableGracefulDegradation(true);
        std::cout << "   ✓ Graceful degradation enabled." << std::endl;
        
        // Example: Set global error level
        std::cout << "\n9. Setting global error level..." << std::endl;
        setGlobalErrorLevel(nlm::LogLevel::Error);
        std::cout << "   ✓ Global error level set to Error." << std::endl;
        
        std::cout << "\n=== All demonstrations completed successfully! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception: " << e.what() << std::endl;
        return 1;
    }
    
    // Shutdown the error handling system
    std::cout << "\n10. Shutting down error handling system..." << std::endl;
    shutdownErrorHandling();
    std::cout << "   ✓ Error handling system shutdown complete." << std::endl;
    
    return 0;
}
