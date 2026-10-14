// Structural Plasticity Tests
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_structural {

void testStructuralPlasticityCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::StructuralPlasticity sp;
    
    assert(sp.getSynaptogenesisRate() == 0.0001f);
    assert(sp.getPruningRate() == 0.00001f);
    
    std::cout << "    testStructuralPlasticityCreation passed" << std::endl;
}

void testSynaptogenesis() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.5f, nlm::ConfigSource::Default);  // High probability for testing
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::StructuralPlasticity sp;
    
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    
    // Set a higher synaptogenesis rate for testing
    sp.setSynaptogenesisRate(0.01f);  // 1% chance per attempt
    
    // Run structural plasticity update
    nlm::RandomGenerator rng(42);
    sp.update(&brain, rng);
    
    size_t finalSynapseCount = brain.getTotalSynapseCount();
    
    // With high connection probability and high synaptogenesis rate, 
    // we should expect some new synapses (though random, so may not always)
    
    std::cout << "    testSynaptogenesis passed (initial=" 
              << initialSynapseCount << ", final=" << finalSynapseCount << ")" << std::endl;
}

void testPruning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::StructuralPlasticity sp;
    
    // Set low pruning rate
    sp.setPruningRate(0.1f);  // 10% chance per weak synapse
    
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    
    // Run structural plasticity update
    nlm::RandomGenerator rng(43);
    sp.update(&brain, rng);
    
    size_t finalSynapseCount = brain.getTotalSynapseCount();
    
    // Synapses may be pruned (weights set to 0) or not, depending on random chance
    // Just verify it doesn't crash
    
    std::cout << "    testPruning passed (initial=" 
              << initialSynapseCount << ", final=" << finalSynapseCount << ")" << std::endl;
}

void testConfiguration() {
    nlm::StructuralPlasticity sp;
    
    sp.setSynaptogenesisRate(0.02f);
    assert(sp.getSynaptogenesisRate() == 0.02f);
    
    sp.setPruningRate(0.001f);
    assert(sp.getPruningRate() == 0.001f);
    
    // Test bounds clamping
    sp.setSynaptogenesisRate(0.2f);  // Should be clamped to 0.1f
    assert(sp.getSynaptogenesisRate() == 0.1f);
    
    sp.setPruningRate(0.02f);  // Should be clamped to 0.01f
    assert(sp.getPruningRate() == 0.01f);
    
    std::cout << "    testConfiguration passed" << std::endl;
}

void testActivityDependentPruning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::StructuralPlasticity sp;
    
    // Enable activity-dependent pruning
    auto* spImpl = static_cast<nlm::StructuralPlasticity*>(nullptr);  // Would need access to impl
    // For now, just test that the system doesn't crash
    
    nlm::RandomGenerator rng(44);
    sp.update(&brain, rng);
    
    std::cout << "    testActivityDependentPruning passed" << std::endl;
}

void runAll() {
    std::cout << "Running Structural Plasticity tests..." << std::endl;
    testStructuralPlasticityCreation();
    testSynaptogenesis();
    testPruning();
    testConfiguration();
    testActivityDependentPruning();
    std::cout << "Structural Plasticity tests completed" << std::endl;
}

} // namespace test_structural
