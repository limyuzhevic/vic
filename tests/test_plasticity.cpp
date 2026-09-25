// Comprehensive Plasticity Tests
// Tests STDP, Hebbian, and other plasticity rules
// Covers all TODOs and placeholder implementations

#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace test_plasticity {

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

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    // Test default values
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 10.0f);
    assert(hebbian.getMinWeight() == -10.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianBCM() {
    nlm::Hebbian hebbian;
    
    // Enable BCM learning
    hebbian.enableBCM(true);
    hebbian.setBCMThreshold(0.5f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.1f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianBCM passed (weight: " << finalWeight << ")" << std::endl;
}

void testHebbianOja() {
    nlm::Hebbian hebbian;
    
    // Enable Oja's learning rule
    hebbian.enableOja(true);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity
    std::vector<nlm::Timestamp> preSpikes = {5.0, 10.0, 15.0};
    std::vector<nlm::Timestamp> postSpikes = {8.0, 13.0, 18.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianOja passed (weight: " << finalWeight << ")" << std::endl;
}

void testHebbianAntiHebbian() {
    nlm::Hebbian hebbian;
    
    // Enable anti-Hebbian for inhibitory synapses
    hebbian.enableAntiHebbian(true);
    
    // Create an inhibitory synapse
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.2f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // For inhibitory synapses with anti-Hebbian, weight should become more negative
    assert(finalWeight < initialWeight);
    
    std::cout << "    testHebbianAntiHebbian passed (weight: " << finalWeight << ")" << std::endl;
}

void testHebbianScaling() {
    nlm::Hebbian hebbian;
    
    // Enable synaptic scaling
    hebbian.enableScaling(true);
    hebbian.setScalingThreshold(0.8f);
    
    // Create a synapse with large weight
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(9.0f);  // Close to max
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity to trigger scaling
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0, 35.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have been scaled (reduced due to large magnitude)
    assert(std::abs(finalWeight) < std::abs(initialWeight));
    
    std::cout << "    testHebbianScaling passed (weight: " << finalWeight << ")" << std::endl;
}

void testHebbianSpikeBased() {
    nlm::Hebbian hebbian;
    
    // Enable spike-based learning
    hebbian.enableSpikeBased(true);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity with timing differences
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0};  // Some before, some after
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed based on spike timing
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianSpikeBased passed (weight: " << finalWeight << ")" << std::endl;
}

void testHebbianRewardModulated() {
    nlm::Hebbian hebbian;
    
    // Enable reward-modulated learning via global modulation
    hebbian.setGlobalLearningMod(2.0f);  // Enhance learning
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike activity
    std::vector<nlm::Timestamp> preSpikes = {5.0, 15.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 20.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed, enhanced by global modulation
    assert(finalWeight != initialWeight);
    
    std::cout << "    testHebbianRewardModulated passed (weight: " << finalWeight << ")" << std::endl;
}

void testTripletSTDP() {
    nlm::TripletSTDP triplet;
    
    assert(triplet.getName() != nullptr);
    assert(std::string(triplet.getName()) == "TripletSTDP");
    
    // Test configuration
    triplet.configure(0.02f, 0.025f, 25.0f, 20.0f);
    
    assert(triplet.getLTPWeight() == 0.02f);
    assert(triplet.getLTDWeight() == 0.025f);
    
    std::cout << "    testTripletSTDP passed" << std::endl;
}

void testRewardModulatedSTDP() {
    nlm::RewardModulatedSTDP rmSTDP;
    
    assert(rmSTDP.getName() != nullptr);
    assert(std::string(rmSTDP.getName()) == "RewardModulatedSTDP");
    
    // Test reward modulation configuration
    rmSTDP.setLearningRate(0.03f);
    rmSTDP.setTraceDecay(0.1f);
    rmSTDP.setBaselineLearningRate(0.001f);
    
    assert(rmSTDP.getLearningRate() == 0.03f);
    assert(rmSTDP.getTraceDecay() == 0.1f);
    assert(rmSTDP.getBaselineLearningRate() == 0.001f);
    
    std::cout << "    testRewardModulatedSTDP passed" << std::endl;
}

void testPlasticityRuleInterface() {
    // Test that PlasticityRule interface works
    nlm::Hebbian hebbian;
    nlm::STDP stdp;
    
    // Test that they can be treated as PlasticityRule
    nlm::PlasticityRule* plasticity1 = &hebbian;
    nlm::PlasticityRule* plasticity2 = &stdp;
    
    assert(plasticity1 != nullptr);
    assert(plasticity2 != nullptr);
    
    // Test enable/disable
    plasticity1->setEnabled(false);
    assert(!plasticity1->isEnabled());
    
    plasticity1->setEnabled(true);
    assert(plasticity1->isEnabled());
    
    std::cout << "    testPlasticityRuleInterface passed" << std::endl;
}

void testSynapticDynamics() {
    // Test synaptic weight changes with multiple plasticity rules
    nlm::Synapse synapse(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Apply Hebbian learning
    nlm::Hebbian hebbian;
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float afterHebbian = synapse.getWeight();
    
    // Apply STDP learning
    nlm::STDP stdp;
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float afterSTDP = synapse.getWeight();
    
    // Weight should have changed from both rules
    assert(afterHebbian != initialWeight);
    assert(afterSTDP != afterHebbian);
    
    std::cout << "    testSynapticDynamics passed (initial: " << initialWeight 
              << ", hebbian: " << afterHebbian << ", stdp: " << afterSTDP << ")" << std::endl;
}

void testPlasticityWithDifferentNeuronTypes() {
    // Test plasticity rules with excitatory and inhibitory synapses
    
    // Excitatory synapse
    nlm::Synapse excSynapse(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    excSynapse.setType(nlm::SynapseType::Excitatory);
    excSynapse.setWeight(0.3f);
    
    // Inhibitory synapse
    nlm::Synapse inhSynapse(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    inhSynapse.setType(nlm::SynapseType::Inhibitory);
    inhSynapse.setWeight(-0.3f);
    
    nlm::Hebbian hebbian;
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
    
    hebbian.update(&excSynapse, preSpikes, postSpikes, 0.001);
    hebbian.update(&inhSynapse, preSpikes, postSpikes, 0.001);
    
    // Excitatory weights should become more positive
    assert(excSynapse.getWeight() > 0.3f);
    
    // Inhibitory weights should become more negative (with anti-Hebbian)
    assert(inhSynapse.getWeight() < -0.3f);
    
    std::cout << "    testPlasticityWithDifferentNeuronTypes passed" << std::endl;
}

void testPlasticityRuleParameters() {
    nlm::Hebbian hebbian;
    
    // Test all configurable parameters
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    hebbian.setMaxWeight(5.0f);
    assert(hebbian.getMaxWeight() == 5.0f);
    
    hebbian.setMinWeight(-5.0f);
    assert(hebbian.getMinWeight() == -5.0f);
    
    hebbian.setBCMThreshold(0.8f);
    assert(hebbian.getBCMThreshold() == 0.8f);
    
    hebbian.setBCMThresholdRate(0.01f);
    assert(hebbian.getBCMThresholdRate() == 0.01f);
    
    hebbian.setPostActivityNorm(0.05f);
    assert(hebbian.getPostActivityNorm() == 0.05f);
    
    hebbian.setPreActivityNorm(0.05f);
    assert(hebbian.getPreActivityNorm() == 0.05f);
    
    hebbian.setLearningRateMod(1.5f);
    assert(hebbian.getLearningRateMod() == 1.5f);
    
    hebbian.setGlobalLearningMod(2.0f);
    assert(hebbian.getGlobalLearningMod() == 2.0f);
    
    hebbian.setScalingThreshold(0.9f);
    assert(hebbian.getScalingThreshold() == 0.9f);
    
    hebbian.setTimeDependentModulation(0.8f);
    assert(hebbian.getTimeDependentModulation() == 0.8f);
    
    hebbian.setSTDPTimeWindow(30.0f);
    assert(hebbian.getSTDPTimeWindow() == 30.0f);
    
    hebbian.setSTDPMaxWeightChange(0.002f);
    assert(hebbian.getSTDPMaxWeightChange() == 0.002f);
    
    std::cout << "    testPlasticityRuleParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive plasticity tests..." << std::endl;
    
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    
    testHebbianCreation();
    testHebbianBCM();
    testHebbianOja();
    testHebbianAntiHebbian();
    testHebbianScaling();
    testHebbianSpikeBased();
    testHebbianRewardModulated();
    
    testTripletSTDP();
    testRewardModulatedSTDP();
    testPlasticityRuleInterface();
    testSynapticDynamics();
    testPlasticityWithDifferentNeuronTypes();
    testPlasticityRuleParameters();
    
    std::cout << "All plasticity tests passed!" << std::endl;
}

} // namespace test_plasticity
