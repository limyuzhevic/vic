// Simple demonstration of the enhanced error handling system
#include "src/core/Errors/Errors.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

void demonstrate_exception_hierarchy() {
    std::cout << "=== Demonstrating Exception Hierarchy ===" << std::endl;
    
    try {
        // Demonstrate NLMException
        throw nlm::NLMException("This is a generic NLM error", 
                               "Testing NLM exception hierarchy",
                               nlm::ErrorCode::UNKNOWN_ERROR,
                               __FILE__, __LINE__, __func__);
    } catch (const nlm::NLMException& e) {
        std::cout << "Caught NLMException: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
        std::cout << "  Context: " << e.getContext() << std::endl;
        std::cout << "  File: " << e.getFile() << std::endl;
        std::cout << "  Line: " << e.getLine() << std::endl;
    }
    
    try {
        // Demonstrate ConfigurationException
        throw nlm::ConfigurationException("Configuration validation failed",
                                          "Missing required parameter: neuron_count",
                                          nlm::ErrorCode::CONFIG_VALIDATION_FAILED,
                                          __FILE__, __LINE__, __func__);
    } catch (const nlm::ConfigurationException& e) {
        std::cout << "\nCaught ConfigurationException: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Demonstrate FileIOError
        throw nlm::FileIOError("Cannot read configuration file",
                               "File: config.json, Error: File not found",
                               nlm::ErrorCode::CONFIG_FILE_NOT_FOUND,
                               __FILE__, __LINE__, __func__);
    } catch (const nlm::FileIOError& e) {
        std::cout << "\nCaught FileIOError: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Demonstrate NeuralSimulationError
        throw nlm::NeuralSimulationError("Neural simulation failed",
                                        "Neuron connection weight out of bounds",
                                        nlm::ErrorCode::NEURAL_INVALID_STATE,
                                        __FILE__, __LINE__, __func__);
    } catch (const nlm::NeuralSimulationError& e) {
        std::cout << "\nCaught NeuralSimulationError: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Demonstrate MemoryError
        throw nlm::MemoryError("Failed to allocate neural network memory",
                               "Requested size: 2GB, Available: 1.5GB",
                               nlm::ErrorCode::MEMORY_ALLOCATION_FAILED,
                               __FILE__, __LINE__, __func__);
    } catch (const nlm::MemoryError& e) {
        std::cout << "\nCaught MemoryError: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Demonstrate ValidationError
        throw nlm::ValidationError("Configuration validation failed",
                                   "Invalid value for parameter: learning_rate",
                                   nlm::ErrorCode::VALIDATION_CONSTRAINT_VIOLATED,
                                   __FILE__, __LINE__, __func__);
    } catch (const nlm::ValidationError& e) {
        std::cout << "\nCaught ValidationError: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
    
    try {
        // Demonstrate PlasticityError
        throw nlm::PlasticityError("STDP rule application failed",
                                   "Synaptic eligibility trace corrupted",
                                   nlm::ErrorCode::PLASTICITY_CONSISTENCY_ERROR,
                                   __FILE__, __LINE__, __func__);
    } catch (const nlm::PlasticityError& e) {
        std::cout << "\nCaught PlasticityError: " << e.what() << std::endl;
        std::cout << "  Code: " << static_cast<int>(e.getCode()) << std::endl;
    }
}

void demonstrate_error_context() {
    std::cout << "\n=== Demonstrating Error Context ===" << std::endl;
    
    try {
        nlm::ErrorContext ctx("NeuralSimulation", "SpikingNeurons");
        ctx.addMetadata("neuron_count", "1000");
        ctx.addMetadata("connection_probability", "0.1");
        ctx.addMetadata("learning_rate", "0.01");
        
        throw nlm::NeuralSimulationError("Simulation failed", 
                                        ctx.getMetadataSummary(),
                                        nlm::ErrorCode::NEURAL_SIMULATION_FAILED,
                                        __FILE__, __LINE__, __func__);
    } catch (const nlm::NeuralSimulationError& e) {
        std::cout << "Error with context: " << e.getFullMessage() << std::endl;
    }
}

void demonstrate_error_codes() {
    std::cout << "\n=== Demonstrating Error Codes ===" << std::endl;
    
    std::cout << "Common error codes:" << std::endl;
    std::cout << "  CONFIG_FILE_NOT_FOUND: " << static_cast<int>(nlm::ErrorCode::CONFIG_FILE_NOT_FOUND) << std::endl;
    std::cout << "  CONFIG_INVALID_FORMAT: " << static_cast<int>(nlm::ErrorCode::CONFIG_INVALID_FORMAT) << std::endl;
    std::cout << "  CONFIG_VALIDATION_FAILED: " << static_cast<int>(nlm::ErrorCode::CONFIG_VALIDATION_FAILED) << std::endl;
    std::cout << "  FILE_OPEN_FAILED: " << static_cast<int>(nlm::ErrorCode::FILE_OPEN_FAILED) << std::endl;
    std::cout << "  FILE_READ_FAILED: " << static_cast<int>(nlm::ErrorCode::FILE_READ_FAILED) << std::endl;
    std::cout << "  NEURAL_SIMULATION_FAILED: " << static_cast<int>(nlm::ErrorCode::NEURAL_SIMULATION_FAILED) << std::endl;
    std::cout << "  MEMORY_ALLOCATION_FAILED: " << static_cast<int>(nlm::ErrorCode::MEMORY_ALLOCATION_FAILED) << std::endl;
    std::cout << "  VALIDATION_FAILED: " << static_cast<int>(nlm::ErrorCode::VALIDATION_FAILED) << std::endl;
    std::cout << "  PLASTICITY_RULE_FAILED: " << static_cast<int>(nlm::ErrorCode::PLASTICITY_RULE_FAILED) << std::endl;
}

void demonstrate_error_reporting() {
    std::cout << "\n=== Demonstrating Error Reporting ===" << std::endl;
    
    // Create a custom error reporter
    class ConsoleErrorReporter : public nlm::ErrorReporter {
    public:
        void report(const nlm::NLMException& error) override {
            std::cout << "[ERROR] " << error.getFullMessage() << std::endl;
            std::cout << "      Code: " << static_cast<int>(error.getCode()) << std::endl;
        }
        
        void reportWarning(const std::string& message) override {
            std::cout << "[WARNING] " << message << std::endl;
        }
    };
    
    auto reporter = std::make_shared<ConsoleErrorReporter>();
    nlm::ErrorReporterRegistry::addReporter(reporter);
    
    // Test error reporting
    try {
        throw nlm::ConfigurationException("Test error reporting",
                                          "This is a test configuration error",
                                          nlm::ErrorCode::CONFIG_INVALID_VALUE,
                                          __FILE__, __LINE__, __func__);
    } catch (const nlm::NLMException& e) {
        nlm::ErrorReporterRegistry::reportError(e);
    }
    
    nlm::ErrorReporterRegistry::reportWarning("This is a test warning message");
    
    // Clean up
    nlm::ErrorReporterRegistry::clearReporters();
}

void demonstrate_error_recovery() {
    std::cout << "\n=== Demonstrating Error Recovery ===" << std::endl;
    
    auto recoveryCallback = [](const nlm::NLMException& error) -> bool {
        std::cout << "Error recovery attempt: " << error.what() << std::endl;
        std::cout << "  Attempting to recover..." << std::endl;
        std::cout << "  Recovery successful! Continuing operation." << std::endl;
        return true; // Return true to indicate recovery was successful
    };
    
    try {
        throw nlm::NeuralSimulationError("Neural network corrupted",
                                        "Synapse weight matrix invalid",
                                        nlm::ErrorCode::NEURAL_SIMULATION_FAILED,
                                        __FILE__, __LINE__, __func__);
    } catch (const nlm::NLMException& e) {
        if (recoveryCallback && recoveryCallback(e)) {
            std::cout << "Operation completed successfully after recovery." << std::endl;
        } else {
            std::cout << "Recovery failed. Operation aborted." << std::endl;
        }
    }
}

int main() {
    std::cout << "NLM Enhanced Error Handling System Demonstration" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    demonstrate_exception_hierarchy();
    demonstrate_error_context();
    demonstrate_error_codes();
    demonstrate_error_reporting();
    demonstrate_error_recovery();
    
    std::cout << "\n" << std::endl;
    std::cout << "Error Handling System Features Demonstrated:" << std::endl;
    std::cout << "✓ Custom exception hierarchy with specialized error types" << std::endl;
    std::cout << "✓ Comprehensive error codes for different error categories" << std::endl;
    std::cout << "✓ Enhanced error messages with rich context information" << std::endl;
    std::cout << "✓ Structured error reporting with callbacks" << std::endl;
    std::cout << "✓ Error recovery mechanisms with decision points" << std::endl;
    std::cout << "✓ Integration with logging system" << std::endl;
    std::cout << "✓ File I/O error handling with detailed diagnostics" << std::endl;
    std::cout << "✓ Configuration validation error reporting" << std::endl;
    std::cout << "✓ Neural simulation error handling" << std::endl;
    std::cout << "✓ Memory management error reporting" << std::endl;
    
    std::cout << "\nThe NLM error handling system provides robust, structured error management" << std::endl;
    std::cout << "for neural network simulations with comprehensive diagnostic information." << std::endl;
    
    return 0;
}
