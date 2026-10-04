// Unit test for NeuralRegion
#include "brain/NeuralRegion.hpp"
#include <cassert>
#include <iostream>

namespace test_neural_region_unit {

void testRegionCreation() {
    nlm::NeuralRegion region;
    
    // Test basic region properties
    assert(region.getId().has_value());
    assert(region.getPopulationCount() == 0);
    assert(region.getNeuronCount() == 0);
    assert(region.getSize() > 0.0f);
    
    std::cout << "    testRegionCreation passed" << std::endl;
}

void testRegionPopulationManagement() {
    nlm::NeuralRegion region;
    
    // Create populations
    nlm::NeuralPopulation pop1;
    nlm::NeuralPopulation pop2;
    
    // Add populations to region
    region.addPopulation(pop1);
    region.addPopulation(pop2);
    
    // Check population count
    assert(region.getPopulationCount() == 2);
    
    // Test population retrieval
    auto populations = region.getPopulations();
    assert(populations.size() == 2);
    
    std::cout << "    testRegionPopulationManagement passed" << std::endl;
}

void testRegionNeuronManagement() {
    nlm::NeuralRegion region;
    
    // Create neurons
    nlm::Neuron neuron1;
    nlm::Neuron neuron2;
    
    // Add neurons to region
    region.addNeuron(neuron1);
    region.addNeuron(neuron2);
    
    // Check neuron count
    assert(region.getNeuronCount() == 2);
    
    // Test neuron retrieval
    auto neurons = region.getNeurons();
    assert(neurons.size() == 2);
    
    std::cout << "    testRegionNeuronManagement passed" << std::endl;
}

void testRegionGeometry() {
    nlm::NeuralRegion region;
    
    // Test region geometry
    nlm::Vector3 center(1.0f, 2.0f, 3.0f);
    region.setCenter(center);
    assert(region.getCenter() == center);
    
    float size = 10.0f;
    region.setSize(size);
    assert(region.getSize() == size);
    
    std::cout << "    testRegionGeometry passed" << std::endl;
}

void testRegionDynamics() {
    nlm::NeuralRegion region;
    
    // Test region dynamics
    region.update(0.001f); // Update for 1ms
    
    // Check that region state changed
    float avgActivity = region.getAverageActivity();
    assert(avgActivity >= 0.0f && avgActivity <= 1.0f);
    
    std::cout << "    testRegionDynamics passed" << std::endl;
}

void runAllNeuralRegionUnitTests() {
    testRegionCreation();
    testRegionPopulationManagement();
    testRegionNeuronManagement();
    testRegionGeometry();
    testRegionDynamics();
}

} // namespace test_neural_region_unit