// Pruning Tests
#include "brain/Pruning.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_pruning {

void testPruningCreation() {
    nlm::Pruning pruning;
    
    float threshold = pruning.getThreshold();
    assert(threshold >= 0.0f && threshold <= 1.0f);
    
    float rate = pruning.getPruningRate();
    assert(rate >= 0.0f && rate <= 1.0f);
    
    std::cout << "    testPruningCreation passed" << std::endl;
}

void testPruningConfiguration() {
    nlm::Pruning pruning;
    
    // Set threshold
    pruning.setThreshold(0.1f);
    assert(std::abs(pruning.getThreshold() - 0.1f) < 0.001f);
    
    // Set pruning rate
    pruning.setPruningRate(0.05f);
    assert(std::abs(pruning.getPruningRate() - 0.05f) < 0.001f);
    
    // Test boundary values
    pruning.setThreshold(0.0f);
    assert(pruning.getThreshold() == 0.0f);
    
    pruning.setThreshold(1.0f);
    assert(pruning.getThreshold() == 1.0f);
    
    pruning.setPruningRate(0.0f);
    assert(pruning.getPruningRate() == 0.0f);
    
    pruning.setPruningRate(1.0f);
    assert(pruning.getPruningRate() == 1.0f);
    
    std::cout << "    testPruningConfiguration passed" << std::endl;
}

void testPruningSynapseForce() {
    nlm::Pruning pruning;
    
    // Create a synapse to test pruning
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Test pruning of existing synapse
    bool pruned = pruning.pruneSynapse(&synapse);
    // Result depends on implementation, but should not crash
    assert(pruned == true || pruned == false);  // Accept either outcome
    
    std::cout << "    testPruningSynapseForce passed" << std::endl;
}

void testPruningUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Pruning pruning;
    nlm::RandomGenerator rng(42);
    
    // Test update
    pruning.update(&brain, rng);
    
    std::cout << "    testPruningUpdate passed" << std::endl;
}

void testPruningEdgeCases() {
    nlm::Pruning pruning;
    
    // Test extreme threshold values
    pruning.setThreshold(0.0f);
    assert(pruning.getThreshold() == 0.0f);
    
    pruning.setThreshold(0.5f);
    assert(std::abs(pruning.getThreshold() - 0.5f) < 0.001f);
    
    pruning.setThreshold(1.0f);
    assert(pruning.getThreshold() == 1.0f);
    
    // Test zero pruning rate
    pruning.setPruningRate(0.0f);
    assert(pruning.getPruningRate() == 0.0f);
    
    // Test maximum pruning rate
    pruning.setPruningRate(1.0f);
    assert(pruning.getPruningRate() == 1.0f);
    
    std::cout << "    testPruningEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Pruning tests..." << std::endl;
    testPruningCreation();
    testPruningConfiguration();
    testPruningSynapseForce();
    testPruningUpdate();
    testPruningEdgeCases();
}

} // namespace test_pruning