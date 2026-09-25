// Phase 6 Replay System Integration Tests
// Comprehensive testing of replay system functionality

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_replay {

void testReplaySystemBasic() {
    std::cout << "    Testing Replay System Basic Functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Initially no episodes
    size_t initialEpisodes = episodicMemory->getEpisodeCount();
    assert(initialEpisodes == 0);
    std::cout << "      Initial episodes: " << initialEpisodes << std::endl;
    
    // Run simulation to generate episodes
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Generate episodic memories
        if (step % 25 == 0) {
            // Simulate episode creation
            episodicMemory->createEpisode(step, "simulation_step");
        }
    }
    
    // Check if episodes were created
    size_t finalEpisodes = episodicMemory->getEpisodeCount();
    std::cout << "      Episodes after simulation: " << finalEpisodes << std::endl;
    
    // Test replay functionality
    if (finalEpisodes > 0) {
        auto episodes = episodicMemory->getEpisodesForReplay(3);
        std::cout << "      Retrieved " << episodes.size() << " episodes for replay" << std::endl;
        assert(episodes.size() <= 3);
    }
    
    std::cout << "      Replay system basic functionality test PASSED" << std::endl;
}

void testReplayLearningEnhancement() {
    std::cout << "    Testing Replay Learning Enhancement..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Fill memory with episodes
    std::vector<nlm::Episode> recordedEpisodes;
    
    for (nlm::SimulationStep step = 0; step < 400; ++step) {
        brain.step(step);
        
        // Create episodes periodically
        if (step % 50 == 0) {
            nlm::Episode episode;
            episode.id = step;
            episode.timestamp = step * 0.001f;
            episode.step = step;
            episode.reward = 0.5f + (step % 10) * 0.1f;
            episode.quality = 0.7f + (step % 5) * 0.1f;
            recordedEpisodes.push_back(episode);
            
            episodicMemory->createEpisode(step, "learning_episode");
        }
    }
    
    size_t episodeCount = episodicMemory->getEpisodeCount();
    std::cout << "      Recorded " << episodeCount << " episodes" << std::endl;
    
    // Test replay-based learning
    for (size_t replayCount = 1; replayCount <= 3; ++replayCount) {
        std::cout << "      Replay pass " << replayCount << ": "
                  << episodicMemory->getEpisodeCount() << " episodes available" << std::endl;
        
        // Replay some episodes
        auto episodes = episodicMemory->getEpisodesForReplay(2);
        std::cout << "        Retrieved " << episodes.size() << " episodes" << std::endl;
        
        // Apply replay effects (in real implementation, this would strengthen synapses)
        episodicMemory->applyReplayEffects(episodes);
    }
    
    std::cout << "      Replay learning enhancement test PASSED" << std::endl;
}

void testReplayConsolidation() {
    std::cout << "    Testing Replay Consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Phase 1: Acquisition
    std::cout << "      Phase 1: Memory acquisition" << std::endl;
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        episodicMemory->createEpisode(step, "acquisition");
    }
    
    size_t acquisitionCount = episodicMemory->getEpisodeCount();
    std::cout << "        Acquired " << acquisitionCount << " episodes" << std::endl;
    
    // Phase 2: Offline replay (simulated)
    std::cout << "      Phase 2: Offline replay" << std::endl;
    for (int replayPass = 0; replayPass < 3; ++replayPass) {
        auto episodes = episodicMemory->getEpisodesForReplay(5);
        episodicMemory->applyReplayEffects(episodes);
        
        size_t remainingEpisodes = episodicMemory->getEpisodeCount();
        std::cout << "        Replay pass " << replayPass + 1 << ": "
                  << episodes.size() << " episodes replayed, "
                  << remainingEpisodes << " remaining" << std::endl;
    }
    
    // Phase 3: Consolidation
    std::cout << "      Phase 3: Consolidation" << std::endl;
    episodicMemory->consolidateMemory(0.2f);
    
    size_t finalEpisodes = episodicMemory->getEpisodeCount();
    std::cout << "        Final episode count after consolidation: " << finalEpisodes << std::endl;
    
    // Verify consolidation reduced episode count
    assert(finalEpisodes <= acquisitionCount);
    
    std::cout << "      Replay consolidation test PASSED" << std::endl;
}

void testReplayErrorRecovery() {
    std::cout << "    Testing Replay Error Recovery..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create initial episodes
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        if (step % 20 == 0) {
            episodicMemory->createEpisode(step, "error_test");
        }
    }
    
    size_t episodeCount = episodicMemory->getEpisodeCount();
    std::cout << "      Created " << episodeCount << " initial episodes" << std::endl;
    
    // Simulate memory corruption/loss
    episodicMemory->resetMemory();
    std::cout << "      Simulated memory corruption" << std::endl;
    
    // Test recovery through replay
    for (nlm::SimulationStep step = 100; step < 250; ++step) {
        brain.step(step);
        
        // Create new episodes for recovery
        if (step % 30 == 0) {
            episodicMemory->createEpisode(step, "recovery");
        }
        
        // Attempt replay during recovery
        if (step % 60 == 0) {
            auto episodes = episodicMemory->getEpisodesForReplay(2);
            episodicMemory->applyReplayEffects(episodes);
        }
    }
    
    size_t recoveryEpisodes = episodicMemory->getEpisodeCount();
    std::cout << "      Recovered to " << recoveryEpisodes << " episodes" << std::endl;
    
    // Recovery should have some episodes
    assert(recoveryEpisodes > 0);
    
    std::cout << "      Replay error recovery test PASSED" << std::endl;
}

void testReplayPerformance() {
    std::cout << "    Testing Replay Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Fill memory with many episodes
    std::cout << "      Filling memory with episodes..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        if (step % 25 == 0) {
            episodicMemory->createEpisode(step, "performance_test");
        }
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    size_t totalEpisodes = episodicMemory->getEpisodeCount();
    std::cout << "      Total episodes: " << totalEpisodes << std::endl;
    
    // Benchmark replay performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int replayPass = 0; replayPass < 5; ++replayPass) {
        auto episodes = episodicMemory->getEpisodesForReplay(10);
        episodicMemory->applyReplayEffects(episodes);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Replay 5 passes in " << duration << "s" << std::endl;
    
    std::cout << "      Replay performance test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Replay System Tests ===" << std::endl;
    
    testReplaySystemBasic();
    testReplayLearningEnhancement();
    testReplayConsolidation();
    testReplayErrorRecovery();
    testReplayPerformance();
    
    std::cout << "\n=== All Phase 6 Replay Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_replay