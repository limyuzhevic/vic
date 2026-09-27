// Simple demonstration of the NLM Error Handling System
#include "core/Error/Error.hpp"
#include "core/ValidationEngine/ValidationEngine.hpp"
#include "core/ErrorReporter/ErrorReporter.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Error Handling System Demonstration ===" << std::endl << std::endl;
    
    // Initialize error reporter
    nlm::ErrorReporter& reporter = nlm::ErrorReporter::getGlobal();
    
    // Demonstrate basic error classification
    std::cout << "1. Testing Error Classification:" << std::endl;
    
    auto systemError = std::make_shared<nlm::SystemError>("System initialization failed");
    std::cout << "   System Error: " << systemError->getMessage() 
              << " (Type: " << static_cast<int>(systemError->getType()) << ")" 
              << " (Severity: " << static_cast<int>(systemError->getSeverity()) << ")" << std::endl;
    
    auto configError = std::make_shared<nlm::ConfigurationError>("Invalid configuration value");
    std::cout << "   Configuration Error: " << configError->getMessage() 
              << " (Type: " << static_cast<int>(configError->getType()) << ")" << std::endl;
    
    auto memoryError = std::make_shared<nlm::MemoryError>("Out of memory");
    std::cout << "   Memory Error: " << memoryError->getMessage() 
              << " (Type: " << static_cast<int>(memoryError->getType()) << ")" << std::endl;
    
    // Report errors
    std::cout << "\n2. Reporting Errors:" << std::endl;
    reporter.logError("Configuration file not found: configs/default.cfg");
    reporter.logWarning("Invalid configuration value for key 'learning_rate'");
    reporter.logInfo("System initialization started");
    reporter.logDebug("Debug information for neural network setup");
    reporter.logCritical("Critical system failure - shutting down");
    
    // Show error statistics
    std::cout << "\n3. Error Statistics:" << std::endl;
    std::cout << "   Total errors: " << reporter.getErrorCount() << std::endl;
    std::cout << "   Error format: " << reporter.formatAllErrors() << std::endl;
    
    // Test validation engine
    std::cout << "\n4. Testing Validation Engine:" << std::endl;
    
    nlm::ValidationEngine validationEngine;
    
    // Add validation rules
    validationEngine.addRule("positive_number",
                           "Validate that number is positive",
                           [](const std::vector<std::string>& data) {
                               try {
                                   double value = std::stod(data[0]);
                                   return value > 0.0;
                               } catch (...) {
                                   return false;
                               }
                           });
    
    // Test validation
    nlm::ValidationResult validResult = validationEngine.validate({"5.0"}, "test_context");
    std::cout << "   Valid number '5.0': " << (validResult.valid ? "✓" : "✗") << std::endl;
    
    nlm::ValidationResult invalidResult = validationEngine.validate({"-2.0"}, "test_context");
    std::cout << "   Invalid number '-2.0': " << (!invalidResult.valid ? "✓" : "✗") << std::endl;
    
    // Test enhanced error
    std::cout << "\n5. Testing Enhanced Error:" << std::endl;
    
    nlm::ErrorContext context;
    context.operation = "neural_simulation";
    context.component = "brain";
    context.metadata["simulation_id"] = "123";
    context.tags["environment"] = "production";
    
    auto enhancedError = std::make_shared<nlm::EnhancedError>(
        "Neural network training failed",
        nlm::ErrorType::NeuralError,
        nlm::ErrorSeverity::Error,
        "neural.cpp",
        42,
        "trainNetwork",
        "Training",
        context
    );
    
    std::cout << "   Enhanced Error Context: " << enhancedError->formatWithContext() << std::endl;
    std::cout << "   Has metadata 'simulation_id': " << (enhancedError->hasContext("simulation_id") ? "✓" : "✗") << std::endl;
    
    // Report the enhanced error
    reporter.report(enhancedError);
    
    // Show final error count
    std::cout << "\n6. Final Error Statistics:" << std::endl;
    std::cout << "   Total errors reported: " << reporter.getErrorCount() << std::endl;
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    return 0;
}