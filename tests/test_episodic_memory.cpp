// Episodic Memory Tests
// Tests for experience encoding and replay

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory em;
    
    // Creation doesn't crash
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryInitialize() {
    nlm::NeuralEpisodicMemory em;
    
    // Initialize with null brain (placeholder)
    em.initialize(nullptr);
    
    // Should not crash
    assert(true);
    
    std::cout << "    testEpisodicMemoryInitialize passed" << std::endl;
}

void testEpisodicMemoryStoreEpisode() {
    nlm::NeuralEpisodicMemory em;
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.positionX = 10.0f;
    episode.positionY = 5.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.5f;
    episode.energy = 1.0f;
    episode.novelty = 0.3f;
    episode.resultingReward = 0.8f;
    episode.age = 0;
    
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.resultingSensoryState = {0.4f, 0.5f, 0.6f};
    
    // Store episode
    em.storeEpisode(episode);
    
    assert(em.getEpisodeCount() == 1);
    
    std::cout << "    testEpisodicMemoryStoreEpisode passed" << std::endl;
}

void testEpisodicMemoryRetrieveSimilar() {
    nlm::NeuralEpisodicMemory em;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.timestamp = 1000;
    em.storeEpisode(episode);
    
    // Retrieve similar episode
    std::vector<float> query = {0.11f, 0.21f, 0.31f};  // Similar to stored episode
    auto results = em.retrieveSimilar(query, 3);
    
    // Should find similar episode
    assert(!results.empty());
    
    std::cout << "    testEpisodicMemoryRetrieveSimilar passed" << std::endl;
}

void testEpisodicMemoryRetrieveTemporal() {
    nlm::NeuralEpisodicMemory em;
    
    // Store episodes at different times
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 1000;
    em.storeEpisode(episode1);
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 2000;
    em.storeEpisode(episode2);
    
    // Retrieve from time window
    auto results = em.retrieveTemporal(1500, 2500, 5);
    
    // Should get episode2 only
    assert(results.size() == 1);
    
    std::cout << "    testEpisodicMemoryRetrieveTemporal passed" << std::endl;
}

void testEpisodicMemoryRetrieveByLocation() {
    nlm::NeuralEpisodicMemory em;
    
    // Store episode at location (10, 5)
    nlm::EpisodicMemoryItem episode;
    episode.positionX = 10.0f;
    episode.positionY = 5.0f;
    em.storeEpisode(episode);
    
    // Retrieve episodes near (10, 5) with radius 1.0
    auto results = em.retrieveByLocation(10.0f, 5.0f, 1.0f, 5);
    
    // Should find the episode
    assert(!results.empty());
    
    std::cout << "    testEpisodicMemoryRetrieveByLocation passed" << std::endl;
}

void testEpisodicMemoryRetrieveAfterAction() {
    nlm::NeuralEpisodicMemory em;
    
    // Store episode with specific action
    nlm::EpisodicMemoryItem episode;
    episode.action = nlm::ActionType::Rotate;
    em.storeEpisode(episode);
    
    // Retrieve episodes after this action
    auto results = em.retrieveAfterAction(nlm::ActionType::Rotate, 5);
    
    // Should find the episode
    assert(!results.empty());
    
    std::cout << "    testEpisodicMemoryRetrieveAfterAction passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::NeuralEpisodicMemory em;
    
    // Store an episode with neural activity
    nlm::EpisodicMemoryItem episode;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.9f, 0.7f, 0.5f};
    em.storeEpisode(episode);
    
    // Replay should not crash
    em.replayEpisode(&episode);
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryReplaySequence() {
    nlm::NeuralEpisodicMemory em;
    
    // Store multiple episodes
    std::vector<size_t> episodeIds;
    for (int i = 0; i < 3; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 1000;
        em.storeEpisode(episode);
        episodeIds.push_back(em.getEpisodeCount() - 1);
    }
    
    // Replay sequence (this is a simplified test)
    // The actual implementation would use episode indices
    em.replaySequence(episodeIds);
    
    std::cout << "    testEpisodicMemoryReplaySequence passed" << std::endl;
}

void testEpisodicMemoryAverageReward() {
    nlm::NeuralEpisodicMemory em;
    
    assert(em.getAverageReward() == 0.0f);
    
    // Store episodes with rewards
    nlm::EpisodicMemoryItem episode1;
    episode1.reward = 0.5f;
    em.storeEpisode(episode1);
    
    nlm::EpisodicMemoryItem episode2;
    episode2.reward = 0.8f;
    em.storeEpisode(episode2);
    
    // Check average reward
    float avg = em.getAverageReward();
    assert(std::abs(avg - 0.65f) < 0.01f);
    
    std::cout << "    testEpisodicMemoryAverageReward passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::NeuralEpisodicMemory em;
    
    // Store episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.age = i;
        em.storeEpisode(episode);
    }
    
    // Consolidate should not crash
    em.consolidate(0.5f);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryClear() {
    nlm::NeuralEpisodicMemory em;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    em.storeEpisode(episode);
    assert(em.getEpisodeCount() == 1);
    
    // Clear all episodes
    em.clear();
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryClear passed" << std::endl;
}

void testEpisodicMemoryReplayEnabled() {
    nlm::NeuralEpisodicMemory em;
    
    // Default should be enabled
    assert(em.isReplayEnabled() == true);
    
    // Disable replay
    em.enableReplay(false);
    assert(em.isReplayEnabled() == false);
    
    // Re-enable replay
    em.enableReplay(true);
    assert(em.isReplayEnabled() == true);
    
    std::cout << "    testEpisodicMemoryReplayEnabled passed" << std::endl;
}

void testEpisodicMemoryRecentEpisodes() {
    nlm::NeuralEpisodicMemory em;
    
    // Store multiple episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.age = i;
        em.storeEpisode(episode);
    }
    
    // Get recent episodes
    auto recent = em.getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    std::cout << "    testEpisodicMemoryRecentEpisodes passed" << std::endl;
}

void testEpisodicMemoryGetEpisode() {
    nlm::NeuralEpisodicMemory em;
    
    // Store an episode
    nlm::EpisodicMemoryItem episode1;
    em.storeEpisode(episode1);
    
    // Get episode by index
    const nlm::EpisodicMemoryItem* retrieved = em.getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->timestamp == 0);
    
    // Get non-existent episode
    const nlm::EpisodicMemoryItem* nonExistent = em.getEpisode(100);
    assert(nonExistent == nullptr);
    
    std::cout << "    testEpisodicMemoryGetEpisode passed" << std::endl;
}

void runAll() {
    std::cout << "Running Episodic Memory tests..." << std::endl;
    testEpisodicMemoryCreation();
    testEpisodicMemoryInitialize();
    testEpisodicMemoryStoreEpisode();
    testEpisodicMemoryRetrieveSimilar();
    testEpisodicMemoryRetrieveTemporal();
    testEpisodicMemoryRetrieveByLocation();
    testEpisodicMemoryRetrieveAfterAction();
    testEpisodicMemoryReplay();
    testEpisodicMemoryReplaySequence();
    testEpisodicMemoryAverageReward();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryClear();
    testEpisodicMemoryReplayEnabled();
    testEpisodicMemoryRecentEpisodes();
    testEpisodicMemoryGetEpisode();
}

} // namespace test_episodic_memory