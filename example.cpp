// Example demonstrating the NLM error handling and validation system

#include "core/ErrorHandling/ErrorHandlingSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>

int main() {
    // Initialize the error handling system
    std::cout << "Initializing NLM error handling system..." << std::endl;
    if (!initializeErrorHandling("", true)) {
        std::cerr << "Failed to initialize error handling system!" << std::endl;
        return 1;
    }
    std::cout << "Error handling system initialized successfully." << std::endl;
    
    try {
        // Example 1: Configuration validation
        std::cout << "\n1. Testing Configuration Validation..." << std::endl;
        nlm::Config config;
        
        // Try to set an empty key (should throw)
        try {
            config.set("", "value");
            std::cerr << "ERROR: Should have thrown exception for empty key!" << std::endl;
        } catch (const nlm::ValidationException& e) {
            std::cout << "Caught expected ValidationException: " << e.what() << std::endl;
        }
        
        // Example 2: Valid configuration
        std::cout << "\n2. Testing Valid Configuration..." << stdendl;
        config.set("simulation.steps", 1000);
        config.set("simulation.dt", 0.001);
        config.set("neuron.count", 100);
        
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Example 3: Neuron creation and validation
        std::cout << "\n3. Testing Neuron Creation..." << std::endl;
        nlm::Neuron neuron(nlm::NeuronId(1));
        std::cout << "Created neuron with ID: " << neuron.getId().index() << std::endl;
        
        // Try to create neuron with invalid ID (should throw)
        try {
            nlm::Neuron invalidNeuron(nlm::INVALID_NEURON_ID);
            std::cerr << "ERROR: Should have thrown exception for invalid neuron ID!" << std::endl;
        } catch (const nlm::ValidationException& e) {
            std::cout << "Caught expected ValidationException: " << e.what() << std::endl;
        }
        
        // Example 4: Error reporting
        std::cout << "\n4. Testing Error Reporting..." << std::endl;
        reportError("This is a test configuration error", nlm::ErrorCode::ConfigValidationFailed, nlm::ErrorCategory::Configuration);
        
        // Example 5: Exception reporting
        std::cout << "\n5. Testing Exception Reporting..." << std::endl;
        try {
            NLM_THROW_CONFIG("This is a test exception");
        } catch (const std::exception& e) {
            reportException(e);
            std::cout << "Exception reported successfully." << std::endl;
        }
        
        // Example 6: Get error statistics
        std::cout << "\n6. Error Statistics..." << std::endl;
        nlm::ErrorStats stats = getErrorStats();
        std::cout << stats.getStatsSummary() << std::endl;
        
        // Example 7: Enable graceful degradation
        std::cout << "\n7. Testing Graceful Degradation..." << std::endl;
        enableGracefulDegradation(true);
        std::cout << "Graceful degradation enabled." << std::endl;
        
        // Example 8: Configuration validation
        std::cout << "\n8. Testing Validation Utilities..." << std::endl;
        
        // Test parameter validation
        bool valid = nlm::ValidationUtils::isInRange(50.0, 0.0, 100.0);
        std::cout << "Range validation (50 in [0, 100]): " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Test state validation
        nlm::NeuronState state;
        valid = nlm::StateValidator::isValidState(state);
        std::cout << "Neuron state validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Test resource validation
        valid = nlm::ResourceValidator::validateMemory(1.0); // Check for 1GB memory
        std::cout << "Memory resource validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Example 9: Integration validation
        std::cout << "\n9. Testing Integration Validation..." << std::endl;
        std::vector<std::string> features = {"logging", "validation", "metrics"};
        valid = nlm::IntegrationValidator::validateComponentCompatibility(
            "test_component",
            {"logging", "validation"},
            features);
        std::cout << "Component compatibility validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        std::cout << "\nAll tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception: " << e.what() << std::endl;
        return 1;
    }
    
    // Shutdown the error handling system
    shutdownErrorHandling();
    std::cout << "\nError handling system shutdown complete." << std::endl;
    
    return 0;
}
