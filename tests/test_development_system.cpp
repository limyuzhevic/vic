// Test for Development System Integration (DevelopmentalStage)
// Covers requirement: 5. Development System Tests

#include "development/DevelopmentSystem.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_development_system {

void testDevelopmentSystemBasic() {
    nlm::DevelopmentSystem development;
    
    // Test initial state
    assert(development.getCurrentStage() == nlm::DevelopmentalStage::Infant);
    assert(development.getDevelopmentTime() == 0.0f);
    assert(development.getMaturationLevel() == 0.0f);
    
    std::cout << "    testDevelopmentSystemBasic passed" << std::endl;
}

void testDevelopmentStageTransitions() {
    nlm::DevelopmentSystem development;
    
    // Set development time to transition stages
    development.setDevelopmentTime(10.0f);
    nlm::DevelopmentalStage stage1 = development.getCurrentStage();
    
    development.setDevelopmentTime(50.0f);
    nlm::DevelopmentalStage stage2 = development.getCurrentStage();
    
    // Stages should have progressed
    assert(stage1 != stage2); // Different stages
    
    std::cout << "    testDevelopmentStageTransitions passed" << std::endl;
}

void testMaturationProcess() {
    nlm::Maturation maturation;
    
    // Test initial state
    assert(maturation.getMaturationLevel() == 0.0f);
    assert(maturation.getMaturationRate() == 1.0f); // Default rate
    
    // Update maturation
    maturation.updateMaturation(0.1f);
    assert(maturation.getMaturationLevel() == 0.1f);
    
    // Test maturation effect on neurons
    nlm::Neuron neuron(nlm::NeuronId(1));
    maturation.applyEffect(neuron);
    
    // Neuron properties may have changed due to maturation
    assert(neuron.getThreshold() != 0.0f); // Should have been modified
    
    std::cout << "    testMaturationProcess passed" << std::endl;
}

void testSynaptogenesis() {
    nlm::Synaptogenesis synaptogenesis;
    
    // Test initial state
    assert(synaptogenesis.getSynapticDensity() == 0.0f);
    assert(synaptogenesis.getGrowthRate() == 1.0f); // Default rate
    
    // Generate new synapses
    synaptogenesis.generateSynapses(10, 0.5f);
    assert(synaptogenesis.getSynapticDensity() > 0.0f);
    
    // Prune synapses
    synaptogenesis.pruneSynapses(0.1f, 0.3f);
    assert(synaptogenesis.getSynapticDensity() >= 0.0f);
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruningProcess() {
    nlm::Pruning pruning;
    
    // Test initial state
    assert(pruning.getPruningRate() == 0.5f); // Default rate
    assert(pruning.getPruningThreshold() == 0.1f);
    
    // Test effect on synapses
    float initialDensity = pruning.getCurrentSynapticDensity();
    pruning.applyPruning(0.2f);
    float newDensity = pruning.getCurrentSynapticDensity();
    
    // Density should decrease or stay the same
    assert(newDensity <= initialDensity);
    
    std::cout << "    testPruningProcess passed" << std::endl;
}

void testDevelopmentIntegration() {
    nlm::DevelopmentSystem development;
    nlm::Maturation maturation;
    
    // Development affects maturation rate
    development.setDevelopmentTime(25.0f); // Should be young adult stage
    float maturityRate = maturation.getMaturationRate();
    
    // Maturation rate should be affected by development stage
    assert(maturityRate > 0.0f);
    
    // Test synaptic development
    nlm::Synaptogenesis synaptogenesis;
    synaptogenesis.generateSynapses(5, 0.3f);
    
    // Pruning should balance synaptogenesis
    nlm::Pruning pruning;
    pruning.applyPruning(0.1f);
    
    // Net synaptic density should be reasonable
    float netDensity = synaptogenesis.getSynapticDensity() - pruning.getCurrentSynapticDensity();
    assert(netDensity >= 0.0f);
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void runAll() {
    testDevelopmentSystemBasic();
    testDevelopmentStageTransitions();
    testMaturationProcess();
    testSynaptogenesis();
    testPruningProcess();
    testDevelopmentIntegration();
}

} // namespace test_development_system
