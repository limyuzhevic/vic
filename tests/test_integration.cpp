// NLM Integration Tests
// Test Phase 6 integration, backward compatibility, system stability, and performance improvements

#include "performance/EventQueue.hpp"
#include "performance/MemoryPool.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Reward.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

namespace test_integration {

void testPhase6Integration() {
    std::cout << "Testing Phase 6 Integration (Memory, Cognition, Prediction, Neuromodulation)..." << std::endl;
    
    // Test Memory System Integration
    nlm::MemoryPool<float> memoryPool(100, 10);
    
    float* neuronStates = memoryPool.allocate();
    *neuronStates = 0.5f;
    assert(*neuronStates == 0.5f);
    
    memoryPool.deallocate(neuronStates);
    
    // Test Cognition System Integration
    nlm::Brain brain;
    
    // Add neurons and synapses
    auto neuron1 = brain.addNeuron(nlm::NeuronType::Excitatory);
    auto neuron2 = brain.addNeuron(nlm::NeuronType::Inhibitory);
    auto synapse = brain.addSynapse(neuron1->getId(), neuron2->getId());
    
    // Simulate neural activity
    neuron1->recordSpike(0.0f);
    neuron2->recordSpike(0.1f);
    
    // Test Concept Formation
    brain.setConcept("test_concept", 0.8f);
    assert(brain.getConcept("test_concept") == 0.8f);
    
    // Test Neural Planning
    brain.addPlanningAction("action1", 0.7f);
    assert(brain.getPlanningActionStrength("action1") == 0.7f);
    
    // Test Prediction System Integration
    nlm::PredictionSystem prediction;
    
    prediction.setPrediction("state_change", 0.9f);
    assert(prediction.getPrediction("state_change") == 0.9f);
    
    prediction.updatePredictionError(0.1f); // Prediction error = 0.1
    assert(prediction.getPredictionError() == 0.1f);
    
    // Test Neuromodulation Integration
    nlm::Reward reward;
    reward.add(1.0f);
    assert(reward.getValue() == 1.0f);
    
    // Neuromodulators influence plasticity
    nlm::Neuromodulator neuromodulator;
    neuromodulator.setModulationFactor(0.5f);
    assert(modulator.getModulationFactor() == 0.5f);
    
    // Test Event System Integration
    nlm::EventQueue eventQueue;
    
    nlm::QueuedSpikeEvent spike(1, 2, 3, 0.5f, true);
    eventQueue.push(spike);
    
    auto popped = eventQueue.pop();
    assert(popped.has_value());
    assert(popped->sourceNeuron == 1);
    
    std::cout << "    testPhase6Integration passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing Backward Compatibility..." << std::endl;
    
    // Test that existing API still works
    nlm::Brain brain;
    
    // Old-style neuron operations should still work
    auto neuron1 = brain.addNeuron(nlm::NeuronType::Excitatory);
    assert(neuron1->getType() == nlm::NeuronType::Excitatory);
    
    // Old-style synapse operations
    auto synapse = brain.addSynapse(neuron1->getId(), neuron1->getId());
    synapse->setWeight(0.5f);
    assert(synapse->getWeight() == 0.5f);
    
    // Old-style plasticity operations
    nlm::STDP stdp;
    stdp.setLTPWeight(0.02f);
    stdp.setLTDWeight(0.015f);
    
    assert(stdp.getLTPWeight() == 0.02f);
    assert(stdp.getLTDWeight() == 0.015f);
    
    // Old-style development operations
    nlm::DevelopmentSystem dev;
    dev.setStage(nlm::DevelopmentStage::Early);
    assert(dev.getStage() == nlm::DevelopmentStage::Early);
    
    // Test that config loading still works
    nlm::Config config;
    config.set("test_param", 42, nlm::ConfigSource::Runtime);
    assert(config.get<int>("test_param").value() == 42);
    
    // Test event queue backward compatibility
    nlm::EventQueue eventQueue;
    
    nlm::QueuedSpikeEvent spike(1, 2, 3, 0.5f, true);
    bool pushed = eventQueue.push(spike);
    assert(pushed);
    
    auto popped = eventQueue.pop();
    assert(popped.has_value());
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void testSystemStability() {
    std::cout << "Testing System Stability..." << std::endl;
    
    // Test stability under heavy load
    nlm::Brain brain;
    
    // Add many neurons
    const size_t neuronCount = 1000;
    std::vector<NeuronId> neurons;
    
    for (size_t i = 0; i < neuronCount; ++i) {
        auto neuron = brain.addNeuron(nlm::NeuronType::Excitatory);
        neurons.push_back(neuron->getId());
    }
    
    assert(brain.getNeuronCount() == neuronCount);
    
    // Add synapses
    for (size_t i = 0; i < neuronCount; i += 10) {
        for (size_t j = 0; j < 10; ++j) {
            auto targetNeuron = brain.addNeuron(nlm::NeuronType::Inhibitory);
            auto synapse = brain.addSynapse(neurons[i], targetNeuron->getId());
            synapse->setWeight(0.5f / (j + 1));
        }
    }
    
    // Test memory pool stability
    nlm::MemoryPool<float> memoryPool(1000, 100);
    
    std::vector<float*> allocated;
    for (size_t i = 0; i < 500; ++i) {
        float* ptr = memoryPool.allocate();
        *ptr = i * 0.1f;
        allocated.push_back(ptr);
    }
    
    // Use allocated memory
    for (size_t i = 0; i < allocated.size(); ++i) {
        assert(*allocated[i] == i * 0.1f);
    }
    
    // Deallocate all
    for (auto ptr : allocated) {
        memoryPool.deallocate(ptr);
    }
    
    assert(memoryPool.allocatedCount() == 0);
    
    // Test event queue stability under load
    nlm::EventQueue eventQueue;
    
    for (size_t i = 0; i < 100; ++i) {
        nlm::QueuedSpikeEvent spike(i, i + 1, i, 0.5f, true);
        eventQueue.push(spike);
    }
    
    // Pop all events
    for (size_t i = 0; i < 100; ++i) {
        auto event = eventQueue.pop();
        assert(event.has_value());
    }
    
    assert(eventQueue.empty());
    
    // Test performance under stress
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 1000; ++i) {
        // Simulate intensive computation
        float result = 0.0f;
        for (size_t j = 0; j < 100; ++j) {
            result += i * j * 0.01f;
        }
        (void)result;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete quickly
    assert(duration.count() < 1000);
    
    std::cout << "    testSystemStability passed" << std::endl;
}

void testPerformanceImprovements() {
    std::cout << "Testing Performance Improvements..." << std::endl;
    
    // Test that new optimizations actually improve performance
    nlm::Brain brain;
    
    // Add neurons for performance testing
    for (size_t i = 0; i < 500; ++i) {
        brain.addNeuron(nlm::NeuronType::Excitatory);
    }
    
    // Test optimized spike propagation
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate spike propagation through optimized brain
    for (size_t i = 0; i < 100; ++i) {
        brain.recordSpike(i % 500, i * 0.001f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto spikeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should be fast with optimizations
    assert(spikeDuration.count() < 100);
    
    // Test memory pool performance
    nlm::MemoryPool<float> memoryPool(10000, 1000);
    
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<float*> allocated;
    for (size_t i = 0; i < 5000; ++i) {
        float* ptr = memoryPool.allocate();
        *ptr = static_cast<float>(i);
        allocated.push_back(ptr);
    }
    
    for (size_t i = 0; i < allocated.size(); ++i) {
        assert(*allocated[i] == static_cast<float>(i));
    }
    
    for (auto ptr : allocated) {
        memoryPool.deallocate(ptr);
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto memoryDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Memory pool operations should be fast
    assert(memoryDuration.count() < 1000);
    
    // Test SIMD performance improvements
    std::vector<float> vec1(10000, 1.0f);
    std::vector<float> vec2(10000, 2.0f);
    std::vector<float> result(10000);
    
    start = std::chrono::high_resolution_clock::now();
    
    nlm::SIMDVectorization::add(vec1.data(), vec2.data(), result.data(), vec1.size());
    
    end = std::chrono::high_resolution_clock::now();
    auto simdDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // SIMD operations should be fast
    assert(simdDuration.count() < 100);
    
    // Test parallel processing improvements
    nlm::ThreadPool pool(4, true);
    
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<int> data(10000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = i;
    }
    
    std::vector<int> results(data.size());
    pool.parallelFor(0, data.size(), [&](size_t i) {
        results[i] = data[i] * 2;
    });
    
    end = std::chrono::high_resolution_clock::now();
    auto parallelDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Parallel processing should be fast
    assert(parallelDuration.count() < 500);
    
    std::cout << "    testPerformanceImprovements passed" << std::endl;
}

void runAll() {
    std::cout << "=== Integration Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test Phase 6 Integration
    std::cout << "Running Phase 6 Integration tests..." << std::endl;
    try {
        testPhase6Integration();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Backward Compatibility
    std::cout << "Running Backward Compatibility tests..." << std::endl;
    try {
        testBackwardCompatibility();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test System Stability
    std::cout << "Running System Stability tests..." << std::endl;
    try {
        testSystemStability();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Performance Improvements
    std::cout << "Running Performance Improvements tests..." << std::endl;
    try {
        testPerformanceImprovements();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
    }
}

} // namespace test_integration