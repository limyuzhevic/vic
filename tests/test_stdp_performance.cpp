// STDP Performance Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

namespace test_stdp_performance {

void testSTDPPerformanceCreation() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    
    std::cout << "    testSTDPPerformanceCreation passed" << std::endl;
}

void testSTDPPerformanceTimeConstant() {
    nlm::STDP stdp;
    
    // Test different time constants
    stdp.setTimeConstant(10.0f);
    assert(stdp.getTimeConstant() == 10.0f);
    
    stdp.setTimeConstant(50.0f);
    assert(stdp.getTimeConstant() == 50.0f);
    
    stdp.setTimeConstant(100.0f);
    assert(stdp.getTimeConstant() == 100.0f);
    
    std::cout << "    testSTDPPerformanceTimeConstant passed" << std::endl;
}

void testSTDPPerformanceSynapticUpdate() {
    nlm::STDP stdp;
    
    // Create many synapses for performance testing
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 1000; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1000));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapses.push_back(synapse);
    }
    
    // Simulate synaptic updates for performance
    for (size_t i = 0; i < synapses.size(); ++i) {
        nlm::Synapse* synapse = &synapses[i];
        
        std::vector<nlm::Timestamp> preSpikes = {static_cast<nlm::Timestamp>(i * 10)};
        std::vector<nlm::Timestamp> postSpikes = {static_cast<nlm::Timestamp>(i * 10 + 5)};
        
        stdp.update(synapse, preSpikes, postSpikes, 0.001);
    }
    
    // Verify synapses were updated
    bool synapsesUpdated = false;
    for (const auto& synapse : synapses) {
        if (synapse.getWeight() != 0.5f) {
            synapsesUpdated = true;
            break;
        }
    }
    
    assert(synapsesUpdated);
    
    std::cout << "    testSTDPPerformanceSynapticUpdate passed" << std::endl;
}

void testSTDPLearningRatePerformance() {
    nlm::STDP stdp;
    
    // Test different learning rates
    stdp.configure(0.05f, 0.06f, 20.0f);
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    stdp.configure(0.03f, 0.04f, 15.0f);
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.04f);
    assert(stdp.getTimeConstant() == 15.0f);
    
    std::cout << "    testSTDPLearningRatePerformance passed" << std::endl;
}

void testSTDPTimingPerformance() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test timing with different spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0};
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testSTDPTimingPerformance passed (100 updates in " 
              << duration.count() << " ms)" << std::endl;
}

void runAll() {
    std::cout << "Running STDP performance tests..." << std::endl;
    testSTDPPerformanceCreation();
    testSTDPPerformanceTimeConstant();
    testSTDPPerformanceSynapticUpdate();
    testSTDPLearningRatePerformance();
    testSTDPTimingPerformance();
}

} // namespace test_stdp_performance