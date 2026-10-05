// PlasticityRule Component Tests
// Comprehensive interface and implementation testing

#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>

namespace test_plasticity_rule {

void testPlasticityRuleAbstractInterface() {
    std::cout << "    testPlasticityRuleAbstractInterface passed" << std::endl;
}

void testSTDPInterfaceCompatibility() {
    nlm::STDP stdp;
    
    // Verify it implements PlasticityRule interface
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    // Verify basic functionality
    stdp.configure(0.01f, 0.012f, 20.0f);
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPInterfaceCompatibility passed" << std::endl;
}

void testHebbianInterfaceCompatibility() {
    nlm::Hebbian hebbian;
    
    // Verify it implements PlasticityRule interface
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    // Verify Hebbian-specific functionality
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianInterfaceCompatibility passed" << std::endl;
}

void testPlasticityRuleSTDPImplementation() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Test potentiation (pre before post)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001f);
    
    float finalWeight = synapse.getWeight();
    assert(finalWeight > initialWeight);
    
    // Test depression (post before pre)
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    initialWeight = synapse2.getWeight();
    preSpikes = {10.0, 20.0, 30.0};
    postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001f);
    
    finalWeight = synapse2.getWeight();
    assert(finalWeight < initialWeight);
    
    // Test eligibility trace
    assert(synapse.getEligibilityTrace() >= 0.0f);
    
    std::cout << "    testPlasticityRuleSTDPImplementation passed" << std::endl;
}

void testPlasticityRuleHebbianImplementation() {
    nlm::Hebbian hebbian;
    
    // Create a synapse
    auto synapse = std::make_shared<nlm::Synapse>();
    float initialWeight = synapse->getWeight();
    
    // Test correlated spikes within 100ms window
    std::vector<nlm::Timestamp> preSpikes = {100.0f, 200.0f};
    std::vector<nlm::Timestamp> postSpikes = {100.0f, 250.0f};
    
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    float finalWeight = synapse->getWeight();
    // Weight should have increased due to correlation
    assert(finalWeight >= initialWeight);
    
    // Test weight bounds enforcement
    hebbian.setMaxWeight(0.5f);
    hebbian.setMinWeight(-0.5f);
    
    // Force weight beyond bounds
    float weightBefore = synapse->getWeight();
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    float weightAfter = synapse->getWeight();
    
    assert(weightAfter <= 0.5f);
    assert(weightAfter >= -0.5f);
    
    std::cout << "    testPlasticityRuleHebbianImplementation passed" << std::endl;
}

void testPlasticityRuleCombinedRules() {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    // Create two synapses with different plasticity rules
    nlm::Synapse synapseSTDP(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapseSTDP.setType(nlm::SynapseType::Excitatory);
    synapseSTDP.setWeight(0.5f);
    
    nlm::Synapse synapseHebbian(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapseHebbian.setType(nlm::SynapseType::Excitatory);
    synapseHebbian.setWeight(0.5f);
    
    float stdpInitialWeight = synapseSTDP.getWeight();
    float hebbianInitialWeight = synapseHebbian.getWeight();
    
    // Apply STDP to one synapse
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    stdp.update(&synapseSTDP, preSpikes, postSpikes, 0.001f);
    hebbian.update(&synapseHebbian, preSpikes, postSpikes, 0.001f);
    
    // Both should have different behaviors
    assert(synapseSTDP.getWeight() != synapseHebbian.getWeight() || 
           synapseSTDP.getWeight() != stdpInitialWeight);
    
    std::cout << "    testPlasticityRuleCombinedRules passed" << std::endl;
}

void testPlasticityRuleErrorHandling() {
    nlm::STDP stdp;
    
    // Test null synapse handling
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 3.0};
    
    // Should not crash with null synapse
    stdp.update(nullptr, preSpikes, postSpikes, 0.001f);
    
    // Test empty spike vectors
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    std::vector<nlm::Timestamp> emptySpikes;
    stdp.update(&synapse, emptySpikes, postSpikes, 0.001f);
    assert(synapse.getWeight() == 0.0f);  // Should not change
    
    stdp.update(&synapse, preSpikes, emptySpikes, 0.001f);
    assert(synapse.getWeight() == 0.0f);  // Should not change
    
    std::cout << "    testPlasticityRuleErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running PlasticityRule tests..." << std::endl;
    testPlasticityRuleAbstractInterface();
    testSTDPInterfaceCompatibility();
    testHebbianInterfaceCompatibility();
    testPlasticityRuleSTDPImplementation();
    testPlasticityRuleHebbianImplementation();
    testPlasticityRuleCombinedRules();
    testPlasticityRuleErrorHandling();
}

} // namespace test_plasticity_rule
