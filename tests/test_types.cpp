// Types Tests
#include "core/Types/Types.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_types {

TEST_CASE("NeuronId equality and ordering operations", "[types][neuron_id]") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    REQUIRE(id1 == id3);
    REQUIRE(id1 != id2);
    REQUIRE(id1 < id2);
    REQUIRE(id2 > id1);
    REQUIRE(id1.index() == 1);
}

TEST_CASE("SynapseId equality and ordering operations", "[types][synapse_id]") {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    REQUIRE(id1 != id2);
    REQUIRE(id1 < id2);
}

TEST_CASE("RegionId equality and ordering operations", "[types][region_id]") {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    REQUIRE(id1 != id2);
}

TEST_CASE("PopulationId equality and ordering operations", "[types][population_id]") {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    REQUIRE(id1 != id2);
}

TEST_CASE("NeuronType enum values are distinct", "[types][neuron_type]") {
    REQUIRE(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory);
    REQUIRE(nlm::NeuronType::Sensory != nlm::NeuronType::Motor);
}

TEST_CASE("SynapseType enum values are distinct", "[types][synapse_type]") {
    REQUIRE(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory);
}

TEST_CASE("PlasticityFlags default initialization", "[types][plasticity_flags]") {
    nlm::PlasticityFlags flags;
    
    REQUIRE(!flags.hebbian);
    REQUIRE(!flags.stdp);
    REQUIRE(!flags.reward_modulated);
}

TEST_CASE("PlasticityFlags mutable properties", "[types][plasticity_flags]") {
    nlm::PlasticityFlags flags;
    
    flags.hebbian = true;
    REQUIRE(flags.hebbian);
}

TEST_CASE("SpikeEvent structure fields", "[types][spike_event]") {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    REQUIRE(event.source_neuron.index() == 1);
    REQUIRE(event.timestamp == 0.5);
    REQUIRE(event.step == 500);
}

} // namespace test_types

// Legacy compatibility
namespace legacy {
    void runAll() {
        std::cout << "Legacy test runner called - use Catch2 runner instead" << std::endl;
    }
} // namespace legacy
