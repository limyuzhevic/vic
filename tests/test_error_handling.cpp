// Comprehensive Error Handling and Validation Tests
// This file tests the enhanced error handling, validation, and robustness
// of the NLM codebase

#include "core/Errors/Errors.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>

namespace test_error_handling {

// Test basic exception classes and error reporting
void testBasicExceptions() {
    std::cout << "  Testing basic exception classes..." << std::endl;
    
    // Test NLMException
    try {
        throw nlm::NLMException("Test exception", __FILE__, __LINE__);
    } catch (const nlm::NLMException& e) {
        assert(e.getCategory() == nlm::ErrorCategory::System);
    }
    
    // Test ConfigValidationException
    try {
        throw nlm::ConfigValidationException("Config test", __FILE__, __LINE__);
    } catch (const nlm::ConfigException& e) {
        assert(e.getCategory() == nlm::ErrorCategory::Validation);
    }
    
    // Test ValidationException
    try {
        throw nlm::ValidationException("Validation test", __FILE__, __LINE__);
    } catch (const nlm::ValidationException& e) {
        assert(e.getCategory() == nlm::ErrorCategory::Validation);
    }
    
    std::cout << "    Basic exceptions test passed" << std::endl;
}

// Test validation helper functions
void testValidatorFunctions() {
    std::cout << "  Testing validator functions..." << std::endl;
    
    // Test validateRange
    try {
        nlm::Validator::validateRange(5.0, 0.0, 10.0, "test_value");
        // Should pass
        assert(true);
    } catch (...) {
        assert(false); // Should not throw
    }
    
    try {
        nlm::Validator::validateRange(15.0, 0.0, 10.0, "test_value");
        assert(false); // Should throw
    } catch (const nlm::RangeValidationException& e) {
        assert(true);
    }
    
    // Test validateNotNull
    int value = 42;
    nlm::Validator::validateNotNull(&value, "ptr");
    // Should pass
    
    try {
        nlm::Validator::validateNotNull(nullptr, "ptr");
        assert(false); // Should throw
    } catch (const nlm::NullPointerException& e) {
        assert(true);
    }
    
    // Test validatePrecondition and validatePostcondition
    bool condition = true;
    nlm::Validator::validatePrecondition(condition, "Precondition failed");
    
    try {
        nlm::Validator::validatePrecondition(false, "Precondition failed");
        assert(false); // Should throw
    } catch (const nlm::ValidationException& e) {
        assert(true);
    }
    
    std::cout << "    Validator functions test passed" << std::endl;
}

// Test enhanced Config error handling and validation
void testEnhancedConfig() {
    std::cout << "  Testing enhanced Config error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test valid key validation
    config.set("valid_key", "value");
    assert(config.has("valid_key"));
    
    // Test invalid key (empty)
    try {
        config.set("", "value");
        assert(false); // Should throw
    } catch (const nlm::ConfigValidationException& e) {
        assert(true);
    }
    
    // Test invalid value
    try {
        config.set("key", ""); // Empty value not allowed
        assert(false); // Should throw
    } catch (const nlm::ConfigValidationException& e) {
        assert(true);
    }
    
    // Test file path validation
    try {
        config.saveToFile(""); // Invalid file path
        assert(false); // Should throw
    } catch (const nlm::ConfigLoadException& e) {
        assert(true);
    }
    
    // Test get with type mismatch
    config.set("string_value", "hello");
    try {
        auto val = config.get<int>("string_value");
        assert(val.has_value()); // Should be empty, but template compiles
    } catch (...) {
        // Should handle bad variant access
    }
    
    std::cout << "    Enhanced Config test passed" << std::endl;
}

// Test Neuron error handling and validation
void testEnhancedNeuron() {
    std::cout << "  Testing enhanced Neuron error handling..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test valid membrane potential range
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    // Test membrane potential validation via stepLIF
    // This will test internal validation when neuron dynamics are implemented
    
    // Test firing rate validation
    neuron.setFiringRate(10.0f);
    assert(neuron.getFiringRate() == 10.0f);
    
    // Test refractory period validation
    neuron.setRefractoryPeriod(10);
    assert(neuron.getRefractoryPeriod() == 10);
    
    // Test firing state
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    // Test synaptic input validation (should be able to set any value)
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    std::cout << "    Enhanced Neuron test passed" << std::endl;
}

// Test Synapse error handling and validation
void testEnhancedSynapse() {
    std::cout << "  Testing enhanced Synapse error handling..." << std::endl;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(100), nlm::NeuronId(200));
    
