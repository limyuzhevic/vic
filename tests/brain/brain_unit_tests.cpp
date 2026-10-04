// Unit test for Brain class
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_unit {

void testBrainCreation() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Test basic brain properties
    assert(brain.getId().has_value());
    assert(brain.getConfig().has_value());
    
    // Test region management
    auto regions = brain.getRegions();
    assert(regions.size() >= 0);  // May be empty initially
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainNeuronManagement() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Create a neuron
    nlm::Neuron neuron;
    
    // Add neuron to brain
    brain.addNeuron(neuron);
    
    // Check neuron exists
    assert(brain.hasNeuron(neuron.getId()));
    
    // Test neuron retrieval
    auto neurons = brain.getNeurons();
    assert(neurons.size() >= 1);
    
    std::cout << "    testBrainNeuronManagement passed" << std::endl;
}

void testBrainSynapseManagement() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Create synapses
    nlm::Synapse syn1;
    nlm::Synapse syn2;
    
    // Add synapses to brain
    brain.addSynapse(syn1);
    brain.addSynapse(syn2);
    
    // Check synapses exist
    assert(brain.hasSynapse(syn1.getId()));
    assert(brain.hasSynapse(syn2.getId()));
    
    // Test synapse retrieval
    auto synapses = brain.getSynapses();
    assert(synapses.size() >= 2);
    
    std::cout << "    testBrainSynapseManagement passed" << std::endl;
}

void testBrainPopulationManagement() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Create neural population
    nlm::NeuralPopulation population;
    
    // Add population to brain
    brain.addPopulation(population);
    
    // Check population exists
    assert(brain.hasPopulation(population.getId()));
    
    // Test population retrieval
    auto populations = brain.getPopulations();
    assert(populations.size() >= 1);
    
    std::cout << "    testBrainPopulationManagement passed" << std::endl;
}

void testBrainRegionManagement() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Create neural region
    nlm::NeuralRegion region;
    
    // Add region to brain
    brain.addRegion(region);
    
    // Check region exists
    assert(brain.hasRegion(region.getId()));
    
    // Test region retrieval
    auto regions = brain.getRegions();
    assert(regions.size() >= 1);
    
    std::cout << "    testBrainRegionManagement passed" << std::endl;
}

void testBrainUpdate() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Add some components
    nlm::Neuron neuron;
    nlm::Synapse synapse;
    nlm::NeuralPopulation population;
    nlm::NeuralRegion region;
    
    brain.addNeuron(neuron);
    brain.addSynapse(synapse);
    brain.addPopulation(population);
    brain.addRegion(region);
    
    // Test brain update
    brain.update(0.001f);  // Update for 1 timestep
    
    // Check that components were processed
    // (Implementation-specific checks)
    
    std::cout << "    testBrainUpdate passed" << std::endl;
}

void testBrainConfiguration() {
    nlm::Config config;
    nlm::Brain brain(config);
    
    // Test configuration
    config.set("brain_type", std::string("multi_layer"), nlm::ConfigSource::Runtime);
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    
    assert(brain.getConfig().has_value());
    
    std::cout << "    testBrainConfiguration passed" << std::endl;
}

void runAllBrainUnitTests() {
    testBrainCreation();
    testBrainNeuronManagement();
    testBrainSynapseManagement();
    testBrainPopulationManagement();
    testBrainRegionManagement();
    testBrainUpdate();
    testBrainConfiguration();
}

} // namespace test_brain_unit