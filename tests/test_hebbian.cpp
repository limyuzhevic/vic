// Hebbian Tests
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianName() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void testHebbianCoActivation() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Many coincident pre and post spikes - should strongly potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.5, 1.0, 1.5, 2.0};
    std::vector<nlm::Timestamp> postSpikes = {0.1, 0.6, 1.1, 1.6, 2.1};  // Very close to pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated activity
    assert(finalWeight > initialWeight);
    
    std::cout << "    testHebbianCoActivation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianCorrelation() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre and post spikes that are somewhat correlated but not perfectly
    // Should show moderate potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 2.0, 4.0, 6.0, 8.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 3.0, 5.0, 7.0, 9.0};  // Regular delay
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have changed (likely increased due to correlation)
    assert(std::abs(finalWeight - initialWeight) > 0.001f);
    
    std::cout << "    testHebbianCorrelation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianInteractionWithSTDP() {
    nlm::Hebbian hebbian;
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse with both plasticity rules enabled
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Enable both plasticity rules
    synapse.enablePlasticity(true, true, false);
    
    float initialWeight = synapse.getWeight();
    
    // STDP scenario (pre before post)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Apply Hebbian update
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Apply STDP update on same spikes
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight change should be combination of both rules
    assert(std::abs(finalWeight - initialWeight) > 0.001f);
    
    std::cout << "    testHebbianInteractionWithSTDP passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianZeroInput() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    float initialWeight = synapse.getWeight();
    
    // Empty spike lists - should not change weight
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianZeroInput passed" << std::endl;
}

void testHebbianAsymmetricCorrelation() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Many pre spikes, few post spikes - less correlation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<nlm::Timestamp> postSpikes = {0.5, 5.5};  // Sparse correlation
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should be different from initial (due to asymmetric correlation)
    assert(std::abs(finalWeight - initialWeight) > 0.001f);
    
    std::cout << "    testHebbianAsymmetricCorrelation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianTemporalDistance() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Same spike count, but temporal distance matters for correlation
    // Tightly synchronized spikes (0.1ms apart)
    std::vector<nlm::Timestamp> preSpikesTightly = {0.0, 0.1, 0.2, 0.3};
    std::vector<nlm::Timestamp> postSpikesTightly = {0.05, 0.15, 0.25, 0.35};
    
    nlm::Synapse synapse1(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    float weightTight = synapse1.getWeight();
    hebbian.update(&synapse1, preSpikesTightly, postSpikesTightly, 0.001);
    float finalWeightTight = synapse1.getWeight();
    
    // Loosely synchronized spikes (10ms apart)
    std::vector<nlm::Timestamp> preSpikesLoose = {0.0, 1.0, 2.0, 3.0};
    std::vector<nlm::Timestamp> postSpikesLoose = {10.0, 11.0, 12.0, 13.0};
    
    nlm::Synapse synapse2(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    float weightLoose = synapse2.getWeight();
    hebbian.update(&synapse2, preSpikesLoose, postSpikesLoose, 0.001);
    float finalWeightLoose = synapse2.getWeight();
    
    // Tightly synchronized should produce stronger effect
    assert((finalWeightTight - weightTight) > (finalWeightLoose - weightLoose));
    
    std::cout << "    testHebbianTemporalDistance passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianCoActivation();
    testHebbianCorrelation();
    testHebbianInteractionWithSTDP();
    testHebbianZeroInput();
    testHebbianAsymmetricCorrelation();
    testHebbianTemporalDistance();
}

} // namespace test_hebbian
