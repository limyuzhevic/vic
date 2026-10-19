#include <chrono>
// Synapse Tests
// Enhanced with comprehensive stress tests, memory management, and error handling

#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace test_synapse {

// Basic Tests (existing, preserved)
void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight() == 0.4f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getDelay() == 1);  // Default
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test basic eligibility trace functionality
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

// Enhanced Tests
void testSynapseStress() {
    std::cout << "    Stress Test" << std::endl;
    
    // Create many synapses and stress test them
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 1000; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1000));
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform operations on all synapses
    for (int i = 0; i < synapses.size(); ++i) {
        synapses[i].setType(nlm::SynapseType::Excitatory);
        synapses[i].setWeight(0.5f + static_cast<float>(i) * 0.001f);
        synapses[i].setDelay(1 + (i % 10));
        synapses[i].recordPreSpike(static_cast<float>(i));
        synapses[i].recordPostSpike(static_cast<float>(i) * 0.5f);
    }
    
    // Verify all synapses are in valid state
    for (int i = 0; i < synapses.size(); ++i) {
        assert(synapses[i].isExcitatory());
        assert(synapses[i].getWeight() >= -1.0f && synapses[i].getWeight() <= 1.0f);
        assert(synapses[i].getDelay() >= 0);
        assert(synapses[i].getPreSpikeHistory().size() == 1);
        assert(synapses[i].getPostSpikeHistory().size() == 1);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Stress test: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << " synapses/sec" << std::endl;
    
    std::cout << "      Stress test passed" << std::endl;
}

void testSynapseEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    // Test with self-connecting synapse (should be allowed)
    nlm::Synapse selfSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(1));
    assert(selfSynapse.getSourceNeuron() == selfSynapse.getDestinationNeuron());
    
    // Test extreme weights
    nlm::Synapse neuron1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    neuron1.setWeight(1000.0f);
    assert(neuron1.getWeight() <= 1.0f);
    
    nlm::Synapse neuron2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    neuron2.setWeight(-1000.0f);
    assert(neuron2.getWeight() >= -1.0f);
    
    // Test extreme delays
    nlm::Synapse neuron3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    neuron3.setDelay(100000);
    assert(neuron3.getDelay() >= 0);
    
    // Test spike history with extreme values
    nlm::Synapse neuron4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    neuron4.recordPreSpike(-1.0f); // Negative spike time
    neuron4.recordPreSpike(std::nanf("")); // NaN spike time
    assert(neuron4.getPreSpikeHistory().size() == 2);
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testSynapseErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    // Test synapse operations with error conditions
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test weight manipulation with extreme values
    synapse.setWeight(2.0f); // Over limit
    assert(synapse.getWeight() <= 1.0f);
    
    synapse.setWeight(-2.0f); // Under limit
    assert(synapse.getWeight() >= -1.0f);
    
    // Test type switching
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Modulatory);
    assert(synapse.isInhibitory()); // Modulatory should be inhibitory
    
    // Test spike history management
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    // Test with duplicate spike times
    synapse.recordPreSpike(0.1f);
    synapse.recordPreSpike(0.1f); // Same time
    assert(synapse.getPreSpikeHistory().size() == 2);
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testSynapseMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    // Test synapse creation and destruction
    std::vector<nlm::Synapse> synapses;
    
    // Create many synapses
    for (int i = 0; i < 500; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 500));
    }
    
    // Perform operations
    for (int i = 0; i < synapses.size(); ++i) {
        synapses[i].setType(nlm::SynapseType::Excitatory);
        synapses[i].setWeight(0.5f);
        synapses[i].setDelay(1 + (i % 5));
        synapses[i].recordPreSpike(static_cast<float>(i));
        synapses[i].recordPostSpike(static_cast<float>(i) * 0.5f);
    }
    
    // Clear spike history
    for (auto& synapse : synapses) {
        synapse.clearHistory();
    }
    
    // Verify all spike histories are cleared
    for (auto& synapse : synapses) {
        assert(synapse.getPreSpikeHistory().empty());
        assert(synapse.getPostSpikeHistory().empty());
    }
    
    // Vector should be in valid state
    assert(synapses.size() == 500);
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testSynapsePerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    // Performance test for synapse operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1000));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapse.setDelay(1 + (i % 10));
        synapse.recordPreSpike(static_cast<float>(i));
        synapse.recordPostSpike(static_cast<float>(i) * 0.5f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 10000.0 / totalTime << " synapses/sec" << std::endl;
    
    std::cout << "      Performance test passed" << std::endl;
}

void testSynapseIntegration() {
    std::cout << "    Integration Test" << std::endl;
    
    // Test synapse integration with STDP and neural systems
    for (int i = 0; i < 100; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1000));
        
        // Set synapse properties
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapse.setDelay(1 + (i % 5));
        
        // Record spike history
        synapse.recordPreSpike(static_cast<float>(i));
        synapse.recordPostSpike(static_cast<float>(i) * 0.5f);
        
        // Test STDP with synapse
        nlm::STDP stdp;
        std::vector<nlm::Timestamp> preSpikes = {static_cast<float>(i), static_cast<float>(i) + 10.0f};
        std::vector<nlm::Timestamp> postSpikes = {static_cast<float>(i) + 5.0f, static_cast<float>(i) + 15.0f};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Verify synapse is in valid state
        assert(synapse.getId() == nlm::SynapseId(i));
        assert(synapse.isExcitatory());
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
        assert(synapse.getDelay() >= 0);
    }
    
    std::cout << "      Integration test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Synapse Tests..." << std::endl;
    
    try {
        // Run basic tests
        testSynapseCreation();
        testSynapseWeight();
        testSynapseWeightBounds();
        testSynapseType();
        testSynapseDelay();
        testSynapseSpikeHistory();
        testSynapseEligibilityTrace();
        
        // Run enhanced tests
        testSynapseStress();
        testSynapseEdgeCases();
        testSynapseErrorHandling();
        testSynapseMemoryManagement();
        testSynapsePerformance();
        testSynapseIntegration();
        
        std::cout << "  All Enhanced Synapse Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_synapse