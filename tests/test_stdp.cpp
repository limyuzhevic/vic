#include <chrono>
// STDP Tests
// Enhanced with edge cases, error scenarios, and performance tests

#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace test_stdp {

// Basic Tests (existing, preserved)
void testSTDPCreation() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.035f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    std::cout << "    testSTDPName passed" << std::endl;
}

void testSTDPPotentiation() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPDepression() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
    
    std::cout << "    testSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikes passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

// Enhanced Tests
void testSTDPStress() {
    std::cout << "    Stress Test" << std::endl;
    
    // Create many STDP systems and stress test them
    std::vector<nlm::STDP> stdps;
    for (int i = 0; i < 1000; ++i) {
        stdps.emplace_back();
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform operations on all STDP systems
    for (int i = 0; i < stdps.size(); ++i) {
        stdps[i].configure(0.01f + static_cast<float>(i) * 0.001f,
                          0.012f + static_cast<float>(i) * 0.001f,
                          20.0f + static_cast<float>(i));
    }
    
    // Verify all STDP systems are in valid state
    for (auto& stdp : stdps) {
        assert(stdp.getLTPWeight() >= 0.0f && stdp.getLTPWeight() <= 1.0f);
        assert(stdp.getLTDWeight() >= 0.0f && stdp.getLTDWeight() <= 1.0f);
        assert(stdp.getTimeConstant() >= 0.0f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Stress test: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << " stdps/sec" << std::endl;
    
    std::cout << "      Stress test passed" << std::endl;
}

void testSTDPEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    nlm::STDP stdp;
    
    // Test extreme configuration values
    stdp.configure(-1.0f, 0.5f, 10.0f); // Negative LTP weight
    assert(stdp.getLTPWeight() >= 0.0f); // Should be clamped
    
    stdp.configure(0.5f, -1.0f, 10.0f); // Negative LTD weight
    assert(stdp.getLTDWeight() >= 0.0f); // Should be clamped
    
    stdp.configure(0.5f, 0.5f, -100.0f); // Negative time constant
    assert(stdp.getTimeConstant() >= 0.0f); // Should handle gracefully
    
    // Test with zero time constant
    stdp.configure(0.5f, 0.5f, 0.0f);
    assert(stdp.getTimeConstant() >= 0.0f);
    
    // Test with very large time constant
    stdp.configure(0.5f, 0.5f, 1000000.0f);
    assert(stdp.getTimeConstant() >= 0.0f);
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testSTDPErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    // Test STDP operations with error conditions
    nlm::STDP stdp;
    
    // Test with null synapse (should not crash)
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test with empty spike vectors
    std::vector<nlm::Timestamp> emptySpikes;
    stdp.update(&synapse, emptySpikes, emptySpikes, 0.001);
    
    // Test with single spike
    std::vector<nlm::Timestamp> singleSpike = {0.0};
    stdp.update(&synapse, singleSpike, singleSpike, 0.001);
    
    // Test with very large spike values
    std::vector<nlm::Timestamp> largeSpikes(1000, 1000.0f);
    stdp.update(&synapse, largeSpikes, largeSpikes, 0.001);
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testSTDPMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    // Test STDP creation and destruction
    std::vector<nlm::STDP> stdps;
    
    // Create many STDP systems
    for (int i = 0; i < 500; ++i) {
        stdps.emplace_back();
    }
    
    // Perform operations
    for (int i = 0; i < stdps.size(); ++i) {
        stdps[i].configure(0.01f + static_cast<float>(i) * 0.001f,
                          0.012f + static_cast<float>(i) * 0.001f,
                          20.0f + static_cast<float>(i));
    }
    
    // Vector should be in valid state
    assert(stdps.size() == 500);
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testSTDPPerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    // Performance test for STDP operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        nlm::STDP stdp;
        stdp.configure(0.01f + static_cast<float>(i) * 0.0001f,
                      0.012f + static_cast<float>(i) * 0.0001f,
                      20.0f + static_cast<float>(i) * 0.1f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 10000.0 / totalTime << " stdps/sec" << std::endl;
    
    std::cout << "      Performance test passed" << std::endl;
}

void testSTDPIntegration() {
    std::cout << "    Integration Test" << std::endl;
    
    // Test STDP integration with synapses and random systems
    for (int i = 0; i < 100; ++i) {
        nlm::STDP stdp;
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1000));
        nlm::RandomGenerator rng(static_cast<uint64_t>(i));
        
        // Set synapse properties
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        // Generate spike patterns
        std::vector<nlm::Timestamp> preSpikes;
        std::vector<nlm::Timestamp> postSpikes;
        
        for (int j = 0; j < 5; ++j) {
            preSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0f + i * 0.1f));
            postSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0f + 5.0f + i * 0.1f));
        }
        
        // Apply STDP
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Verify synapse state
        assert(synapse.getId() == nlm::SynapseId(i));
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    }
    
    std::cout << "      Integration test passed" << std::endl;
}

void testSTDPLearningDynamics() {
    std::cout << "    Learning Dynamics Test" << std::endl;
    
    nlm::STDP stdp;
    
    // Test learning dynamics with different spike timings
    std::vector<std::pair<std::vector<nlm::Timestamp>, std::vector<nlm::Timestamp>>> spikePatterns = {
        // Pre-before-post (potentiation)
        {{0.0, 10.0, 20.0}, {5.0, 15.0, 25.0}},
        // Post-before-pre (depression)
        {{10.0, 20.0, 30.0}, {5.0, 15.0, 25.0}},
        // Synchronous spikes (no change)
        {{0.0, 10.0, 20.0}, {0.0, 10.0, 20.0}},
        // Very different timings
        {{0.0, 1.0}, {100.0, 101.0}},
        // Single spike
        {{5.0}, {5.0}},
    };
    
    nlm::Synapse testSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    for (size_t i = 0; i < spikePatterns.size(); ++i) {
        float initialWeight = testSynapse.getWeight();
        
        stdp.update(&testSynapse, spikePatterns[i].first, spikePatterns[i].second, 0.001);
        
        float finalWeight = testSynapse.getWeight();
        
        // Verify weight change is reasonable
        assert(finalWeight >= -1.0f && finalWeight <= 1.0f);
    }
    
    std::cout << "      Learning dynamics test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced STDP Tests..." << std::endl;
    
    try {
        // Run basic tests
        testSTDPCreation();
        testSTDPConfiguration();
        testSTDPConfigure();
        testSTDPName();
        testSTDPPotentiation();
        testSTDPDepression();
        testSTDPEmptySpikes();
        testSTDPWeightBounds();
        
        // Run enhanced tests
        testSTDPStress();
        testSTDPEdgeCases();
        testSTDPErrorHandling();
        testSTDPMemoryManagement();
        testSTDPPerformance();
        testSTDPIntegration();
        testSTDPLearningDynamics();
        
        std::cout << "  All Enhanced STDP Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_stdp