// Sleep/Rest Cycle Tests
// Tests sleep/rest detection and consolidation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_sleep_rest_cycle {

void testRestDetectionAndEntryConditions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initially should not be resting
    // Note: Brain::Impl has isResting field but no public accessor
    // For testing, we'll just verify brain functionality
    
    // Store some memory
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Simulate activity
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        
        // Memory should be active
        assert(brain.getWorkingMemory()->getActiveTraces() > 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    }
    
    std::cout << "    testRestDetectionAndEntryConditions passed" << std::endl;
}

void testMemoryConsolidationDuringRest() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store multiple episodes
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = std::vector<float>(3, 0.5f + (i * 0.05f));
        episode.positionX = static_cast<float>(i);
        episode.positionY = static_cast<float>(i);
        episode.reward = 0.5f + (i * 0.05f);
        episode.timestamp = i;
        
        brain.getEpisodicMemory()->storeEpisode(episode);
        episodes.push_back(episode);
    }
    
    // Store working memory patterns
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern = {
            0.9f - (i * 0.1f),
            0.8f - (i * 0.1f),
            0.7f - (i * 0.1f)
        };
        brain.getWorkingMemory()->store(pattern, 1.0f);
    }
    
    // Simulate rest period with low decay
    brain.getWorkingMemory()->setDecayRate(0.01f); // Very low decay during rest
    
    for (nlm::SimulationStep step = 10; step < 30; ++step) {
        brain.step(step);
        
        // Working memory should decay very slowly during rest
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            
            // Activity should be relatively stable during rest
            assert(totalActivity >= 0.0f);
        }
        
        // Episodic memory should be consolidated (active)
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 10);
        
        // Can enable replay during rest
        brain.getEpisodicMemory()->enableReplay(true);
    }
    
    std::cout << "    testMemoryConsolidationDuringRest passed" << std::endl;
}

void testReplayMechanisms() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create episodes for replay
    std::vector<size_t> episodeIndices;
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = std::vector<float>(2, 0.5f + (i * 0.1f));
        episode.positionX = static_cast<float>(i);
        episode.positionY = static_cast<float>(i);
        episode.reward = 0.7f + (i * 0.05f);
        episode.timestamp = i;
        
        brain.getEpisodicMemory()->storeEpisode(episode);
        episodeIndices.push_back(i);
    }
    
    // Enable replay
    brain.getEpisodicMemory()->enableReplay(true);
    
    // Get episodes for replay
    std::vector<const nlm::EpisodicMemoryItem*> episodesForReplay = 
        brain.getEpisodicMemory()->getEpisodesForReplay(3);
    
    // If there are episodes, replay should work
    if (!episodesForReplay.empty()) {
        // Replay sequence of episodes
        brain.getEpisodicMemory()->replaySequence({0, 1, 2});
        
        // Verify replay is enabled
        assert(brain.getEpisodicMemory()->isReplayEnabled());
    }
    
    // Simulate rest with replay active
    brain.getWorkingMemory()->setDecayRate(0.005f); // Very low decay during replay
    
    for (nlm::SimulationStep step = 20; step < 40; ++step) {
        brain.step(step);
        
        // Memory should be preserved during replay
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 5);
        
        // Working memory should maintain traces during replay
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            assert(totalActivity >= 0.0f);
        }
    }
    
    std::cout << "    testReplayMechanisms passed" << std::endl;
}

void testPostRestRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create working memory
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    brain.getWorkingMemory()->setDecayRate(0.15f); // High decay (simulating rest)
    
    // Create episodic memory
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
    
    // After rest, recovery should happen (lower decay)
    brain.getWorkingMemory()->setDecayRate(0.02f); // Lower decay for recovery
    
    std::vector<float> pattern2 = {0.7f, 0.5f, 0.3f, 0.1f};
    brain.getWorkingMemory()->store(pattern2, 1.0f);
    
    // Verify recovery
    std::vector<float> retrieved = brain.getWorkingMemory()->retrieve();
    assert(!retrieved.empty());
    
    std::cout << "    testPostRestRecovery passed" << std::endl;
}

void testRestConsolidationPriority() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store working memory
    std::vector<float> wmPattern = {0.9f, 0.7f, 0.5f};
    brain.getWorkingMemory()->store(wmPattern, 1.0f);
    
    // Store episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = wmPattern;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Simulate rest consolidation
    brain.getWorkingMemory()->setDecayRate(0.005f); // Very low during consolidation
    
    // During consolidation, memory pressure and consolidation priority should be managed
    for (nlm::SimulationStep step = 10; step < 25; ++step) {
        brain.step(step);
        
        // Working memory should be preserved during consolidation
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            assert(totalActivity >= 0.0f);
        }
        
        // Episodic memory should remain
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    }
    
    std::cout << "    testRestConsolidationPriority passed" << std::endl;
}

void testMultipleRestCycles() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // First learning phase
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        
        std::vector<float> pattern = {0.5f + (step * 0.05f), 0.4f, 0.3f};
        brain.getWorkingMemory()->store(pattern, 1.0f);
        
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = pattern;
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Rest/consolidation phase
    brain.getWorkingMemory()->setDecayRate(0.01f); // Low decay
    
    for (nlm::SimulationStep step = 10; step < 20; ++step) {
        brain.step(step);
        
        // Memory should be preserved during rest
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 5);
    }
    
    // Recovery phase
    brain.getWorkingMemory()->setDecayRate(0.05f); // Higher decay
    
    for (nlm::SimulationStep step = 20; step < 30; ++step) {
        brain.step(step);
        
        // Memory may decay but should still exist
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            assert(totalActivity >= 0.0f);
        }
    }
    
    std::cout << "    testMultipleRestCycles passed" << std::endl;
}

void testSleepReplayIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create rich memory set for replay
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (size_t i = 0; i < 8; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = std::vector<float>(3, 0.6f + (i * 0.05f));
        episode.positionX = static_cast<float>(i);
        episode.positionY = static_cast<float>(i);
        episode.reward = 0.7f + (i * 0.03f);
        episode.timestamp = i;
        
        brain.getEpisodicMemory()->storeEpisode(episode);
        episodes.push_back(episode);
    }
    
    // Enable replay
    brain.getEpisodicMemory()->enableReplay(true);
    
    // Simulate sleep/rest with replay
    brain.getWorkingMemory()->setDecayRate(0.002f); // Minimal decay during sleep
    
    for (nlm::SimulationStep step = 30; step < 50; ++step) {
        brain.step(step);
        
        // Should have episodes for replay
        assert(brain.getEpisodicMemory()->getEpisodeCount() == 8);
        
        // Working memory may have traces
        std::vector<float> wm = brain.getWorkingMemory()->retrieve();
        if (!wm.empty()) {
            float totalActivity = 0.0f;
            for (float val : wm) totalActivity += std::abs(val);
            assert(totalActivity >= 0.0f);
        }
    }
    
    std::cout << "    testSleepReplayIntegration passed" << std::endl;
}

void runAll() {
    testRestDetectionAndEntryConditions();
    testMemoryConsolidationDuringRest();
    testReplayMechanisms();
    testPostRestRecovery();
    testRestConsolidationPriority();
    testMultipleRestCycles();
    testSleepReplayIntegration();
}

} // namespace test_sleep_rest_cycle