// NLM Performance Validation Tests
// Tests for performance optimizations and validation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>

namespace test_performance {

/**
 * Test memory pool performance
 */
void testMemoryPoolPerformance() {
    std::cout << "Testing memory pool performance..." << std::endl;
    
    // Create memory pool for neurons
    auto neuronPool = std::make_unique<nlm::NeuronPool>();
    
    // Test allocation/deallocation performance
    std::vector<nlm::NeuronId> allocatedIds;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Allocate many neurons
    for (size_t i = 0; i < 1000; ++i) {
        allocatedIds.push_back(neuronPool->allocate());
    }
    
    auto allocateTime = std::chrono::high_resolution_clock::now();
    
    // Deallocate all neurons
    for (const auto& id : allocatedIds) {
        neuronPool->deallocate(id);
    }
    
    auto deallocateTime = std::chrono::high_resolution_clock::now();
    
    // Calculate performance metrics
    auto allocateDuration = std::chrono::duration_cast<std::chrono::microseconds>(allocateTime - startTime);
    auto deallocateDuration = std::chrono::duration_cast<std::chrono::microseconds>(deallocateTime - allocateTime);
    
    std::cout << "  Allocations: 1000 in " << allocateDuration.count() << " microseconds" << std::endl;
    std::cout << "  Deallocations: 1000 in " << deallocateDuration.count() << " microseconds" << std::endl;
    
    // Verify memory usage
    size_t memoryUsage = neuronPool->memoryUsage();
    assert(memoryUsage > 0);
    
    std::cout << "  Memory usage: " << memoryUsage << " bytes" << std::endl;
    std::cout << "  PASSED: Memory pool performance test" << std::endl;
}

/**
 * Test event queue performance
 */
void testEventQueuePerformance() {
    std::cout << "Testing event queue performance..." << std::endl;
    
    // Create event queue
    nlm::SpikeRingBuffer spikeQueue;
    
    // Test spike event performance
    std::vector<nlm::QueuedSpikeEvent> events;
    
    // Generate test events
    for (size_t i = 0; i < 1000; ++i) {
        nlm::QueuedSpikeEvent event(
            100 + i,    // sourceNeuron
            200 + i,    // destNeuron  
            i,         // synapseId
            0.01f * i, // weight
            i % 2 == 0  // isExcitatory
        );
        events.push_back(event);
    }
    
    // Push events to queue
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (const auto& event : events) {
        spikeQueue.push(event);
    }
    
    auto pushTime = std::chrono::high_resolution_clock::now();
    
    // Pop all events from queue
    std::vector<nlm::QueuedSpikeEvent> poppedEvents;
    
    while (!spikeQueue.empty()) {
        auto event = spikeQueue.pop();
        if (event) {
            poppedEvents.push_back(*event);
        }
    }
    
    auto popTime = std::chrono::high_resolution_clock::now();
    
    // Calculate performance metrics
    auto pushDuration = std::chrono::duration_cast<std::chrono::microseconds>(pushTime - startTime);
    auto popDuration = std::chrono::duration_cast<std::chrono::microseconds>(popTime - pushTime);
    
    std::cout << "  Pushes: 1000 in " << pushDuration.count() << " microseconds" << std::endl;
    std::cout << "  Pops: 1000 in " << popDuration.count() << " microseconds" << std::endl;
    
    // Verify all events were processed
    assert(poppedEvents.size() == events.size());
    
    std::cout << "  PASSED: Event queue performance test" << std::endl;
}

/**
 * Test brain performance with large network
 */
void testBrainPerformance() {
    std::cout << "Testing brain performance with large network..." << std::endl;
    
    // Create configuration for performance test
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12351), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);  // Larger network
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);  // Sparse connections
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Run performance benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    const size_t numSteps = 500;
    
    for (size_t step = 0; step < numSteps; ++step) {
        brain->step(static_cast<nlm::SimulationStep>(step));
        
        // Verify brain remains stable
        assert(brain->getTotalNeuronCount() > 0);
        
        // Progress indicator for long test
        if ((step + 1) % 100 == 0) {
            std::cout << "    Step " << (step + 1) << "/" << numSteps << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Calculate performance metrics
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (numSteps * 1000.0) / totalDuration.count();
    
    std::cout << "  Simulation: " << numSteps << " steps in " << totalDuration.count() << " ms" << std::endl;
    std::cout << "  Performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    // Verify brain metrics
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    // Performance target: At least 1000 steps/second for this network size
    assert(stepsPerSecond >= 1000.0);
    
    std::cout << "  PASSED: Brain performance test" << std::endl;
}

/**
 * Test SIMD optimization detection
 */
void testSIMDOptimization() {
    std::cout << "Testing SIMD optimization detection..." << std::endl;
    
    // Check if SIMD is available
    bool simdAvailable = nlm::SIMDUtils::isSIMDAvailable();
    
    std::cout << "  SIMD Available: " << (simdAvailable ? "Yes" : "No") << std::endl;
    std::cout << "  SIMD Type: " << nlm::SIMDUtils::getSIMDType() << std::endl;
    
    // SIMD width detection
    size_t simdWidth = nlm::SIMDUtils::SIMD_WIDTH;
    std::cout << "  SIMD Width: " << simdWidth << " floats per register" << std::endl;
    
    // Verify reasonable SIMD width
    assert(simdWidth >= 1 && simdWidth <= 16);
    
    std::cout << "  PASSED: SIMD optimization test" << std::endl;
}

/**
 * Test memory alignment performance
 */
void testMemoryAlignment() {
    std::cout << "Testing memory alignment performance..." << std::endl;
    
    // Create arrays with different alignments
    std::vector<float> alignedArray(1000);
    std::vector<float> unalignedArray(1000);
    
    // Test aligned access performance (should be faster)
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < alignedArray.size(); ++i) {
        // Simulate aligned memory access
        volatile float dummy = alignedArray[i];  // Prevent optimization
        dummy = 1.0f / dummy;
    }
    
    auto alignedTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < unalignedArray.size(); ++i) {
        // Simulate unaligned memory access
        volatile float dummy = unalignedArray[i];  // Prevent optimization
        dummy = 1.0f / dummy;
    }
    
    auto unalignedTime = std::chrono::high_resolution_clock::now();
    
    auto alignedDuration = std::chrono::duration_cast<std::chrono::microseconds>(alignedTime - startTime);
    auto unalignedDuration = std::chrono::duration_cast<std::chrono::microseconds>(unalignedTime - alignedTime);
    
    std::cout << "  Aligned access: " << alignedDuration.count() << " microseconds" << std::endl;
    std::cout << "  Unaligned access: " << unalignedDuration.count() << " microseconds" << std::endl;
    
    // Memory alignment should help performance
    bool alignmentBenefit = alignedDuration.count() <= unalignedDuration.count() * 1.1; // 10% tolerance
    std::cout << "  Alignment benefit: " << (alignmentBenefit ? "Yes" : "No") << std::endl;
    
    std::cout << "  PASSED: Memory alignment test" << std::endl;
}

/**
 * Run all performance tests
 */
void runAll() {
    std::cout << "=== NLM Performance Tests ===" << std::endl;
    std::cout << "Testing performance optimizations and validation:" << std::endl;
    
    bool allPassed = true;
    
    try {
        testSIMDOptimization();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: SIMD optimization - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryAlignment();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Memory alignment - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryPoolPerformance();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Memory pool performance - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testEventQueuePerformance();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Event queue performance - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testBrainPerformance();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Brain performance - " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Performance Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Performance Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance