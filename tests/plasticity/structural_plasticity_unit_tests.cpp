// Unit test for StructuralPlasticity
#include "plasticity/StructuralPlasticity.hpp"
#include <cassert>
#include <iostream>

namespace test_structural_plasticity_unit {

void testStructuralPlasticityCreation() {
    nlm::StructuralPlasticity plasticity;
    
    // Test basic structural plasticity properties
    assert(plasticity.getGrowthRate() > 0.0f);
    assert(plasticity.getPruningRate() > 0.0f);
    assert(plasticity.getMaxConnections() > 0);
    
    std::cout << "    testStructuralPlasticityCreation passed" << std::endl;
}

void testStructuralPlasticityGrowth() {
    nlm::StructuralPlasticity plasticity;
    
    // Test structural plasticity growth
    nlm::NeuronId neuronId = 1;
    float growthSignal = 0.5f;
    
    plasticity.applyGrowth(neuronId, growthSignal);
    
    std::cout << "    testStructuralPlasticityGrowth passed" << std::endl;
}

void testStructuralPlasticityPruning() {
    nlm::StructuralPlasticity plasticity;
    
    // Test structural plasticity pruning
    nlm::NeuronId neuronId = 1;
    float activity = 0.1f;
    
    plasticity.applyPruning(neuronId, activity);
    
    std::cout << "    testStructuralPlasticityPruning passed" << std::endl;
}

void testStructuralPlasticityParameters() {
    nlm::StructuralPlasticity plasticity;
    
    // Test structural plasticity parameters
    plasticity.setGrowthRate(0.01f);
    plasticity.setPruningRate(0.005f);
    plasticity.setMaxConnections(1000);
    
    assert(plasticity.getGrowthRate() == 0.01f);
    assert(plasticity.getPruningRate() == 0.005f);
    assert(plasticity.getMaxConnections() == 1000);
    
    std::cout << "    testStructuralPlasticityParameters passed" << std::endl;
}

void runAllStructuralPlasticityUnitTests() {
    testStructuralPlasticityCreation();
    testStructuralPlasticityGrowth();
    testStructuralPlasticityPruning();
    testStructuralPlasticityParameters();
}

} // namespace test_structural_plasticity_unit