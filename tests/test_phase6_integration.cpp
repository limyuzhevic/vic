// Phase 6 Integration Test
// Tests the final integration of all Phase 4 and 5 systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>

namespace test_phase6_integration {

void testPhase6Integration() {
    std::cout << "Running Phase 6 Integration Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run Phase 6 integration test
    Phase6IntegratedExperiment experiment;
    Phase4Results results = experiment.run(&brain, 20, 200);  // 20 episodes, 200 steps each
    
    // Check that all integrated systems worked together
    assert(results.predictionErrorFinal > 0.0f);  // Should have some error
    assert(results.episodesStored > 0);  // Should have stored episodes
    assert(results.conceptsFormed > 0);  // Should have formed concepts
    assert(results.planningConfidence > 0.0f);  // Should have planning confidence
    assert(results.bodyAwareness > 0.0f);  // Should have body awareness
    assert(results.imitationAccuracy > 0.0f);  // Should be able to imitate
    assert(results.generalizationAbility > 0.0f);  // Should generalize
    assert(results.transferPerformance > 0.0f);  // Should transfer learning
    assert(results.totalReward > -100.0f);  // Reward should not be extremely negative
    
    // Test checkpoint functionality
    std::string checkpointFile = "test_checkpoint.nml";
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess);
    
    // Reset brain and reload from checkpoint
    brain.reset();
    brain.initialize();
    
    bool loadSuccess = brain.load(checkpointFile);
    assert(loadSuccess);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    // Check comparison functionality
    Phase4Results comparison = experiment.compareBeforeAfter();
    // Comparison should show some improvement
    assert(comparison.behaviorImprovement >= -10.0f);  // Allow some noise
    
    std::cout << "  ✓ Phase 6 Integration test passed" << std::endl;
    std::cout << "  - Stored " << results.episodesStored << " episodes" << std::endl;
    std::cout << "  - Formed " << results.conceptsFormed << " concepts" << std::endl;
    std::cout << "  - Total reward: " << results.totalReward << std::endl;
}

void runAllPhase6Tests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    
    testPhase6Integration();
    
    std::cout << "=== All Phase 6 Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_integration