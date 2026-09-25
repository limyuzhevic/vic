#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Error Hierarchy..." << std::endl;
    
    // Test configuration error
    try {
        throwConfigError("Invalid neuron count", NLMConfigError::ErrorType::InvalidParameter, "Brain initialization");
    } catch (const NLMConfigError& e) {
        std::cout << "Caught config error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
    }
    
    // Test memory error
    try {
        throwMemoryError("Allocation failed", NLMMemoryError::ErrorType::AllocationFailure, "WorkingMemory::store");
    } catch (const NLMMemoryError& e) {
        std::cout << "Caught memory error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
    }
    
    // Test plasticity error
    try {
        throwPlasticityError("Invalid synaptic weight", NLMPlasticityError::ErrorType::InvalidSynapticWeight, "STDP::update");
    } catch (const NLMPlasticityError& e) {
        std::cout << "Caught plasticity error: " << e.what() << std::endl;
        std::cout << "Error category: " << static_cast<int>(e.getCategory()) << std::endl;
    }
    
    // Test validation with recovery
    auto& errorHandler = getErrorHandler();
    bool isValid = [](int x) { return x > 0; };
    
    int result = errorHandler.validateAndRecover<int>(
        "validateInput", "InputProcessor", 5, isValid, 
        ErrorHandler::RecoveryStrategy::Fallback, 0, 
        "Input validation"
    );
    std::cout << "Validation result: " << result << std::endl;
    
    // Test performance reporting
    std::cout << "\n" << errorHandler.getPerformanceReport();
    
    std::cout << "\nError hierarchy test completed successfully!" << std::endl;
    return 0;
}
