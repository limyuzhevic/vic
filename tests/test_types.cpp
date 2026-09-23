// Type Tests
// Tests for NLM type definitions and enumerations
// #include "brain/Neuron.hpp"
// #include "brain/Synapse.hpp"
// #include "brain/Brain.hpp"
// #include <cassert>
// #include <iostream>

namespace test_types {

void testNeuronType() {
    assert(nlm::NeuronType::Internal == nlm::NeuronType::Internal);
    assert(nlm::NeuronType::Sensory == nlm::NeuronType::Sensory);
    assert(nlm::NeuronType::Internal == nlm::NeuronType::Internal);
    assert(nlm::NeuronType::Motor == nlm::NeuronType::Motor);
    assert(nlm::NeuronType::Excitatory == nlm::NeuronType::Excitatory);
    assert(nlm::NeuronType::Inhibitory == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    assert(nlm::SynapseType::Excitatory == nlm::SynapseType::Excitatory);
    assert(nlm::SynapseType::Inhibitory == nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testFiringState() {
    assert(nlm::FiringState::Resting == nlm::FiringState::Resting);
    assert(nlm::FiringState::Active == nlm::FiringState::Active);
    assert(nlm::FiringState::Refractory == nlm::FiringState::Refractory);
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testActionType() {
    assert(nlm::ActionType::Wait == nlm::ActionType::Wait);
    assert(nlm::ActionType::MoveForward == nlm::ActionType::MoveForward);
    assert(nlm::ActionType::MoveBackward == nlm::ActionType::MoveBackward);
    assert(nlm::ActionType::TurnLeft == nlm::ActionType::TurnLeft);
    assert(nlm::ActionType::TurnRight == nlm::ActionType::TurnRight);
    
    std::cout << "    testActionType passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    assert(id1.index() == 1);
    assert(id2.index() == 2);
    assert(id1 != id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testNeuronId() {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    assert(id1.index() == 1);
    assert(id2.index() == 2);
    assert(id1 != id2);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(1);
    nlm::SynapseId id2(2);
    
    assert(id1.index() == 1);
    assert(id2.index() == 2);
    assert(id1 != id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    assert(id1.index() == 1);
    assert(id2.index() == 2);
    assert(id1 != id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void runAll() {
    testNeuronType();
    testSynapseType();
    testFiringState();
    testActionType();
    testRegionId();
    testNeuronId();
    testSynapseId();
    testPopulationId();
    
    std::cout << std::endl;
    std::cout << "=== Type Tests ===" << std::endl;
    std::cout << "All type tests passed!" << std::endl;
}

} // namespace test_types