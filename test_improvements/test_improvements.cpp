// NLM (熙然) - Neural Learning Machine
// Tests for improved constants and error handling
// 
// This file tests the new constants system and error handling patterns
to verify they work correctly and provide the expected functionality.

#include "core/Constants.hpp"
#include "core/ErrorHandling.hpp"
#include <cassert>
#include <iostream>

namespace test_improvements {

void test_constants_system() {
    std::cout << "Testing constants system..." << std::endl;
    
    // Test neuron constants
    assert(nlm::constants::NeuronConstants::MEMBRANE_CAPACITANCE == 1.0f);
    assert(nlm::constants::NeuronConstants::TIME_CONSTANT == 20.0f);
    assert(nlm::constants::NeuronConstants::DEFAULT_MEMBRANE_POTENTIAL == -70.0f);
    assert(nlm::constants::NeuronConstants::DEFAULT_THRESHOLD == -55.0f);
    
    // Test synapse constants
    assert(nlm::constants::SynapseConstants::DEFAULT_STDP_LTP_WEIGHT == 0.01f);
    assert(nlm::constants::SynapseConstants::DEFAULT_SYNAPTOGENESIS_RATE == 0.0001f);
    
    // Test memory constants
    assert(nlm::constants::MemoryConstants::DEFAULT_WORKING_MEMORY_CAPACITY == 100);
    assert(nlm::constants::MemoryConstants::MEMORY_ACTIVATION_THRESHOLD == 0.1f);
    
    // Test simulation constants
    assert(nlm::constants::SimulationConstants::DEFAULT_TIMESTEP == 0.001);
    assert(nlm::constants::SimulationConstants::DEFAULT_REPLAY_INTERVAL == 100);
    
    std::cout << "✓ Constants system test passed" << std::endl;
}

void test_error_handling() {
    std::cout << "Testing error handling system..." << std::endl;
    
    // Test error info creation
    auto error = nlm::ErrorInfo(
        nlm::ErrorCategory::ValidationError,
        "Test error message",
        "Test context",
        "Test suggestion",
        123
    );
    
    assert(error.getCategory() == nlm::ErrorCategory::ValidationError);
    assert(error.getMessage() == "Test error message");
    assert(error.getContext() == "Test context");
    assert(error.getSuggestion() == "Test suggestion");
    assert(error.getErrorCode() == 123);
    assert(error.isRecoverable() == true);
    
    // Test error string conversion
    std::string errorStr = nlm::toString(error.getCategory());
    assert(!errorStr.empty());
    
    // Test error report generation
    std::string report = error.getReport();
    assert(report.find("Error Report") != std::string::npos);
    assert(report.find("Test error message") != std::string::npos);
    
    // Test success result
    auto result = nlm::Success<int>(42);
    assert(result.isSuccess() == true);
    assert(result.isFailure() == false);
    assert(result.value() == 42);
    assert(result.valueOr(0) == 42);
    
    // Test failure result
    auto failure = nlm::Failure<int>(
        nlm::ErrorCategory::MemoryAllocationError,
        "Out of memory",
        "Neuron creation",
        "Reduce neuron count or increase memory"
    );
    
    assert(failure.isSuccess() == false);
    assert(failure.isFailure() == true);
    assert(failure.hasCategory(nlm::ErrorCategory::MemoryAllocationError) == true);
    
    try {
        int value = failure.value();
        assert(false && "Should have thrown");
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()).find("Error Report") != std::string::npos);
    }
    
    // Test factory functions
    auto configError = nlm::ConfigFailure<int>("Invalid config file");
    assert(configError.hasCategory(nlm::ErrorCategory::ConfigError) == true);
    
    auto initError = nlm::InitFailure<int>("Initialization failed");
    assert(initError.hasCategory(nlm::ErrorCategory::InitializationError) == true);
    
    auto memoryError = nlm::MemoryFailure<int>("Out of memory");
    assert(memoryError.hasCategory(nlm::ErrorCategory::MemoryAllocationError) == true);
    
    auto validationError = nlm::ValidationFailure<int>("Invalid input");
    assert(validationError.hasCategory(nlm::ErrorCategory::ValidationError) == true);
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

void test_backward_compatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    // Test legacy namespace
    assert(nlm::legacy::MEMBRANE_CAPACITANCE == nlm::constants::NeuronConstants::MEMBRANE_CAPACITANCE);
    assert(nlm::legacy::TIME_CONSTANT == nlm::constants::NeuronConstants::TIME_CONSTANT);
    assert(nlm::legacy::DEFAULT_THRESHOLD == nlm::constants::NeuronConstants::DEFAULT_THRESHOLD);
    assert(nlm::legacy::DEFAULT_STDP_LTP_WEIGHT == nlm::constants::SynapseConstants::DEFAULT_STDP_LTP_WEIGHT);
    assert(nlm::legacy::DEFAULT_SYNAPTOGENESIS_RATE == nlm::constants::SynapseConstants::DEFAULT_SYNAPTOGENESIS_RATE);
    assert(nlm::legacy::DEFAULT_NEURON_COUNT == nlm::constants::ConfigConstants::DEFAULT_NEURON_COUNT);
    
    std::cout << "✓ Backward compatibility test passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "NLM Improvements Test Suite" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    test_constants_system();
    test_error_handling();
    test_backward_compatibility();
    
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "All tests passed! ✓" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
}

} // namespace test_improvements

int main() {
    test_improvements::run_all_tests();
    return 0;
}
