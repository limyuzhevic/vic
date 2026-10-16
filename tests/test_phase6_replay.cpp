// Phase 6 Replay System Tests
// Test memory replay and consolidation

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace nlm;

namespace test_phase6_replay {

void testReplaySystem() {
    std::cout << "  Testing Replay System..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testReplay();
    assert(success);
    
    std::cout << "    PASSED" << std::endl;
}

void testMemoryReplay() {
    std::cout << "  Testing Memory Replay..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Check if episodic memory has episodes for replay
    size_t episodeCount = episodicMemory->getEpisodeCount();
    if (episodeCount > 0) {
        // Get episodes for replay
        auto episodes = episodicMemory->getEpisodesForReplay(3);
        assert(!episodes.empty());
        
        // Verify episodes contain valid data
        for (const auto& episode : episodes) {
            assert(episode.step >= 0);
            assert(episode.reward >= 0.0f);
            assert(episode.endStep > episode.step);
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testReplayConsolidation() {
    std::cout << "  Testing Replay Consolidation..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Store some neural activity
    for (int i = 0; i < 30; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Check if episodic memory grows
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        size_t initialCount = episodicMemory->getEpisodeCount();
        
        // Run more steps to potentially create more episodes
        for (int i = 30; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        size_t finalCount = episodicMemory->getEpisodeCount();
        
        // Memory should not decrease (may stay the same or increase)
        assert(finalCount >= initialCount);
        
        // Test replay if there are episodes
        if (finalCount > 0) {
            auto episodes = episodicMemory->getEpisodesForReplay(5);
            assert(!episodes.empty());
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testReplayPerformance() {
    std::cout << "  Testing Replay Performance..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Measure replay performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
        
        // Periodic replay checks
        if (i % 25 == 0) {
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
                auto episodes = episodicMemory->getEpisodesForReplay(2);
                // Replay should be fast
                assert(!episodes.empty());
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete within reasonable time
    assert(elapsed < 10.0); // 10 seconds max
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Replay System Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testReplaySystem();
    testMemoryReplay();
    testReplayConsolidation();
    testReplayPerformance();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Replay System Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_replay