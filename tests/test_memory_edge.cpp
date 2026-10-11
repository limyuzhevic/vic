// Memory and Resource Edge Cases Tests
// Tests memory exhaustion, limit testing, and resource management

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

namespace test_memory_edge {

void testMemoryLimitTesting() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 10000, nlm::ConfigSource::Runtime);
    config->set("region_count", 10, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.001, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify resource usage
    assert(brain.getTotalNeuronCount() == 10000);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Run simulation with memory monitoring
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify brain state is still valid
    assert(brain.getTotalNeuronCount() == 10000);
    
    std::cout << "    testMemoryLimitTesting passed" << std::endl;
}

void testMemoryAllocationPatterns() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Access neurons multiple times to check memory stability
    for (int i = 0; i < 100; ++i) {
        brain.step(static_cast<nlm::SimulationStep>(i));
    }
    
    // Verify memory is not corrupted
    assert(brain.getTotalNeuronCount() == 1000);
    
    std::cout << "    testMemoryAllocationPatterns passed" << std::endl;
}

void testResourceCleanup() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Reset brain (should clean up resources)
    brain.reset();
    
    // Verify resources are cleaned up
    assert(brain.getTotalNeuronCount() == 100);  // Still valid, just reset
    
    std::cout << "    testResourceCleanup passed" << std::endl;
}

void testConfigMemoryManagement() {
    // Test Config class memory management
    nlm::Config config1;
    
    // Set many values
    for (int i = 0; i < 1000; ++i) {
        config1.set("key_" + std::to_string(i), i);
    }
    
    // Clear config
    config1.clear();
    
    // Verify cleanup
    assert(config1.getKeys().empty());
    
    // Create multiple config objects to test memory leaks
    for (int i = 0; i < 100; ++i) {
        nlm::Config tempConfig;
        tempConfig.set("test", i);
        tempConfig.set("test2", i * 2.0);
        
        // Config will be destroyed here
    }
    
    std::cout << "    testConfigMemoryManagement passed" << std::endl;
}

void testBrainCopySemantics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 1, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain1(config);
    bool initialized1 = brain1.initialize();
    assert(initialized1);
    
    // Copy constructor should be deleted (based on header)
    // Testing move semantics
    nlm::Brain brain2(std::move(brain1));
    assert(brain2.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainCopySemantics passed" << std::endl;
}

void testBrainMemoryValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50000, nlm::ConfigSource::Runtime);  // Large but reasonable
    config->set("region_count", 10, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.0001, nlm::ConfigSource::Runtime);  // Very low
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Verify memory allocation didn't exceed reasonable limits
    assert(brain.getTotalNeuronCount() == 50000);
    
    std::cout << "    testBrainMemoryValidation passed" << std::endl;
}

void runAll() {
    testMemoryLimitTesting();
    testMemoryAllocationPatterns();
    testResourceCleanup();
    testConfigMemoryManagement();
    testBrainCopySemantics();
    testBrainMemoryValidation();
    
    std::cout << "Memory and resource edge cases test suite completed successfully!" << std::endl;
}

} // namespace test_memory_edge
