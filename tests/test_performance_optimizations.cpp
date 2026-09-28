// NLM Performance Optimizations Tests
// Test new performance features: Memory Pool, SIMD Optimization, Parallel Processing, Cache-Friendly Data Structures

#include "performance/MemoryPool.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/Performance.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

namespace test_performance_optimizations {

void testMemoryPoolSystem() {
    std::cout << "Testing Memory Pool System..." << std::endl;
    
    // Test NeuronPool
    nlm::NeuronPool neuronPool;
    
    // Allocate neurons
    auto neuron1 = neuronPool.allocate();
    auto neuron2 = neuronPool.allocate();
    auto neuron3 = neuronPool.allocate();
    
    assert(neuronPool.size() == 3);
    assert(neuronPool.memoryUsage() > 0);
    
    // Access neuron states
    float* potential1 = neuronPool.getState<float>(neuron1);
    assert(potential1 != nullptr);
    
    // Deallocate neurons
    neuronPool.deallocate(neuron1);
    neuronPool.deallocate(neuron2);
    neuronPool.deallocate(neuron3);
    
    assert(neuronPool.size() == 0);
    
    // Test SynapsePool
    nlm::SynapsePool synapsePool;
    
    // Allocate synapses
    auto synapse1 = synapsePool.allocate(1, 2);
    auto synapse2 = synapsePool.allocate(2, 3);
    auto synapse3 = synapsePool.allocate(3, 4);
    
    assert(synapsePool.size() == 3);
    assert(synapsePool.memoryUsage() > 0);
    
    // Get synapse properties
    NeuronId source1 = synapsePool.getSource(synapse1);
    NeuronId dest1 = synapsePool.getDestination(synapse1);
    assert(source1 == 1);
    assert(dest1 == 2);
    
    // Deallocate synapses
    synapsePool.deallocate(synapse1);
    synapsePool.deallocate(synapse2);
    synapsePool.deallocate(synapse3);
    
    assert(synapsePool.size() == 0);
    
    // Test generic MemoryPool
    nlm::MemoryPool<int> intPool(100, 10);
    
    int* ptr1 = intPool.allocate();
    *ptr1 = 42;
    assert(*ptr1 == 42);
    
    int* ptr2 = intPool.allocate();
    *ptr2 = 100;
    assert(*ptr2 == 100);
    
    assert(intPool.allocatedCount() == 2);
    assert(intPool.availableCount() == 98);
    
    intPool.deallocate(ptr1);
    intPool.deallocate(ptr2);
    
    assert(intPool.allocatedCount() == 0);
    assert(intPool.availableCount() == 100);
    
    std::cout << "    testMemoryPoolSystem passed" << std::endl;
}

void testSIMDOptimizationSystem() {
    std::cout << "Testing SIMD Optimization System..." << std::endl;
    
    // Test SIMD operations
    std::vector<float> vec1(1000, 1.0f);
    std::vector<float> vec2(1000, 2.0f);
    std::vector<float> result(1000);
    
    // Test vector addition
    nlm::SIMDVectorization::add(vec1.data(), vec2.data(), result.data(), vec1.size());
    
    for (size_t i = 0; i < vec1.size(); ++i) {
        assert(result[i] == 3.0f);
    }
    
    // Test vector multiplication
    std::vector<float> result2(1000);
    nlm::SIMDVectorization::multiply(vec1.data(), vec2.data(), result2.data(), vec1.size());
    
    for (size_t i = 0; i < vec1.size(); ++i) {
        assert(result2[i] == 2.0f);
    }
    
    // Test dot product
    float dot = nlm::SIMDVectorization::dot(vec1.data(), vec2.data(), vec1.size());
    assert(dot == 3000.0f); // sum of 1000 * 3.0f
    
    // Test SIMD availability check
    bool simdAvailable = nlm::SIMDVectorization::isAvailable();
    // Just check that the function runs without crashing
    (void)simdAvailable;
    
    // Test SIMD stats
    auto stats = nlm::SIMDVectorization::getStatistics();
    assert(stats.operations >= 0);
    
    std::cout << "    testSIMDOptimizationSystem passed" << std::endl;
}

void testParallelProcessingSystem() {
    std::cout << "Testing Parallel Processing System..." << std::endl;
    
    // Test thread pool creation
    nlm::ThreadPool pool(4, true); // 4 threads, blocking mode
    assert(pool.getThreadCount() == 4);
    assert(pool.isBlockingMode());
    
    // Test parallel operations
    std::vector<int> data(1000);
    std::vector<int> results(1000);
    
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = i * 2;
    }
    
    pool.parallelFor(0, data.size(), [&](size_t i) {
        results[i] = data[i] * 3;
    });
    
