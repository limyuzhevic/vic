// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_neuron {

TEST_CASE("Neuron creation and ID assignment", "[neuron][creation][id]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    REQUIRE(neuron.getId() == nlm::NeuronId(1));
    REQUIRE(neuron.getType() == nlm::NeuronType::Internal);
}

TEST_CASE("Neuron type modifications", "[neuron][type]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    REQUIRE(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    REQUIRE(neuron.getType() == nlm::NeuronType::Inhibitory);
}

TEST_CASE("Membrane potential management", "[neuron][membrane]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    REQUIRE(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    REQUIRE(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    REQUIRE(neuron.getMembranePotential() == -60.0f);
}

TEST_CASE("Neuron firing state management", "[neuron][firing]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    REQUIRE(!neuron.isFiring());
    REQUIRE(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    REQUIRE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    REQUIRE(neuron.isRefractory());
    REQUIRE(!neuron.isFiring());
}

TEST_CASE("Refractory period decrement", "[neuron][refractory]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    REQUIRE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    REQUIRE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    REQUIRE(!neuron.isRefractory());
}

TEST_CASE("Current injection management", "[neuron][current]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    REQUIRE(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    REQUIRE(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    REQUIRE(neuron.getTotalCurrent() == 0.0f);
}

TEST_CASE("Synaptic input management", "[neuron][synaptic]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    REQUIRE(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    REQUIRE(neuron.getTotalCurrent() == -3.0f);
}

TEST_CASE("Spike history management", "[neuron][spike_history]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    REQUIRE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    REQUIRE(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    REQUIRE(neuron.getSpikeHistory().empty());
}

TEST_CASE("Random initialization", "[neuron][random]") {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    REQUIRE(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    REQUIRE(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
}

TEST_CASE("Neuron reset operation", "[neuron][reset]") {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    REQUIRE(neuron.getMembranePotential() == -70.0f);
    REQUIRE(neuron.getTotalCurrent() == 0.0f);
    REQUIRE(neuron.getSpikeHistory().empty());
}

} // namespace test_neuron
