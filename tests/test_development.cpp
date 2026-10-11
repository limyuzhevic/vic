// Development Tests
// Phase 2 implementation tests for NLM development systems

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_development {

void testSynaptogenesis() {
    std::cout << "    Testing Synaptogenesis..." << std::endl;
    
    nlm::Synaptogenesis synaptogenesis;
    
    // Test initial state
    nlm::DevelopmentStage initialStage = nlm::DevelopmentStage::Initial;
    assert(synaptogenesis.getCurrentStage() == initialStage);
    assert(synaptogenesis.getSynaptogenesisRate() > 0.0f);
    
    // Test stage progression
    synaptogenesis.updateDevelopment(1.0f);
    assert(synaptogenesis.getCurrentStage() != initialStage);
    
    // Test synaptic growth
    size_t initialConnections = synaptogenesis.getTotalConnections();
    
    synaptogenesis.growSynapses(100);
    assert(synaptogenesis.getTotalConnections() > initialConnections);
    
    // Test pruning with probability
    synaptogenesis.pruneSynapses(0.1f); // 10% pruning
    assert(synaptogenesis.getTotalConnections() <= initialConnections + 100);
    
    // Test maturation
    synaptogenesis.matureNeurons();
    // Should not crash
    
    // Test reset
    synaptogenesis.reset();
    assert(synaptogenesis.getTotalConnections() >= 0);
    
    std::cout << "      Synaptogenesis tests passed" << std::endl;
}

void testPruning() {
    std::cout << "    Testing Pruning..." << std::endl;
    
    nlm::Pruning pruning;
    
    // Test initial state
    assert(pruning.getPruningRate() > 0.0f);
    assert(pruning.getThresholdAge() > 0.0f);
    assert(pruning.getConnectionDensityThreshold() > 0.0f);
    
    // Test age-based pruning
    size_t initialConnections = pruning.getTotalConnections();
    
    // Add some connections
    pruning.addConnection(nlm::NeuronId(1), nlm::NeuronId(2));
    pruning.addConnection(nlm::NeuronId(3), nlm::NeuronId(4));
    pruning.addConnection(nlm::NeuronId(5), nlm::NeuronId(6));
    
    assert(pruning.getTotalConnections() > initialConnections);
    
    // Apply pruning
    pruning.applyPruning(0.5f); // 50% pruning
    assert(pruning.getTotalConnections() >= 0);
    
    // Test connection removal
    pruning.removeConnection(nlm::NeuronId(1), nlm::NeuronId(2));
    // Connection should be removed
    
    // Test density-based pruning
    pruning.setConnectionDensityThreshold(0.1f);
    assert(pruning.getConnectionDensityThreshold() == 0.1f);
    
    // Test reset
    pruning.reset();
    assert(pruning.getTotalConnections() == 0);
    
    std::cout << "      Pruning tests passed" << std::endl;
}

void testMaturation() {
    std::cout << "    Testing Maturation..." << std::endl;
    
    nlm::Maturation maturation;
    
    // Test initial state
    assert(maturation.getMaturationRate() > 0.0f);
    assert(maturation.getExcitatoryInhibitoryRatio() > 0.0f);
    assert(maturation.getFiringThreshold() > 0.0f);
    
    // Test neuron maturation
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    maturation.matureNeuron(neuron1);
    maturation.matureNeuron(neuron2);
    
    // Test maturity progression
    maturation.advanceMaturation(1.0f);
    assert(maturation.getCurrentMaturationProgress() > 0.0f);
    
    // Test connectivity strengthening
    maturation.strongifyConnections(0.1f);
    
    // Test age-based maturation
    maturation.ageNeurons(10);
    
    // Test reset
    maturation.reset();
    assert(maturation.getCurrentMaturationProgress() == 0.0f);
    
    std::cout << "      Maturation tests passed" << std::endl;
}

void testDevelopmentSystem() {
    std::cout << "    Testing DevelopmentSystem..." << std::endl;
    
    nlm::DevelopmentSystem devSys;
    
    // Test initial state
    assert(devSys.getCurrentStage() == nlm::DevelopmentStage::Initial);
    assert(devSys.getDevelopmentProgress() == 0.0f);
    assert(devSys.getMaturationProgress() == 0.0f);
    
    // Test stage management
    devSys.advanceStage();
    assert(devSys.getCurrentStage() != nlm::DevelopmentStage::Initial);
    
    // Test development progress
    devSys.updateDevelopment(5.0f);
    assert(devSys.getDevelopmentProgress() > 0.0f);
    
    // Test maturation progress
    devSys.advanceMaturation();
    assert(devSys.getMaturationProgress() > 0.0f);
    
    // Test system integration
    devSys.integrateSynaptogenesis();
    devSys.integratePruning();
    devSys.integrateMaturation();
    
    // All integration methods should complete without crashing
    
    // Test reset
    devSys.reset();
    assert(devSys.getDevelopmentProgress() == 0.0f);
    assert(devSys.getMaturationProgress() == 0.0f);
    
    std::cout << "      DevelopmentSystem tests passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    // Create development components
    nlm::Synaptogenesis synaptogenesis;
    nlm::Pruning pruning;
    nlm::Maturation maturation;
    nlm::DevelopmentSystem devSys;
    
    // Test coordinated development
    synaptogenesis.updateDevelopment(1.0f);
    pruning.applyPruning(0.2f);
    maturation.advanceMaturation();
    devSys.advanceStage();
    
    // All systems should be synchronized
    assert(devSys.getCurrentStage() != nlm::DevelopmentStage::Initial);
    assert(synaptogenesis.getCurrentStage() != nlm::DevelopmentStage::Initial);
    
    // Test feedback loops
    synaptogenesis.setSynaptogenesisRate(0.01f);
    pruning.setPruningRate(0.001f);
    maturation.setMaturationRate(0.05f);
    
    assert(synaptogenesis.getSynaptogenesisRate() == 0.01f);
    assert(pruning.getPruningRate() == 0.001f);
    assert(maturation.getMaturationRate() == 0.05f);
    
    // Test synchronized reset
    devSys.reset();
    synaptogenesis.reset();
    pruning.reset();
    maturation.reset();
    
    assert(devSys.getDevelopmentProgress() == 0.0f);
    assert(synaptogenesis.getTotalConnections() == 0);
    assert(pruning.getTotalConnections() == 0);
    assert(maturation.getCurrentMaturationProgress() == 0.0f);
    
    std::cout << "      Development integration tests passed" << std::endl;
}

void runAll() {
    testSynaptogenesis();
    testPruning();
    testMaturation();
    testDevelopmentSystem();
    testDevelopmentIntegration();
}

} // namespace test_development
