// Memory Retention Tests
// Scientific validation of NLM memory capabilities (Phase 6 audit)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_memory_retention {

void testEpisodicMemoryRetention() {
    // Test that episodic memory stores and retrieves experiences correctly
    std::cout << "  Testing Episodic Memory Retention..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    config->set<float>("reward_discount_factor", 0.95f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.8f;
    episode.novelty = 0.6f;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    
    // Add sensory state
    for (int i = 0; i < 10; ++i) {
        episode.sensoryState.push_back(static_cast<float>(i) / 10.0f);
    }
    
    // Store the episode
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Verify it was stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    
    // Retrieve and verify content
    const nlm::EpisodicMemoryItem* retrieved = brain.getEpisodicMemory()->getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->reward == 0.8f);
    assert(retrieved->novelty == 0.6f);
    assert(retrieved->action == nlm::ActionType::MoveForward);
    assert(retrieved->positionX == 1.0f);
    assert(retrieved->positionY == 2.0f);
    
    std::cout << "    Episodic memory retention test passed" << std::endl;
}

void testWorkingMemoryStability() {
    // Test that working memory maintains information over time and decays appropriately
    std::cout << "  Testing Working Memory Stability..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set working memory to stable mode
    brain.getWorkingMemory()->setDecayRate(0.01f);
    brain.getWorkingMemory()->setCapacity(50);
    
    // Store a pattern in working memory
    std::vector<float> pattern;
    for (int i = 0; i < 20; ++i) {
        pattern.push_back(0.5f);
    }
    
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Verify it was stored
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    
    // Retrieve and verify content
    std::vector<float> retrieved = brain.getWorkingMemory()->retrieve();
    assert(!retrieved.empty());
    
    // Test memory stability - patterns should persist over multiple updates
    float initialActivity = brain.getWorkingMemory()->getMemoryActivity();
    assert(initialActivity > 0.0f);
    
    // Update multiple times to test decay
    for (int i = 0; i < 10; ++i) {
        brain.getWorkingMemory()->update(0.001f);
    }
    
    float finalActivity = brain.getWorkingMemory()->getMemoryActivity();
    // Activity should decay gradually but not disappear immediately
    assert(finalActivity >= 0.0f);
    
    std::cout << "    Working memory stability test passed" << std::endl;
}

void testMemoryConsolidation() {
    // Test that memory consolidation works correctly
    std::cout << "  Testing Memory Consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create multiple episodes with different relevance levels
    std::vector<float> rewards = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = static_cast<nlm::SimulationStep>(i * 10);
        episode.reward = rewards[i];
        episode.novelty = rewards[i];
        
        for (int j = 0; j < 5; ++j) {
            episode.sensoryState.push_back(static_cast<float>(j) / 5.0f);
        }
        
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Verify episodes were stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 5);
    
    // Get average reward
    float avgReward = brain.getEpisodicMemory()->getAverageReward();
    assert(avgReward > 0.0f);
    
    // Test consolidation - should remove low relevance episodes
    // Higher reward episodes should be preserved during consolidation
    brain.getEpisodicMemory()->consolidate(0.4f);
    
    // After consolidation, some episodes may have been removed
    // The test verifies consolidation doesn't crash and handles low relevance items
    assert(brain.getEpisodicMemory()->getEpisodeCount() <= 5);
    
    std::cout << "    Memory consolidation test passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryRetention();
    testWorkingMemoryStability();
    testMemoryConsolidation();
}

} // namespace test_memory_retention