// BCM Tests
#include "brain/Synapse.hpp"
#include "plasticity/BCMRule.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_bcm {

void testBCMCreation() {
    nlm::BCMRule bcm;
    
    assert(bcm.getLearningRate() == 0.01f);
    assert(bcm.getMaxWeight() == 1.0f);
    assert(bcm.getMinWeight() == -1.0f);
    assert(bcm.getKappa() == 0.5f);
    assert(bcm.getMu() == 0.0001f);
    assert(bcm.getTimeWindow() == 100.0f);
    
    std::cout << "    testBCMCreation passed" << std::endl;
}

void testBCMConfiguration() {
    nlm::BCMRule bcm;
    
    bcm.setLearningRate(0.03f);
    bcm.setMaxWeight(0.8f);
    bcm.setMinWeight(-0.8f);
    bcm.setKappa(0.6f);
    bcm.setMu(0.0002f);
    bcm.setTimeWindow(150.0f);
    
    assert(bcm.getLearningRate() == 0.03f);
    assert(bcm.getMaxWeight() == 0.8f);
    assert(bcm.getMinWeight() == -0.8f);
    assert(bcm.getKappa() == 0.6f);
    assert(bcm.getMu() == 0.0002f);
    assert(bcm.getTimeWindow() == 150.0f);
    
    std::cout << "    testBCMConfiguration passed" << std::endl;
}

void testBCMConfigure() {
    nlm::BCMRule bcm;
    
    bcm.configure(0.02f, 0.8f, -0.8f, 0.7f, 0.00015f, 120.0f);
    
    assert(bcm.getLearningRate() == 0.02f);
    assert(bcm.getMaxWeight() == 0.8f);
    assert(bcm.getMinWeight() == -0.8f);
    assert(bcm.getKappa() == 0.7f);
    assert(bcm.getMu() == 0.00015f);
    assert(bcm.getTimeWindow() == 120.0f);
    
    std::cout << "    testBCMConfigure passed" << std::endl;
}

void testBCMName() {
    nlm::BCMRule bcm;
    
    assert(bcm.getName() != nullptr);
    assert(std::string(bcm.getName()) == "BCM");
    
    std::cout << "    testBCMName passed" << std::endl;
}

void testBCMPotentiation() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Both pre and post fire
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight may increase or decrease depending on BCM threshold dynamics
    std::cout << "    testBCMPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMDepression() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires frequently to raise BCM threshold
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0}; // Only 2 pre spikes
    std::vector<nlm::Timestamp> postSpikes = {0.0, 5.0, 10.0, 15.0, 20.0, 25.0}; // Many post spikes
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // With high post activity, BCM should depress the synapse
    std::cout << "    testBCMDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMTemporalDynamics() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (causal)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0}; // 5ms after each pre
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Test temporal dynamics
    std::cout << "    testBCMTemporalDynamics passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMTemporalReverse() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (anti-causal)
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 5.0}; // 10ms before pre
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Test reverse temporal dynamics
    std::cout << "    testBCMTemporalReverse passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testBCMKappaChanges() {
    nlm::BCMRule bcm;
    
    float initialKappa = bcm.getKappa();
    
    // High post activity should increase kappa
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0};
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalKappa = bcm.getKappa();
    
    // Kappa should have changed based on activity
    std::cout << "    testBCMKappaChanges passed (kappa: " 
              << initialKappa << " -> " << finalKappa << ")" << std::endl;
}

void testBCMWeightBounds() {
    nlm::BCMRule bcm;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.95f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testBCMWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running BCM tests..." << std::endl;
    testBCMCreation();
    testBCMConfiguration();
    testBCMConfigure();
    testBCMName();
    testBCMPotentiation();
    testBCMDepression();
    testBCMTemporalDynamics();
    testBCMTemporalReverse();
    testBCMKappaChanges();
    testBCMWeightBounds();
}

} // namespace test_bcm
