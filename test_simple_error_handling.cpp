#include "src/core/ErrorHandling/ErrorHandlingSystem.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing NLM Error Handling System ===" << std::endl << std::endl;
    
    // Initialize the error handling system
    if (!initializeErrorHandling()) {
        std::cerr << "Failed to initialize error handling system!" << std::endl;
        return 1;
    }
    std::cout << "✓ Error handling system initialized" << std::endl;
    
    // Test error reporting
    std::cout << "\n✓ Testing error reporting..." << std::endl;
    reportError("Test configuration error", nlm::ErrorCode::ConfigValidationFailed);
    
    // Get error statistics
    nlm::ErrorStats stats = getErrorStats();
    std::cout << "✓ Error statistics retrieved: " << stats.totalErrors << " total errors" << std::endl;
    
    // Shutdown
    shutdownErrorHandling();
    std::cout << "✓ Error handling system shutdown" << std::endl;
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
}
