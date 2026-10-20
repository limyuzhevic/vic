// Core Types Tests
// Tests for neural types, IDs, enumerations

#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>

namespace test_types {

void testNeuronId() {
    nlm::NeuronId id1;
    nlm::NeuronId id2(42);
    
    assert(id1.value == 0);
    assert(id2.value == 42);
    assert(id2.index() == 42);
    
    // Test equality
    assert(id1 == nlm::INVALID_NEURON_ID);
    assert(id2 != nlm::INVALID_NEURON_ID);
    
    // Test hash
    auto hash1 = std::hash<nlm::NeuronId>{}(id1);
    auto hash2 = std::hash<nlm::NeuronId>{}(id2);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1;
    nlm::SynapseId id2(123);
    
    assert(id1.value == 0);
    assert(id2.value == 123);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1;
    nlm::RegionId id2(999);
    
    assert(id1.value == 0);
    assert(id2.value == 999);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1;
    nlm::PopulationId id2(555);
    
    assert(id1.value == 0);
    assert(id2.value == 555);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testEnumerations() {
    // Test NeuronType enumeration
    assert(static_cast<int>(nlm::NeuronType::Excitatory) == 0);
    assert(static_cast<int>(nlm::NeuronType::Inhibitory) == 1);
    assert(static_cast<int>(nlm::NeuronType::Sensory) == 4);
    
    // Test SynapseType enumeration
    assert(static_cast<int>(nlm::SynapseType::Excitatory) == 0);
    assert(static_cast<int>(nlm::SynapseType::Inhibitory) == 1);
    
    // Test DevelopmentalStage enumeration
    assert(static_cast<int>(nlm::DevelopmentalStage::Initial) == 0);
    assert(static_cast<int>(nlm::DevelopmentalStage::Adult) == 3);
    
    // Test ActionType enumeration
    assert(static_cast<int>(nlm::ActionType::MoveForward) == 0);
    assert(static_cast<int>(nlm::ActionType::TurnLeft) == 5);
    
    // Test WorldObjectType enumeration
    assert(static_cast<int>(nlm::WorldObjectType::Empty) == 0);
    assert(static_cast<int>(nlm::WorldObjectType::Resource) == 1);
    
    std::cout << "    testEnumerations passed" << std::endl;
}

void runAll() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testEnumerations();
}

} // namespace test_types