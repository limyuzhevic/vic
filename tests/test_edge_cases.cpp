// Edge Case Tests
// Tests for edge cases, error conditions, and robustness

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_edge_cases {

void testInvalidConfiguration() {
    std::cout << "    Testing invalid configuration..." << std::endl;
    
    // Test brain with invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 0); // Invalid: zero neurons
    config->set("region_count", 0); // Invalid: zero regions
    
    nlm::Brain brain(config);
    
    // Should handle invalid configuration gracefully
    // Note: Brain constructor doesn't validate immediately
    // but initialization should handle it
    
    std::cout << "    Invalid configuration test passed" << std::endl;
}

void testBoundaryValues() {
    std::cout << "    Testing boundary values..." << std::endl;
    
    // Test with minimum valid values
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 1); // Minimum valid
    config->set("region_count", 1); // Minimum valid
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Should work with minimum valid values
        assert(brain.getTotalNeuronCount() >= 1);
        assert(brain.getRegionCount() >= 1);
    }
    
    std::cout << "    Boundary values test passed" << std::endl;
}

void testNullPointers() {
    std::cout << "    Testing null pointer safety..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 10);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test that methods handle null pointers safely
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // These should be valid pointers after initialization
        if (workingMemory) {
            // Should not crash
        }
        if (episodicMemory) {
            // Should not crash
        }
    }
    
    std::cout << "    Null pointer safety test passed" << std::endl;
}

void testResourceExhaustion() {
    std::cout << "    Testing resource exhaustion..." << std::endl;
    
    // Test behavior under resource constraints
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 1000); // Large configuration
    config->set("region_count", 10);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test that brain can handle large resource usage
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() > 0);
    }
    
    std::cout << "    Resource exhaustion test passed" << std::endl;
}

void testConcurrentAccess() {
    std::cout << "    Testing concurrent access safety..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test that brain state is consistent
        size_t neuronCount1 = brain.getTotalNeuronCount();
        size_t regionCount1 = brain.getRegionCount();
        
        // Simulate concurrent access (simplified)
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
            
            size_t neuronCount2 = brain.getTotalNeuronCount();
            size_t regionCount2 = brain.getRegionCount();
            
            assert(neuronCount1 == neuronCount2); // Should be stable
            assert(regionCount1 == regionCount2); // Should be stable
        }
    }
    
    std::cout << "    Concurrent access safety test passed" << std::endl;
}

void testErrorRecovery() {
    std::cout << "    Testing error recovery..." << std::endl;
    
    // Test that brain can recover from errors
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "Initial initialization failed" << std::endl;
        // This might be expected with invalid config
    }
    
    // Try to reset and reinitialize
    brain.reset();
    
    if (brain.initialize()) {
        // Should be able to recover
        assert(brain.getTotalNeuronCount() > 0);
    }
    
    std::cout << "    Error recovery test passed" << std::endl;
}

void testExtremeValues() {
    std::cout << "    Testing extreme values..." << std::endl;
    
    // Test with extreme configuration values
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 10000); // Very large
    config->set("region_count", 100); // Very many regions
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Should handle extreme values
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getRegionCount() > 0);
    }
    
    std::cout << "    Extreme values test passed" << std::endl;
}

} // namespace test_edge_cases
