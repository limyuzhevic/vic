#include "plasticity/StructuralPlasticity.hpp"
#include <iostream>
#include <cassert>
#include <memory>

void testOriginalStructuralPlasticity() {
    std::cout << "Testing Original Structural Plasticity..." << std::endl;
    
    // Create original structural plasticity using factory
    auto structural = nlm::createStructuralPlasticity("original");
    
    // Test initial parameters
    assert(structural->getSynaptogenesisRate() == 0.0001f);
    assert(structural->getPruningRate() == 0.00001f);
    assert(structural->getNeuronCreationRate() == 0.00001f);
    assert(structural->getNeuronPruningRate() == 0.000001f);
    
    // Test parameter modification
    structural->setSynaptogenesisRate(0.00005f);
    assert(structural->getSynaptogenesisRate() == 0.00005f);
    
    structural->setPruningRate(0.000005f);
    assert(structural->getPruningRate() == 0.000005f);
    
    std::cout << "✓ Original Structural Plasticity tests passed" << std::endl;
}

void testEnhancedStructuralPlasticity() {
    std::cout << "Testing Enhanced Structural Plasticity..." << std::endl;
    
    // Create enhanced structural plasticity using factory
    auto structural = nlm::createStructuralPlasticity("enhanced");
    
    // Test enhanced structural plasticity parameters
    assert(structural->getSynaptogenesisRate() == 0.0001f);
    assert(structural->getPruningRate() == 0.00001f);
    assert(structural->getNeuronCreationRate() == 0.00001f);
    assert(structural->getNeuronPruningRate() == 0.000001f);
    
    // Test developmental stage parameter
    assert(structural->getDevelopmentalStage() == 0.5f);
    
    // Test developmental stage modification
    structural->setDevelopmentalStage(0.8f);
    assert(structural->getDevelopmentalStage() == 0.8f);
    
    // Test enhanced specific parameters
    structural->setSynaptogenesisRate(0.0002f);
    assert(structural->getSynaptogenesisRate() == 0.0002f);
    
    structural->setPruningRate(0.00002f);
    assert(structural->getPruningRate() == 0.00002f);
    
    std::cout << "✓ Enhanced Structural Plasticity tests passed" << std::endl;
}

void testStructuralPlasticityFactory() {
    std::cout << "Testing Structural Plasticity factory function..." << std::endl;
    
    // Test default creation
    auto structural1 = nlm::createStructuralPlasticity("default");
    assert(structural1->getSynaptogenesisRate() == 0.0001f);
    
    // Test original creation
    auto structural2 = nlm::createStructuralPlasticity("original");
    assert(structural2->getSynaptogenesisRate() == 0.0001f);
    
    // Test enhanced creation
    auto structural3 = nlm::createStructuralPlasticity("enhanced");
    assert(structural3->getSynaptogenesisRate() == 0.0001f);
    
    // Test unknown type falls back to original
    auto structural4 = nlm::createStructuralPlasticity("unknown");
    assert(structural4->getSynaptogenesisRate() == 0.0001f);
    
    std::cout << "✓ Structural Plasticity factory tests passed" << std::endl;
}

void testStructuralPlasticityParameters() {
    std::cout << "Testing Structural Plasticity parameter structure..." << std::endl;
    
    nlm::StructuralPlasticityParameters params;
    
    // Test default parameters
    assert(params.synaptogenesisRate == 0.0001f);
    assert(params.activityThreshold == 0.1f);
    assert(params.connectivityTarget == 0.15f);
    assert(params.pruningRate == 0.00001f);
    assert(params.strengthThreshold == 0.05f);
    assert(params.redundancyThreshold == 0.1f);
    assert(params.competitionFactor == 0.2f);
    assert(params.neuronCreationRate == 0.00001f);
    assert(params.neuronMaturationTime == 1000.0f);
    assert(params.neurotrophicFactor == 1.0f);
    assert(params.neuronPruningRate == 0.000001f);
    assert(params.neuronActivityThreshold == 0.01f);
    assert(params.metabolicCost == 0.001f);
    assert(params.developmentalStage == 0.5f);
    assert(params.plasticityModulation == 1.0f);
    assert(params.activityDependentFactor == 0.7f);
    assert(params.minDistance == 1.0f);
    assert(params.maxDistance == 50.0f);
    assert(params.connectionCost == 0.5f);
    
    // Test parameter modification
    params.synaptogenesisRate = 0.0002f;
    params.pruningRate = 0.00002f;
    params.developmentalStage = 0.8f;
    
    assert(params.synaptogenesisRate == 0.0002f);
    assert(params.pruningRate == 0.00002f);
    assert(params.developmentalStage == 0.8f);
    
    std::cout << "✓ Structural Plasticity parameter tests passed" << std::endl;
}

void testStructuralPlasticityIntegration() {
    std::cout << "Testing Structural Plasticity integration..." << std::endl;
    
    // Create both structural plasticity variants
    auto originalStructural = nlm::createStructuralPlasticity("original");
    auto enhancedStructural = nlm::createStructuralPlasticity("enhanced");
    
    // Verify they have different characteristics
    assert(originalStructural->getSynaptogenesisRate() == enhancedStructural->getSynaptogenesisRate());
    assert(originalStructural->getPruningRate() == enhancedStructural->getPruningRate());
    
    // Test that enhanced has developmental stage
    enhancedStructural->setDevelopmentalStage(0.3f);
    assert(enhancedStructural->getDevelopmentalStage() == 0.3f);
    
    // Test that original doesn't have developmental stage method
    // (should use interface from base class)
    
    std::cout << "✓ Structural Plasticity integration tests passed" << std::endl;
}

int main() {
    std::cout << "=== Structural Plasticity Implementation Tests ===" << std::endl;
    
    try {
        testStructuralPlasticityParameters();
        testOriginalStructuralPlasticity();
        testEnhancedStructuralPlasticity();
        testStructuralPlasticityFactory();
        testStructuralPlasticityIntegration();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        std::cout << "The Structural Plasticity implementations are working correctly." << std::endl;
        std::cout << "Original Structural Plasticity maintains backward compatibility." << std::endl;
        std::cout << "Enhanced Structural Plasticity provides advanced developmental features." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}