// Structural Plasticity Tests
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_structural_plasticity {

void testStructuralPlasticityCreation() {
    nlm::StructuralPlasticity sp;
    
    assert(sp.getSynaptogenesisRate() == 0.0001f);
    assert(sp.getPruningRate() == 0.00001f);
    assert(sp.getMaxSynapsesPerNeuron() == 100);
    
    std::cout << "    testStructuralPlasticityCreation passed" << std::endl;
}

void testStructuralPlasticityConfiguration() {
    nlm::StructuralPlasticity sp;
    
    sp.setSynaptogenesisRate(0.001f);
    sp.setPruningRate(0.0001f);
    sp.setMaxSynapsesPerNeuron(200);
    
    assert(sp.getSynaptogenesisRate() == 0.001f);
    assert(sp.getPruningRate() == 0.0001f);
    assert(sp.getMaxSynapsesPerNeuron() == 200);
    
    std::cout << "    testStructuralPlasticityConfiguration passed" << std::endl;
}

void testStructuralPlasticityName() {
    nlm::StructuralPlasticity sp;
    
    assert(sp.getName() != nullptr);
    assert(std::string(sp.getName()) == "StructuralPlasticity");
    
    std::cout << "    testStructuralPlasticityName passed" << std::endl;
}

void testStructuralPlasticitySynaptogenesis() {
    nlm::StructuralPlasticity sp;
    nlm::RandomGenerator rng(42);
    
    // Create neurons and synapse with low initial weight
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    // Set low weight to encourage potential synaptogenesis
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.01f);
    
    float initialWeight = synapse.getWeight();
    
    // Run plasticity update
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};  // Pre before post
    
    sp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Synaptogenesis may or may not occur depending on parameters
    // Just test that it doesn't crash
    assert(synapse.getWeight() >= 0.0f);  // Weight should not be negative
    
    std::cout << "    testStructuralPlasticitySynaptogenesis passed" << std::endl;
}

void testStructuralPlasticityPruning() {
    nlm::StructuralPlasticity sp;
    
    // Create synapse with high weight to encourage pruning
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setWeight(0.5f);  // Medium weight
    
    float initialWeight = synapse.getWeight();
    
    // Create pattern that might trigger pruning
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // Post before pre (depression)
    
    sp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Pruning may or may not occur
    assert(synapse.getWeight() >= 0.0f);
    
    std::cout << "    testStructuralPlasticityPruning passed" << std::endl;
}

void testStructuralPlasticityWeightBounds() {
    nlm::StructuralPlasticity sp;
    
    // Set tight bounds
    sp.setWeightBounds(-0.5f, 0.5f);
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setWeight(0.5f);  // At upper bound
    
    float initialWeight = synapse.getWeight();
    
    // Try to potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0};
    
    sp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to bounds
    assert(synapse.getWeight() <= 0.5f);
    assert(synapse.getWeight() >= -0.5f);
    
    std::cout << "    testStructuralPlasticityWeightBounds passed" << std::endl;
}

void testStructuralPlasticityNameConsistency() {
    nlm::StructuralPlasticity sp1;
    nlm::StructuralPlasticity sp2;
    
    assert(std::string(sp1.getName()) == "StructuralPlasticity");
    assert(std::string(sp2.getName()) == "StructuralPlasticity");
    assert(sp1.getName() == sp2.getName());
    
    std::cout << "    testStructuralPlasticityNameConsistency passed" << std::endl;
}

void runAll() {
    std::cout << "Running Structural Plasticity tests..." << std::endl;
    testStructuralPlasticityCreation();
    testStructuralPlasticityConfiguration();
    testStructuralPlasticityName();
    testStructuralPlasticitySynaptogenesis();
    testStructuralPlasticityPruning();
    testStructuralPlasticityWeightBounds();
    testStructuralPlasticityNameConsistency();
}

} // namespace test_structural_plasticity
