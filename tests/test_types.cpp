// Types Tests
#include "core/Types/Types.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>

namespace test_types {

void testNeuronId() {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    nlm::test::TestAssert::equal(true, (id1 == id3), "testNeuronId");
    nlm::test::TestAssert::equal(true, (id1 != id2), "testNeuronId");
    nlm::test::TestAssert::equal(true, (id1 < id2), "testNeuronId");
    nlm::test::TestAssert::equal(true, (id2 > id1), "testNeuronId");
    nlm::test::TestAssert::equal(1, id1.index(), "testNeuronId");
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    nlm::test::TestAssert::equal(true, (id1 != id2), "testSynapseId");
    nlm::test::TestAssert::equal(true, (id1 < id2), "testSynapseId");
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    nlm::test::TestAssert::equal(true, (id1 != id2), "testRegionId");
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    nlm::test::TestAssert::equal(true, (id1 != id2), "testPopulationId");
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType() {
    nlm::test::TestAssert::equal(true, (nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory), "testNeuronType");
    nlm::test::TestAssert::equal(true, (nlm::NeuronType::Sensory != nlm::NeuronType::Motor), "testNeuronType");
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    nlm::test::TestAssert::equal(true, (nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory), "testSynapseType");
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    nlm::test::TestAssert::equal(false, flags.hebbian, "testPlasticityFlags");
    nlm::test::TestAssert::equal(false, flags.stdp, "testPlasticityFlags");
    nlm::test::TestAssert::equal(false, flags.reward_modulated, "testPlasticityFlags");
    
    flags.hebbian = true;
    nlm::test::TestAssert::equal(true, flags.hebbian, "testPlasticityFlags");
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    nlm::test::TestAssert::equal(1, event.source_neuron.index(), "testSpikeEvent");
    nlm::test::TestAssert::equal(0.5, event.timestamp, "testSpikeEvent");
    nlm::test::TestAssert::equal(500, event.step, "testSpikeEvent");
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

void runAll() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testNeuronType();
    testSynapseType();
    testPlasticityFlags();
    testSpikeEvent();
}

} // namespace test_types
