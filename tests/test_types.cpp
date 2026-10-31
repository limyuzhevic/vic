// Types Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_types {

void testNeuronId() {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    assert(id1 == id3);
    assert(id1 != id2);
    assert(id1 < id2);
    assert(id2 > id1);
    assert(id1.index() == 1);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    assert(id1 != id2);
    assert(id1 < id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType() {
    nlm::NeuronType type1 = nlm::NeuronType::Excitatory;
    nlm::NeuronType type2 = nlm::NeuronType::Inhibitory;
    
    assert(static_cast<int>(type1) != static_cast<int>(type2));
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    nlm::SynapseType type1 = nlm::SynapseType::Excitatory;
    nlm::SynapseType type2 = nlm::SynapseType::Inhibitory;
    
    assert(static_cast<int>(type1) != static_cast<int>(type2));
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testDevelopmentalStage() {
    nlm::DevelopmentalStage stage1 = nlm::DevelopmentalStage::Initial;
    nlm::DevelopmentalStage stage2 = nlm::DevelopmentalStage::Adult;
    
    assert(static_cast<int>(stage1) != static_cast<int>(stage2));
    
    std::cout << "    testDevelopmentalStage passed" << std::endl;
}

void testFiringState() {
    nlm::FiringState state1 = nlm::FiringState::Resting;
    nlm::FiringState state2 = nlm::FiringState::Active;
    
    assert(static_cast<int>(state1) != static_cast<int>(state2));
    
    std::cout << "    testFiringState passed" << std::endl;
}

void runAll() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testNeuronType();
    testSynapseType();
    testDevelopmentalStage();
    testFiringState();
}

} // namespace test_types
