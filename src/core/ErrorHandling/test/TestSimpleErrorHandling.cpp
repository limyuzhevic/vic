// Simple test to verify the error handling system
#include <iostream>
#include <string>

// Include the main header to expose error handling functions
#include "core/ErrorHandling/include/nlm/ErrorHandling.hpp"

int main() {
    std::cout << "=== Simple Error Handling Test ===" << std::endl;
    
    // Initialize error handling
    std::unordered_map<std::string, std::string> config;
    config["min_handled_severity"] = "WARNING";
    config["error_retention_hours"] = "24";
    config["enable_structured_reporting"] = "true";
    config["enable_error_correlation"] = "true";
    
    nlm::initializeErrorHandling(config);
    
    std::cout << "Error handling system initialized successfully." << std::endl;
    
    // Create and handle an error
    auto error = nlm::ConfigurationError::createConfigurationError(
        "Test configuration error",
        "test_field",
        "Use default configuration"
    );
    
    std::cout << "Error created: " << error->getMessage() << std::endl;
    std::cout << "Error category: " << static_cast<int>(error->getCategory()) << std::endl;
    std::cout << "Error severity: " << static_cast<int>(error->getSeverity()) << std::endl;
    
    // Handle the error
    nlm::handleError(std::move(error));
    
    std::cout << "Error handled successfully." << std::endl;
    
    // Check error statistics
    auto stats = nlm::getErrorHandlingStats();
    std::cout << "Total errors: " << stats.totalErrors << std::endl;
    std::cout << "Total recoveries: " << stats.totalRecoveries << std::endl;
    
    std::cout << "=== Test Completed Successfully ===" << std::endl;
    return 0;
}