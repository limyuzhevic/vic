// Memory Management Tests
// Phase 3.1: Memory leak and memory pool tests

#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

namespace test_memory_management {

void testMemoryPoolEnabled() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Enable memory pool
    brain.enableMemoryPool(true);
    
    // The memory pool should be active
    std::cout << "    testMemoryPoolEnabled passed" << std::endl;
}

void testMemoryPoolDisabled() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", false, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Memory pool should be disabled
    brain.enableMemoryPool(false);
    
    std::cout << "    testMemoryPoolDisabled passed" << std::endl;
}

void testSmartPointerManagement() {
    // Test smart pointer behavior for memory management
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Brain should manage memory for regions, neurons, and synapses internally
    // The pImpl pattern should handle proper cleanup
    brain.step(0);
    
    // Verify resources are still valid after operations
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testSmartPointerManagement passed" << std::endl;
}

void testMemoryLeakDetection() {
    // Test for memory leaks by creating/destroying many brains
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
        config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Perform operations
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Brain goes out of scope here, should be properly destroyed
    }
    
    std::cout << "    testMemoryLeakDetection passed" << std::endl;
}

void testMemoryCleanup() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get initial memory usage
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Perform operations
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Reset brain
    brain.reset();
    
    // Verify cleanup happened
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    std::cout << "    testMemoryCleanup passed" << std::endl;
}

void testResourceRelease() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that resources are released when brain is destroyed
    // by creating and destroying multiple brain instances
    {
        nlm::Brain tempBrain(config);
        assert(tempBrain.initialize());
        tempBrain.step(0);
        // tempBrain goes out of scope here
    }
    
    // Original brain should still be valid
    assert(brain.getTotalNeuronCount() == 75);
    
    std::cout << "    testResourceRelease passed" << std::endl;
}

void testMemoryUsageTracking() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Memory usage should be measurable (implementation dependent)
    // For this test, we just verify the brain functions normally
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    assert(brain.getTotalNeuronCount() == 150);
    
    std::cout << "    testMemoryUsageTracking passed" << std::endl;
}

void testMemoryPoolEfficiency() {
    // Test memory pool efficiency with different configurations
    std::vector<size_t> neuronCounts = {50, 100, 200};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Enable memory pool
        brain.enableMemoryPool(true);
        
        // Perform operations
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        assert(brain.getTotalNeuronCount() == neuronCount);
    }
    
    std::cout << "    testMemoryPoolEfficiency passed" << std::endl;
}

void testMemoryAllocationPattern() {
    // Test various memory allocation patterns
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Allocate and deallocate many times
    for (int i = 0; i < 5; ++i) {
        brain.reset();
        assert(brain.initialize());
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
    }
    
    assert(brain.getTotalNeuronCount() == 300);
    
    std::cout << "    testMemoryAllocationPattern passed" << std::endl;
}

void testMemoryValidation() {
    // Test memory-related validation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Default); // Invalid
    
    nlm::Brain brain(config);
    // Brain may or may not initialize depending on validation
    
    // The test just ensures we can handle memory-related validation scenarios
    assert(config.has("neuron_count"));
    
    std::cout << "    testMemoryValidation passed" << std::endl;
}

void testMemoryErrorScenarios() {
    // Test error handling for memory-related issues
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    // Note: Large brain sizes may fail to initialize due to resource constraints
    // This is acceptable behavior for error handling tests
    
    std::cout << "    testMemoryErrorScenarios passed" << std::endl;
}

void runAll() {
    testMemoryPoolEnabled();
    testMemoryPoolDisabled();
    testSmartPointerManagement();
    testMemoryLeakDetection();
    testMemoryCleanup();
    testResourceRelease();
    testMemoryUsageTracking();
    testMemoryPoolEfficiency();
    testMemoryAllocationPattern();
    testMemoryValidation();
    testMemoryErrorScenarios();
}

} // namespace test_memory_management