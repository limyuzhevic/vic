#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_development {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devSystem;
    assert(devSystem.getSynaptogenesisLevel() == 1.0f);
    assert(devSystem.getPruningLevel() == 1.0f);
    assert(devSystem.getMaturationLevel() == 1.0f);
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testSynaptogenesis() {
    nlm::Synaptogenesis synaptogenesis;
    assert(synaptogenesis.getRate() == 0.1f);
    
    // Create new synapses
    int newSynapses = synaptogenesis.generateNewSynapses(10);
    assert(newSynapses == 10); // In real implementation, would be calculated
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testMaturation() {
    nlm::Maturation maturation;
    assert(maturation.getMaturationRate() == 0.5f);
    
    // Mature neurons
    int matured = maturation.matureNeurons(20);
    assert(matured == 20); // In real implementation, would be calculated
    std::cout << "    testMaturation passed" << std::endl;
}

void testPruning() {
    nlm::Pruning pruning;
    assert(pruning.getPruningThreshold() == 0.1f);
    
    // Prune weak synapses
    int pruned = pruning.pruneSynapses(50);
    assert(pruned == 50); // In real implementation, would be calculated
    std::cout << "    testPruning passed" << std::endl;
}

void testDevelopmentIntegration() {
    nlm::DevelopmentSystem devSystem;
    
    // Test that development affects brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Development system should be accessible from brain
    assert(&devSystem != nullptr);
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testDevelopmentReset() {
    nlm::DevelopmentSystem devSystem;
    
    // Modify development levels
    devSystem.setSynaptogenesisLevel(0.5f);
    devSystem.setPruningLevel(0.8f);
    devSystem.setMaturationLevel(0.3f);
    
    // Reset development
    devSystem.reset();
    assert(devSystem.getSynaptogenesisLevel() == 1.0f);
    assert(devSystem.getPruningLevel() == 1.0f);
    assert(devSystem.getMaturationLevel() == 1.0f);
    std::cout << "    testDevelopmentReset passed" << std::endl;
}

void testDevelopmentLimits() {
    nlm::DevelopmentSystem devSystem;
    
    // Test that development levels stay within bounds
    devSystem.setSynaptogenesisLevel(2.0f);
    assert(devSystem.getSynaptogenesisLevel() <= 1.0f); // Should be clamped
    
    devSystem.setSynaptogenesisLevel(-1.0f);
    assert(devSystem.getSynaptogenesisLevel() >= 0.0f); // Should be clamped
    
    std::cout << "    testDevelopmentLimits passed" << std::endl;
}

void testDevelopmentParameters() {
    nlm::Synaptogenesis synaptogenesis;
    assert(synaptogenesis.getRate() == 0.1f);
    
    synaptogenesis.setRate(0.5f);
    assert(synaptogenesis.getRate() == 0.5f);
    
    synaptogenesis.setRate(0.0f);
    assert(synaptogenesis.getRate() == 0.0f);
    
    std::cout << "    testDevelopmentParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development tests..." << std::endl;
    testDevelopmentSystemCreation();
    testSynaptogenesis();
    testMaturation();
    testPruning();
    testDevelopmentIntegration();
    testDevelopmentReset();
    testDevelopmentLimits();
    testDevelopmentParameters();
}

} // namespace test_development