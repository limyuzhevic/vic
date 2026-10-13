#include <iostream>
#include <cassert>
#include "../src/core/Exceptions/NLMErrors.hpp"

int main() {
    std::cout << "Testing NLM Exception Hierarchy..." << std::endl;
    
    // Test 1: Create a ConfigurationException
    try {
        nlm::ConfigurationException exc(
            nlm::ConfigurationException::ConfigurationErrorCode::InvalidParameter,
            "Invalid configuration parameter",
            __FILE__,
            __LINE__,
            "main"
        );
        exc.addMetadata("param_name", "learning_rate");
        exc.addMetadata("expected_type", "double");
        
        std::cout << "Test 1 - ConfigurationException: " << exc.what() << std::endl;
        std::cout << "  Error code: " << static_cast<int>(exc.getErrorCode()) << std::endl;
        std::cout << "  Metadata - param_name: " << exc.getMetadata("param_name") << std::endl;
        std::cout << "  Serialized: " << exc.serialize() << std::endl;
    } catch (...) {
        std::cout << "Test 1 FAILED" << std::endl;
        return 1;
    }
    
    // Test 2: Create an InitializationException with cause
    try {
        nlm::InitializationException init_exc(
            nlm::InitializationException::InitializationErrorCode::Failed,
            "Initialization failed: out of memory",
            __FILE__,
            __LINE__,
            "main"
        );
        
        nlm::MemoryException mem_exc(
            nlm::MemoryException::MemoryErrorCode::OutOfMemory,
            "Failed to allocate memory for neural network",
            __FILE__,
            __LINE__,
            "main"
        );
        
        init_exc.setCause(std::make_shared<nlm::MemoryException>(mem_exc));
        
        std::cout << "Test 2 - InitializationException with cause: " << init_exc.what() << std::endl;
        std::cout << "  Formatted with cause: " << init_exc.formatWithCause() << std::endl;
        
        // Test cloning
        auto cloned = init_exc.clone();
        assert(cloned->getMessage() == init_exc.getMessage());
        assert(cloned->hasCause() == init_exc.hasCause());
    } catch (...) {
        std::cout << "Test 2 FAILED" << std::endl;
        return 1;
    }
    
    // Test 3: Create and throw SimulationException
    try {
        nlm::SimulationException sim_exc(
            nlm::SimulationException::SimulationErrorCode::ConvergenceFailed,
            "Simulation failed to converge after 1000 steps",
            __FILE__,
            __LINE__,
            "main"
        );
        
        sim_exc.addMetadata("max_iterations", "1000");
        sim_exc.addMetadata("current_iterations", "1000");
        
        std::cout << "Test 3 - SimulationException: " << sim_exc.what() << std::endl;
        std::cout << "  Error code: " << static_cast<int>(sim_exc.getErrorCode()) << std::endl;
    } catch (...) {
        std::cout << "Test 3 FAILED" << std::endl;
        return 1;
    }
    
    // Test 4: Test different exception types
    std::vector<std::shared_ptr<nlm::NLMException>> exceptions;
    
    exceptions.push_back(std::make_shared<nlm::MemoryException>(
        nlm::MemoryException::MemoryErrorCode::AllocationFailed,
        "Memory allocation failed"
    ));
    
    exceptions.push_back(std::make_shared<nlm::IOException>(
        nlm::IOException::IOErrorCode::FileReadError,
        "Failed to read file"
    ));
    
    exceptions.push_back(std::make_shared<nlm::NeuralException>(
        nlm::NeuralException::NeuralErrorCode::InvalidTopology,
        "Invalid neural network topology"
    ));
    
    exceptions.push_back(std::make_shared<nlm::SystemException>(
        nlm::SystemException::SystemErrorCode::SystemCallFailed,
        "System call failed"
    ));
    
    std::cout << "Test 4 - Exception hierarchy test passed" << std::endl;
    for (const auto& exc : exceptions) {
        std::cout << "  - " << exc->what() << " (Code: " << static_cast<int>(exc->getErrorCode()) << ")" << std::endl;
    }
    
    // Test 5: Test exception chaining with helper functions
    try {
        // Simulate a chain of exceptions
        nlm::ConfigurationException config_exc(
            nlm::ConfigurationException::ConfigurationErrorCode::FileNotFound,
            "Configuration file not found"
        );
        
        nlm::IOException io_exc(
            nlm::IOException::IOErrorCode::FileNotFound,
            "Cannot open config file"
        );
        
        io_exc.setCause(std::make_shared<nlm::ConfigurationException>(config_exc));
        
        std::cout << "Test 5 - Exception chaining: " << io_exc.formatWithCause() << std::endl;
    } catch (...) {
        std::cout << "Test 5 FAILED" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
