#!/usr/bin/env g++ -std=c++20 -O2 -I./src -o test_error_handling
// Compile: g++ -std=c++20 -O2 -I./src -o test_error_handling test_error_handling_implementation.cpp

// Test file to demonstrate NLM error handling capabilities
// This file can be compiled and run to test the error handling implementation

#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "Testing NLM Error Handling Implementation" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Get global error handler
    auto& errorHandler = getErrorHandler();
    
    // Test 1: Configuration error handling
    std::cout << "\nTest 1: Configuration Error Handling" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    try {
        throw NLMConfigError("Invalid neuron count for simulation", 
                            NLMConfigError::ErrorType::InvalidParameter,
                            "SimulationSetup");
    } catch (const NLMConfigError& e) {
        std::cout << "✓ Caught configuration error: " << e.what() << std::endl;
        std::cout << "  Category: " << static_cast<int>(e.getCategory()) << std::endl;
        std::cout << "  Context: " << e.getContext() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 2: Memory error handling
    std::cout << "\nTest 2: Memory Error Handling" << std::endl;
    std::cout << "------------------------------" << std::endl;
    try {
        throw NLMMemoryError("Allocation failed for memory trace", 
                            NLMMemoryError::ErrorType::AllocationFailure,
                            "WorkingMemory::store");
    } catch (const NLMMemoryError& e) {
        std::cout << "✓ Caught memory error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 3: Plasticity error handling
    std::cout << "\nTest 3: Plasticity Error Handling" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    try {
        throw NLMPlasticityError("Invalid synaptic weight for STDP update", 
                               NLMPlasticityError::ErrorType::InvalidSynapticWeight,
                               "STDP::update");
    } catch (const NLMPlasticityError& e) {
        std::cout << "✓ Caught plasticity error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 4: Neuromodulation error handling
    std::cout << "\nTest 4: Neuromodulation Error Handling" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    try {
        throw NLMNeuromodulationError("Dopamine level out of valid range", 
                                     NLMNeuromodulationError::ErrorType::DopamineError,
                                     "Dopamine::setLevel");
    } catch (const NLMNeuromodulationError& e) {
        std::cout << "✓ Caught neuromodulation error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 5: Development error handling
    std::cout << "\nTest 5: Development Error Handling" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    try {
        throw NLMDevelopmentError("Synaptogenesis failed during critical period", 
                                 NLMDevelopmentError::ErrorType::SynaptogenesisError,
                                 "DevelopmentSystem::update");
    } catch (const NLMDevelopmentError& e) {
        std::cout << "✓ Caught development error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 6: Environment error handling
    std::cout << "\nTest 6: Environment Error Handling" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    try {
        throw NLMEnvironmentError("Boundary violation in world simulation", 
                                  NLMEnvironmentError::ErrorType::BoundaryViolation,
                                  "World::update");
    } catch (const NLMEnvironmentError& e) {
        std::cout << "✓ Caught environment error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 7: Action error handling
    std::cout << "\nTest 7: Action Error Handling" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    try {
        throw NLMActionError("Action selection failed due to invalid state", 
                            NLMActionError::ErrorType::ActionSelectionError,
                            "Brain::produceAction");
    } catch (const NLMActionError& e) {
        std::cout << "✓ Caught action error: " << e.what() << std::endl;
        std::cout << "  Error Type: " << static_cast<int>(e.getErrorType()) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 8: Error performance tracking
    std::cout << "\nTest 8: Performance Tracking" << std::endl;
    std::cout << "---------------------------" << std::endl;
    try {
        // Simulate some error handling operations
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            try {
                throw NLMConfigError("Test error", NLMConfigError::ErrorType::InvalidParameter, "Test");
            } catch (...) {}
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::micro>(endTime - startTime);
        errorHandler.recordPerformanceImpact("errorHandling", "validation", duration.count());
        
        std::cout << "✓ Performance tracking completed" << std::endl;
        std::cout << "  Error handling performance: " << duration.count() << " microseconds" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 9: Validation with recovery
    std::cout << "\nTest 9: Validation with Recovery" << std::endl;
    std::cout << "------------------------------" << std::endl;
    try {
        auto& handler = getErrorHandler();
        
        // Test valid value
        int result = handler.validateAndRecover<int>(
            "validateInput", "InputProcessor", 5,
            [](int x) { return x > 0; },
            ErrorHandler::RecoveryStrategy::Fallback, 0,
            "Input validation"
        );
        std::cout << "✓ Valid input passed validation: " << result << std::endl;
        
        // Test invalid value with recovery
        int invalidResult = handler.validateAndRecover<int>(
            "validateInput", "InputProcessor", -1,
            [](int x) { return x > 0; },
            ErrorHandler::RecoveryStrategy::Fallback, 1,
            "Input validation"
        );
        std::cout << "✓ Invalid input handled with recovery: " << invalidResult << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    // Test 10: Performance report
    std::cout << "\nTest 10: Performance Report" << std::endl;
    std::cout << "--------------------------" << std::endl;
    try {
        std::string report = errorHandler.getPerformanceReport();
        std::cout << "✓ Performance report generated:" << std::endl;
        std::cout << report << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Error Handling Implementation Test Complete" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    return 0;
}
