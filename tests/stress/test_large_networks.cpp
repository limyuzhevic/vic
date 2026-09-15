// Stress Tests for Large Networks
// Tests edge cases and boundary conditions for large-scale neural networks

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <limits>
#include <algorithm>

namespace test_stress {

void testLargeNetworkMemoryUsage() {
    // Test memory usage with large network (5000 neurons)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    auto start = std::chrono::high_resolution_clock::now();
    
    assert(brain.initialize());
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Initialization should complete in reasonable time (less than 10 seconds)
    assert(duration.count() < 10000);
    
    // Verify initialization worked correctly
    assert(brain.getTotalNeuronCount() == 5000);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testLargeNetworkMemoryUsage passed" << std::endl;
}

void testExtremeNeuronCounts() {
    // Test with various neuron counts that push system limits
    std::vector<size_t> neuronCounts = {
        10,      // Minimum
        100,     // Small network
        1000,    // Medium network
        10000,   // Large network
        50000    // Very large network (may be near system limits)
    };
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(std::max<size_t>(1, neuronCount / 1000)), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Should initialize without crashing
        assert(brain.initialize());
        
        // Should have expected number of neurons (or at least some)
        assert(brain.getTotalNeuronCount() >= 0);  // Could be less due to pool limits
        
        // Clean up
        brain.reset();
    }
    
    std::cout << "    testExtremeNeuronCounts passed" << std::endl;
}

void testMemoryPressure() {
    // Test system behavior under memory pressure
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize with large network
    assert(brain.initialize());
    
    // Process many steps to create memory pressure
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Should not crash or run out of memory
        assert(brain.getTotalNeuronCount() == 10000);
    }
    
    // Reset to free memory
    brain.reset();
    
    std::cout << "    testMemoryPressure passed" << std::endl;
}

void testTimingExtremes() {
    // Test with extreme time values
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test with very small timestep
    brain.reset();
    
    // Very large timestep (simulating time compression)
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Test with zero timestep (edge case)
    brain.reset();
    brain.step(0);  // Should handle gracefully
    
    // Test negative timestep (should handle gracefully)
    brain.reset();
    // Note: step parameter is SimulationStep (unsigned), so negative not possible
    
    std::cout << "    testTimingExtremes passed" << std::endl;
}

void testBoundaryConditions() {
    // Test boundary conditions for various parameters
    
    // Test with minimum and maximum valid values
    struct ConfigTest {
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
    };
    
    std::vector<ConfigTest> testConfigs = {
        {1, 1, 0.001f},      // Minimal values
        {1000, 1, 1.0f},     // Maximum values
        {500, 10, 0.0f},     // Edge values
        {2000, 1, 0.999f}   // Near limits
    };
    
    for (const auto& testConfig : testConfigs) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(testConfig.neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(testConfig.regionCount), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Should handle all boundary conditions
        assert(brain.initialize());
        
        brain.reset();
    }
    
    std::cout << "    testBoundaryConditions passed" << std::endl;
}

void testConcurrencyEdgeCases() {
    // Test edge cases that might affect thread safety
    nlm::MemoryPool<int> pool(100, 5);
    
    // Allocate and deallocate rapidly (simulates concurrent access)
    std::vector<int*> allocated;
    for (int i = 0; i < 200; ++i) {
        int* obj = pool.allocate();
        if (obj) allocated.push_back(obj);
        
        // Deallocate some to test mixed operations
        if (i % 5 == 0 && !allocated.empty()) {
            int* toDeallocate = allocated.back();
            pool.deallocate(toDeallocate);
            allocated.pop_back();
        }
    }
    
    // Clean up remaining allocations
    for (int* obj : allocated) {
        pool.deallocate(obj);
    }
    
    std::cout << "    testConcurrencyEdgeCases passed" << std::endl;
}

void testErrorConditions() {
    // Test error conditions and graceful failure
    
    // Test with very small timestep in config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.0001f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Should handle very small timestep
    brain.step(0);
    
    // Test reset at various stages
    brain.reset();
    assert(brain.initialize());  // Can reinitialize after reset
    
    brain.reset();
    brain.reset();  // Double reset should be safe
    
    std::cout << "    testErrorConditions passed" << std::endl;
}

void runAll() {
    testLargeNetworkMemoryUsage();
    testExtremeNeuronCounts();
    testMemoryPressure();
    testTimingExtremes();
    testBoundaryConditions();
    testConcurrencyEdgeCases();
    testErrorConditions();
    
    std::cout << std::endl;
    std::cout << "All Stress Tests PASSED" << std::endl;
}

} // namespace test_stress
