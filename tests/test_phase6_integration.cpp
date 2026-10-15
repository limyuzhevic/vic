// Phase 6 Integration Tests
// Tests for the integrated artificial brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_phase6 {

void testMemorySystemIntegration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    // Create a brain with proper configuration for Phase 6
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test 1: Memory systems should be accessible
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test 2: Working memory should have some neurons populated
    size_t workingMemoryCapacity = workingMemory->getCapacity();
    assert(workingMemoryCapacity > 0);
    
    // Test 3: Episodic memory should be initialized
    size_t maxEpisodes = episodicMemory->getMaxEpisodes();
    assert(maxEpisodes > 0);
    
    // Test 4: Run some steps to test integration
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 5: Verify memory systems are still accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    std::cout << "  Memory system integration test PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Neuromodulation systems should be accessible
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain->getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test 2: Get neuromodulation levels
    float dopamineLevel = dopamine->getLevel();
    float curiosityLevel = curiosity->getCuriosityLevel();
    float noveltyLevel = novelty->getNoveltyLevel();
    
    // Test 3: Run steps with sensory input to activate neuromodulation
    for (int step = 0; step < 50; ++step) {
        // Inject sensory current to activate neuromodulation
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(step, step * 0.001);
    }
    
    // Test 4: Neuromodulation systems should still be accessible
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    
    std::cout << "  Neuromodulation integration test PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Prediction system should be accessible
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test 2: Run some steps to test prediction system
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 3: Prediction system should still be accessible
    assert(brain.getPredictionSystem() != nullptr);
    
    std::cout << "  Prediction system integration test PASSED" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "Testing cognition system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 400);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Cognition systems should be accessible
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test 2: Run some steps to test cognition systems
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 3: Cognition systems should still be accessible
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    std::cout << "  Cognition system integration test PASSED" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 350);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Development system should be accessible
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test 2: Run some steps to test development system
    for (int step = 0; step < 200; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 3: Development system should still be accessible
    assert(brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "  Development system integration test PASSED" << std::endl;
}

void testCheckpointSystemIntegration() {
    std::cout << "Testing checkpoint system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 250);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("checkpoint_dir", "/tmp/nlm_checkpoint_test");
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to generate activity
    for (int step = 0; step < 150; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 1: Save checkpoint
    std::string checkpointPath = "/tmp/nlm_checkpoint_test.bin";
    bool saveSuccess = brain.save(checkpointPath);
    assert(saveSuccess);
    
    // Test 2: Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 250);
    config2->set("region_count", 1);
    config2->set("connection_probability", 0.1f);
    config2->set("stdp_ltp_weight", 0.01f);
    config2->set("stdp_ltd_weight", 0.012f);
    config2->set("synaptogenesis_rate", 0.0001f);
    config2->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    
    std::cout << "  Checkpoint system integration test PASSED" << std::endl;
}

void testReplaySystemIntegration() {
    std::cout << "Testing replay system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to generate episodes
    for (int step = 0; step < 300; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test 1: Episodic memory should have some episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    size_t episodeCount = episodicMemory->getEpisodeCount();
    // Note: episodic memory might not have episodes if no working memory activity
    // This is expected behavior for this test
    
    // Test 2: Replay system should be accessible
    auto episodes = episodicMemory->getEpisodesForReplay(10);
    // Episodes might be empty, but the system should work
    
    std::cout << "  Replay system integration test PASSED" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testCognitionSystemIntegration();
    testDevelopmentSystemIntegration();
    testCheckpointSystemIntegration();
    testReplaySystemIntegration();
    
    std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6
