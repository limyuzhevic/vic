// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace test_stdp {

TEST_CASE("STDP creation with default parameters", "[stdp][creation][default]") {
    nlm::STDP stdp;
    
    REQUIRE(stdp.getLTPWeight() == 0.01f);
    REQUIRE(stdp.getLTDWeight() == 0.012f);
    REQUIRE(stdp.getTimeConstant() == 20.0f);
}

TEST_CASE("STDP parameter configuration", "[stdp][config]") {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    REQUIRE(stdp.getLTPWeight() == 0.05f);
    REQUIRE(stdp.getLTDWeight() == 0.06f);
    REQUIRE(stdp.getTimeConstant() == 30.0f);
}

TEST_CASE("STDP configure method", "[stdp][configure]") {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    REQUIRE(stdp.getLTPWeight() == 0.03f);
    REQUIRE(stdp.getLTDWeight() == 0.035f);
    REQUIRE(stdp.getTimeConstant() == 25.0f);
}

TEST_CASE("STDP name property", "[stdp][name]") {
    nlm::STDP stdp;
    
    const char* name = stdp.getName();
    REQUIRE(name != nullptr);
    REQUIRE(std::string(name) == "STDP");
}

TEST_CASE("STDP potentiation (pre before post)", "[stdp][potentiation]") {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    REQUIRE(finalWeight > initialWeight);
}

TEST_CASE("STDP depression (post before pre)", "[stdp][depression]") {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    REQUIRE(finalWeight < initialWeight);
}

TEST_CASE("STDP with empty spike trains", "[stdp][empty]") {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    REQUIRE(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    REQUIRE(synapse.getWeight() == initialWeight);
}

TEST_CASE("STDP weight bounds enforcement", "[stdp][bounds]") {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    REQUIRE(synapse.getWeight() <= 1.0f);
}

} // namespace test_stdp
