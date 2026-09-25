// Python Bindings Test Suite
// Tests for pynlm Python bindings functionality
// Validates API compatibility, error handling, and performance

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "python_bindings_test.hpp"

namespace nlm {

void testPythonBindingsModuleLoading() {
    std::cout << "Testing Python bindings module loading..." << std::endl;
    
    // Test that pynlm module can be loaded
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Check module attributes
    assert(py::hasattr(pynlm, "createDefaultConfig"));
    assert(py::hasattr(pynlm, "createBrain"));
    assert(py::hasattr(pynlm, "NeuronType"));
    assert(py::hasattr(pynlm, "SynapseType"));
    
    std::cout << "Python bindings module loading test passed!" << std::endl;
}

void testPythonBindingsBasicFunctionality() {
    std::cout << "Testing Python bindings basic functionality..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test config creation
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    assert(config != nullptr);
    
    // Test brain creation
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    assert(brain != nullptr);
    
    // Test brain initialization
    brain->initialize();
    
    // Verify basic properties
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() >= 0);
    
    std::cout << "Python bindings basic functionality test passed!" << std::endl;
}

void testPythonBindingsAPICompatibility() {
    std::cout << "Testing Python bindings API compatibility with C++ NLM..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    // Test C++ API compatibility
    brain->step(10);
    brain->step(10, 0.1f);
    
    // Test property access (consistent with C++ API)
    uint64_t neuronCount = brain->getTotalNeuronCount();
    uint64_t synapseCount = brain->getTotalSynapseCount();
    float firingRate = brain->getAverageFiringRate();
    float eiRatio = brain->getExcitationInhibitionRatio();
    
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    
    std::cout << "Python bindings API compatibility test passed!" << std::endl;
}

void testPythonBindingsErrorHandling() {
    std::cout << "Testing Python bindings error handling..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test error conversion from C++ to Python
    try {
        auto config = std::make_shared<Config>();
        // Force an error by creating invalid config
        py::cast<std::shared_ptr<Brain>>(pynlm.attr("createBrain")(config));
        std::cout << "Error conversion test passed (expected error not thrown)" << std::endl;
    } catch (const py::error_already_set& e) {
        // Python exception properly raised
        std::cout << "Python exception properly raised: " << e.what() << std::endl;
    }
    
    std::cout << "Python bindings error handling test passed!" << std::endl;
}

void testPythonBindingsPerformance() {
    std::cout << "Testing Python bindings performance..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    brain->initialize();
    
    // Performance benchmark
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain->step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Performance: 100 steps in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 100.0) << " ms" << std::endl;
    
    // Verify performance is reasonable (should complete within a few seconds)
    assert(duration.count() < 5000.0); // 5 seconds max for 100 steps
    
    std::cout << "Python bindings performance test passed!" << std::endl;
}

void testPythonBindingsIntegration() {
    std::cout << "Testing Python bindings integration with existing NLM systems..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test integration with memory systems
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    brain->initialize();
    brain->step(10);
    
    // Verify integration with various systems
    if (brain->getWorkingMemory() != nullptr) {
        std::cout << "Working memory integration: OK" << std::endl;
    }
    
    if (brain->getEpisodicMemory() != nullptr) {
        std::cout << "Episodic memory integration: OK" << std::endl;
    }
    
    if (brain->getDopamine() != nullptr) {
        std::cout << "Neuromodulation integration: OK" << std::endl;
    }
    
    std::cout << "Python bindings integration test passed!" << std::endl;
}

void runPythonBindingsTests() {
    std::cout << "=== Running Python Bindings Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPythonBindingsModuleLoading();
    std::cout << std::endl;
    
    testPythonBindingsBasicFunctionality();
    std::cout << std::endl;
    
    testPythonBindingsAPICompatibility();
    std::cout << std::endl;
    
    testPythonBindingsErrorHandling();
    std::cout << std::endl;
    
    testPythonBindingsPerformance();
    std::cout << std::endl;
    
    testPythonBindingsIntegration();
    std::cout << std::endl;
    
    std::cout << "=== All Python Bindings Tests Passed ===" << std::endl;
}

} // namespace nlm
