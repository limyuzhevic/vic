// Test the new error handling system
// This file demonstrates the improved error handling capabilities of NLM

#include "core/Error/NLMException.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>

void testConfigurationErrors() {
    std::cout << "Testing configuration errors..." << std::endl;
    
    try {
        throw nlm::ConfigurationException("Invalid configuration value", "brain.neuron_count", "Config");
    } catch (const nlm::ConfigurationException& e) {
        std::cout << "Caught configuration error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Error severity: " << static_cast<int>(e.getSeverity()) << std::endl;
        std::cout << "Error key: " << e.getKey() << std::endl;
    }
}

void testMemoryErrors() {
    std::cout << "\nTesting memory errors..." << std::endl;
    
    try {
        throw nlm::MemoryException("Out of memory", 1024 * 1024 * 1024);  // 1GB
    } catch (const nlm::MemoryException& e) {
        std::cout << "Caught memory error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Size requested: " << e.getSizeRequested() << " bytes" << std::endl;
    }
}

void testNeuralErrors() {
    std::cout << "\nTesting neural errors..." << std::endl;
    
    NeuronId testNeuron(42);
    
    try {
        throw nlm::NeuralException("Invalid neuron state", testNeuron, "Brain");
    } catch (const nlm::NeuralException& e) {
        std::cout << "Caught neural error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Neuron ID: " << e.getNeuronId().index() << std::endl;
    }
}

void testPlasticityErrors() {
    std::cout << "\nTesting plasticity errors..." << std::endl;
    
    try {
        throw nlm::PlasticityException("STDP rule not found", "stdp", "Plasticity");
    } catch (const nlm::PlasticityException& e) {
        std::cout << "Caught plasticity error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Plasticity rule: " << e.getRule() << std::endl;
    }
}

void testIOErrors() {
    std::cout << "\nTesting I/O errors..." << std::endl;
    
    try {
        throw nlm::IOError("Cannot read file", "brain_state.bin", nlm::IOError::Operation::Read, "Checkpoint");
    } catch (const nlm::IOError& e) {
        std::cout << "Caught I/O error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Filepath: " << e.getFilepath() << std::endl;
        std::cout << "Operation: " << static_cast<int>(e.getOperation()) << std::endl;
    }
}

void testIntegrationErrors() {
    std::cout << "\nTesting integration errors..." << std::endl;
    
    try {
        throw nlm::IntegrationException("Memory system not integrated", "Integration");
    } catch (const nlm::IntegrationException& e) {
        std::cout << "Caught integration error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "Error severity: " << static_cast<int>(e.getSeverity()) << std::endl;
    }
}

void testErrorHandlingSystem() {
    std::cout << "\nTesting error handling system..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Test error reporter
    nlm::ErrorReporter& errorReporter = nlm::ErrorReporter::getInstance();
    
    // Add a test error handler
    auto testErrorHandler = std::make_shared<nlm::DefaultErrorHandler>();
    errorReporter.addErrorHandler(testErrorHandler);
    
    // Report an error
    nlm::ErrorContext context;
    context.operation = "Test operation";
    context.component = "TestComponent";
    context.suggestions.push_back("Check configuration");
    context.suggestions.push_back("Verify system state");
    
    try {
        throw nlm::NLMException("Test error for error handling system", nlm::ErrorCategory::Validation);
    } catch (const nlm::NLMException& e) {
        errorReporter.reportError(e, context);
    }
    
    // Check error statistics
    std::cout << "Total errors reported: " << errorReporter.getTotalErrors() << std::endl;
    std::cout << "Errors by category Configuration: " << errorReporter.getErrorsByCategory(nlm::ErrorCategory::Configuration) << std::endl;
    std::cout << "Errors by severity Error: " << errorReporter.getErrorsBySeverity(nlm::ErrorSeverity::Error) << std::endl;
    
    // Clean up
    errorReporter.removeErrorHandler(testErrorHandler);
}

void testErrorReportingMultipleErrors() {
    std::cout << "\nTesting multiple error reporting..." << std::endl;
    
    nlm::ErrorReporter& errorReporter = nlm::ErrorReporter::getInstance();
    
    // Clear statistics
    errorReporter.clearStatistics();
    
    // Report multiple different types of errors
    nlm::ErrorContext context;
    context.operation = "Brain initialization";
    
    try {
        throw nlm::ConfigurationException("Invalid config value", "brain.learning_rate");
    } catch (const nlm::NLMException& e) {
        errorReporter.reportError(e, context);
    }
    
    try {
        throw nlm::MemoryException("Memory allocation failed", 512 * 1024 * 1024);
    } catch (const nlm::NLMException& e) {
        errorReporter.reportError(e, context);
    }
    
    try {
        throw nlm::IOError("File not found", "checkpoints/ep001.bin", nlm::IOError::Operation::Read);
    } catch (const nlm::NLMException& e) {
        errorReporter.reportError(e, context);
    }
    
    // Print statistics
    std::cout << "Final error statistics:" << std::endl;
    std::cout << "Total errors: " << errorReporter.getTotalErrors() << std::endl;
    std::cout << "Configuration errors: " << errorReporter.getErrorsByCategory(nlm::ErrorCategory::Configuration) << std::endl;
    std::cout << "Memory errors: " << errorReporter.getErrorsByCategory(nlm::ErrorCategory::Memory) << std::endl;
    std::cout << "I/O errors: " << errorReporter.getErrorsByCategory(nlm::ErrorCategory::I/O) << std::endl;
    std::cout << "Error errors: " << errorReporter.getErrorsBySeverity(nlm::ErrorSeverity::Error) << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling Test Suite ===" << std::endl;
    
    testConfigurationErrors();
    testMemoryErrors();
    testNeuralErrors();
    testPlasticityErrors();
    testIOErrors();
    testIntegrationErrors();
    testErrorHandlingSystem();
    testErrorReportingMultipleErrors();
    
    std::cout << "\n=== All tests completed successfully ===" << std::endl;
    return 0;
}
