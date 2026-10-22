// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>
#include <cmath>

namespace test_stdp {

void testSTDPCreation() {
    nlm::test::TestAssert::equal(0.01f, nlm::STDP().getLTPWeight(), "testSTDPCreation");
    nlm::test::TestAssert::equal(0.012f, nlm::STDP().getLTDWeight(), "testSTDPCreation");
    nlm::test::TestAssert::equal(20.0f, nlm::STDP().getTimeConstant(), "testSTDPCreation");
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    nlm::test::TestAssert::equal(0.05f, stdp.getLTPWeight(), "testSTDPConfiguration");
    nlm::test::TestAssert::equal(0.06f, stdp.getLTDWeight(), "testSTDPConfiguration");
    nlm::test::TestAssert::equal(30.0f, stdp.getTimeConstant(), "testSTDPConfiguration");
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    nlm::test::TestAssert::equal(0.03f, stdp.getLTPWeight(), "testSTDPConfigure");
    nlm::test::TestAssert::equal(0.035f, stdp.getLTDWeight(), "testSTDPConfigure");
    nlm::test::TestAssert::equal(25.0f, stdp.getTimeConstant(), "testSTDPConfigure");
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    nlm::test::TestAssert::notNull(stdp.getName(), "testSTDPName");
    nlm::test::TestAssert::equal(std::string("STDP"), std::string(stdp.getName()), "testSTDPName");
    
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
    nlm::test::TestAssert::greaterThan(finalWeight, initialWeight, "testSTDPPotentiation");
    
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
    nlm::test::TestAssert::lessThan(finalWeight, initialWeight, "testSTDPDepression");
    
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
    nlm::test::TestAssert::equal(initialWeight, synapse.getWeight(), "testSTDPEmptySpikes");
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    nlm::test::TestAssert::equal(initialWeight, synapse.getWeight(), "testSTDPEmptySpikes");
    
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
    nlm::test::TestAssert::lessThan(1.0f, synapse.getWeight() + 1e-6f, "testSTDPWeightBounds");
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
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
}

} // namespace test_stdp
