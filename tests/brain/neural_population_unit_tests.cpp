// Unit test for NeuralPopulation
#include "brain/NeuralPopulation.hpp"
#include <cassert>
#include <iostream>

namespace test_neural_population_unit {

void testPopulationCreation() {
    nlm::NeuralPopulation population;
    
    // Test basic population properties
    assert(population.getId().has_value());
    assert(population.getNeuronCount() == 0);
    assert(population.getRegionId().has_value());
    
    std::cout << "    testPopulationCreation passed" << std::endl;
}

void testPopulationNeuronManagement() {
    nlm::NeuralPopulation population;
    
    // Create neurons
    nlm::Neuron neuron1;
    nlm::Neuron neuron2;
    nlm::Neuron neuron3;
    
    // Add neurons to population
    population.addNeuron(neuron1);
    population.addNeuron(neuron2);
    population.addNeuron(neuron3);
    
    // Check neuron count
    assert(population.getNeuronCount() == 3);
    
    // Test neuron retrieval
    auto neurons = population.getNeurons();
    assert(neurons.size() == 3);
    
    std::cout << "    testPopulationNeuronManagement passed" << std::endl;
}

void testPopulationDynamics() {
    nlm::NeuralPopulation population;
    
    // Test population dynamics
    population.update(0.001f); // Update for 1ms
    
    // Check that population state changed
    float avgPotential = population.getAverageMembranePotential();
    assert(avgPotential >= -70.0f && avgPotential <= 50.0f);
    
    std::cout << "    testPopulationDynamics passed" << std::endl;
}

void testPopulationConfiguration() {
    nlm::NeuralPopulation population;
    
    // Test population configuration
    population.setDensity(0.8f);
    population.setSpread(2.5f);
    
    assert(population.getDensity() == 0.8f);
    assert(population.getSpread() == 2.5f);
    
    std::cout << "    testPopulationConfiguration passed" << std::endl;
}

void testPopulationRegion() {
    nlm::NeuralPopulation population;
    
    // Test region association
    nlm::RegionId regionId = 1;
    population.setRegionId(regionId);
    assert(population.getRegionId() == regionId);
    
    std::cout << "    testPopulationRegion passed" << std::endl;
}

void runAllNeuralPopulationUnitTests() {
    testPopulationCreation();
    testPopulationNeuronManagement();
    testPopulationDynamics();
    testPopulationConfiguration();
    testPopulationRegion();
}

} // namespace test_neural_population_unit