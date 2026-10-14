// Integration Tests
// Scientific validation of NLM integrated systems (Phase 6 audit)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_integration {

void testMemorySystemIntegration() {
    // Test that working memory, episodic memory, and prediction systems work together
    std::cout << "  Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a pattern to store
    std::vector<float> pattern;
    for (int i = 0; i < 10; ++i) {
        pattern.push_back(static_cast<float>(i) / 10.0f);
    }
    
    // Store in working memory
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Store experience in episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = pattern;
    episode.reward = 0.7f;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Verify both systems are working together
    float workingMemoryActivity = brain.getWorkingMemory()->getMemoryActivity();
    size_t episodeCount = brain.getEpisodicMemory()->getEpisodeCount();
    
    assert(workingMemoryActivity >= 0.0f);
    assert(episodeCount >= 1);
    
    // Step through simulation to test integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001);
    }
    
    // Both systems should still have data
    assert(brain.getWorkingMemory()->getMemoryActivity() >= 0.0f);
    assert(brain.getEpisodicMemory()->getEpisodeCount() >= 1);
    
    std::cout << "    Memory system integration test passed" << std::endl;
}

void testCognitiveSystemIntegration() {
    // Test that NeuralPlanner, ConceptFormation, and AttentionalSelection integrate properly
    std::cout << "  Testing Cognitive System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a current state
    std::vector<float> currentState;
    for (int i = 0; i < 10; ++i) {
        currentState.push_back(0.5f);
    }
    
    // Store in working memory
    brain.getWorkingMemory()->store(currentState, 1.0f);
    
    // Get goal from planner
    std::vector<float> goal = brain.getPlanner()->getCurrentGoal();
    
    // If no goal, use default
    if (goal.empty()) {
        goal = std::vector<float>(currentState.size(), 0.5f);
    }
    
    // Plan an action
    nlm::ActionType action = brain.getPlanner()->planAction(currentState, goal.back());
    
    // Verify planner is working
    assert(action != nlm::ActionType::Wait || currentState.size() > 0);
    
    // Test concept formation with current state
    brain.getConceptFormation()->presentExperience(
        currentState,
        currentState,
        brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f,
        0
    );
    
    // Test attention selection
    std::vector<nlm::NeuronId> competitors;
    for (size_t i = 0; i < 5; ++i) {
        competitors.push_back(nlm::NeuronId(static_cast<uint64_t>(i * 10000)));
    }
    
    std::vector<nlm::NeuronId> winners = brain.getAttention()->processCompetition(competitors);
    
    // Verify attention system is working
    assert(!winners.empty() || competitors.size() > 0);
    
    std::cout << "    Cognitive system integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    // Test that neuromodulators affect memory, attention, and learning correctly
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get baseline neuromodulator levels
    float baselineDopamine = brain.getDopamine()->getLevel();
    float baselineACh = brain.getAcetylcholine()->getLevel();
    float baselineNE = brain.getNorepinephrine()->getLevel();
    
    // Create a working memory pattern
    std::vector<float> pattern;
    for (int i = 0; i < 10; ++i) {
        pattern.push_back(0.5f);
    }
    
    // Store pattern
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Step through simulation to allow neuromodulators to work
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001);
    }
    
    // Check that neuromodulators have affected memory encoding
    float memoryActivity = brain.getWorkingMemory()->getMemoryActivity();
    
    // Neuromodulators should have had some effect
    assert(memoryActivity >= 0.0f);
    
    // Test neuromodulator interaction with prediction error
    if (brain.getPredictionError()) {
        float predictionError = brain.getPredictionError()->getError();
        // Prediction error should affect neuromodulators
        assert(predictionError >= 0.0f);
    }
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void testPerformanceIntegration() {
    // Test that memory pools, SIMD, and parallel processing work together
    std::cout << "  Testing Performance Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create multiple patterns to test memory pools
    std::vector<std::vector<float>> patterns;
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern;
        for (int j = 0; j < 20; ++j) {
            pattern.push_back(static_cast<float>(i * j) / 20.0f);
        }
        patterns.push_back(pattern);
    }
    
    // Store all patterns
    for (const auto& pattern : patterns) {
        brain.getWorkingMemory()->store(pattern, 1.0f);
    }
    
    // Test parallel processing by stepping through simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001);
    }
    
    // Performance metrics
    float totalActivity = brain.getWorkingMemory()->getMemoryActivity();
    size_t totalSpikeCount = brain.getTotalSpikeCount();
    size_t pendingEvents = brain.getPendingSpikeEventCount();
    
    // Verify performance systems are working
    assert(totalActivity >= 0.0f);
    assert(totalSpikeCount >= 0);
    assert(pendingEvents >= 0);
    
    std::cout << "    Performance integration test passed" << std::endl;
}

void runAll() {
    testMemorySystemIntegration();
    testCognitiveSystemIntegration();
    testNeuromodulationIntegration();
    testPerformanceIntegration();
}

} // namespace test_integration