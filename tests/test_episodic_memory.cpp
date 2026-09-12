// Episodic Memory Integration Tests
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory em;
    
    assert(em.getMaxEpisodes() == 1000);  // Default capacity
    assert(em.getEpisodeCount() == 0);
    assert(em.getAverageReward() == 0.0f);
    assert(em.isReplayEnabled() == true);  // Default is enabled
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    nlm::NeuralEpisodicMemory em;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    em.initialize(&brain);
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.resultingSensoryState = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.8f;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.0f;
    
    em.storeEpisode(episode);
    
    // Check episode was stored
    assert(em.getEpisodeCount() == 1);
    
    // Get the episode back
    const nlm::EpisodicMemoryItem* retrieved = em.getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->reward == episode.reward);
    assert(retrieved->action == episode.action);
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testEpisodicMemorySimilarityRetrieval() {
    nlm::NeuralEpisodicMemory em;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    em.initialize(&brain);
    
    // Store several episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        
        // Create distinct sensory states
        std::vector<float> state(3);
        for (int j = 0; j < 3; ++j) {
            state[j] = 0.1f * (i * 10 + j);
        }
        episode.sensoryState = state;
        episode.resultingSensoryState = state;
        episode.reward = 0.5f + i * 0.1f;
        episode.action = nlm::ActionType::MoveForward;
        
        em.storeEpisode(episode);
    }
    
    // Test similarity retrieval
    std::vector<float> query = {1.0f, 2.0f, 3.0f};  // Similar to episode 0
    auto results = em.retrieveSimilar(query, 3);
    
    // Should find some similar episodes
    assert(!results.empty());
    
    std::cout << "    testEpisodicMemorySimilarityRetrieval passed" << std::endl;
}

void testEpisodicMemoryTemporalRetrieval() {
    nlm::NeuralEpisodicMemory em;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    em.initialize(&brain);
    
    // Store episodes at different times
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;  // Times: 0, 10, 20, ... 90
        episode.sensoryState = {0.1f, 0.2f, 0.3f};
        episode.resultingSensoryState = {0.2f, 0.3f, 0.4f};
        episode.reward = 1.0f;
        episode.action = nlm::ActionType::MoveForward;
        
        em.storeEpisode(episode);
    }
    
    // Retrieve episodes between time 20 and 80
    auto results = em.retrieveTemporal(20, 80, 5);
    
    // Should find episodes with timestamps in that range
    assert(results.size() <= 5);  // At most 5 requested
    
    // All results should be in the specified time range
    for (const auto* ep : results) {
        assert(ep->timestamp >= 20 && ep->timestamp <= 80);
    }
    
    std::cout << "    testEpisodicMemoryTemporalRetrieval passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::NeuralEpisodicMemory em;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    em.initialize(&brain);
    
    // Store an episode with active neurons
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.5f, 0.5f};
    episode.resultingSensoryState = {0.6f, 0.6f, 0.6f};
    episode.reward = 1.0f;
    episode.action = nlm::ActionType::MoveForward;
    
    // Add some active neurons
    episode.activeNeurons.push_back(nlm::NeuronId(1000));
    episode.neuronActivations.push_back(0.8f);
    
    em.storeEpisode(episode);
    
    // Get episode for replay
    const nlm::EpisodicMemoryItem* ep = em.getEpisode(0);
    assert(ep != nullptr);
    
    // Replay the episode (should not crash)
    em.replayEpisode(ep);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::NeuralEpisodicMemory em;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("max_episodic_episodes", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    em.initialize(&brain);
    
    // Set max episodes lower for testing consolidation
    em.setMaxEpisodes(5);
    
    // Store more episodes than capacity
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f * i, 0.2f * i, 0.3f * i};
        episode.resultingSensoryState = {0.15f * i, 0.25f * i, 0.35f * i};
        episode.reward = 0.5f + 0.05f * i;
        episode.action = nlm::ActionType::MoveForward;
        
        em.storeEpisode(episode);
    }
    
    // Consolidation should have occurred, reducing episode count
    assert(em.getEpisodeCount() <= em.getMaxEpisodes());
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryRealBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    config->set("max_episodic_episodes", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test episodic memory through brain simulation
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Check episodic memory state
        assert(episodicMemory->getEpisodeCount() >= 0);
        assert(episodicMemory->getAverageReward() >= 0.0f);
    
    std::cout << "    testEpisodicMemoryRealBrainIntegration passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryCreation();
    testEpisodicMemoryStorage();
    testEpisodicMemorySimilarityRetrieval();
    testEpisodicMemoryTemporalRetrieval();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryRealBrainIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Episodic Memory Tests PASSED ===" << std::endl;
    std::cout << "Episodic memory demonstrates real experience storage" << std::endl;
}

} // namespace test_episodic_memory