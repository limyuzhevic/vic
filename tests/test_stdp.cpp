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

void testSTDPNullSynapse() {
    nlm::STDP stdp;
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Test nullptr synapse - should not crash
    stdp.update(nullptr, preSpikes, postSpikes, 0.001);
    
    std::cout << "    testSTDPNullSynapse passed" << std::endl;
}

void testSTDPEmptySpikeVectors() {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes only
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes only
    preSpikes = {1.0, 2.0, 3.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Both empty
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikeVectors passed" << std::endl;
}

void testSTDPSimultaneousSpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Simultaneous spikes (0 time difference)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0};  // Same times
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change for simultaneous spikes
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPSimultaneousSpikes passed" << std::endl;
}

void testSTDPNegativeTimeDiff() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Post fires significantly before pre (negative time difference)
    std::vector<nlm::Timestamp> preSpikes = {100.0, 200.0, 300.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0, 20.0};  // Much earlier
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should decrease (post before pre causes depression)
    assert(synapse.getWeight() < initialWeight);
    
    std::cout << "    testSTDPNegativeTimeDiff passed" << std::endl;
}

void testSTDPSingleSpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Single pre spike
    std::vector<nlm::Timestamp> preSpikes = {10.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change
    assert(synapse.getWeight() != initialWeight);
    
    // Single post spike
    synapse.setWeight(initialWeight);
    preSpikes = {10.0};
    postSpikes = {15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() != initialWeight);
    
    std::cout << "    testSTDPSingleSpikes passed" << std::endl;
}

void testSTDPBoundaryWeights() {
    nlm::STDP stdp;
    
    // Test -1.0 weight boundary
    nlm::Synapse synapse1(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(-1.0f);
    float initialWeight1 = synapse1.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse1, preSpikes, postSpikes, 0.001);
    assert(synapse1.getWeight() >= -1.0f);
    
    // Test 0.0 weight boundary
    nlm::Synapse synapse2(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.0f);
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    assert(synapse2.getWeight() <= 1.0f);
    
    // Test 1.0 weight boundary
    nlm::Synapse synapse3(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(1.0f);
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    assert(synapse3.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPBoundaryWeights passed" << std::endl;
}

void testSTDPZeroEfficacy() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setEfficacy(0.0f);  // Zero efficacy
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change with zero efficacy
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPZeroEfficacy passed" << std::endl;
}

void testSTDPNaNDelta() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Test NaN in spike times (should be handled by float conversion)
    std::vector<nlm::Timestamp> preSpikes = {0.0, std::nanf("")};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not crash
    assert(std::isfinite(synapse.getWeight()));
    
    std::cout << "    testSTDPNaNDelta passed" << std::endl;
}

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    testSTDPNullSynapse();
    testSTDPEmptySpikeVectors();
    testSTDPSimultaneousSpikes();
    testSTDPNegativeTimeDiff();
    testSTDPSingleSpikes();
    testSTDPBoundaryWeights();
    testSTDPZeroEfficacy();
    testSTDPNaNDelta();
}

} // namespace test_stdp
