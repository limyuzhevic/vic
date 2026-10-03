// Types Tests
#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>

namespace test_types {

void testNeuronId(nullptr) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    assert(id1 == id3);
    assert(id1 != id2);
    assert(id1 < id2);
    assert(id2 > id1);
    assert(id1.index(nullptr) == 1);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId(nullptr) {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    assert(id1 != id2);
    assert(id1 < id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId(nullptr) {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId(nullptr) {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType(nullptr) {
    assert(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory);
    assert(nlm::NeuronType::Sensory != nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType(nullptr) {
    assert(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags(nullptr) {
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    flags.hebbian = true;
    assert(flags.hebbian);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent(nullptr) {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    assert(event.source_neuron.index(nullptr) == 1);
    assert(event.timestamp == 0.5);
    assert(event.step == 500);
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

void runAll(nullptr) {
    testNeuronId(nullptr);
    testSynapseId(nullptr);
    testRegionId(nullptr);
    testPopulationId(nullptr);
    testNeuronType(nullptr);
    testSynapseType(nullptr);
    testPlasticityFlags(nullptr);
    testSpikeEvent(nullptr);
}

} // namespace test_types