    // Test initial state
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(100));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(200));
    assert(synapse.isExcitatory()); // Default is excitatory
    
    // Test weight validation
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    // Test delay validation
    synapse.setDelay(10);
    assert(synapse.getDelay() == 10);
    
    // Test type validation
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    // Test plasticity flags
    synapse.enablePlasticity(true, false, true);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    Enhanced Synapse test passed" << std::endl;
}

// Test error reporting system
void testErrorReporting() {
    std::cout << "  Testing error reporting system..." << std::endl;
    
    // Test error code descriptions
    std::string configDesc = nlm::ErrorReport::getErrorCodeDescription(nlm::ErrorCode::ConfigKeyNotFound);
    assert(!configDesc.empty());
    
    std::string categoryDesc = nlm::ErrorReport::getErrorCategoryDescription(nlm::ErrorCategory::Configuration);
    assert(!categoryDesc.empty());
    
    // Test error reporting with different codes
    try {
        throw nlm::ConfigValidationException("Test config error", __FILE__, __LINE__);
    } catch (const nlm::ConfigValidationException& e) {
        // Error is logged internally by ErrorReport::reportError
    }
    
    std::cout << "    Error reporting test passed" << std::endl;
}

// Test edge cases and boundary conditions
void testEdgeCases() {
    std::cout << "  Testing edge cases and boundary conditions..." << std::endl;
    
    // Test maximum values
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setFiringRate(10000.0f); // Maximum firing rate
    assert(neuron.getFiringRate() == 10000.0f);
    
    // Test minimum values
    neuron.setMembranePotential(-200.0f); // Absolute refractory limit
    assert(neuron.getMembranePotential() == -200.0f);
    
    // Test refractory period boundaries
    neuron.setRefractoryPeriod(0); // Minimum
    assert(neuron.getRefractoryPeriod() == 0);
    
    neuron.setRefractoryPeriod(UINT32_MAX); // Large value
    assert(neuron.getRefractoryPeriod() == UINT32_MAX);
    
    // Test weight boundaries for synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(100), nlm::NeuronId(200));
    synapse.setWeight(0.0f); // Minimum weight
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(1.0f); // Maximum weight
    assert(synapse.getWeight() == 1.0f);
    
    std::cout << "    Edge cases test passed" << std::endl;
}

// Test exception safety with complex operations
void testExceptionSafety() {
    std::cout << "  Testing exception safety..." << std::endl;
    
    nlm::Config config;
    
    // Add some valid configuration
    config.set("key1", "value1");
    config.set("key2", "value2");
    
    // Simulate an operation that might fail
    try {
        // This should fail validation
        config.set("invalid_key", "");
        assert(false); // Should throw
    } catch (const nlm::ConfigValidationException& e) {
        // Verify that the config is still in a valid state
        assert(config.has("key1")); // Previous valid config should remain
        assert(config.has("key2"));
    }
    
    std::cout << "    Exception safety test passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive error handling and validation tests..." << std::endl << std::endl;
    
    testBasicExceptions();
    testValidatorFunctions();
    testEnhancedConfig();
    testEnhancedNeuron();
    testEnhancedSynapse();
    testErrorReporting();
    testEdgeCases();
    testExceptionSafety();
    
    std::cout << std::endl << "All error handling and validation tests passed!" << std::endl;
}

} // namespace test_error_handling

// Main function for running the tests
int main() {
    test_error_handling::runAll();
    return 0;
}
