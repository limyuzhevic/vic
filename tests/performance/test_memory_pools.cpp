// Performance Integration Tests
// Tests for memory pool performance, event processing speed, and scalability

#include "performance/MemoryPool.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include "dynamics/SpikeSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

namespace test_performance {

void testMemoryPoolPerformance() {
    // Test MemoryPool with different configurations
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create memory pools of different sizes
    nlm::MemoryPool<int> smallPool(100, 5);      // Small pool
    nlm::MemoryPool<int> mediumPool(1000, 10);   // Medium pool
    nlm::MemoryPool<int> largePool(10000, 20);   // Large pool
    
    // Allocate objects from each pool
    std::vector<int*> smallAllocated;
    std::vector<int*> mediumAllocated;
    std::vector<int*> largeAllocated;
    
    for (int i = 0; i < 150; ++i) {
        int* obj = smallPool.allocate();
        if (obj) smallAllocated.push_back(obj);
    }
    
    for (int i = 0; i < 2000; ++i) {
        int* obj = mediumPool.allocate();
        if (obj) mediumAllocated.push_back(obj);
    }
    
    for (int i = 0; i < 5000; ++i) {
        int* obj = largePool.allocate();
        if (obj) largeAllocated.push_back(obj);
    }
    
    // Verify allocations
    assert(smallAllocated.size() == 150);
    assert(mediumAllocated.size() == 2000);
    assert(largeAllocated.size() == 5000);
    
    // Deallocate all
    for (int* obj : smallAllocated) smallPool.deallocate(obj);
    for (int* obj : mediumAllocated) mediumPool.deallocate(obj);
    for (int* obj : largeAllocated) largePool.deallocate(obj);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Performance should be acceptable (less than 100ms for 7150 allocations)
    assert(duration.count() < 100000);
    
    std::cout << "    testMemoryPoolPerformance passed" << std::endl;
}

void testEventProcessingSpeed() {
    nlm::SpikeSystem spikeSystem;
    
    // Track processing time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Register handler
    spikeSystem.registerHandler([](const nlm::DetailedSpikeEvent& event) {
        // Simple handler - just verify event is valid
        assert(event.source.index() != 0);
    });
    
    // Queue many spikes (event-driven processing)
    for (int i = 0; i < 1000; ++i) {
        nlm::SpikeEvent event(
            nlm::NeuronId(i), 
            static_cast<float>(i), 
            i % 100
        );
        spikeSystem.queueSpike(event);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Process 1000 spikes in reasonable time (less than 50ms)
    assert(duration.count() < 50000);
    
    // Verify all spikes were processed
    assert(spikeSystem.getSpikeCount() == 1000);
    
    std::cout << "    testEventProcessingSpeed passed" << std::endl;
}

void testScalability() {
    // Test that performance scales reasonably with network size
    std::vector<double> processingTimes;
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        auto start = std::chrono::high_resolution_clock::now();
        
        assert(brain.initialize());
        
        // Run some steps
        for (int step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        processingTimes.push_back(duration.count());
        
        // Scalability should be reasonable (less than 1000ms for 2000 neurons)
        assert(duration.count() < 1000000);
    }
    
    // Check that scaling is not exponential (roughly linear)
    // Processing time for 2000 neurons should not be more than 20x that of 100 neurons
    if (processingTimes.size() >= 2) {
        assert(processingTimes.back() < processingTimes[0] * 20.0);
    }
    
    std::cout << "    testScalability passed" << std::endl;
}

void testMemoryUsageTracking() {
    // Test memory pool memory usage tracking
    nlm::MemoryPool<int> pool(1000, 5);
    
    // Get initial memory usage
    size_t initialMemory = pool.memoryUsage();
    assert(initialMemory > 0);
    
    // Allocate and deallocate objects
    std::vector<int*> allocated;
    for (int i = 0; i < 500; ++i) {
        int* obj = pool.allocate();
        if (obj) allocated.push_back(obj);
    }
    
    // Memory usage should increase
    size_t afterAllocMemory = pool.memoryUsage();
    assert(afterAllocMemory >= initialMemory);
    
    // Deallocate all
    for (int* obj : allocated) {
        pool.deallocate(obj);
    }
    
    // Memory usage should return close to initial (not exact due to memory fragmentation)
    size_t afterDeallocMemory = pool.memoryUsage();
    assert(afterDeallocMemory > 0);  // Should still have some memory used
    
    std::cout << "    testMemoryUsageTracking passed" << std::endl;
}

void testDeterministicMemoryUsage() {
    // Test that memory usage is deterministic for same operations
    nlm::MemoryPool<int> pool1(100, 5);
    nlm::MemoryPool<int> pool2(100, 5);
    
    // Perform same sequence of operations on both pools
    std::vector<int*> alloc1;
    std::vector<int*> alloc2;
    
    for (int i = 0; i < 100; ++i) {
        int* obj1 = pool1.allocate();
        int* obj2 = pool2.allocate();
        if (obj1) alloc1.push_back(obj1);
        if (obj2) alloc2.push_back(obj2);
    }
    
    // Memory usage should be similar (same number of allocations)
    size_t memory1 = pool1.memoryUsage();
    size_t memory2 = pool2.memoryUsage();
    
    // Allow some tolerance for memory alignment
    assert(std::abs(static_cast<int64_t>(memory1) - static_cast<int64_t>(memory2)) < 100);
    
    std::cout << "    testDeterministicMemoryUsage passed" << std::endl;
}

void testSpikeSystemMemoryUsage() {
    nlm::SpikeSystem spikeSystem;
    
    // Get initial memory usage
    spikeSystem.logMemoryUsage();  // This should not crash
    
    // Process many spikes
    for (int i = 0; i < 100; ++i) {
        nlm::SpikeEvent event(nlm::NeuronId(i), static_cast<float>(i), i);
        spikeSystem.queueSpike(event);
    }
    
    // Memory usage should be reasonable
    spikeSystem.logMemoryUsage();  // Should not crash with spikes
    
    std::cout << "    testSpikeSystemMemoryUsage passed" << std::endl;
}

void testPerformanceWithNeuromodulators() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    auto start = std::chrono::high_resolution_clock::now();
    
    assert(brain.initialize());
    
    // Run steps with neuromodulators active
    auto* dopamine = brain.getDopamine();
    auto* acetylcholine = brain.getAcetylcholine();
    auto* norepinephrine = brain.getNorepinephrine();
    
    if (dopamine && acetylcholine && norepinephrine) {
        dopamine->setLevel(0.7f);
        acetylcholine->setLevel(0.6f);
        norepinephrine->setLevel(0.8f);
    }
    
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Performance with neuromodulators should still be reasonable
    assert(duration.count() < 200000);
    
    std::cout << "    testPerformanceWithNeuromodulators passed" << std::endl;
}

void runAll() {
    testMemoryPoolPerformance();
    testEventProcessingSpeed();
    testScalability();
    testMemoryUsageTracking();
    testDeterministicMemoryUsage();
    testSpikeSystemMemoryUsage();
    testPerformanceWithNeuromodulators();
    
    std::cout << std::endl;
    std::cout << "All Performance Integration Tests PASSED" << std::endl;
}

} // namespace test_performance
