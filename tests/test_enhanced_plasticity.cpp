// Enhanced Plasticity Tests
// Tests for new Phase 2 plasticity implementations

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_enhanced_plasticity {

// ========== RewardModulatedSTDP Tests ==========

void testRewardModulatedSTDPConstruction() {
    nlm::RewardModulatedSTDP rstdp;
    
    assert(rstdp.getBaseLTPWeight() == 0.01f);
    assert(rstdp.getBaseLTDWeight() == 0.012f);
    assert(rstdp.getDopamineScale() == 0.5f);
    
    std::cout << "    testRewardModulatedSTDPConstruction passed" << std::endl;
}

void testRewardModulatedSTDPParameters() {
    nlm::RewardModulatedSTDP rstdp;
    
    rstdp.setBaseLTPWeight(0.03f);
    rstdp.setBaseLTDWeight(0.04f);
    rstdp.setDopamineScale(0.8f);
    
    assert(rstdp.getBaseLTPWeight() == 0.03f);
    assert(rstdp.getBaseLTDWeight() == 0.04f);
    assert(rstdp.getDopamineScale() == 0.8f);
    
    std::cout << "    testRewardModulatedSTDPParameters passed" << std::endl;
}

void testRewardModulatedSTDPName() {
    nlm::RewardModulatedSTDP rstdp;
    
    assert(rstdp.getName() != nullptr);
    assert(std::string(rstdp.getName()) == "RewardModulatedSTDP");
    
    std::cout << "    testRewardModulatedSTDPName passed" << std::endl;
}

void testRewardModulatedSTDPWeightChange() {
    nlm::RewardModulatedSTDP rstdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post spikes with moderate timing difference
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed (could be potentiation or depression)
    assert(std::abs(finalWeight - initialWeight) > 1e-6f);
    
    std::cout << "    testRewardModulatedSTDPWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

// ========== TripletSTDP Tests ==========

void testTripletSTDPConstruction() {
    nlm::TripletSTDP tstdp;
    
    assert(tstdp.getAplus() == 0.05f);
    assert(tstdp.getAminus() == 0.05f);
    assert(tstdp.getBaselineWeight() == 1.0f);
    
    std::cout << "    testTripletSTDPConstruction passed" << std::endl;
}

void testTripletSTDPParameters() {
    nlm::TripletSTDP tstdp;
    
    tstdp.setAplus(0.08f);
    tstdp.setAminus(0.06f);
    tstdp.setBaselineWeight(0.9f);
    tstdp.setUtilization(0.3f);
    
    assert(tstdp.getAplus() == 0.08f);
    assert(tstdp.getAminus() == 0.06f);
    assert(tstdp.getBaselineWeight() == 0.9f);
    assert(tstdp.getUtilization() == 0.3f);
    
    std::cout << "    testTripletSTDPParameters passed" << std::endl;
}

void testTripletSTDPName() {
    nlm::TripletSTDP tstdp;
    
    assert(tstdp.getName() != nullptr);
    assert(std::string(tstdp.getName()) == "TripletSTDP");
    
    std::cout << "    testTripletSTDPName passed" << std::endl;
}

void testTripletSTDPWeightChange() {
    nlm::TripletSTDP tstdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(1.0f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike timing that should produce weight change
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    tstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed due to triplet interactions
    assert(std::abs(finalWeight - initialWeight) > 1e-6f);
    
    // Weight should remain within bounds
    assert(finalWeight >= tstdp.getBaselineWeight() - 0.5f);
    assert(finalWeight <= tstdp.getBaselineWeight() + 0.5f);
    
    std::cout << "    testTripletSTDPWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

// ========== HebbianRule Tests (in PlasticityRule.hpp) ==========

void testHebbianRuleConstruction() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(hebbian.getMinWeight() == -1.0f);
    
    std::cout << "    testHebbianRuleConstruction passed" << std::endl;
}

void testHebbianRuleParameters() {
    nlm::HebbianRule hebbian;
    
    hebbian.setLearningRate(0.02f);
    hebbian.setMaxWeight(2.0f);
    hebbian.setMinWeight(-2.0f);
    
    assert(hebbian.getLearningRate() == 0.02f);
    assert(hebbian.getMaxWeight() == 2.0f);
    assert(hebbian.getMinWeight() == -2.0f);
    
    std::cout << "    testHebbianRuleParameters passed" << std::endl;
}

void testHebbianRuleName() {
    nlm::HebbianRule hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "HebbianRule");
    
    std::cout << "    testHebbianRuleName passed" << std::endl;
}

void testHebbianRuleWeightChange() {
    nlm::HebbianRule hebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated spikes (within Hebbian window)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed due to correlated activity
    assert(std::abs(finalWeight - initialWeight) > 1e-6f);
    
    std::cout << "    testHebbianRuleWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

// ========== AntiHebbianRule Tests ==========

void testAntiHebbianRuleConstruction() {
    nlm::AntiHebbianRule antiHebbian;
    
    assert(antiHebbian.getLearningRate() == 0.01f);
    
    std::cout << "    testAntiHebbianRuleConstruction passed" << std::endl;
}

void testAntiHebbianRuleParameters() {
    nlm::AntiHebbianRule antiHebbian;
    
    antiHebbian.setLearningRate(0.03f);
    
    assert(antiHebbian.getLearningRate() == 0.03f);
    
    std::cout << "    testAntiHebbianRuleParameters passed" << std::endl;
}

void testAntiHebbianRuleName() {
    nlm::AntiHebbianRule antiHebbian;
    
    assert(antiHebbian.getName() != nullptr);
    assert(std::string(antiHebbian.getName()) == "AntiHebbianRule");
    
    std::cout << "    testAntiHebbianRuleName passed" << std::endl;
}

void testAntiHebbianRuleWeightChange() {
    nlm::AntiHebbianRule antiHebbian;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated spikes (within anti-Hebbian window)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    antiHebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Anti-Hebbian should decrease weight for correlated spikes
    assert(finalWeight < initialWeight);
    
    std::cout << "    testAntiHebbianRuleWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

// ========== BCMRule Tests ==========

void testBCMRuleConstruction() {
    nlm::BCMRule bcm;
    
    assert(bcm.getSigma() == 0.1f);
    assert(bcm.getLearningRate() == 0.01f);
    assert(bcm.getThetaM() == 0.5f);
    
    std::cout << "    testBCMRuleConstruction passed" << std::endl;
}

void testBCMRuleParameters() {
    nlm::BCMRule bcm;
    
    bcm.setSigma(0.2f);
    bcm.setLearningRate(0.02f);
    bcm.setThetaM(0.8f);
    
    assert(bcm.getSigma() == 0.2f);
    assert(bcm.getLearningRate() == 0.02f);
    assert(bcm.getThetaM() == 0.8f);
    
    std::cout << "    testBCMRuleParameters passed" << std::endl;
}

void testBCMRuleName() {
    nlm::BCMRule bcm;
    
    assert(bcm.getName() != nullptr);
    assert(std::string(bcm.getName()) == "BCMRule");
    
    std::cout << "    testBCMRuleName passed" << std::endl;
}

void testBCMRuleWeightChange() {
    nlm::BCMRule bcm;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike patterns for BCM rule
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    bcm.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed (could be potentiation or depression)
    assert(std::abs(finalWeight - initialWeight) > 1e-6f);
    
    std::cout << "    testBCMRuleWeightChange passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

// ========== Cross-Compatibility Tests ==========

void testAllPlasticityRulesImplementPlasticityRule() {
    // Test that all plasticity rules can be used polymorphically
    std::vector<nlm::PlasticityRule*> rules;
    
    rules.push_back(new nlm::STDP());
    rules.push_back(new nlm::RewardModulatedSTDP());
    rules.push_back(new nlm::TripletSTDP());
    rules.push_back(new nlm::HebbianRule());
    rules.push_back(new nlm::AntiHebbianRule());
    rules.push_back(new nlm::BCMRule());
    
    // Test that all rules have required methods
    for (auto* rule : rules) {
        assert(rule->getName() != nullptr);
        assert(strlen(rule->getName()) > 0);
    }
    
    // Clean up
    for (auto* rule : rules) {
        delete rule;
    }
    
    std::cout << "    testAllPlasticityRulesImplementPlasticityRule passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Plasticity Tests..." << std::endl;
    
    // RewardModulatedSTDP tests
    testRewardModulatedSTDPConstruction();
    testRewardModulatedSTDPParameters();
    testRewardModulatedSTDPName();
    testRewardModulatedSTDPWeightChange();
    
    // TripletSTDP tests
    testTripletSTDPConstruction();
    testTripletSTDPParameters();
    testTripletSTDPName();
    testTripletSTDPWeightChange();
    
    // HebbianRule tests
    testHebbianRuleConstruction();
    testHebbianRuleParameters();
    testHebbianRuleName();
    testHebbianRuleWeightChange();
    
    // AntiHebbianRule tests
    testAntiHebbianRuleConstruction();
    testAntiHebbianRuleParameters();
    testAntiHebbianRuleName();
    testAntiHebbianRuleWeightChange();
    
    // BCMRule tests
    testBCMRuleConstruction();
    testBCMRuleParameters();
    testBCMRuleName();
    testBCMRuleWeightChange();
    
    // Cross-compatibility tests
    testAllPlasticityRulesImplementPlasticityRule();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Plasticity Tests PASSED ===" << std::endl;
}

} // namespace test_enhanced_plasticity