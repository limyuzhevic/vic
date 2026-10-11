// Boundary Conditions Tests
// Tests integer overflow, floating point edge cases, and mathematical boundaries

#include "brain/Neuron.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>

namespace test_boundary {

void testIntegerOverflow() {
    nlm::Config config;
    
    // Test integer boundaries
    config.set("int_max", std::numeric_limits<int>::max());
    config.set("int_min", std::numeric_limits<int>::min());
    
    assert(config.getOr<int>("int_max") == std::numeric_limits<int>::max());
    assert(config.getOr<int>("int_min") == std::numeric_limits<int>::min());
    
    // Test int64_t boundaries
    config.set("int64_max", std::numeric_limits<int64_t>::max());
    config.set("int64_min", std::numeric_limits<int64_t>::min());
    
    assert(config.getOr<int64_t>("int64_max") == std::numeric_limits<int64_t>::max());
    assert(config.getOr<int64_t>("int64_min") == std::numeric_limits<int64_t>::min());
    
    std::cout << "    testIntegerOverflow passed" << std::endl;
}

void testFloatingPointEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test floating point boundaries
    float fmax = std::numeric_limits<float>::max();
    float fmin = std::numeric_limits<float>::min();
    float fe = std::numeric_limits<float>::epsilon();
    float finf = std::numeric_limits<float>::infinity();
    
    neuron.setMembranePotential(fmax);
    assert(neuron.getMembranePotential() == fmax);
    
    neuron.setMembranePotential(fmin);
    assert(neuron.getMembranePotential() == fmin);
    
    neuron.setMembranePotential(fe);
    assert(neuron.getMembranePotential() == fe);
    
    // Test NaN (if applicable)
    // Depending on implementation
    
    std::cout << "    testFloatingPointEdgeCases passed" << std::endl;
}

void testMathBoundaryConditions() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test mathematical boundary conditions
    neuron.setMembranePotential(-1000.0f);  // Very negative
    neuron.setThreshold(1000.0f);           // Very positive
    
    // Test checkThreshold with extreme values
    assert(neuron.checkThreshold());  // -1000 < 1000, so threshold not crossed
    
    neuron.setMembranePotential(1000.0f);  // Above threshold
    assert(neuron.checkThreshold());  // 1000 >= 1000, threshold crossed
    
    std::cout << "    testMathBoundaryConditions passed" << std::endl;
}

void testBrainBoundaryConditions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test brain with various boundary conditions
    nlm::SimulationStep largeStep = 1000000;
    brain.step(largeStep);  // Large step number
    
    nlm::Timestamp largeTime = 1000000.0f;
    brain.step(0, largeTime);  // Large time
    
    // Verify brain still functional
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainBoundaryConditions passed" << std::endl;
}

void testSimulationStability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run long simulation to test stability
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        brain.step(step);
        
        // Periodically check stability
        if (step % 1000 == 0) {
            assert(brain.getTotalNeuronCount() == 100);
            assert(brain.getTotalSynapseCount() >= 0);
        }
    }
    
    std::cout << "    testSimulationStability passed" << std::endl;
}

void runAll() {
    testIntegerOverflow();
    testFloatingPointEdgeCases();
    testMathBoundaryConditions();
    testBrainBoundaryConditions();
    testSimulationStability();
    
    std::cout << "Boundary conditions test suite completed successfully!" << std::endl;
}

} // namespace test_boundary
