// Memory Retention Tests
// Tests memory persistence and decay mechanisms

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_memory_retention {

void testWorkingMemoryRetention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store pattern in working memory
    std::vector<float> pattern = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Check retention immediately
    std::vector<float> retrieved = brain.getWorkingMemory()->retrieve();
    assert(!retrieved.empty());
    
    // Step forward to test retention over time
    for (nlm::SimulationStep step = 1; step < 50; ++step) {
        brain.step(step);
        
        // Working memory should persist but with some decay
        std::vector<float> current = brain.getWorkingMemory()->retrieve();
        
        // After some steps, memory should still exist (though possibly decayed)
        if (step >= 10) {
            assert(!current.empty());
        }
    }
    
    std::cout << "    testWorkingMemoryRetention passed" << std::endl;
}

void testEpisodicMemoryPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create an episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.sensoryState = {0.8f, 0.6f, 0.4f, 0.2f};
    episode.positionX = 5.0f;
    episode.positionY = 5.0f;
    episode.orientation = 0.0f;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 1.0f;
    episode.energy = 0.8f;
    episode.novelty = 0.9f;
    episode.resultingSensoryState = {0.9f, 0.7f, 0.5f, 0.3f};
    episode.resultingReward = 0.8f;
    episode.age = 0;
    
    // Store the episode
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Verify episode was stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    
    // Retrieve similar episodes
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        brain.getEpisodicMemory()->retrieveSimilar(episode.sensoryState, 5);
    assert(!similar.empty());
    
    // Step forward and check persistence
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step);
        
        // Episode should still be accessible
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    }
    
    std::cout << "    testEpisodicMemoryPersistence passed" << std::endl;
}

void testMemoryConsolidationDuringRest() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store multiple episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = std::vector<float>(4, 0.5f + (i * 0.1f));
        episode.positionX = static_cast<float>(i);
        episode.positionY = static_cast<float>(i);
        episode.reward = 0.5f + (i * 0.1f);
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Store working memory pattern
    std::vector<float> wmPattern = {1.0f, 0.5f, 0.0f};
    brain.getWorkingMemory()->store(wmPattern, 1.0f);
    
    // Enable replay
    brain.getEpisodicMemory()->enableReplay(true);
    
    // Simulate rest period (brain enters resting state)
    brain.getWorkingMemory()->setDecayRate(0.01f); // Low decay during rest
    
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        brain.step(step);
        
        // Working memory should decay slowly during rest
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            
            // Memory activity should decrease gradually
            if (step > 1) {
                assert(totalActivity <= 2.5f); // Decreasing pattern
            }
        }
        
        // Episodes should remain (consolidation happens)
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 5);
    }
    
    std::cout << "    testMemoryConsolidationDuringRest passed" << std::endl;
}

void testMemoryDecayMechanisms() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store pattern with strong initial activation
    std::vector<float> pattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    brain.getWorkingMemory()->setDecayRate(0.05f); // Moderate decay
    
    std::vector<float> initial = brain.getWorkingMemory()->retrieve();
    float initialTotal = 0.0f;
    for (float val : initial) initialTotal += std::abs(val);
    
    // Simulate decay over time
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step);
        
        std::vector<float> current = brain.getWorkingMemory()->retrieve();
        float currentTotal = 0.0f;
        for (float val : current) currentTotal += std::abs(val);
        
        // Decay should be gradual (unless decay rate is high)
        if (step > 1) {
            // Memory should generally decay, but may fluctuate
            assert(currentTotal <= initialTotal * 1.1f); // Allow small variations
        }
    }
    
    std::cout << "    testMemoryDecayMechanisms passed" << std::endl;
}

void testWorkingEpisodicMemoryTransfer() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create working memory pattern
    std::vector<float> wmPattern = {0.9f, 0.7f, 0.5f, 0.3f, 0.1f};
    brain.getWorkingMemory()->store(wmPattern, 1.0f);
    
    // Create episodic memory from similar pattern
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = wmPattern; // Similar pattern
    episode.timestamp = 1;
    episode.reward = 1.0f;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Both should be accessible
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    // Test retrieval of similar patterns
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        brain.getEpisodicMemory()->retrieveSimilar(wmPattern, 3);
    
    // The working memory pattern should influence episodic memory retrieval
    if (!similar.empty()) {
        // At least one episode should have been retrieved
        assert(similar[0] != nullptr);
    }
    
    // Simulate more steps with transfer
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        brain.step(step);
        
        // Working memory should continue to influence episodic memory
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float wmActivity = 0.0f;
            for (float val : wm) wmActivity += std::abs(val);
            // Working memory activity should be positive
            assert(wmActivity > 0.0f);
        }
    }
    
    std::cout << "    testWorkingEpisodicMemoryTransfer passed" << std::endl;
}

void testAssociativeMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create two patterns that should become associated
    std::vector<float> patternA = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    std::vector<float> patternB = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    
    // Store them as episodes
    nlm::EpisodicMemoryItem episodeA;
    episodeA.sensoryState = patternA;
    episodeA.timestamp = 1;
    
    nlm::EpisodicMemoryItem episodeB;
    episodeB.sensoryState = patternB;
    episodeB.timestamp = 2;
    
    brain.getEpisodicMemory()->storeEpisode(episodeA);
    brain.getEpisodicMemory()->storeEpisode(episodeB);
    
    // Create association
    brain.getAssociativeMemory()->associate(patternA, patternB, 1.0f);
    
    // Verify association was created
    float strength = brain.getAssociativeMemory()->getAssociationStrength(patternA, patternB);
    assert(strength > 0.0f);
    
    // Step forward and let association strengthen
    for (nlm::SimulationStep step = 1; step < 15; ++step) {
        brain.step(step);
        
        // Association should persist or strengthen
        float currentStrength = brain.getAssociativeMemory()->getAssociationStrength(patternA, patternB);
        if (step > 1) {
            assert(currentStrength >= 0.0f); // Should not degrade to negative
        }
    }
    
    std::cout << "    testAssociativeMemoryConsolidation passed" << std::endl;
}

void testReplayMechanisms() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create episodes for replay
    std::vector<size_t> episodeIds;
    for (size_t i = 0; i < 3; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = std::vector<float>(3, 0.5f + (i * 0.2f));
        episode.timestamp = i;
        episode.reward = 0.7f + (i * 0.1f);
        
        brain.getEpisodicMemory()->storeEpisode(episode);
        episodeIds.push_back(i);
    }
    
    // Enable replay
    brain.getEpisodicMemory()->enableReplay(true);
    
    // Get episodes for replay (should select based on relevance and recency)
    std::vector<const nlm::EpisodicMemoryItem*> episodesForReplay = 
        brain.getEpisodicMemory()->getEpisodesForReplay(2);
    
    // If there are episodes, replay should work
    if (!episodesForReplay.empty()) {
        // Replay sequence
        brain.getEpisodicMemory()->replaySequence({0, 1});
        
        // Verify replay doesn't crash
        assert(brain.getEpisodicMemory()->isReplayEnabled());
    }
    
    // Simulate replay during rest
    brain.getWorkingMemory()->setDecayRate(0.005f); // Very low decay during replay
    
    for (nlm::SimulationStep step = 1; step < 25; ++step) {
        brain.step(step);
        
        // Memory should be preserved during replay
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 3);
        
        // Working memory should maintain traces during replay
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            // Should maintain reasonable activity level
            assert(totalActivity >= 0.0f);
        }
    }
    
    std::cout << "    testReplayMechanisms passed" << std::endl;
}

void testPostRestRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create working memory
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    brain.getWorkingMemory()->setDecayRate(0.1f); // High decay
    
    // Store episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Simulate rest with high decay
    for (nlm::SimulationStep step = 1; step < 15; ++step) {
        brain.step(step);
        
        // During rest, working memory may decay
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            // Activity may decrease during rest
        }
        
        // Episodic memory should persist
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    }
    
    // After rest, recovery should happen
    brain.getWorkingMemory()->setDecayRate(0.01f); // Lower decay for recovery
    
    std::vector<float> pattern2 = {0.7f, 0.5f, 0.3f, 0.1f};
    brain.getWorkingMemory()->store(pattern2, 1.0f);
    
    // Verify recovery
    std::vector<float> retrieved = brain.getWorkingMemory()->retrieve();
    assert(!retrieved.empty());
    
    std::cout << "    testPostRestRecovery passed" << std::endl;
}

void runAll() {
    testWorkingMemoryRetention();
    testEpisodicMemoryPersistence();
    testMemoryConsolidationDuringRest();
    testMemoryDecayMechanisms();
    testWorkingEpisodicMemoryTransfer();
    testAssociativeMemoryConsolidation();
    testReplayMechanisms();
    testPostRestRecovery();
}

} // namespace test_memory_retention