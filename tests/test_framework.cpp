// NLM Test Framework Demonstration - New Test Style
// Demonstrates parameterized tests, fixtures, and performance tracking

#include "nlm_test_framework.h"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"

using namespace nlm;
using namespace nlm::test;

// Simple test case
void simpleTestCase() {
    assert(1 + 1 == 2);
    std::cout << "    simpleTestCase passed" << std::endl;
}

// Performance test with timing
void performanceTest() {
    const int iterations = 1000;
    Neuron neuron(NeuronId(1));
    RandomGenerator rng(123);
    neuron.initializeRandom(rng);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        neuron.setMembranePotential(neuron.getRestingPotential());
        neuron.injectCurrent(1.0f);
        neuron.step(0.0, 0.001f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    double timePerIteration = (duration.count() / 1000.0) / iterations; // milliseconds per iteration
    
    NLM_TEST_ASSERT(neuron.getMembranePotential() > neuron.getRestingPotential(), 
                   "Neuron should have been depolarized");
    
    std::cout << "    performanceTest passed: " << iterations << " iterations in " 
              << std::fixed << std::setprecision(3) << duration.count() / 1000.0 << "ms ("
              << timePerIteration << "ms per iteration)" << std::endl;
}

// Stress test with many neurons
void stressTest() {
    const int neuronCount = 100;
    std::vector<Neuron> neurons;
    
    for (int i = 0; i < neuronCount; ++i) {
        neurons.emplace_back(NeuronId(i));
        neurons[i].setMembranePotential(neurons[i].getRestingPotential());
    }
    
    // Stress test: many operations
    for (int step = 0; step < 10; ++step) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            neurons[i].injectCurrent(5.0f);
            neurons[i].step(static_cast<double>(step), 0.001f);
            neurons[i].recordSpike(static_cast<double>(step));
        }
    }
    
    // Verify results
    size_t totalSpikes = 0;
    for (const auto& neuron : neurons) {
        totalSpikes += neuron.getSpikeHistory().size();
    }
    
    NLM_TEST_ASSERT(totalSpikes == neurons.size() * 10, 
                   "Expected " << neurons.size() * 10 << " spikes, got " << totalSpikes);
    
    std::cout << "    stressTest passed: " << neuronCount << " neurons, " 
              << totalSpikes << " total spikes" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Test Framework Demonstration ===" << std::endl;
    std::cout << "Testing new modern test framework capabilities:" << std::endl;
    
    // Test 1: Simple test case
    simpleTestCase();
    
    // Test 2: Performance test
    performanceTest();
    
    // Test 3: Stress test
    stressTest();
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
}

} // namespace test_framework
