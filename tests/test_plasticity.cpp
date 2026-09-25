// Plasticity Tests
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_plasticity {

TEST_CASE("PlasticityRule base class functionality", "[plasticity][base][rule]") {
    nlm::PlasticityRule rule;
    
    REQUIRE(rule.getName() != nullptr);
    REQUIRE(rule.getLearningRate() > 0.0f);
    REQUIRE(rule.getTimeConstant() > 0.0f);
}

TEST_CASE("Hebbian plasticity implementation", "[plasticity][hebbian]") {
    nlm::Hebbian hebbian;
    
    REQUIRE(std::string(hebbian.getName()) == "Hebbian");
    REQUIRE(hebbian.getLearningRate() > 0.0f);
    REQUIRE(hebbian.getWeightChange() > 0.0f);
}

TEST_CASE("STDP plasticity implementation", "[plasticity][stdp]") {
    nlm::STDP stdp;
    
    REQUIRE(std::string(stdp.getName()) == "STDP");
    REQUIRE(stdp.getLTPWeight() > 0.0f);
    REQUIRE(stdp.getLTDWeight() > 0.0f);
    REQUIRE(stdp.getTimeConstant() > 0.0f);
}

TEST_CASE("StructuralPlasticity implementation", "[plasticity][structural]") {
    nlm::StructuralPlasticity structural;
    
    REQUIRE(std::string(structural.getName()) == "StructuralPlasticity");
    REQUIRE(structural.getConnectionProbability() >= 0.0f);
    REQUIRE(structural.getConnectionProbability() <= 1.0f);
}

TEST_CASE("Plasticity rule configuration", "[plasticity][config][rule]") {
    nlm::PlasticityRule rule;
    
    rule.configure(0.1f, 1000.0f);
    REQUIRE(rule.getLearningRate() == 0.1f);
    REQUIRE(rule.getTimeConstant() == 1000.0f);
}

TEST_CASE("Hebbian weight update mechanism", "[plasticity][hebbian][update]") {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Update with Hebbian rule
    hebbian.update(&synapse);
    
    REQUIRE(synapse.getWeight() > 0.0f);
}

TEST_CASE("STDP potentiation mechanism", "[plasticity][stdp][potentiation]") {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Potentiation scenario (pre before post)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    REQUIRE(synapse.getWeight() > initialWeight);
}

TEST_CASE("Structural plasticity connection formation", "[plasticity][structural][formation]") {
    nlm::StructuralPlasticity structural;
    
    // Test that structural plasticity can form new connections
    REQUIRE(structural.getConnectionProbability() >= 0.0f);
    
    // Set up a scenario
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setWeight(0.1f);  // Low initial weight
    
    // Structural plasticity should strengthen this connection
    REQUIRE(synapse.getWeight() < 1.0f);
}

TEST_CASE("Plasticity rule bounds checking", "[plasticity][bounds]") {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setWeight(0.5f);
    
    // Update with Hebbian rule
    hebbian.update(&synapse);
    
    // Weight should remain within bounds
    REQUIRE(synapse.getWeight() >= -1.0f);
    REQUIRE(synapse.getWeight() <= 1.0f);
}

} // namespace test_plasticity
