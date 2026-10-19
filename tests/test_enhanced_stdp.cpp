// Enhanced STDP Tests - Advanced Plasticity
#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_enhanced_stdp {

void testSTDPTimeWindow() {
    nlm::STDP stdp;
    
    // Configure STDP parameters
    stdp.configure(0.02f, 0.015f, 20.0f);  // LTP, LTD, time constant
    
    // Create test synapse
    nlm::SynapseId synId(1);
    nlm::NeuronId source(1), dest(2);
    nlm::Synapse synapse(synId, source, dest);
    synapse.setWeight(0.5f);
    
    // Enable STDP
    synapse.enablePlasticity(false, true, false);  // Only STDP
    
    // Test spike timing patterns
    std::vector<double> preSpikes = {0.0, 20.0, 40.0};  // ms
    std::vector<double> postSpikes = {10.0, 30.0};     // ms (pre before post)
    
    // Run STDP update
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change due to pre-before-post pairing
    float weightAfter = synapse.getWeight();
    assert(std::abs(weightAfter - 0.5f) > 0.001f);  // Should have changed
    
    std::cout << "    testSTDPTimeWindow passed" << std::endl;
}

void testSTDPPostBeforePre() {
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.015f, 20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.8f);
    synapse.enablePlasticity(false, true, false);  // Only STDP
    
    // Test post-before-pre pairing (should cause LTD)
    std::vector<double> preSpikes = {30.0, 50.0};    // ms
    std::vector<double> postSpikes = {10.0, 25.0};  // ms (post before pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightAfter = synapse.getWeight();
    // Weight should decrease due to post-before-pre pairing
    assert(weightAfter < 0.8f);  // Should have depressed
    
    std::cout << "    testSTDPPostBeforePre passed" << std::endl;
}

void testSTDPNoChangeForSimultaneous() {
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.015f, 20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.enablePlasticity(false, true, false);  // Only STDP
    
    // Test simultaneous spikes (should have minimal effect)
    std::vector<double> preSpikes = {10.0, 20.0, 30.0};
    std::vector<double> postSpikes = {10.0, 20.0, 30.0};  // Same timing
    
    float weightBefore = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfter = synapse.getWeight();
    
    // Weight should not change significantly for simultaneous spikes
    assert(std::abs(weightAfter - weightBefore) < 0.001f);
    
    std::cout << "    testSTDPNoChangeForSimultaneous passed" << std::endl;
}

void testSTDPNeuromodulation() {
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.015f, 20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.enablePlasticity(false, true, true);  // Reward-modulated STDP
    
    // Set eligibility trace
    synapse.setEligibilityTrace(1.0f);
    
    // Test spike timing
    std::vector<double> preSpikes = {0.0};
    std::vector<double> postSpikes = {15.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change with eligibility trace
    float weightAfter = synapse.getWeight();
    assert(std::abs(weightAfter - 0.5f) > 0.001f);
    
    std::cout << "    testSTDPNeuromodulation passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.015f, 20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.enablePlasticity(false, true, false);  // Only STDP
    
    // Create extreme spike timing to cause large weight change
    std::vector<double> preSpikes = {0.0};
    std::vector<double> postSpikes = {100.0};  // Large dt - small effect
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should still be within bounds
    float weightAfter = synapse.getWeight();
    assert(weightAfter >= -1.0f && weightAfter <= 1.0f);  // Default STDP bounds
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void runAll() {
    testSTDPTimeWindow();
    testSTDPPostBeforePre();
    testSTDPNoChangeForSimultaneous();
    testSTDPNeuromodulation();
    testSTDPWeightBounds();
}

} // namespace test_enhanced_stdp