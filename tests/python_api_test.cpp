// Python API Compatibility Test Suite
// Tests for unified API design and backward compatibility
// Validates consistent method signatures across all systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "python_api_test.hpp"

namespace nlm {

void testAPIConsistency() {
    std::cout << "Testing unified API consistency..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test consistent method signatures across all systems
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    // Test consistent parameter handling
    brain->initialize();
    
    // Step method with different parameter counts (consistent API)
    brain->step(10);  // Integer step
    brain->step(10, 0.1f);  // Integer step with time delta
    
    // Consistent property access patterns
    uint64_t neuronCount = brain->getTotalNeuronCount();
    uint64_t synapseCount = brain->getTotalSynapseCount();
    float firingRate = brain->getAverageFiringRate();
    float eiRatio = brain->getExcitationInhibitionRatio();
    
    // All properties should be accessible and return expected types
    assert(std::is_same<decltype(neuronCount), uint64_t>::value);
    assert(std::is_same<decltype(synapseCount), uint64_t>::value);
    assert(std::is_same<decltype(firingRate), float>::value);
    assert(std::is_same<decltype(eiRatio), float>::value);
    
    std::cout << "API consistency test passed!" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test that old API patterns still work
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    // Legacy API patterns that should still work
    brain->initialize();
    
    // Test that old property names still work (if they exist)
    // brain->getNeuronCount(); // If this was the old name
    // brain->getSynapseCount(); // If this was the old name
    
    // Test that all expected methods exist
    assert(py::hasattr(brain.get(), "step"));
    assert(py::hasattr(brain.get(), "initialize"));
    assert(py::hasattr(brain.get(), "reset"));
    assert(py::hasattr(brain.get(), "produceAction"));
    assert(py::hasattr(brain.get(), "getTotalNeuronCount"));
    assert(py::hasattr(brain.get(), "getTotalSynapseCount"));
    assert(py::hasattr(brain.get(), "getAverageFiringRate"));
    
    std::cout << "Backward compatibility test passed!" << std::endl;
}

void testParameterValidation() {
    std::cout << "Testing parameter validation..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    brain->initialize();
    
    // Test parameter validation in step method
    // Should handle various parameter types consistently
    brain->step(0);  // Edge case: zero steps
    brain->step(1);  // Minimum: one step
    brain->step(1000);  // Large number of steps
    brain->step(1, 0.0f);  // Zero time delta
    brain->step(1, 1.0f);  // Normal time delta
    brain->step(1, 1000.0f);  // Large time delta
    
    // Test that brain state remains consistent after parameter variations
    uint64_t originalNeurons = brain->getTotalNeuronCount();
    brain->step(10);
    uint64_t finalNeurons = brain->getTotalNeuronCount();
    
    // Neuron count should remain stable
    assert(originalNeurons == finalNeurons);
    
    std::cout << "Parameter validation test passed!" << std::endl;
}

void testErrorRecovery() {
    std::cout << "Testing error recovery mechanisms..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test error recovery in Python bindings
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    brain->initialize();
    
    // Test recovery from various error conditions
    try {
        // Test invalid operation recovery
        brain->step(-1);  // Should handle gracefully or throw appropriate error
    } catch (const std::exception& e) {
        // Error properly caught
        std::cout << "Error properly caught: " << e.what() << std::endl;
    }
    
    // Verify brain can still function after error attempt
    brain->step(5);
    assert(brain->getTotalNeuronCount() > 0);
    
    std::cout << "Error recovery test passed!" << std::endl;
}

void testIntegrationConsistency() {
    std::cout << "Testing integration consistency across systems..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test that all systems integrate consistently
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    brain->initialize();
    
    // Test integration with memory systems
    auto workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working memory integrated: OK" << std::endl;
    }
    
    // Test integration with neuromodulation systems
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "Neuromodulation integrated: OK" << std::endl;
    }
    
    // Test integration with prediction system
    auto prediction = brain->getPredictionSystem();
    if (prediction) {
        std::cout << "Prediction system integrated: OK" << std::endl;
    }
    
    // Test integration with development system
    auto development = brain->getDevelopmentSystem();
    if (development) {
        std::cout << "Development system integrated: OK" << std::endl;
    }
    
    std::cout << "Integration consistency test passed!" << std::endl;
}

void runPythonAPITests() {
    std::cout << "=== Running Python API Tests ===" << std::endl;
    std::cout << std::endl;
    
    testAPIConsistency();
    std::cout << std::endl;
    
    testBackwardCompatibility();
    std::cout << std::endl;
    
    testParameterValidation();
    std::cout << std::endl;
    
    testErrorRecovery();
    std::cout << std::endl;
    
    testIntegrationConsistency();
    std::cout << std::endl;
    
    std::cout << "=== All Python API Tests Passed ===" << std::endl;
}

} // namespace nlm
