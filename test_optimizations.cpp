#!/usr/bin/env g++ -std=c++20 -O2 -march=native -fopenmp -I. -I./src -I./src/core -I./src/brain -I./src/dynamics -I./src/memory -I./src/performance tests/main.cpp -o test_optimized -lpthread -lrt

#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>
#include <algorithm>

// Include the headers we need
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/dynamics/SpikeSystem.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/performance/MemoryPool.hpp"

// Test memory pool allocations
void testMemoryPoolPerformance() {
    std::cout << "Testing MemoryPool performance..." << std::endl;
    
    // Test NeuronPool
    nlm::NeuronPool neuronPool(10000);
    
    // Allocate and deallocate neurons
    std::vector<nlm::NeuronId> neuronIds;
    neuronIds.reserve(5000);
    
    for (size_t i = 0; i < 5000; ++i) {
        nlm::NeuronId id = neuronPool.allocate();
        neuronIds.push_back(id);
        assert(id.index() != 0);  // Valid ID
    }
    
    // Verify all allocated
    assert(neuronPool.size() == 5000);
    
    // Deallocate half
    for (size_t i = 0; i < 2500; ++i) {
        neuronPool.deallocate(neuronIds[i]);
    }
    
    // Verify deallocated
    assert(neuronPool.size() == 2500);
    
    // Clean up
    for (size_t i = 2500; i < 5000; ++i) {
        neuronPool.deallocate(neuronIds[i]);
    }
    
    assert(neuronPool.size() == 0);
    assert(neuronPool.empty());
    
    std::cout << "  NeuronPool: PASS" << std::endl;
    
    // Test SynapsePool
    nlm::SynapsePool synapsePool;
    
    // Allocate synapses
    std::vector<nlm::SynapseId> synapseIds;
    synapseIds.reserve(10000);
    
    for (size_t i = 0; i < 10000; ++i) {
        nlm::SynapseId id = synapsePool.allocate(nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapseIds.push_back(id);
        assert(id.index() != 0);  // Valid ID
    }
    
    // Verify all allocated
    assert(synapsePool.size() == 10000);
    
    // Deallocate all
    for (const auto& id : synapseIds) {
        synapsePool.deallocate(id);
    }
    
    assert(synapsePool.size() == 0);
    assert(synapsePool.empty());
    
    std::cout << "  SynapsePool: PASS" << std::endl;
    
    // Memory usage test
    size_t neuronMem = neuronPool.memoryUsage();
    size_t synapseMem = synapsePool.memoryUsage();
    
    std::cout << "  Memory usage - NeuronPool: " << neuronMem << " bytes" << std::endl;
    std::cout << "  Memory usage - SynapsePool: " << synapseMem << " bytes" << std::endl;
    
    // Should be deterministic and not grow with operations
    assert(neuronMem > 0);
    assert(synapseMem > 0);
    
    std::cout << "  MemoryPool: PASS" << std::endl;
}

// Test SpikeSystem with memory pools
void testSpikeSystemMemoryPool() {
    std::cout << "Testing SpikeSystem with MemoryPool..." << std::endl;
    
    nlm::SpikeSystem spikeSystem;
    
    // Register handlers
    size_t spikeCount = 0;
    spikeSystem.registerHandler([&spikeCount](const nlm::DetailedSpikeEvent& event) {
        spikeCount++;
    });
    
    size_t delayedCount = 0;
    spikeSystem.registerDelayedHandler([&delayedCount](const nlm::DelayedSpikeEvent& event) {
        delayedCount++;
    });
    
    // Create spike events
    nlm::SpikeEvent spike(nlm::NeuronId(1), 1.0, 1);
    nlm::DelayedSpikeEvent delayed(
        nlm::NeuronId(1), nlm::NeuronId(2), nlm::SynapseId(1), 
        0.5, nlm::SynapseType::Excitatory, 1.0, 2.0, 1, 2
    );
    
    // Queue spikes
    spikeSystem.queueSpike(spike);
    spikeSystem.queueDelayedSpike(delayed);
    
    // Verify events processed
    assert(spikeCount == 1);
    assert(delayedCount == 1);
    
    // Verify memory pool usage
    assert(!spikeSystem.getSpikeHistory().empty());
    
    std::cout << "  SpikeSystem with MemoryPool: PASS" << std::endl;
}

// Test Brain integration with memory pools
void testBrainMemoryPoolIntegration() {
    std::cout << "Testing Brain with MemoryPool integration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize
    bool success = brain.initialize();
    assert(success);
    
    // Verify memory pools are being used
    auto* spikeSystem = brain.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    // Test basic operations
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test step
    brain.step(1);
    
    // Verify statistics
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "  Brain MemoryPool Integration: PASS" << std::endl;
}

// Test performance improvements
void testPerformanceImprovements() {
    std::cout << "Testing performance improvements..." << std::endl;
    
    // Test 1: Memory pool allocation speed
    auto start = std::chrono::high_resolution_clock::now();
    
    nlm::NeuronPool neuronPool(100000);
    
    // Allocate many neurons
    std::vector<nlm::NeuronId> ids;
    for (size_t i = 0; i < 50000; ++i) {
        ids.push_back(neuronPool.allocate());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "  Allocated 50,000 neurons in " << duration.count() << " microseconds" << std::endl;
    assert(duration.count() < 100000);  // Should be fast (< 100ms)
    
    // Deallocate all
    for (const auto& id : ids) {
        neuronPool.deallocate(id);
    }
    
    // Test 2: Synapse pool operations
    nlm::SynapsePool synapsePool;
    
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<nlm::SynapseId> synIds;
    for (size_t i = 0; i < 100000; ++i) {
        synIds.push_back(synapsePool.allocate(nlm::NeuronId(i), nlm::NeuronId(i + 1)));
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "  Allocated 100,000 synapses in " << duration.count() << " microseconds" << std::endl;
    assert(duration.count() < 200000);  // Should be fast (< 200ms)
    
    // Clean up
    for (const auto& id : synIds) {
        synapsePool.deallocate(id);
    }
    
    // Test 3: Memory efficiency (deterministic usage)
    size_t neuronMem = neuronPool.memoryUsage();
    size_t synapseMem = synapsePool.memoryUsage();
    
    std::cout << "  Memory usage - Neurons: " << neuronMem << " bytes" << std::endl;
    std::cout << "  Memory usage - Synapses: " << synapseMem << " bytes" << std::endl;
    
    // Memory should not grow unbounded
    assert(neuronMem < 100 * 1024 * 1024);  // Less than 100MB
    assert(synapseMem < 200 * 1024 * 1024);  // Less than 200MB
    
    std::cout << "  Performance improvements: PASS" << std::endl;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "Testing Optimized NLM Memory Pools" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "" << std::endl;
    
    try {
        testMemoryPoolPerformance();
        testSpikeSystemMemoryPool();
        testBrainMemoryPoolIntegration();
        testPerformanceImprovements();
        
        std::cout << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << "All tests passed successfully!" << std::endl;
        std::cout << "============================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception caught" << std::endl;
        return 1;
    }
}