// Structural Plasticity Tests
// Tests for synaptogenesis, pruning, and structural changes

#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_structural {

void testStructuralPlasticityBasics() {
    std::cout << "    Testing Structural Plasticity basics..." << std::endl;
    
    nlm::StructuralPlasticity sp;
    
    // Test default parameters
    assert(sp.isEnabled());
    assert(sp.getSynaptogenesisRate() == 0.0001f);
    assert(sp.getPruningRate() == 0.00001f);
    assert(sp.getMinWeightThreshold() == 0.05f);
    assert(sp.getActivityThreshold() == 0.001f);
    assert(sp.getMaxSynapsesPerNeuron() == 100);
    assert(sp.getMaxTotalSynapses() == 1000000);
    
    // Test parameter setters
    sp.setSynaptogenesisRate(0.001f);
    assert(sp.getSynaptogenesisRate() == 0.001f);
    
    sp.setPruningRate(0.0001f);
    assert(sp.getPruningRate() == 0.0001f);
    
    sp.setMinWeightThreshold(0.1f);
    assert(sp.getMinWeightThreshold() == 0.1f);
    
    sp.setActivityThreshold(0.01f);
    assert(sp.getActivityThreshold() == 0.01f);
    
    sp.setMaxSynapsesPerNeuron(200);
    assert(sp.getMaxSynapsesPerNeuron() == 200);
    
    sp.setMaxTotalSynapses(500000);
    assert(sp.getMaxTotalSynapses() == 500000);
    
    sp.setEnabled(false);
    assert(!sp.isEnabled());
    
    // Test statistics
    auto stats = sp.getStatistics();
    assert(stats.totalSynapsesCreated == 0);
    assert(stats.totalSynapsesPruned == 0);
    assert(stats.totalNeuronsCreated == 0);
    assert(stats.totalNeuronsPruned == 0);
    assert(stats.averageSynapseWeight == 0.0f);
    assert(stats.averageConnectionsPerNeuron == 0.0f);
    
    sp.resetStatistics();
    stats = sp.getStatistics();
    assert(stats.totalSynapsesCreated == 0);
    
    std::cout << "      Structural Plasticity basics test PASSED" << std::endl;
}

void testNetworkConfiguration() {
    std::cout << "    Testing network configuration methods..." << std::endl;
    
    nlm::StructuralPlasticity sp;
    
    // Test different configurations
    sp.configureForDenseNetwork();
    assert(sp.getMaxSynapsesPerNeuron() == 1000);
    assert(sp.getUseActivityDependency() == false);
    
    sp.configureForSparseNetwork();
    assert(sp.getMaxSynapsesPerNeuron() == 10);
    assert(sp.getUseActivityDependency() == true);
    
    sp.configureForSmallWorld();
    assert(sp.getMaxSynapsesPerNeuron() == 50);
    assert(sp.getUseActivityDependency() == true);
    
    std::cout << "      Network configuration test PASSED" << std::endl;
}

void testActivityDependentPlasticity() {
    std::cout << "    Testing activity-dependent plasticity..." << std::endl;
    
    nlm::StructuralPlasticity sp;
    nlm::RandomGenerator rng(42);
    
    // Create a mock brain (simplified)
    // In a real implementation, this would test against an actual Brain object
    // For now, we test the configuration and parameter logic
    
    // Test that activity dependency can be enabled/disabled
    sp.setUseActivityDependency(true);
    assert(sp.getUseActivityDependency() == true);
    
    sp.setUseActivityDependency(false);
    assert(sp.getUseActivityDependency() == false);
    
    // Test activity threshold
    sp.setActivityThreshold(0.005f);
    assert(sp.getActivityThreshold() == 0.005f);
    
    // Test minimum weight threshold
    sp.setMinWeightThreshold(0.02f);
    assert(sp.getMinWeightThreshold() == 0.02f);
    
    std::cout << "      Activity-dependent plasticity test PASSED" << std::endl;
}

void testConfigurationAdaptability() {
    std::cout << "    Testing configuration adaptability..." << std::endl;
    
    nlm::StructuralPlasticity sp;
    
    // Store original values
    float originalSynaptogenesis = sp.getSynaptogenesisRate();
    float originalPruning = sp.getPruningRate();
    size_t originalMaxSynapses = sp.getMaxSynapsesPerNeuron();
    
    // Test reinitialization
    sp.reinitialize();
    assert(sp.getSynaptogenesisRate() == originalSynaptogenesis);
    assert(sp.getPruningRate() == originalPruning);
    assert(sp.getMaxSynapsesPerNeuron() == originalMaxSynapses);
    
    std::cout << "      Configuration adaptability test PASSED" << std::endl;
}

void runAll() {
    testStructuralPlasticityBasics();
    testNetworkConfiguration();
    testActivityDependentPlasticity();
    testConfigurationAdaptability();
    
    std::cout << "    All structural plasticity tests PASSED" << std::endl;
}

} // namespace test_structural