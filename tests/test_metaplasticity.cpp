// MetaPlasticityRule Tests
// Enhanced plasticity with metaplasticity for self-regulation

#include "plasticity/MetaPlasticityRule.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_metaplasticity {

void testMetaPlasticityCreation() {
    nlm::MetaPlasticityRule metaPlasticity;
    
    assert(metaPlasticity.getName() != nullptr);
    assert(std::string(metaPlasticity.getName()) == "MetaPlasticityRule");
    
    // Check default parameters
    assert(metaPlasticity.getMetaPlasticityRate() == 0.001f);
    assert(metaPlasticity.getPlasticityThreshold() == 0.1f);
    assert(metaPlasticity.getHebbianSaturationThreshold() == 1.0f);
    assert(metaPlasticity.isMetaPlasticityEnabled() == true);
    
    std::cout << "    testMetaPlasticityCreation passed" << std::endl;
}

void testMetaPlasticityConfiguration() {
    nlm::MetaPlasticityRule metaPlasticity;
    
    metaPlasticity.setMetaPlasticityRate(0.005f);
    metaPlasticity.setPlasticityThreshold(0.2f);
    metaPlasticity.setHebbianSaturationThreshold(2.0f);
    metaPlasticity.setMetaPlasticityEnabled(true);
    
    assert(metaPlasticity.getMetaPlasticityRate() == 0.005f);
    assert(metaPlasticity.getPlasticityThreshold() == 0.2f);
    assert(metaPlasticity.getHebbianSaturationThreshold() == 2.0f);
    assert(metaPlasticity.isMetaPlasticityEnabled() == true);
    
    std::cout << "    testMetaPlasticityConfiguration passed" << std::endl;
}

void testMetaPlasticityDisable() {
    nlm::MetaPlasticityRule metaPlasticity;
    metaPlasticity.setMetaPlasticityEnabled(false);
    
    assert(metaPlasticity.isMetaPlasticityEnabled() == false);
    
    std::cout << "    testMetaPlasticityDisable passed" << std::endl;
}

void testMetaPlasticityStateUpdate() {
    nlm::MetaPlasticityRule metaPlasticity;
    
    // Create a synapse for testing
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    float initialWeight = synapse.getWeight();
    
    metaPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change significantly due to metaplasticity modulation
    float finalWeight = synapse.getWeight();
    assert(std::abs(finalWeight - initialWeight) < 0.01f);
    
    std::cout << "    testMetaPlasticityStateUpdate passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testMetaPlasticityModulation() {
    nlm::MetaPlasticityRule metaPlasticity;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse with high Hebbian activity
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike patterns that would generate high activity
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
    
    metaPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // With high activity, metaplasticity should constrain weight changes
    // Weight change should be small due to modulation
    assert(std::abs(finalWeight - initialWeight) < 0.05f);
    
    std::cout << "    testMetaPlasticityModulation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testMetaPlasticityLowActivity() {
    nlm::MetaPlasticityRule metaPlasticity;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse with low activity
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Create spike patterns with minimal overlap
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // Minimal overlap
    
    metaPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // With low activity, weight change should be minimal
    assert(std::abs(finalWeight - initialWeight) < 0.01f);
    
    std::cout << "    testMetaPlasticityLowActivity passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testMetaPlasticityWeightBounds() {
    nlm::MetaPlasticityRule metaPlasticity;
    
    // Test with very high weight
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.95f);  // Near max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
    
    metaPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    // Test with very low weight
    nlm::Synapse synapse2(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(-0.95f);  // Near min
    
    preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
    
    metaPlasticity.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse2.getWeight() >= -1.0f);
    
    std::cout << "    testMetaPlasticityWeightBounds passed" << std::endl;
}

void testMetaPlasticityParameterClamping() {
    nlm::MetaPlasticityRule metaPlasticity;
    
    // Test clamping of parameter ranges
    metaPlasticity.setMetaPlasticityRate(-0.01f);  // Should clamp to min
    assert(metaPlasticity.getMetaPlasticityRate() == 0.0001f);
    
    metaPlasticity.setMetaPlasticityRate(0.1f);   // Should clamp to max
    assert(metaPlasticity.getMetaPlasticityRate() == 0.01f);
    
    metaPlasticity.setPlasticityThreshold(-0.1f);  // Should clamp to min
    assert(metaPlasticity.getPlasticityThreshold() == 0.0f);
    
    metaPlasticity.setPlasticityThreshold(2.0f);   // Should clamp to max
    assert(metaPlasticity.getPlasticityThreshold() == 1.0f);
    
    metaPlasticity.setHebbianSaturationThreshold(0.05f);  // Should clamp to min
    assert(metaPlasticity.getHebbianSaturationThreshold() == 0.1f);
    
    metaPlasticity.setHebbianSaturationThreshold(10.0f);   // Should clamp to max
    assert(metaPlasticity.getHebbianSaturationThreshold() == 5.0f);
    
    std::cout << "    testMetaPlasticityParameterClamping passed" << std::endl;
}

void runAll() {
    std::cout << "Running MetaPlasticityRule tests..." << std::endl;
    testMetaPlasticityCreation();
    testMetaPlasticityConfiguration();
    testMetaPlasticityDisable();
    testMetaPlasticityStateUpdate();
    testMetaPlasticityModulation();
    testMetaPlasticityLowActivity();
    testMetaPlasticityWeightBounds();
    testMetaPlasticityParameterClamping();
}

} // namespace test_metaplasticity
