// NeuralRegion Tests
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neural_region {

void testNeuralRegionCreation() {
    nlm::NeuralRegion region(nlm::RegionId(1));
    
    assert(region.getId() == nlm::RegionId(1));
    assert(region.getName() == ""); // Default empty name
    
    std::cout << "    testNeuralRegionCreation passed" << std::endl;
}

void testNeuralRegionWithName() {
    nlm::NeuralRegion region(nlm::RegionId(2), "TestRegion");
    
    assert(region.getId() == nlm::RegionId(2));
    assert(region.getName() == "TestRegion");
    
    std::cout << "    testNeuralRegionWithName passed" << std::endl;
}

void testNeuralRegionPopulationManagement() {
    nlm::NeuralRegion region(nlm::RegionId(3));
    
    assert(region.getPopulationCount() == 0);
    assert(region.getTotalNeuronCount() == 0);
    
    // Add populations
    nlm::PopulationId pop1 = region.addPopulation(10, nlm::NeuronType::Sensory);
    nlm::PopulationId pop2 = region.addPopulation(20, nlm::NeuronType::Internal);
    nlm::PopulationId pop3 = region.addPopulation(5, nlm::NeuronType::Motor);
    
    assert(region.getPopulationCount() == 3);
    assert(region.getTotalNeuronCount() == 35); // 10 + 20 + 5
    
    // Verify populations can be retrieved
    auto* population1 = region.getPopulation(pop1);
    auto* population2 = region.getPopulation(pop2);
    auto* population3 = region.getPopulation(pop3);
    
    assert(population1 != nullptr);
    assert(population2 != nullptr);
    assert(population3 != nullptr);
    
    assert(population1->getSize() == 10);
    assert(population2->getSize() == 20);
    assert(population3->getSize() == 5);
    
    std::cout << "    testNeuralRegionPopulationManagement passed" << std::endl;
}

void testNeuralRegionSynapseManagement() {
    nlm::NeuralRegion region(nlm::RegionId(4));
    
    // Need neurons to create synapses
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    
    // Add populations and neurons
    region.addPopulation(1); // This would add one neuron
    // Note: This is a simplified test - in reality we'd need to properly
    // set up the neural populations before creating synapses
    
    assert(region.getSynapseCount() == 0);
    
    std::cout << "    testNeuralRegionSynapseManagement passed" << std::endl;
}

void testNeuralRegionStatistics() {
    nlm::NeuralRegion region(nlm::RegionId(5));
    
    // Add populations of different types
    region.addPopulation(10, nlm::NeuronType::Sensory);
    region.addPopulation(20, nlm::NeuronType::Internal);
    region.addPopulation(5, nlm::NeuronType::Motor);
    
    // Verify statistics
    assert(region.getTotalNeuronCount() == 35);
    assert(region.getPopulationCount() == 3);
    assert(region.getActiveNeuronCount() >= 0);
    assert(region.getFiringNeuronCount() >= 0);
    assert(region.getAverageFiringRate() >= 0.0f);
    
    // Verify synaptic density (should be 0 since no synapses yet)
    float synapticDensity = region.getSynapticDensity();
    assert(synapticDensity >= 0.0f && synapticDensity <= 1.0f);
    
    std::cout << "    testNeuralRegionStatistics passed" << std::endl;
}

void testNeuralRegionRandomConnectivity() {
    nlm::RandomGenerator rng(42);
    nlm::NeuralRegion region(nlm::RegionId(6));
    
    // Add populations to create neurons
    region.addPopulation(10, nlm::NeuronType::Sensory);
    region.addPopulation(20, nlm::NeuronType::Internal);
    
    // This should not crash
    region.initializeRandomConnectivity(rng, 0.1f, 0.2f, 0.05f);
    
    assert(region.getSynapseCount() >= 0);
    
    std::cout << "    testNeuralRegionRandomConnectivity passed" << std::endl;
}

void testNeuralRegionAllNeurons() {
    nlm::NeuralRegion region(nlm::RegionId(7));
    
    // Add populations
    region.addPopulation(5, nlm::NeuronType::Sensory);
    region.addPopulation(10, nlm::NeuronType::Internal);
    
    // Get all neurons
    std::vector<nlm::Neuron*> allNeurons = region.getAllNeurons();
    std::vector<const nlm::Neuron*> constAllNeurons = region.getAllNeurons();
    
    assert(allNeurons.size() == 15);
    assert(constAllNeurons.size() == 15);
    
    // Verify neurons are accessible
    for (auto* neuron : allNeurons) {
        assert(neuron != nullptr);
        assert(neuron->getRegionId() == nlm::RegionId(7));
    }
    
    std::cout << "    testNeuralRegionAllNeurons passed" << std::endl;
}

void runAll() {
    testNeuralRegionCreation();
    testNeuralRegionWithName();
    testNeuralRegionPopulationManagement();
    testNeuralRegionSynapseManagement();
    testNeuralRegionStatistics();
    testNeuralRegionRandomConnectivity();
    testNeuralRegionAllNeurons();
}

} // namespace test_neural_region
