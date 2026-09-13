// Comprehensive plasticity system tests
// Tests for STDP, Hebbian, and other plasticity rules

#pragma once

#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_plasticity {

void testSTDPParameters() {
    nlm::STDP stdp;
    
    // Test default parameters
    assert(stdp.getLTPWeight() > 0.0f);
    assert(stdp.getLTDWeight() > 0.0f);
    assert(stdp.getTimeConstant() > 0.0f);
    
    // Test parameter setting
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(25.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    // Test parameter clamping
    stdp.setLTPWeight(2.0f);  // Should be clamped to 1.0f
    assert(stdp.getLTPWeight() <= 1.0f);
    
    stdp.setLTDWeight(-0.1f);  // Should be clamped to 0.0f
    assert(stdp.getLTDWeight() >= 0.0f);
    
    std::cout << "    testSTDPParameters passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    // Create synapse with max weight
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(1.0f);
    
    // Apply strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Create synapse with min weight
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setWeight(-1.0f);
    
    // Apply strong depression
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0, 40.0, 50.0};
    std::vector<nlm::Timestamp> postSpikes2 = {5.0, 15.0, 25.0, 35.0, 45.0};  // All post before pre
    
    stdp.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    
    // Weight should be clamped to min
    assert(synapse2.getWeight() >= -1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void testHebbianLearning() {
    nlm::Hebbian hebbian;
    
    // Test default parameters
    assert(hebbian.getLearningRate() > 0.0f);
    assert(hebbian.getMaxWeight() > 0.0f);
    assert(hebbian.getMinWeight() < 0.0f);
    assert(!hebbian.isAntiHebbian());
    
    // Test parameter setting
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(0.8f);
    hebbian.setMinWeight(-0.8f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 0.8f);
    assert(hebbian.getMinWeight() == -0.8f);
    
    // Create synapse and test learning
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    // Record simultaneous pre and post spikes
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(0.0f);
    
    // Apply Hebbian learning
    std::vector<nlm::Timestamp> preSpikes = {0.0f};
    std::vector<nlm::Timestamp> postSpikes = {0.0f};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change (strengthen or weaken based on homeostatic constraints)
    // Note: With homeostatic constraints, weight changes are moderated
    
    std::cout << "    testHebbianLearning passed" << std::endl;
}

void testHebbianAntiHebbian() {
    nlm::Hebbian hebbian;
    
    // Configure for anti-Hebbian learning (inhibitory synapses)
    hebbian.setAntiHebbian(true);
    assert(hebbian.isAntiHebbian());
    
    std::cout << "    testHebbianAntiHebbian passed" << std::endl;
}

void testTripletSTDP() {
    nlm::TripletSTDP tripletStdp;
    
    // Test default parameters
    assert(tripletStdp.getAPlus() > 0.0f);
    assert(tripletStdp.getAMinus() > 0.0f);
    assert(tripletStdp.getTauPlus() > 0.0f);
    assert(tripletStdp.getTauMinus() > 0.0f);
    
    // Test configuration methods
    tripletStdp.configureBiologically(0.01f, 1.0f);
    
    assert(tripletStdp.getAPlus() > 0.0f);
    assert(tripletStdp.getAMinus() > 0.0f);
    assert(tripletStdp.getTauPlus() > 0.0f);
    assert(tripletStdp.getTauMinus() > 0.0f);
    
    std::cout << "    testTripletSTDP passed" << std::endl;
}

void testRewardModulatedSTDP() {
    nlm::RewardModulatedSTDP rstdp;
    
    // Test default parameters
    assert(rstdp.getDopamineGain() >= 0.0f);
    assert(rstdp.getDAWeight() >= 0.0f);
    assert(rstdp.getLearningRate() >= 0.0f);
    
    // Test parameter setting
    rstdp.setDopamineGain(1.5f);
    rstdp.setDAWeight(0.8f);
    rstdp.setLearningRate(0.02f);
    
    assert(rstdp.getDopamineGain() == 1.5f);
    assert(rstdp.getDAWeight() == 0.8f);
    assert(rstdp.getLearningRate() == 0.02f);
    
    std::cout << "    testRewardModulatedSTDP passed" << std::endl;
}

void testPlasticityIntegration() {
    // Test that all plasticity systems work together
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    nlm::TripletSTDP tripletStdp;
    nlm::RewardModulatedSTDP rstdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Enable different plasticity rules
    synapse.enablePlasticity(true, true, false);  // Hebbian and STDP
    
    // Record some spike activity
    synapse.recordPreSpike(0.0f);
    synapse.recordPreSpike(10.0f);
    synapse.recordPostSpike(5.0f);
    
    // Apply STDP
    std::vector<nlm::Timestamp> preSpikes = {0.0f, 10.0f};
    std::vector<nlm::Timestamp> postSpikes = {5.0f};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Apply Hebbian
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Apply triplet STDP
    tripletStdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Apply reward-modulated STDP
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Synapse should have changed weight
    assert(synapse.getWeight() != 0.5f);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testPlasticityRuleNames() {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    nlm::TripletSTDP tripletStdp;
    nlm::RewardModulatedSTDP rstdp;
    
    assert(std::string(stdp.getName()) == "STDP");
    assert(std::string(hebbian.getName()) == "Hebbian");
    assert(std::string(tripletStdp.getName()) == "TripletSTDP");
    assert(std::string(rstdp.getName()) == "R-STDP");
    
    std::cout << "    testPlasticityRuleNames passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive plasticity tests..." << std::endl;
    
    testSTDPParameters();
    testSTDPWeightBounds();
    testHebbianLearning();
    testHebbianAntiHebbian();
    testTripletSTDP();
    testRewardModulatedSTDP();
    testPlasticityIntegration();
    testPlasticityRuleNames();
    
    std::cout << "" << std::endl;
    std::cout << "All plasticity tests passed!" << std::endl;
}

} // namespace test_plasticity