    for (size_t i = 0; i < data.size(); ++i) {
        assert(results[i] == data[i] * 3);
    }
    
    // Test parallel reduction
    int sum = pool.parallelReduce<int>(0, data.size(), 0, 
        [&](size_t i) { return data[i]; },
        [&](int a, int b) { return a + b; });
    
    int expectedSum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        expectedSum += data[i];
    }
    assert(sum == expectedSum);
    
    // Test parallel map
    std::vector<int> mapped(1000);
    pool.parallelMap(data.begin(), data.end(), mapped.begin(),
        [](int x) { return x + 100; });
    
    for (size_t i = 0; i < data.size(); ++i) {
        assert(mapped[i] == data[i] + 100);
    }
    
    // Test parallel sort
    std::vector<int> unsorted(100);
    std::vector<int> sorted(100);
    
    for (size_t i = 0; i < unsorted.size(); ++i) {
        unsorted[i] = 100 - i;
    }
    
    pool.parallelSort(unsorted.begin(), unsorted.end());
    
    // Copy sorted results
    std::copy(unsorted.begin(), unsorted.end(), sorted.begin());
    std::sort(sorted.begin(), sorted.end());
    
    assert(std::equal(unsorted.begin(), unsorted.end(), sorted.begin()));
    
    std::cout << "    testParallelProcessingSystem passed" << std::endl;
}

void testCacheFriendlyDataStructures() {
    std::cout << "Testing Cache-Friendly Data Structures..." << std::endl;
    
    // Test SparseConnectivity
    nlm::SparseConnectivity connectivity(10000, 0.1f); // 10k neurons, 10% density
    
    // Add connections
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10; ++j) {
            int target = (i * 10 + j) % 10000;
            connectivity.addConnection(i, target, 0.5f);
        }
    }
    
    assert(connectivity.connectionCount() > 0);
    assert(connectivity.getConnectionCount(0) >= 10);
    
    // Test performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        int target = i % 10000;
        float weight = connectivity.getConnectionWeight(i, target);
        (void)weight; // Use result to prevent optimization
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should be fast due to cache-friendly design
    assert(duration.count() < 1000000); // Less than 1 second for 1000 accesses
    
    // Test OptimizedBrain
    nlm::OptimizedBrain optimizedBrain(500, 0.1f);
    
    // Update neurons
    optimizedBrain.updateNeuronMembranePotential(0, 10.0f);
    assert(optimizedBrain.getNeuronMembranePotential(0) == 10.0f);
    
    // Simulate spike
    optimizedBrain.recordSpike(0, 0.0f);
    
    // Access states efficiently
    for (size_t i = 0; i < 100; ++i) {
        float potential = optimizedBrain.getNeuronMembranePotential(i);
        (void)potential;
    }
    
    std::cout << "    testCacheFriendlyDataStructures passed" << std::endl;
}

void testPerformanceProfiling() {
    std::cout << "Testing Performance Profiling..." << std::endl;
    
    // Test performance metrics collection
    nlm::Performance performance;
    
    // Record some performance data
    performance.recordAllocation(sizeof(float) * 1000, std::chrono::microseconds(100));
    performance.recordDeallocation(sizeof(float) * 500, std::chrono::microseconds(50));
    
    // Get performance statistics
    auto stats = performance.getStatistics();
    assert(stats.allocations > 0);
    assert(stats.deallocations > 0);
    
    // Test memory pool statistics
    nlm::MemoryPool<int> pool(100, 10);
    
    int* ptr = pool.allocate();
    *ptr = 42;
    
    auto poolStats = pool.getStatistics();
    assert(poolStats.allocatedCount == 1);
    assert(poolStats.availableCount == 99);
    
    pool.deallocate(ptr);
    
    std::cout << "    testPerformanceProfiling passed" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Optimizations Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test Memory Pool System
    std::cout << "Running Memory Pool System tests..." << std::endl;
    try {
        testMemoryPoolSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test SIMD Optimization System
    std::cout << "Running SIMD Optimization System tests..." << std::endl;
    try {
        testSIMDOptimizationSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Parallel Processing System
    std::cout << "Running Parallel Processing System tests..." << std::endl;
    try {
        testParallelProcessingSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Cache-Friendly Data Structures
    std::cout << "Running Cache-Friendly Data Structures tests..." << std::endl;
    try {
        testCacheFriendlyDataStructures();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Performance Profiling
    std::cout << "Running Performance Profiling tests..." << std::endl;
    try {
        testPerformanceProfiling();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All Performance Optimizations Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Performance Optimizations Tests FAILED ===" << std::endl;
    }
}

} // namespace test_performance_optimizations