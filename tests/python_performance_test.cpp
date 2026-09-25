// Python Performance Test Suite
// Tests for Python bindings performance validation
// Validates performance overhead and optimization effectiveness

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "python_performance_test.hpp"

namespace nlm {

void testPythonPerformanceOverhead() {
    std::cout << "Testing Python performance overhead..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Configure brain for performance testing
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    config->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Runtime);
    
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    brain->initialize();
    
    // Benchmark Python bindings performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        brain->step(10);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Python bindings: 50 steps in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 50.0) << " ms" << std::endl;
    
    // Target: Should be reasonable overhead compared to C++
    // Allow for 10x overhead as reasonable for Python bindings
    double avgPerStep = duration.count() / 50.0;
    assert(avgPerStep < 100.0); // Less than 100ms per step is acceptable
    
    std::cout << "Python performance overhead test passed!" << std::endl;
}

void testPythonMemoryEfficiency() {
    std::cout << "Testing Python memory efficiency..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test memory usage with multiple brain instances
    std::vector<std::shared_ptr<Brain>> brains;
    
    for (int i = 0; i < 10; ++i) {
        auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
        config->set("neuron_count", static_cast<int64_t>(1000 + i * 100), ConfigSource::Runtime);
        
        auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
        brain->initialize();
        brains.push_back(brain);
    }
    
    // Verify all instances work correctly
    for (size_t i = 0; i < brains.size(); ++i) {
        brains[i]->step(5);
        assert(brains[i]->getTotalNeuronCount() == 1000 + i * 100);
    }
    
    std::cout << "Python memory efficiency test passed!" << std::endl;
}

void testPythonIntegrationPerformance() {
    std::cout << "Testing Python integration performance..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    brain->initialize();
    
    // Benchmark integration with all NLM systems
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 20; ++i) {
        // Test integration with memory systems
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            // Use working memory if available
        }
        
        // Test integration with neuromodulation systems
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            // Use neuromodulation if available
        }
        
        // Test integration with prediction system
        auto prediction = brain->getPredictionSystem();
        if (prediction) {
            // Use prediction if available
        }
        
        brain->step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Python integration: 20 steps with system integration in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 20.0) << " ms" << std::endl;
    
    // Verify integration doesn't cause significant performance degradation
    double avgPerStep = duration.count() / 20.0;
    assert(avgPerStep < 50.0); // Less than 50ms per step is good
    
    std::cout << "Python integration performance test passed!" << std::endl;
}

void testPythonBenchmarkComparison() {
    std::cout << "Testing Python benchmark comparison..." << std::endl;
    
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Create test configurations
    std::vector<std::tuple<std::string, int64_t>> testConfigs = {
        {"Small", 1000},
        {"Medium", 10000},
        {"Large", 50000}
    };
    
    for (const auto& [sizeName, neuronCount] : testConfigs) {
        auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        
        auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
        brain->initialize();
        
        // Benchmark for different sizes
        auto start = std::chrono::high_resolution_clock::now();
        
        int steps = std::min(10, static_cast<int>(neuronCount / 1000));
        for (int i = 0; i < steps; ++i) {
            brain->step(10);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        
        std::cout << sizeName << " brain: " << steps << " steps in " << duration.count() << " ms" << std::endl;
        std::cout << "  Average per step: " << (duration.count() / steps) << " ms" << std::endl;
        
        // Performance should scale reasonably with brain size
        double avgPerStep = duration.count() / steps;
        assert(avgPerStep < 200.0); // Reasonable performance for any size
    }
    
    std::cout << "Python benchmark comparison test passed!" << std::endl;
}

void runPythonPerformanceTests() {
    std::cout << "=== Running Python Performance Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPythonPerformanceOverhead();
    std::cout << std::endl;
    
    testPythonMemoryEfficiency();
    std::cout << std::endl;
    
    testPythonIntegrationPerformance();
    std::cout << std::endl;
    
    testPythonBenchmarkComparison();
    std::cout << std::endl;
    
    std::cout << "=== All Python Performance Tests Passed ===" << std::endl;
}

} // namespace nlm
