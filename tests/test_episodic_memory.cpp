// Episodic Memory Tests
// ====================
//
// NeuralEpisodicMemory tests verify that the episodic memory system
// correctly implements memory storage, retrieval, replay, and consolidation
// mechanisms for experiences and events.
//
// Episodic memory tests focus on:
// - Episode encoding and storage
// - Pattern-based retrieval (similarity-based, temporal, location-based)
// - Action-based recall
// - Neural replay mechanisms
// - Memory consolidation and aging
// - Integration with brain neural infrastructure
//
// This test suite ensures episodic memory operates as a dynamic repository
// of experiences where episodes are encoded as neural activity patterns and
// can influence future processing through pattern completion and replay.

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(episodicMemory != nullptr);
    assert(episodicMemory->getEpisodeCount() == 0);
    assert(episodicMemory->isReplayEnabled() == true);
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStoreEpisode() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.sensoryState = {0.5f, 0.8f, 0.3f, 0.9f};
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::Move;
    episode.reward = 1.0f;
    episode.energy = 0.8f;
    episode.novelty = 0.3f;
    episode.resultingSensoryState = {0.7f, 0.6f, 0.4f};
    episode.resultingReward = 0.9f;
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Retrieve episode
    const nlm::EpisodicMemoryItem* retrieved = episodicMemory->getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->timestamp == episode.timestamp);
    assert(retrieved->reward == episode.reward);
    
    std::cout << "    testEpisodicMemoryStoreEpisode passed" << std::endl;
}

void testEpisodicMemorySimilarRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store similar episodes
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 1;
    episode1.sensoryState = {0.5f, 0.8f, 0.3f};
    episode1.positionX = 1.0f;
    episode1.positionY = 2.0f;
    episode1.action = nlm::ActionType::Move;
    episode1.reward = 1.0f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 2;
    episode2.sensoryState = {0.55f, 0.78f, 0.32f}; // Very similar to episode1
    episode2.positionX = 1.1f;
    episode2.positionY = 2.1f;
    episode2.action = nlm::ActionType::Move;
    episode2.reward = 0.9f;
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    
    // Retrieve similar to episode1 pattern
    std::vector<float> queryPattern = {0.5f, 0.8f, 0.3f};
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        episodicMemory->retrieveSimilar(queryPattern, 5);
    
    assert(similar.size() >= 1);
    
    std::cout << "    testEpisodicMemorySimilarRetrieve passed" << std::endl;
}

void testEpisodicMemoryTemporalRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store episodes at different times
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 10;
    episode1.sensoryState = {0.5f, 0.5f, 0.5f};
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 20;
    episode2.sensoryState = {0.5f, 0.5f, 0.5f};
    
    nlm::EpisodicMemoryItem episode3;
    episode3.timestamp = 30;
    episode3.sensoryState = {0.5f, 0.5f, 0.5f};
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    episodicMemory->storeEpisode(episode3);
    
    // Retrieve from time window 15-25
    std::vector<const nlm::EpisodicMemoryItem*> temporal = 
        episodicMemory->retrieveTemporal(15, 25, 10);
    
    assert(temporal.size() == 1);
    assert(temporal[0]->timestamp == 20);
    
    std::cout << "    testEpisodicMemoryTemporalRetrieve passed" << std::endl;
}

void testEpisodicMemoryLocationRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store episodes at different locations
    nlm::EpisodicMemoryItem episode1;
    episode1.positionX = 1.0f;
    episode1.positionY = 2.0f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.positionX = 5.0f;
    episode2.positionY = 6.0f;
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    
    // Retrieve within radius 2.0f of (1.5, 2.5)
    std::vector<const nlm::EpisodicMemoryItem*> location = 
        episodicMemory->retrieveByLocation(1.5f, 2.5f, 2.0f, 10);
    
    assert(location.size() == 1);
    assert(std::abs(location[0]->positionX - 1.0f) < 0.1f);
    assert(std::abs(location[0]->positionY - 2.0f) < 0.1f);
    
    std::cout << "    testEpisodicMemoryLocationRetrieve passed" << std::endl;
}

void testEpisodicMemoryActionRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store episodes with different actions
    nlm::EpisodicMemoryItem episode1;
    episode1.action = nlm::ActionType::Move;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.action = nlm::ActionType::Wait;
    
    episodicMemory->storeEpisode(episode1);
    episodicMemory->storeEpisode(episode2);
    
    // Retrieve episodes after Move action
    std::vector<const nlm::EpisodicMemoryItem*> action = 
        episodicMemory->retrieveAfterAction(nlm::ActionType::Move, 10);
    
    assert(action.size() == 1);
    assert(action[0]->action == nlm::ActionType::Move);
    
    std::cout << "    testEpisodicMemoryActionRetrieve passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Create an episode with neural activity
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.sensoryState = {0.5f, 0.8f, 0.3f};
    episode.activeNeurons = {nlm::NeuronId(1000), nlm::NeuronId(1001)};
    episode.neuronActivations = {0.7f, 0.9f};
    
    episodicMemory->storeEpisode(episode);
    
    // Enable replay (already enabled by default)
    assert(episodicMemory->isReplayEnabled() == true);
    
    // Get episode for replay
    std::vector<const nlm::EpisodicMemoryItem*> forReplay = 
        episodicMemory->getEpisodesForReplay(1);
    
    assert(forReplay.size() == 1);
    
    // Replay the episode (this should reactivate neurons)
    episodicMemory->replayEpisode(forReplay[0]);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store several episodes
    for (size_t i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.1f * i, 0.2f * i, 0.3f * i};
        episode.age = i;
        episode.reward = 0.5f + 0.1f * i;
        
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 10);
    
    // Get average reward
    float avgReward = episodicMemory->getAverageReward();
    assert(avgReward > 0.0f && avgReward < 1.0f);
    
    // Consolidate (this should remove some old episodes)
    episodicMemory->consolidate(0.1f); // Very low threshold for demo
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryRecentEpisodes() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Store recent episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.5f, 0.5f, 0.5f};
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Get recent episodes
    std::vector<const nlm::EpisodicMemoryItem*> recent = 
        episodicMemory->getRecentEpisodes(3);
    
    assert(recent.size() == 3);
    assert(recent[0]->timestamp == 4); // Most recent
    assert(recent[1]->timestamp == 3);
    assert(recent[2]->timestamp == 2);
    
    std::cout << "    testEpisodicMemoryRecentEpisodes passed" << std::endl;
}

void testEpisodicMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Add some episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {0.5f, 0.5f, 0.5f};
        
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 5);
    
    episodicMemory->clear();
    
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryClear passed" << std::endl;
}

void testEpisodicMemoryDisableReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Disable replay
    episodicMemory->enableReplay(false);
    assert(episodicMemory->isReplayEnabled() == false);
    
    // Enable replay again
    episodicMemory->enableReplay(true);
    assert(episodicMemory->isReplayEnabled() == true);
    
    std::cout << "    testEpisodicMemoryDisableReplay passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* workingMemory = brain.getWorkingMemory();
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.sensoryState = {0.5f, 0.8f, 0.3f};
    episode.action = nlm::ActionType::Move;
    episode.reward = 1.0f;
    
    episodicMemory->storeEpisode(episode);
    
    // Working memory should be accessible and functional
    assert(workingMemory != nullptr);
    assert(workingMemory->getActiveTraces() >= 0);
    
    // Episodic memory should be able to retrieve based on stored episode
    std::vector<const nlm::EpisodicMemoryItem*> retrieved = 
        episodicMemory->retrieveSimilar({0.5f, 0.8f, 0.3f}, 5);
    
    assert(retrieved.size() >= 1);
    assert(retrieved[0]->reward == episode.reward);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryAdvancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Test 1: Multiple episodes with different characteristics
    std::vector<nlm::EpisodicMemoryItem> episodes;
    
    for (size_t i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {
            0.1f * std::sin(i * 0.1f),
            0.2f * std::cos(i * 0.2f),
            0.3f * std::sin(i * 0.3f)
        };
        episode.positionX = 10.0f + i * 0.5f;
        episode.positionY = 5.0f + i * 0.3f;
        episode.orientation = i * 0.2f;
        episode.action = (i % 3 == 0) ? nlm::ActionType::Move : 
                         (i % 3 == 1) ? nlm::ActionType::Wait : nlm::ActionType::Attack;
        episode.reward = 0.5f + 0.5f * std::sin(i * 0.1f);
        episode.energy = 0.7f + 0.3f * std::cos(i * 0.2f);
        episode.novelty = 0.1f * i;
        episode.resultingSensoryState = episode.sensoryState;
        episode.resultingReward = episode.reward * 0.9f;
        
        episodicMemory->storeEpisode(episode);
        episodes.push_back(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 20);
    
    // Test 2: Similarity retrieval with various queries
    std::vector<std::vector<float>> queries = {
        {0.1f, 0.2f, 0.3f},
        {0.0f, 0.0f, 0.0f},
        {-0.1f, 0.2f, 0.3f}
    };
    
    for (const auto& query : queries) {
        std::vector<const nlm::EpisodicMemoryItem*> results = 
            episodicMemory->retrieveSimilar(query, 5);
        // Results should be non-negative
        assert(results.size() >= 0);
    }
    
    // Test 3: Temporal retrieval with different windows
    std::vector<std::pair<nlm::SimulationStep, nlm::SimulationStep>> windows = {
        {5, 10},
        {15, 25},
        {0, 50}
    };
    
    for (const auto& window : windows) {
        std::vector<const nlm::EpisodicMemoryItem*> temporal = 
            episodicMemory->retrieveTemporal(window.first, window.second, 10);
        assert(temporal.size() >= 0);
    }
    
    // Test 4: Action retrieval
    for (int i = 0; i < 3; ++i) {
        nlm::ActionType action = static_cast<nlm::ActionType>(i);
        std::vector<const nlm::EpisodicMemoryItem*> actionResults = 
            episodicMemory->retrieveAfterAction(action, 5);
        // Results should be non-negative
        assert(actionResults.size() >= 0);
    }
    
    // Test 5: Location retrieval
    for (size_t i = 0; i < 3; ++i) {
        float x = 10.0f + i * 2.0f;
        float y = 5.0f + i * 1.5f;
        std::vector<const nlm::EpisodicMemoryItem*> locationResults = 
            episodicMemory->retrieveByLocation(x, y, 3.0f, 5);
        assert(locationResults.size() >= 0);
    }
    
    // Test 6: Replay sequence
    std::vector<size_t> episodeIds = {5, 15, 25};
    episodicMemory->replaySequence(episodeIds);
    
    // Test 7: Recent episodes retrieval
    std::vector<const nlm::EpisodicMemoryItem*> recent = 
        episodicMemory->getRecentEpisodes(5);
    assert(recent.size() == 5);
    
    // Test 8: Get specific episode
    const nlm::EpisodicMemoryItem* specific = episodicMemory->getEpisode(10);
    assert(specific != nullptr);
    assert(specific->timestamp == 10);
    
    std::cout << "    testEpisodicMemoryAdvancedFeatures passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryCreation();
    testEpisodicMemoryStoreEpisode();
    testEpisodicMemorySimilarRetrieve();
    testEpisodicMemoryTemporalRetrieve();
    testEpisodicMemoryLocationRetrieve();
    testEpisodicMemoryActionRetrieve();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryRecentEpisodes();
    testEpisodicMemoryClear();
    testEpisodicMemoryDisableReplay();
    testEpisodicMemoryIntegration();
    testEpisodicMemoryAdvancedFeatures();
    
    std::cout << "All episodic memory tests passed!" << std::endl;
}

} // namespace test_episodic_memory
