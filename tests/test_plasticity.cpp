#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_plasticity {

void testPlasticityRuleBase() {
    // Test abstract base class interface
    nlm::PlasticityRule* rule = new nlm::HebbianRule();
    
    assert(rule->isEnabled() == true);
    rule->setEnabled(false);
    assert(rule->isEnabled() == false);
    
    delete rule;
    std::cout << "    testPlasticityRuleBase passed" << std::endl;
}

void testHebbianRule() {
    nlm::HebbianRule hebbianRule;
    
    assert(hebbianRule.getName() != nullptr);
    assert(std::string(hebbianRule.getName()) == "Hebbian");
    
    assert(hebbianRule.getLearningRate() == 0.01f);
    
    hebbianRule.setLearningRate(0.1f);
    assert(hebbianRule.getLearningRate() == 0.1f);
    
    hebbianRule.setLearningRate(-0.1f);
    assert(hebbianRule.getLearningRate() == 0.0f); // Should be clamped
    
    std::cout << "    testHebbianRule passed" << std::endl;
}

void testAntiHebbianRule() {
    nlm::AntiHebbianRule antiHebbianRule;
    
    assert(antiHebbianRule.getName() != nullptr);
    assert(std::string(antiHebbianRule.getName()) == "AntiHebbian");
    
    assert(antiHebbianRule.isEnabled() == true);
    std::cout << "    testAntiHebbianRule passed" << std::endl;
}

void testBCMRule() {
    nlm::BCMRule bcmRule;
    
    assert(bcmRule.getName() != nullptr);
    assert(std::string(bcmRule.getName()) == "BCM");
    
    assert(bcmRule.isEnabled() == true);
    std::cout << "    testBCMRule passed" << std::endl;
}

void testStructuralPlasticity() {
    nlm::StructuralPlasticity structuralPlasticity;
    
    assert(structuralPlasticity.getMaxSynapses() == 1000);
    assert(structuralPlasticity.getPruningRate() == 0.1f);
    
    // Test synapse formation
    int newSynapses = structuralPlasticity.formNewSynapses(10);
    assert(newSynapses == 10); // In real implementation
    
    // Test synapse elimination
    int eliminatedSynapses = structuralPlasticity.eliminateSynapses(5);
    assert(eliminatedSynapses == 5); // In real implementation
    
    std::cout << "    testStructuralPlasticity passed" << std::endl;
}

void testHebbianPlasticity() {
    nlm::HebbianRule hebbianRule;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike trains
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Apply Hebbian learning
    hebbianRule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    assert(finalWeight >= initialWeight); // Should potentiate
    
    std::cout << "    testHebbianPlasticity passed" << std::endl;
}

void testPlasticityIntegration() {
    nlm::HebbianRule hebbianRule;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Apply plasticity rule
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};
    
    hebbianRule.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change
    assert(synapse.getWeight() != 0.5f);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testPlasticityReset() {
    nlm::HebbianRule hebbianRule;
    nlm::StructuralPlasticity structuralPlasticity;
    
    // Modify internal state
    hebbianRule.setLearningRate(0.2f);
    structuralPlasticity.setPruningRate(0.3f);
    
    // Reset
    hebbianRule.reset();
    structuralPlasticity.reset();
    
    assert(hebbianRule.getLearningRate() == 0.01f); // Default
    assert(structuralPlasticity.getPruningRate() == 0.1f); // Default
    
    std::cout << "    testPlasticityReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity tests..." << std::endl;
    testPlasticityRuleBase();
    testHebbianRule();
    testAntiHebbianRule();
    testBCMRule();
    testStructuralPlasticity();
    testHebbianPlasticity();
    testPlasticityIntegration();
    testPlasticityReset();
}

} // namespace test_plasticity