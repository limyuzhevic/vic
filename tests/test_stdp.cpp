// Enhanced STDP Tests
// Now includes comprehensive tests for spike-timing dependent plasticity and learning
// #include "brain/Synapse.hpp"
// #include "plasticity/STDP.hpp"
// #include "core/Random/Random.hpp"
// #include <cassert>
// #include <iostream>
// #include <cmath>

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

void testSTDPMixedSpiking() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Mixed spiking pattern
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0};  // All post after pre (LTP)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float afterLTP = synapse.getWeight();
    assert(afterLTP > initialWeight);
    
    // Now reverse the order for depression
    std::vector<nlm::Timestamp> postSpikes2 = {5.0, 15.0, 25.0, 35.0, 45.0};
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0, 40.0, 50.0};
    
    // Reset synapse weight
    synapse.setWeight(0.5f);
    float initialWeight2 = synapse.getWeight();
    
    stdp.update(&synapse, preSpikes2, postSpikes2, 0.001);
    
    float afterLTD = synapse.getWeight();
    assert(afterLTD < initialWeight2);
    
    std::cout << "    testSTDPMixedSpiking passed" << std::endl;
}

void testSTDPZeroTimeConstant() {
    nlm::STDP stdp;
    stdp.setTimeConstant(0.0f);
    
    // With zero time constant, plasticity should have no effect
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should not change with zero time constant
    assert(std::abs(finalWeight - initialWeight) < 0.001f);
    
    std::cout << "    testSTDPZeroTimeConstant passed" << std::endl;
}

void runAll() {
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPMixedSpiking();
    testSTDPZeroTimeConstant();
    
    std::cout << std::endl;
    std::cout << "=== STDP Tests ===" << std::endl;
    std::cout << "All STDP tests passed!" << std::endl;
}

} // namespace test_stdp