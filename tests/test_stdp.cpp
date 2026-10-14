// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_stdp {

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

void testSTDPParameterValidation() {
    nlm::STDP stdp;
    
    // Test parameter boundaries
    stdp.setLTPWeight(0.0f);
    assert(stdp.getLTPWeight() == 0.0f);
    
    stdp.setLTPWeight(0.1f);
    assert(stdp.getLTPWeight() == 0.1f);
    
    stdp.setLTDWeight(0.0f);
    assert(stdp.getLTDWeight() == 0.0f);
    
    stdp.setLTDWeight(0.2f);
    assert(stdp.getLTDWeight() == 0.2f);
    
    stdp.setTimeConstant(1.0f);
    assert(stdp.getTimeConstant() == 1.0f);
    
    stdp.setTimeConstant(1000.0f);
    assert(stdp.getTimeConstant() == 1000.0f);
    
    std::cout << "    testSTDPParameterValidation passed" << std::endl;
}

void testSTDPConfigureBoundary() {
    nlm::STDP stdp;
    
    // Test configure with boundary values
    stdp.configure(0.001f, 0.002f, 1.0f);
    assert(stdp.getLTPWeight() == 0.001f);
    assert(stdp.getLTDWeight() == 0.002f);
    assert(stdp.getTimeConstant() == 1.0f);
    
    // Test maximum values
    stdp.configure(1.0f, 1.0f, 10000.0f);
    assert(stdp.getLTPWeight() == 1.0f);
    assert(stdp.getLTDWeight() == 1.0f);
    assert(stdp.getTimeConstant() == 10000.0f);
    
    std::cout << "    testSTDPConfigureBoundary passed" << std::endl;
}

void testSTDPTimeConstantEffect() {
    nlm::STDP stdp1;
    nlm::STDP stdp2;
    
    stdp1.setTimeConstant(10.0f);
    stdp2.setTimeConstant(100.0f);
    
    // Create same synapse pairs
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    synapse2.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0};
    
    stdp1.update(&synapse1, preSpikes, postSpikes, 0.001);
    stdp2.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    // Different time constants should produce different weight changes
    assert(synapse1.getWeight() != synapse2.getWeight());
    
    std::cout << "    testSTDPTimeConstantEffect passed" << std::endl;
}

void testSTDPPotentiationDepressionBalance() {
    nlm::STDP stdp;
    
    // Create two synapses for comparison
    nlm::Synapse synapsePrePost(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));  // pre before post
    nlm::Synapse synapsePostPre(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));  // post before pre
    
    synapsePrePost.setType(nlm::SynapseType::Excitatory);
    synapsePostPre.setType(nlm::SynapseType::Excitatory);
    synapsePrePost.setWeight(0.5f);
    synapsePostPre.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // post after pre
    
    // For synapsePrePost, we want pre before post pattern
    stdp.update(&synapsePrePost, preSpikes, postSpikes, 0.001);
    
    // For synapsePostPre, we want post before pre pattern (reverse)
    stdp.update(&synapsePostPre, postSpikes, preSpikes, 0.001);
    
    // The pre-before-post should potentiate
    assert(synapsePrePost.getWeight() > 0.5f);
    
    // The post-before-pre should depress
    assert(synapsePostPre.getWeight() < 0.5f);
    
    std::cout << "    testSTDPPotentiationDepressionBalance passed" << std::endl;
}

void testSTDPNumericalStability() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(1.0f);  // At max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    // Update multiple times to test stability
    for (int i = 0; i < 100; ++i) {
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        // Weight should never exceed bounds
        assert(synapse.getWeight() <= 1.0f);
        assert(synapse.getWeight() >= -1.0f);
    }
    
    std::cout << "    testSTDPNumericalStability passed" << std::endl;
}

void testSTDPNameValidation() {
    nlm::STDP stdp;
    
    const char* name = stdp.getName();
    assert(name != nullptr);
    std::string nameStr = name;
    assert(nameStr == "STDP");
    
    // Test RewardModulatedSTDP if available
    // This would require including the appropriate header
    
    std::cout << "    testSTDPNameValidation passed" << std::endl;
}
