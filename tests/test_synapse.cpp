// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace test_synapse {

TEST_CASE("Synapse creation with proper identifiers", "[synapse][creation][id]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getId() == nlm::SynapseId(1));
    REQUIRE(synapse.getSourceNeuron() == nlm::NeuronId(1));
    REQUIRE(synapse.getDestinationNeuron() == nlm::NeuronId(2));
}

TEST_CASE("Synapse weight management", "[synapse][weight]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    REQUIRE(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    REQUIRE(synapse.getWeight() == 0.7f);
    
    synapse.addToWeight(-0.3f);
    REQUIRE(synapse.getWeight() == 0.4f);
}

TEST_CASE("Synapse weight bounds enforcement", "[synapse][weight][bounds]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    REQUIRE(synapse.getWeight() <= 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    REQUIRE(synapse.getWeight() >= -1.0f);  // Should be clamped
}

TEST_CASE("Synapse type management", "[synapse][type]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    REQUIRE(synapse.getType() == nlm::SynapseType::Excitatory);
    REQUIRE(synapse.isExcitatory());
    REQUIRE(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    REQUIRE(synapse.getType() == nlm::SynapseType::Inhibitory);
    REQUIRE(synapse.isInhibitory());
    REQUIRE(!synapse.isExcitatory());
}

TEST_CASE("Synapse delay management", "[synapse][delay]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getDelay() == 1);  // Default
    
    synapse.setDelay(5);
    REQUIRE(synapse.getDelay() == 5);
}

TEST_CASE("Synapse spike history management", "[synapse][history]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getPreSpikeHistory().empty());
    REQUIRE(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    REQUIRE(synapse.getPreSpikeHistory().size() == 2);
    REQUIRE(synapse.getPostSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    REQUIRE(synapse.getPreSpikeHistory().empty());
    REQUIRE(synapse.getPostSpikeHistory().empty());
}

TEST_CASE("Synapse eligibility trace management", "[synapse][eligibility]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    REQUIRE(synapse.getEligibilityTrace() == 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    REQUIRE(synapse.getEligibilityTrace() < 0.5f);
}

TEST_CASE("Synapse efficacy management", "[synapse][efficacy]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(synapse.getEfficacy() == 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    REQUIRE(synapse.getEfficacy() == 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    REQUIRE(synapse.getEfficacy() <= 2.0f);  // Clamped
}

TEST_CASE("Synapse random initialization", "[synapse][random]") {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    REQUIRE(synapse.getWeight() > 0.0f);
    
    // Delay should be in valid range
    REQUIRE(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
}

TEST_CASE("Synapse reset operation", "[synapse][reset]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    REQUIRE(synapse.getWeight() == 0.0f);
    REQUIRE(synapse.getPreSpikeHistory().empty());
    REQUIRE(synapse.getPostSpikeHistory().empty());
    REQUIRE(synapse.getEligibilityTrace() == 0.0f);
}

TEST_CASE("Synapse plasticity flags management", "[synapse][plasticity]") {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    REQUIRE(!synapse.getPlasticityFlags().hebbian);
    REQUIRE(!synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    REQUIRE(synapse.getPlasticityFlags().hebbian);
    REQUIRE(synapse.getPlasticityFlags().stdp);
    REQUIRE(!synapse.getPlasticityFlags().reward_modulated);
}

} // namespace test_synapse
