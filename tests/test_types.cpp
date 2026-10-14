// Types Tests
#include "core/Types/Types.hpp"
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
    assert(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory);
    assert(nlm::NeuronType::Sensory != nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    assert(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    flags.hebbian = true;
    assert(flags.hebbian);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    assert(event.source_neuron.index() == 1);
    assert(event.timestamp == 0.5);
    assert(event.step == 500);
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

void testInvalidIds() {
    // Test that invalid IDs work correctly
    nlm::NeuronId invalidNeuron = nlm::INVALID_NEURON_ID;
    nlm::SynapseId invalidSynapse = nlm::INVALID_SYNAPSE_ID;
    nlm::RegionId invalidRegion = nlm::INVALID_REGION_ID;
    nlm::PopulationId invalidPopulation = nlm::INVALID_POPULATION_ID;
    
    assert(invalidNeuron.index() == UINT64_MAX);
    assert(invalidSynapse.index() == UINT64_MAX);
    assert(invalidRegion.index() == UINT64_MAX);
    assert(invalidPopulation.index() == UINT64_MAX);
    
    std::cout << "    testInvalidIds passed" << std::endl;
}

void testNeuronIdOperations() {
    nlm::NeuronId id1(100);
    nlm::NeuronId id2(200);
    nlm::NeuronId id3(100);
    
    // Test comparison operators
    assert(id1 == id3);
    assert(id1 != id2);
    assert(id1 < id2);
    assert(id2 > id1);
    assert(id1 <= id2);
    assert(id2 >= id1);
    assert(id1 <= id3);
    assert(id3 >= id1);
    
    // Test index() method
    assert(id1.index() == 100);
    assert(id2.index() == 200);
    
    std::cout << "    testNeuronIdOperations passed" << std::endl;
}

void testSynapseIdOperations() {
    nlm::SynapseId id1(500);
    nlm::SynapseId id2(600);
    nlm::SynapseId id3(500);
    
    // Test comparison operators
    assert(id1 == id3);
    assert(id1 != id2);
    assert(id1 < id2);
    assert(id2 > id1);
    
    // Test arithmetic operations
    nlm::SynapseId sum = id1 + id2;
    assert(sum.index() == 1100);
    
    nlm::SynapseId diff = id2 - id1;
    assert(diff.index() == 100);
    
    // Test increment/decrement
    nlm::SynapseId incremented = ++id1;
    assert(incremented.index() == 501);
    assert(id1.index() == 501);
    
    nlm::SynapseId decremented = --id2;
    assert(decremented.index() == 599);
    assert(id2.index() == 599);
    
    std::cout << "    testSynapseIdOperations passed" << std::endl;
}

void testEnumConversions() {
    // Test all neuron types
    assert(static_cast<uint8_t>(nlm::NeuronType::Excitatory) == 0);
    assert(static_cast<uint8_t>(nlm::NeuronType::Inhibitory) == 1);
    assert(static_cast<uint8_t>(nlm::NeuronType::Modulatory) == 2);
    assert(static_cast<uint8_t>(nlm::NeuronType::Sensory) == 3);
    assert(static_cast<uint8_t>(nlm::NeuronType::Motor) == 4);
    assert(static_cast<uint8_t>(nlm::NeuronType::Internal) == 5);
    
    // Test all synapse types
    assert(static_cast<uint8_t>(nlm::SynapseType::Excitatory) == 0);
    assert(static_cast<uint8_t>(nlm::SynapseType::Inhibitory) == 1);
    assert(static_cast<uint8_t>(nlm::SynapseType::Modulatory) == 2);
    assert(static_cast<uint8_t>(nlm::SynapseType::Electrical) == 3);
    assert(static_cast<uint8_t>(nlm::SynapseType::GapJunction) == 4);
    
    // Test all developmental stages
    assert(static_cast<uint8_t>(nlm::DevelopmentalStage::Initial) == 0);
    assert(static_cast<uint8_t>(nlm::DevelopmentalStage::CriticalPeriod) == 1);
    assert(static_cast<uint8_t>(nlm::DevelopmentalStage::Maturation) == 2);
    assert(static_cast<uint8_t>(nlm::DevelopmentalStage::Adult) == 3);
    assert(static_cast<uint8_t>(nlm::DevelopmentalStage::Aging) == 4);
    
    std::cout << "    testEnumConversions passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    assert(!flags.structural);
    assert(!flags.eligible);
    
    // Test bit operations
    flags.hebbian = true;
    assert(flags.hebbian);
    assert(!flags.stdp);
    
    flags.stdp = true;
    assert(flags.stdp);
    assert(flags.hebbian);
    
    // Test reset
    flags = nlm::PlasticityFlags();
    assert(!flags.hebbian);
    assert(!flags.stdp);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEventEquality() {
    nlm::SpikeEvent event1(nlm::NeuronId(1), 0.5, 100);
    nlm::SpikeEvent event2(nlm::NeuronId(1), 0.5, 100);
    nlm::SpikeEvent event3(nlm::NeuronId(2), 0.5, 100);
    
    assert(event1.source_neuron == event2.source_neuron);
    assert(event1.timestamp == event2.timestamp);
    assert(event1.step == event2.step);
    assert(event1 == event2);
    assert(event1 != event3);
    
    std::cout << "    testSpikeEventEquality passed" << std::endl;
}

void testDelayedSpikeEvent() {
    nlm::DelayedSpikeEvent event(
        nlm::NeuronId(1), nlm::NeuronId(2), nlm::SynapseId(100),
        0.5f, nlm::SynapseType::Excitatory,
        0.1, 0.2, 100, 105
    );
    
    assert(event.source_neuron.index() == 1);
    assert(event.destination_neuron.index() == 2);
    assert(event.synapse_id.index() == 100);
    assert(event.weight == 0.5f);
    assert(event.synapse_type == nlm::SynapseType::Excitatory);
    assert(event.timestamp == 0.1);
    assert(event.delivery_time == 0.2);
    assert(event.step == 100);
    assert(event.delivery_step == 105);
    assert(event.is_excitatory == true);
    
    std::cout << "    testDelayedSpikeEvent passed" << std::endl;
}

void testFiringState() {
    assert(static_cast<uint8_t>(nlm::FiringState::Resting) == 0);
    assert(static_cast<uint8_t>(nlm::FiringState::Active) == 1);
    assert(static_cast<uint8_t>(nlm::FiringState::Refractory) == 2);
    assert(static_cast<uint8_t>(nlm::FiringState::Inhibited) == 3);
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getState().firingState == nlm::FiringState::Resting);
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Inhibited);
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testInvalidIdComparison() {
    nlm::NeuronId id1(UINT64_MAX);
    nlm::NeuronId id2(UINT64_MAX);
    
    assert(id1 == id2);
    assert(!(id1 < id2));
    assert(!(id1 > id2));
    
    nlm::SynapseId syn1(UINT64_MAX);
    nlm::SynapseId syn2(UINT64_MAX);
    
    assert(syn1 == syn2);
    
    std::cout << "    testInvalidIdComparison passed" << std::endl;
}

void testConstantValues() {
    // Test constant values
    assert(nlm::INVALID_NEURON_ID.index() == UINT64_MAX);
    assert(nlm::INVALID_SYNAPSE_ID.index() == UINT64_MAX);
    assert(nlm::INVALID_REGION_ID.index() == UINT64_MAX);
    assert(nlm::INVALID_POPULATION_ID.index() == UINT64_MAX);
    
    // Test default values
    nlm::NeuronState defaultState;
    assert(defaultState.membranePotential == -70.0f);
    assert(defaultState.restingPotential == -70.0f);
    assert(defaultState.threshold == -55.0f);
    assert(defaultState.resetPotential == -70.0f);
    assert(defaultState.firingState == nlm::FiringState::Resting);
    assert(defaultState.refractoryRemaining == 0);
    
    std::cout << "    testConstantValues passed" << std::endl;
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
    testInvalidIds();
    testNeuronIdOperations();
    testSynapseIdOperations();
    testEnumConversions();
    testPlasticityFlags();
    testSpikeEventEquality();
    testDelayedSpikeEvent();
    testFiringState();
    testInvalidIdComparison();
    testConstantValues();
}

} // namespace test_types
