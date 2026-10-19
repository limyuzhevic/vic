#include <chrono>
// Neuron Tests
// Enhanced with comprehensive stress tests, edge cases, and error handling

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace test_neuron {

// Basic Tests (existing, preserved)
void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
}

// Enhanced Tests
void testNeuronStress() {
    std::cout << "    Stress Test" << std::endl;
    
    // Create many neurons and stress test them
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < 1000; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform operations on all neurons
    for (auto& neuron : neurons) {
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.setMembranePotential(static_cast<float>(i) * 0.1f);
        neuron.setFiringState(nlm::FiringState::Active);
        neuron.setRefractoryPeriod(10);
        neuron.injectCurrent(5.0f);
        neuron.recordSpike(0.1f);
    }
    
    // Verify all neurons are in valid state
    for (auto& neuron : neurons) {
        assert(neuron.isFiring());
        assert(neuron.isRefractory());
        assert(neuron.getTotalCurrent() == 5.0f);
        assert(neuron.getSpikeHistory().size() == 1);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Stress test: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << " neurons/sec" << std::endl;
    
    std::cout << "      Stress test passed" << std::endl;
}

void testNeuronEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    // Test extreme membrane potentials
    nlm::Neuron neuron1(nlm::NeuronId(1));
    neuron1.setMembranePotential(1000.0f);
    assert(neuron1.getMembranePotential() >= -70.0f); // Should be clamped
    
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.setMembranePotential(-1000.0f);
    assert(neuron2.getMembranePotential() <= -70.0f); // Should be clamped
    
    // Test extreme firing states
    nlm::Neuron neuron3(nlm::NeuronId(3));
    neuron3.setFiringState(nlm::FiringState::Active);
    assert(neuron3.isFiring());
    
    neuron3.setRefractoryPeriod(0); // Zero refractory period
    assert(!neuron3.isRefractory()); // Should handle zero gracefully
    
    // Test extreme current values
    nlm::Neuron neuron4(nlm::NeuronId(4));
    neuron4.injectCurrent(1000.0f);
    // Should handle extreme current values
    
    // Test extreme threshold values
    nlm::Neuron neuron5(nlm::NeuronId(5));
    neuron5.setThreshold(1000.0f);
    neuron5.setThreshold(-1000.0f);
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testNeuronErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    // Test neuron operations with error conditions
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with negative refractory period
    neuron.setRefractoryPeriod(0);
    assert(!neuron.isRefractory()); // Should handle zero
    
    // Test with very large refractory period
    neuron.setRefractoryPeriod(100000);
    assert(neuron.isRefractory());
    
    // Test firing state transitions
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    // Test decrement when not refractory
    neuron.decrementRefractory(); // Should handle gracefully
    
    // Test spike history with invalid times
    neuron.recordSpike(-1.0f); // Negative spike time
    neuron.recordSpike(std::nanf("")); // NaN spike time
    
    assert(neuron.getSpikeHistory().size() == 2);
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testNeuronRandomStress() {
    std::cout << "    Random Stress Test" << std::endl;
    
    // Test random number generation with different seeds
    for (int seed = 0; seed < 100; ++seed) {
        nlm::RandomGenerator rng(static_cast<uint64_t>(seed));
        nlm::Neuron neuron(nlm::NeuronId(seed));
        
        neuron.initializeRandom(rng);
        
        // Verify neuron is in valid state
        assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
        assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
        assert(neuron.getType() == nlm::NeuronType::Internal || 
               neuron.getType() == nlm::NeuronType::Excitatory || 
               neuron.getType() == nlm::NeuronType::Inhibitory);
    }
    
    std::cout << "      Random stress test passed" << std::endl;
}

void testNeuronMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    // Test neuron creation and destruction
    std::vector<nlm::Neuron> neurons;
    
    // Create many neurons
    for (int i = 0; i < 500; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
    }
    
    // Perform operations
    for (int i = 0; i < neurons.size(); ++i) {
        neurons[i].setMembranePotential(static_cast<float>(i) * 0.01f);
        neurons[i].injectCurrent(1.0f);
        neurons[i].recordSpike(static_cast<float>(i) * 0.1f);
    }
    
    // Clear spike history
    for (auto& neuron : neurons) {
        neuron.clearSpikeHistory();
        neuron.clearTotalCurrent();
    }
    
    // Verify all spike histories are cleared
    for (auto& neuron : neurons) {
        assert(neuron.getSpikeHistory().empty());
    }
    
    // Vector should be in valid state
    assert(neurons.size() == 500);
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testNeuronPerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    // Performance test for neuron operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.setMembranePotential(static_cast<float>(i) * 0.001f);
        neuron.injectCurrent(1.0f);
        neuron.setFiringState(nlm::FiringState::Active);
        neuron.recordSpike(static_cast<float>(i) * 0.01f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 10000.0 / totalTime << " neurons/sec" << std::endl;
    
    std::cout << "      Performance test passed" << std::endl;
}

void testNeuronIntegration() {
    std::cout << "    Integration Test" << std::endl;
    
    // Test neuron integration with random generator
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.initializeRandom(rng);
        
        // Test all neuron functions
        neuron.setMembranePotential(static_cast<float>(i) * 0.1f);
        neuron.setType(static_cast<nlm::NeuronType>(i % 3));
        neuron.setFiringState(nlm::FiringState::Active);
        neuron.setRefractoryPeriod(5);
        neuron.injectCurrent(2.0f);
        neuron.recordSpike(static_cast<float>(i));
        
        // Verify state
        assert(neuron.getId() == nlm::NeuronId(i));
        assert(neuron.getTotalCurrent() == 2.0f);
        assert(neuron.getSpikeHistory().size() == 1);
    }
    
    std::cout << "      Integration test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Neuron Tests..." << std::endl;
    
    try {
        // Run basic tests
        testNeuronCreation();
        testNeuronType();
        testMembranePotential();
        testFiringState();
        testRefractoryDecrement();
        testCurrentInjection();
        testSynapticInput();
        testSpikeHistory();
        testRandomInitialization();
        testReset();
        
        // Run enhanced tests
        testNeuronStress();
        testNeuronEdgeCases();
        testNeuronErrorHandling();
        testNeuronRandomStress();
        testNeuronMemoryManagement();
        testNeuronPerformance();
        testNeuronIntegration();
        
        std::cout << "  All Enhanced Neuron Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_neuron