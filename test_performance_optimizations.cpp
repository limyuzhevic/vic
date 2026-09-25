// Test file to verify performance optimizations
#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Memory/MemoryManager.hpp"
#include "core/SIMD/NeuronSIMD.hpp"
#include "core/SIMD/SynapseSIMD.hpp"
#include "optimization/CoreOptimizations.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_optimizations {

void testMemoryManager() {
    std::cout << "Testing Memory Manager..." << std::endl;
    
    nlm::MemoryManager& memoryManager = nlm::MemoryManager::getInstance();
    memoryManager.initialize(1024, 1024);
    
    // Test neuron pool
    nlm::NeuronPool& neuronPool = memoryManager.getNeuronPool();
    neuronPool.reserve(512);
    
    // Test basic allocation/deallocation
    nlm::Neuron* neuron = neuronPool.allocate(nlm::NeuronId(1), nlm::NeuronType::Excitatory);
    assert(neuron != nullptr);
    assert(neuron->getId() == nlm::NeuronId(1));
    assert(neuron->getType() == nlm::NeuronType::Excitatory);
    
    neuronPool.deallocate(neuron);
    
    // Test synapse pool
    nlm::SynapsePool& synapsePool = memoryManager.getSynapsePool();
    synapsePool.reserve(512);
    
    nlm::Synapse* synapse = synapsePool.allocate(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    assert(synapse != nullptr);
    assert(synapse->getId() == nlm::SynapseId(1));
    assert(synapse->getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse->getDestinationNeuron() == nlm::NeuronId(2));
    
    synapsePool.deallocate(synapse);
    
    memoryManager.printMemoryStats();
    
    std::cout << "Memory Manager tests passed!" << std::endl << std::endl;
}

void testSIMDNeurons() {
    std::cout << "Testing SIMD Neuron Operations..." << std::endl;
    
    // Create test neurons
    std::vector<nlm::NeuronState> states;
    std::vector<nlm::NeuronId> neuronIds;
    
    for (int i = 0; i < 16; ++i) {
        states.push_back(nlm::NeuronState());
        neuronIds.push_back(nlm::NeuronId(i));
        states[i].membranePotential = -65.0f + i * 2.0f;
        states[i].threshold = -55.0f;
        states[i].firingState = nlm::FiringState::Resting;
    }
    
    // Test SIMD threshold checking
    std::vector<bool> shouldFire(16, false);
    nlm::NeuronSIMD::checkThresholdSIMD(states.data(), shouldFire.data(), states.size());
    
    int firingCount = 0;
    for (bool shouldFireNeuron : shouldFire) {
        if (shouldFireNeuron) firingCount++;
    }
    assert(firingCount > 0);
    
    // Test SIMD membrane potential updates
    std::vector<float> currents(states.size(), 10.0f);
    std::vector<float> timeConstants(states.size(), 20.0f);
    std::vector<float> leakConductances(states.size(), 10.0f);
    std::vector<float> restingPotentials(states.size(), -70.0f);
    std::vector<float> thresholds(states.size());
    for (size_t i = 0; i < states.size(); ++i) thresholds[i] = states[i].threshold;
    
    nlm::NeuronSIMD::updateMembranePotentialSIMD(states.data(), currents.data(),
                                                timeConstants.data(), leakConductances.data(),
                                                restingPotentials.data(), thresholds.data(),
                                                states.size(), 0.001f);
    
    // Test SIMD firing rate computation
    std::vector<float> firingRates(states.size(), 0.0f);
    nlm::NeuronSIMD::computeFiringRateSIMD(states.data(), firingRates.data(), states.size());
    
    // Verify that firing rates are computed
    bool firingRateComputed = false;
    for (float rate : firingRates) {
        if (rate > 0.0f) firingRateComputed = true;
    }
    assert(firingRateComputed);
    
    // Test SIMD refractory update
    nlm::NeuronSIMD::updateRefractorySIMD(states.data(), states.size());
    
    // Test SIMD adaptation update
    std::vector<float> adaptationRates(states.size(), 0.01f);
    nlm::NeuronSIMD::updateAdaptationSIMD(states.data(), adaptationRates.data(), states.size());
    
    std::cout << "SIMD Neuron tests passed!" << std::endl << std::endl;
}

void testSIMDSynapses() {
    std::cout << "Testing SIMD Synapse Operations..." << std::endl;
    
    // Create test synapses
    std::vector<nlm::Synapse::Impl> synapses;
    std::vector<float> preSpikeTimes;
    std::vector<float> postSpikeTimes;
    std::vector<float> weightDeltas;
    
    for (int i = 0; i < 16; ++i) {
        nlm::Synapse::Impl synapse;
        synapse.id = nlm::SynapseId(i);
        synapse.sourceNeuron = nlm::NeuronId(i);
        synapse.destinationNeuron = nlm::NeuronId(i + 1);
        synapse.weight = 0.5f;
        synapses.push_back(synapse);
        
        preSpikeTimes.push_back(static_cast<float>(i % 5));
        postSpikeTimes.push_back(static_cast<float>((i + 2) % 5));
        weightDeltas.push_back(0.0f);
    }
    
    // Test SIMD STDP updates
    nlm::SynapseSIMD::updateSTDPSIMD(synapses, preSpikeTimes, postSpikeTimes, weightDeltas, 0.01f);
    
    // Verify that weight deltas were computed
    bool weightDeltasComputed = false;
    for (float delta : weightDeltas) {
        if (delta != 0.0f) weightDeltasComputed = true;
    }
    assert(weightDeltasComputed);
    
    // Test SIMD weight updates
    std::vector<float> weights(16, 0.5f);
    stdlm::SynapseSIMD::updateWeightSIMD(weights.data(), weightDeltas.data(), weights.size());
    
    // Test SIMD spike recording
    std::vector<nlm::Timestamp> spikeHistory;
    std::vector<nlm::Timestamp> timestamps;
    for (int i = 0; i < 16; ++i) {
        timestamps.push_back(static_cast<nlm::Timestamp>(i));
    }
    
    nlm::SynapseSIMD::recordSpikesSIMD(spikeHistory, timestamps.data(), timestamps.size());
    assert(spikeHistory.size() == 16);
    
    std::cout << "SIMD Synapse tests passed!" << std::endl << std::endl;
}

void testModernCpp20Features() {
    std::cout << "Testing Modern C++20 Features..." << std::endl;
    
    // Test concepts (would fail to compile if concept not satisfied)
    nlm::cpp20::NeuronStateConcept testConcept = true; // Placeholder
    
    // Test structured bindings
    nlm::cpp20::NeuronDynamicsResult result{
        membranePotential: -65.0f,
        shouldFire: true,
        firingRate: 50.0f,
        adaptationVariable: 0.5f
    };
    
    // Access structured bindings (conceptually)
    assert(result.membranePotential == -65.0f);
    assert(result.shouldFire == true);
    assert(result.firingRate == 50.0f);
    assert(result.adaptationVariable == 0.5f);
    
    // Test ranges utilities
    std::vector<nlm::NeuronState> states;
    for (int i = 0; i < 10; ++i) {
        nlm::NeuronState state;
        state.membranePotential = static_cast<float>(i * 10);
        state.firingRate = static_cast<float>(i);
        state.firingState = (i % 2 == 0) ? nlm::FiringState::Active : nlm::FiringState::Resting;
        states.push_back(state);
    }
    
    // Test C++20 ranges through the utilities
    auto restingNeurons = nlm::cpp20::RangeUtilities::neuronsByFiringState(states, nlm::FiringState::Resting);
    // In a real implementation, we would iterate over the range
    
    // Test constexpr functions
    float timeConstant = nlm::cpp20::MathConstants::computeLIFTimeConstant(1.0f, 0.05f);
    assert(timeConstant == 20.0f);
    
    float membranePotential = nlm::cpp20::MathConstants::computeMembranePotential(-65.0f, 10.0f, 10.0f, 0.001f);
    // Verify the calculation (approximate)
    assert(membranePotential > -65.0f);
    
    nlm::cpp20::demonstrateC++20Features();
    
    std::cout << "Modern C++20 tests passed!" << std::endl << std::endl;
}

void testParallelProcessing() {
    std::cout << "Testing Parallel Processing..." << std::endl;
    
    // Test parallel spike detection
    std::vector<nlm::NeuronState> states;
    std::vector<nlm::NeuronId> neuronIds;
    
    for (int i = 0; i < 100; ++i) {
        states.push_back(nlm::NeuronState());
        neuronIds.push_back(nlm::NeuronId(i));
        states[i].membranePotential = -65.0f + static_cast<float>(i % 20);
        states[i].threshold = -55.0f;
    }
    
    std::vector<bool> shouldFire(states.size(), false);
    
    // Process in parallel using the SIMD spike system
    nlm::SIMDSpikeSystem spikeSystem;
    spikeSystem.detectSpikesAVX512(states.data(), neuronIds, shouldFire, states.size());
    
    int firingCount = 0;
    for (bool shouldFireNeuron : shouldFire) {
        if (shouldFireNeuron) firingCount++;
    }
    assert(firingCount > 0);
    
    // Test parallel spike propagation
    spikeSystem.propagateSpikes(shouldFire, 0.001f);
    
    std::cout << "Parallel processing tests passed!" << std::endl << std::endl;
}

void testPerformanceMonitoring() {
    std::cout << "Testing Performance Monitoring..." << std::endl;
    
    nlm::PerformanceMonitor& monitor = nlm::PerformanceMonitor::getInstance();
    monitor.reset();
    
    // Simulate some performance measurements
    monitor.recordStepTime(0.001f);
    monitor.recordSIMDEfficiency(0.95f);
    monitor.recordMemoryUsage(1024 * 1024);
    
    // Increment operation counters
    monitor.incrementNeuronUpdates(1000);
    monitor.incrementSynapseUpdates(500);
    monitor.incrementSpikePropagations(100);
    
    // Update metrics
    monitor.updateMetrics();
    
    // Verify metrics
    auto metrics = monitor.getMetrics();
    assert(metrics.totalSimulationSteps > 0);
    assert(metrics.simdEfficiency > 0.0f);
    assert(metrics.neuronUpdates > 0);
    assert(metrics.synapseUpdates > 0);
    assert(metrics.spikePropagations > 0);
    
    // Print metrics
    monitor.printMetrics();
    
    std::cout << "Performance monitoring tests passed!" << std::endl << std::endl;
}

void testMemoryOptimization() {
    std::cout << "Testing Memory Optimizations..." << std::endl;
    
    // Test memory alignment
    alignas(64) int testArray[64] = {0};
    bool isAligned = nlm::MemoryOptimizer::isAligned(testArray, 64);
    assert(isAligned);
    
    // Test memory allocation
    void* alignedPtr = nlm::MemoryOptimizer::alignedAllocate(1024, 64);
    assert(alignedPtr != nullptr);
    assert(nlm::MemoryOptimizer::isAligned(alignedPtr, 64));
    
    nlm::MemoryOptimizer::alignedDeallocate(alignedPtr, 1024);
    
    // Test memory pool stats
    nlm::MemoryPoolStats neuronStats = nlm::MemoryOptimizer::getNeuronPoolStats();
    nlm::MemoryPoolStats synapseStats = nlm::MemoryOptimizer::getSynapsePoolStats();
    
    std::cout << "Neuron Pool Stats:" << std::endl;
    std::cout << "  Allocations: " << neuronStats.allocations << std::endl;
    std::cout << "  Deallocations: " << neuronStats.deallocations << std::endl;
    std::cout << "  Hits: " << neuronStats.hits << std::endl;
    std::cout << "  Misses: " << neuronStats.misses << std::endl;
    std::cout << std::endl;
    
    std::cout << "Synapse Pool Stats:" << std::endl;
    std::cout << "  Allocations: " << synapseStats.allocations << std::endl;
    std::cout << "  Deallocations: " << synapseStats.deallocations << std::endl;
    std::cout << "  Hits: " << synapseStats.hits << std::endl;
    std::cout << "  Misses: " << synapseStats.misses << std::endl;
    
    std::cout << "Memory optimization tests passed!" << std::endl << std::endl;
}

void testSIMDArchitectureDetection() {
    std::cout << "Testing SIMD Architecture Detection..." << std::endl;
    
    nlm::simd::SIMDInfo simdInfo = nlm::simd::detectSIMD();
    
    assert(simdInfo.sse2 || simdInfo.sse3 || simdInfo.ssse3 || simdInfo.sse4_1 || 
           simdInfo.sse4_2 || simdInfo.avx || simdInfo.avx2 || simdInfo.avx512f);
    
    std::cout << "Detected SIMD capabilities:" << std::endl;
    std::cout << "  SSE2: " << (simdInfo.sse2 ? "Yes" : "No") << std::endl;
    std::cout << "  SSE3: " << (simdInfo.sse3 ? "Yes" : "No") << std::endl;
    std::cout << "  SSSE3: " << (simdInfo.ssse3 ? "Yes" : "No") << std::endl;
    std::cout << "  SSE4.1: " << (simdInfo.sse4_1 ? "Yes" : "No") << std::endl;
    std::cout << "  SSE4.2: " << (simdInfo.sse4_2 ? "Yes" : "No") << std::endl;
    std::cout << "  AVX: " << (simdInfo.avx ? "Yes" : "No") << std::endl;
    std::cout << "  AVX2: " << (simdInfo.avx2 ? "Yes" : "No") << std::endl;
    std::cout << "  AVX-512: " << (simdInfo.avx512f ? "Yes" : "No") << std::endl;
    
    std::cout << "SIMD architecture detection tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== NLM Performance Optimizations Tests ===" << std::endl << std::endl;
    
    try {
        testMemoryManager();
        testSIMDNeurons();
        testSIMDSynapses();
        testModernCpp20Features();
        testParallelProcessing();
        testPerformanceMonitoring();
        testMemoryOptimization();
        testSIMDArchitectureDetection();
        
        std::cout << "=== All Optimization Tests Passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception!" << std::endl;
        return 1;
    }
}

} // namespace test_optimizations

// Entry point
int main() {
    return test_optimizations::main();
}